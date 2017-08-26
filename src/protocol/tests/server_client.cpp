/**
 * @cond   internal
 * @file   tests/server_client.cpp
 * @brief  Unit tests for protocol::tcp::Acceptor and protocol::tcp::Connection
 */

#include <exception>
#include <chrono>
#include <thread>

#include <boost/system/system_error.hpp>
#include <gtest/gtest.h>

#include <protocol/service/singleton.hpp>
#include <protocol/tcp/client/connection.hpp>
#include <protocol/tcp/server/acceptor.hpp>
#include <protocol/tcp/socket/read_one.hpp>
#include <protocol/tcp/socket/write_one.hpp>
#include "protocol/utility/get_available_port.hpp"

namespace protocol {
namespace tcp {

using server::Acceptor;
using client::Connection;
using std::exception_ptr;

namespace sc = std::chrono;
namespace ba = boost::asio;
namespace bs = boost::system;

class ServerClient : public ::testing::Test {
 protected:
  ServerClient() {
  }

  virtual ~ServerClient() {

  }

  virtual void SetUp() {
    service::singleton::TearDown();
  }

  virtual void TearDown() {
    service::singleton::TearDown();
  }

};

/**
 * @test Tests the basic concepts of WriteOne
 */
TEST_F(ServerClient, BasicWriteRead) {
  bool error = false;
  socket::sock::Ptr server_sock;
  uint16_t port = utility::GetAvailablePort();
  Acceptor acceptor(port, {[&](bs::error_code ec, Acceptor &acceptor, socket::sock::Ptr sock) {
    if (ec) {
      error = true;
      acceptor.Stop();
      return;
    }
    ASSERT_TRUE(sock.get());
    server_sock = sock;
  }});
  ASSERT_TRUE(!error);

  socket::sock::Ptr client_sock;
  auto connection = Connection::Start("127.0.0.1", port, 100, {[&](exception_ptr eptr, socket::sock::Ptr sock) {
    if (eptr) {
      error = true;
      return;
    }
    ASSERT_TRUE(sock.get());
    client_sock = sock;
  }});
  ASSERT_TRUE(!error);
}

}  // namespace tcp
}  // namespace protocol

/// @endcond internal
