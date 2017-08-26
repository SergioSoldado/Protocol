/**
 * @file   protocol/tcp/client/src/connection.cpp
 * @brief  Class definition of protocol::tcp::client::Connection
 */

#include <iostream>
#include <system_error>

#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <protocol/service/singleton.hpp>
#include <protocol/tcp/client/connection.hpp>

namespace protocol {
namespace tcp {
namespace client {

using boost::format;
using std::make_exception_ptr;
using std::system_error;
using std::system_category;
namespace ba = boost::asio;
namespace bs = boost::system;
using ba::ip::tcp;

#define BIND(x) boost::bind(&Connection::x, shared_from_this())               ///< Helper bind to member method
#define BIND2(x, y, z) boost::bind(&Connection::x, shared_from_this(), y, z)  ///< Helper bind to member method

connection::Ptr Connection::Start(
    std::string uri, const uint16_t port, const size_t timeout_ms, const Callback& on_done) {
  connection::Ptr new_(new Connection(on_done));
  new_->Start(std::move(uri), port, timeout_ms);
  return new_;
}

Connection::Connection(const Callback& on_done)
  : stopped_(false),
    deadline_(service::singleton::Instance()),
    //sock_(new tcp::socket(deadline_.get_io_service())),
    resolver_(deadline_.get_io_service()),
    on_done_(on_done) {
  sock_ = std::shared_ptr<boost::asio::ip::tcp::socket>(new boost::asio::ip::tcp::socket(deadline_.get_io_service()));
}

void Connection::Start(std::string uri, const uint16_t port, const size_t timeout_ms) {
  if (stopped_) return;

  std::cout << "Connecting to " << uri << ":" << port << std::endl;

  if (timeout_ms) {
    deadline_.expires_from_now(boost::posix_time::milliseconds(timeout_ms));
    deadline_.async_wait(BIND(HandleDeadline));
  }
  resolver_.async_resolve(tcp::resolver::query(uri, std::to_string(port)), BIND2(HandleResolve, _1, _2));
}

void Connection::Stop() {
  if (stopped_) return;
  stopped_ = true;
  deadline_.cancel();
  resolver_.cancel();
}

void Connection::HandleDeadline() {
  if (stopped_) return;

  if (deadline_.expires_at() <= ba::deadline_timer::traits_type::now()) {
    Stop();
    on_done_(make_exception_ptr(system_error(ETIMEDOUT, system_category(), "Operation timed out")),
             sock_);
  }

  // Put the actor back to sleep.
  deadline_.async_wait(boost::bind(&Connection::HandleDeadline, this));
}

Connection::~Connection() {
  Stop();
}

void Connection::HandleConnect(const bs::error_code& error, ba::ip::tcp::resolver::iterator endpoint_iterator) {
  if (stopped_) return;

  if (error && (endpoint_iterator == tcp::resolver::iterator())) {
      Stop();
      on_done_(make_exception_ptr(std::system_error(error.value(), system_category(),
                                                    (format("Couldn't connect: %1%") % error.message()).str())), sock_);
    return;
    };

  auto host = sock_->remote_endpoint().address().to_string();
  auto port = sock_->remote_endpoint().port();
  if (!error) {
    Stop();
    std::cout << "Successfully connected to " << host << ":" << port << std::endl;
    on_done_(nullptr, sock_);
  } else if (endpoint_iterator != tcp::resolver::iterator()) {
    sock_->close();
    tcp::endpoint endpoint = *endpoint_iterator;
    std::cout << "Couldn't connect to " << host << ":" << port << ", error: " << error.message() << ". Trying next endpoint" <<  std::endl;
    sock_->async_connect(endpoint, BIND2(HandleConnect, _1, ++endpoint_iterator));
  }
}

void Connection::HandleResolve(const bs::error_code& error, ba::ip::tcp::resolver::iterator endpoint_iterator) {
  if (stopped_) return;
  if (error) {
    on_done_(
        make_exception_ptr(
            std::system_error(error.value(), system_category(),
                              (format("Couldn't resolve address: %1%") % error.message()).str())), sock_);
    Stop();
  }

  tcp::endpoint endpoint = *endpoint_iterator;
  sock_->async_connect(endpoint, BIND2(HandleConnect, _1, ++endpoint_iterator));
}

}  // namespace client
}  // namespace tcp
}  // namespace protocol
