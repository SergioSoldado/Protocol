/**
 * @file   protocol/tcp/client/src/http_get.cpp
 * @brief  Class definition of protocol::tcp::client::http::HTTPGet
 */

#include <algorithm>
#include <cstdio>
#include <future>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <protocol/service/singleton.hpp>
#include <protocol/tcp/client/http_get.hpp>
#include <protocol/tcp/socket/buffer.hpp>

#include <boost/asio/coroutine.hpp>  // Attention on the order including these two
#include <boost/asio/yield.hpp>      // changing it may result in compilation errors

namespace protocol {
namespace tcp {
namespace client {
namespace http {

using std::make_exception_ptr;
using std::system_error;
using std::system_category;
using boost::system::error_code;
using std::istringstream;
using std::ostringstream;
using std::pair;
using std::runtime_error;
using std::stoi;
using std::string;
using std::vector;
using ::protocol::tcp::socket::Buffer;
namespace ba = boost::asio;
namespace bs = boost::system;
/// A type of input stream character iterator
using ichar_iter = std::istream_iterator<istringstream::char_type>;

#define BIND(x) boost::bind(&HTTPGet::x, shared_from_this())               ///< Helper bind to member method
#define BIND2(x, y, z) boost::bind(&HTTPGet::x, shared_from_this(), y, z)  ///< Helper bind to member method

http_get::Ptr HTTPGet::Create(protocol::tcp::socket::sock::Ptr sock, std::string path, const Callback& on_done) {
  http_get::Ptr new_(new HTTPGet(sock, std::move(path), on_done));
  return new_;
}

HTTPGet::HTTPGet(protocol::tcp::socket::sock::Ptr sock, std::string path, const Callback& on_done)
    : stopped_(true),
      sock_(sock),
      buffer_(protocol::tcp::socket::Buffer::Create("")),
      on_done_callback_(on_done),
      path_(std::move(path)),
      request_headers_(),
      status_code_(-1),
      headers_(),
      body_() {
  if (!sock_) throw std::invalid_argument("Socket can't be null");
  deadline_ = std::shared_ptr<ba::deadline_timer>(new ba::deadline_timer(sock_->get_io_service()));
}

void HTTPGet::Start(const size_t timeout_ms) {
  if (!stopped_) throw std::runtime_error("Can't restart operation while still under execution");

  stopped_ = false;
  buffer_->Reset();

  if (timeout_ms) {
    deadline_->expires_from_now(boost::posix_time::milliseconds(timeout_ms));
    deadline_->async_wait(BIND(OnTimeout));
  }

  operator()(bs::error_code(), 0);
}

void HTTPGet::Stop() {
  if (!stopped_) stopped_ = true;

  deadline_->cancel();
}

const protocol::tcp::socket::sock::Ptr HTTPGet::GetSock() const {
  return sock_;
}

void HTTPGet::operator()(bs::error_code error = bs::error_code(), std::size_t = 0) {
  if (stopped_) return;

  if (error) {
    Stop();
    on_done_callback_(make_exception_ptr(std::system_error(error.value(), system_category(), error.message())),
                      shared_from_this());
    return;
  }

  reenter(this) {
    for (;;) {
      yield {
        // Send the request
        buffer_->Reset();
        *buffer_ << "GET " << path_ << " HTTP/1.1\r\n"
             << "Host: " << sock_->remote_endpoint().address().to_string() << "\r\n"
             << "Connection: Keep-Alive\r\n";

        for (auto &header : request_headers_) {
          *buffer_ << header.first << ": " << header.second << "\r\n";
        }

        *buffer_ << "\r\n";

        ba::async_write(*sock_, **buffer_, BIND2(operator(), _1, _2));
      }
      yield {
        buffer_->Reset();
        ba::async_read_until(*sock_, **buffer_, "\r\n\r\n", BIND2(operator(), _1, _2));
      }
      yield {
        std::string key, value;
        *buffer_ >> key;
        *buffer_ >> status_code_;
        std::cout << "HTTP Get response status<" << status_code_ << ">" << std::endl;

        // Get header fields
        while (true) {
          key.clear();
          value.clear();
          *buffer_ >> key;
          getline(buffer_->GetIStream(), value);
          std::cout << "Found header field <" << key << ", " << value << ">" << std::endl;
          if (key.empty() || value.empty()) {
            break;
          }
          headers_.push_back({key, value});
          if (*boost::asio::buffers_begin((**buffer_).data()) == '\r') {
            (**buffer_).consume(2);
            break;
          }
        }

        body_ = buffer_->ToString();
        buffer_->Reset();

        int content_length = 0;
        try {
          content_length = stoi(GetHeaderField(headers_, "Content-Length"));
        } catch (const std::exception &e) {
          std::cout << "Failed to get content length" << std::endl;
        }

        if (static_cast<int>(body_.size()) > content_length) {
          std::cout << "Got invalid content length" << std::endl;
          content_length = body_.size();
        }

        size_t to_read = content_length - body_.size();
        ba::async_read(*sock_, **buffer_, ba::transfer_exactly(to_read), BIND2(operator(), _1, _2));
      }
      yield {
        Stop();
        body_ = body_ +  buffer_->ToString();
        std::cout << "Got http content, body: " << body_ << std::endl;
        buffer_->Reset();
        on_done_callback_(nullptr, shared_from_this());
      }
    }
  }
}

void HTTPGet::OnTimeout() {
  if (stopped_) return;

  if (deadline_->expires_at() <= ba::deadline_timer::traits_type::now()) {
    Stop();
    on_done_callback_(make_exception_ptr(system_error(ETIMEDOUT, system_category(), "Operation timed out")),
                      shared_from_this());
  }
}

int HTTPGet::GetStatus() const {
    return status_code_;
  }

const std::string& HTTPGet::GetBody() const {
  return body_;
}

const Headers& HTTPGet::GetHeaders() const {
  return headers_;
}

void HTTPGet::SetRequestHeaders(const Headers& requestHeaders) {
  request_headers_ = requestHeaders;
}

}  // namespace http
}  // namespace client
}  // namespace tcp
}  // namespace protocol

