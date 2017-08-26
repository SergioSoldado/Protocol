/**
 * @cond   internal
 * @file   protocol/tcp/server/tests/acceptor.cpp
 * @brief  Unit tests for protocol::tcp::server::Acceptor
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

#include <boost/system/system_error.hpp>
#include <gtest/gtest.h>

#include <protocol/tcp/server/acceptor.hpp>
#include "protocol/utility/get_available_port.hpp"

namespace protocol {
namespace tcp {
namespace server {

namespace sc = std::chrono;
namespace ba = boost::asio;
namespace bs = boost::system;

/**
 * @test Tests the basic concepts
 */
TEST(Acceptor, Basics) {
  bool error = false;
  Acceptor acceptor(utility::GetAvailablePort(), {[&](bs::error_code ec, Acceptor &acceptor, socket::sock::Ptr) {
    if (ec) {
      error = true;
      acceptor.Stop();
    }
  }});
  ASSERT_TRUE(!error);
}

/**
 * @test Tests attempt to bind to non-available address
 */
TEST(Acceptor, BindError) {
  bool error = false;
  uint16_t port = utility::GetAvailablePort();
  ASSERT_NO_THROW(Acceptor acceptor(port, {[&](bs::error_code ec, Acceptor &acceptor, socket::sock::Ptr) {
    if (ec) {
      error = true;
      acceptor.Stop();
    }
  }}));
  ASSERT_TRUE(!error);

  ASSERT_NO_THROW(
  Acceptor acceptor2(port, {[&](bs::error_code ec, Acceptor &, socket::sock::Ptr) {
    if (ec) {
      error = true;
    }
  }}));
  ASSERT_TRUE(!error);

  // Tries to bind on port occupied by previous acceptor
  ASSERT_THROW(Acceptor(port, {[&](bs::error_code ec, Acceptor &acceptor, socket::sock::Ptr) {
    if (ec) {
      acceptor.Stop();
    }
  }}), boost::exception);
}

}  // namespace server
}  // namespace tcp
}  // namespace protocol

/// @endcond internal
