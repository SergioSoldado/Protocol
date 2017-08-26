/**
 * @file   protocol/tcp/client/src/http_post.cpp
 * @brief  Class definition of protocol::tcp::client::http::HTTPPost
 */

#include <cstdio>
#include <future>
#include <iostream>
#include <ostream>
#include <sstream>
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

#include <protocol/service/singleton.hpp>
#include <protocol/tcp/client/http_post.hpp>
#include <protocol/tcp/socket/buffer.hpp>

#include <boost/asio/coroutine.hpp>  // Attention on the order including these two
#include <boost/asio/yield.hpp>      // changing it may result in compilation errors

namespace protocol {
namespace tcp {
namespace client {
namespace http {

namespace ba = boost::asio;
namespace bs = boost::system;

using std::istringstream;
using std::make_exception_ptr;
using std::system_error;
using std::system_category;
using std::ostringstream;
using std::runtime_error;
using std::stoi;
using std::string;
using std::vector;

using boost::system::error_code;

using ::protocol::tcp::socket::Buffer;

/// A type of input stream character iterator
using ichar_iter = std::istream_iterator<istringstream::char_type>;

#define BIND(x) boost::bind(&HTTPPost::x, shared_from_this())               ///< Helper bind to member method
#define BIND2(x, y, z) boost::bind(&HTTPPost::x, shared_from_this(), y, z)  ///< Helper bind to member method

http_post::Ptr HTTPPost::Create(protocol::tcp::socket::sock::Ptr sock, std::string path, const Callback& on_done,
                                Headers headers = Headers(), std::string body = "") {
  http_post::Ptr new_(new HTTPPost(sock, std::move(path), on_done, std::move(headers), std::move(body)));
  return new_;
}

HTTPPost::HTTPPost(protocol::tcp::socket::sock::Ptr sock, std::string path, const Callback& on_done, Headers headers,
                   std::string body)
    : stopped_(true),
      sock_(sock),
      buffer_(Buffer::Create("")),
      on_done_callback_(on_done),
      path_(std::move(path)),
      request_headers_(headers),
      request_content_(body),
      response_status_code_(-1),
      response_headers_(),
      response_content_() {
  if (!sock_) throw std::invalid_argument("Socket can't be null");
  deadline_ = std::shared_ptr<ba::deadline_timer>(new ba::deadline_timer(sock_->get_io_service()));
}

void HTTPPost::Start(const size_t timeout_ms) {
  if (!stopped_) throw std::runtime_error("Can't restart operation while still under execution");

  stopped_ = false;
  response_status_code_ = -1;
  response_headers_.clear();
  response_content_.clear();

  if (timeout_ms) {
    deadline_->expires_from_now(boost::posix_time::milliseconds(timeout_ms));
    deadline_->async_wait(BIND(OnTimeout));
  }

  operator()(bs::error_code(), 0);
}

void HTTPPost::Stop() {
  if (!stopped_) stopped_ = true;

  deadline_->cancel();
}

const protocol::tcp::socket::sock::Ptr HTTPPost::GetSock() const {
  return sock_;
}

protocol::tcp::socket::buffer::Ptr HTTPPost::GetBuffer() {
  return buffer_;
}

void HTTPPost::operator()(bs::error_code error = bs::error_code(), std::size_t = 0) {
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
        *buffer_ << "POST " << path_ << " HTTP/1.1\r\n"
             << "Host: " << sock_->remote_endpoint().address().to_string() << "\r\n"
             << "Content-Length: " << request_content_.size() << "\r\n"
             << "Connection: Keep-Alive\r\n";

        for (auto &header : request_headers_) {
          *buffer_ << header.first << ": " << header.second << "\r\n";
        }

        *buffer_ << "\r\n";

        *buffer_ << request_content_;

        ba::async_write(*sock_, **buffer_, BIND2(operator(), _1, _2));
      }
      yield {
        // Read the response header
        buffer_->Reset();
        ba::async_read_until(*sock_, **buffer_, "\r\n\r\n", BIND2(operator(), _1, _2));
      }
      yield {
        // Parse the response header and read the content in existent
        std::string key, value;
        *buffer_ >> key;
        *buffer_ >> response_status_code_;
        std::cout << "HTTP Post response status<" << response_status_code_ << ">" << std::endl;

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
          response_headers_.push_back({key, value});
          if (*boost::asio::buffers_begin((**buffer_).data()) == '\r') {
            (**buffer_).consume(2);
            break;
          }
        }

        response_content_ = buffer_->ToString();
        buffer_->Reset();

        int content_length = 0;
        try {
          content_length = stoi(GetHeaderField(response_headers_, "Content-Length"));
        } catch (const std::exception &e) {
          std::cout <<" Failed to get content length" << std::endl;
        }

        if (static_cast<int>(response_content_.size()) > content_length) {
          std::cout << "Got invalid content length" << std::endl;
          content_length = response_content_.size();
        }

        size_t to_read = content_length - response_content_.size();
        ba::async_read(*sock_, **buffer_, ba::transfer_exactly(to_read), BIND2(operator(), _1, _2));
      }
      yield {
        // Perform callback
        Stop();
        response_content_ = response_content_ + buffer_->ToString();
        std::cout << "HTTP Post successful, returned content: " << buffer_->ToString() << std::endl;
        on_done_callback_(nullptr, shared_from_this());
      }
    }
  }
}

void HTTPPost::OnTimeout() {
  if (stopped_) return;

  if (deadline_->expires_at() <= ba::deadline_timer::traits_type::now()) {
    Stop();
    on_done_callback_(make_exception_ptr(system_error(ETIMEDOUT, system_category(), "Operation timed out")),
                      shared_from_this());
  }
}

const std::string& HTTPPost::GetRequestBody() const {
  return request_content_;
}

const Headers& HTTPPost::GetRequestHeaders() const {
  return request_headers_;
}

const std::string& HTTPPost::GetPath() const {
  return path_;
}

const std::string& HTTPPost::GetResponseContent() const {
  return response_content_;
}

const Headers& HTTPPost::GetResponseHeaders() const {
  return response_headers_;
}

int HTTPPost::GetResponseStatusCode() const {
  return response_status_code_;
}

void HTTPPost::SetRequestBody(std::string content) {
  request_content_ = std::move(content);
}

void HTTPPost::SetRequestHeaders(Headers headers) {
  request_headers_ = std::move(headers);
}

void HTTPPost::SetOnDoneCallback(const Callback& callback) {
  on_done_callback_ = callback;
}

}  // namespace http
}  // namespace client
}  // namespace tcp
}  // namespace protocol

