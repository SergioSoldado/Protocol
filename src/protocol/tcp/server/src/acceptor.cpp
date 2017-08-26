/**
 * @file   protocol/tcp/server/src/acceptor.cpp
 * @brief  Class definition of protocol::tcp::server::Acceptor
 */

#include <iostream>

#include <boost/bind.hpp>
#include <cppunit/extensions/HelperMacros.h>

#include <protocol/service/singleton.hpp>
#include <protocol/tcp/server/acceptor.hpp>

namespace protocol {
namespace tcp {
namespace server {

namespace ba = boost::asio;
namespace bs = boost::system;
using ba::ip::tcp;

Acceptor::Acceptor(uint16_t port, Callback on_new_client)
  : stopped_(false),
    acceptor_(service::singleton::Instance(), tcp::endpoint(tcp::v4(), port)),
    on_new_client_(on_new_client) {
  socket::sock::Ptr sock_(new ba::ip::tcp::socket(acceptor_.get_io_service()));
  acceptor_.async_accept(*sock_, boost::bind(&Acceptor::HandleAccept, this, _1, sock_));
}

Acceptor::~Acceptor() {
  if (!stopped_) Stop();
}

void Acceptor::Stop() {
  if (stopped_) return;

  stopped_ = true;
  acceptor_.cancel();
  if (acceptor_.is_open())
    acceptor_.close();
}

void Acceptor::HandleAccept(const bs::error_code &ec, std::shared_ptr<ba::ip::tcp::socket> sock) {
  if (stopped_) return;

  if (ec) {
    std::cout << "Error accepting connection: " << ec.message() << std::endl;
    on_new_client_(ec, *this, sock);
  } else {
    on_new_client_(ec, *this, sock);
  }

  if (stopped_) return;

  socket::sock::Ptr sock_(new ba::ip::tcp::socket(acceptor_.get_io_service()));
  acceptor_.async_accept(*sock_, boost::bind(&Acceptor::HandleAccept, this, _1, sock_));
}

}  // namespace server
}  // namespace tcp
}  // namespace protocol
