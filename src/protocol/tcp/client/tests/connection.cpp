/**
 * @cond  internal
 * @file  protocol/tcp/client/tests/connection.cpp
 * @brief Unit tests for protocol::tcp::client::Connection
 */

#include <cerrno>
#include <chrono>
#include <future>
#include <thread>

#include <boost/system/system_error.hpp>
#include <gtest/gtest.h>

#include <protocol/tcp/client/connection.hpp>

#include "protocol/utility/get_available_port.hpp"

namespace protocol {
namespace tcp {
namespace client {

using std::promise;
using std::exception_ptr;
namespace sc = std::chrono;
namespace ba = boost::asio;
namespace bs = boost::system;
using boost::asio::ip::tcp;

/**
 * @test Tests the basic concepts
 */
TEST(Connection, Basics) {
  auto connection =
      Connection::Start("127.0.0.1", utility::GetAvailablePort(), 1000, [](exception_ptr, socket::sock::Ptr) {});
}

/**
 * @test Toture test, io_service
 */
TEST(Connection, Torture) {
  std::vector<connection::Ptr> connections;
  for (size_t i = 0; i < 50; ++i) {
    connections.push_back(
        Connection::Start("127.0.0.1", utility::GetAvailablePort(), 1000, [](exception_ptr, socket::sock::Ptr) {}));
  }
}

/**
 * @test Tests attempt to bind to non-available address
 * ToDo For some reason using host addresses besides an IP address (e.g. "localhost") hangs the test. This didn't happen before..
 */
TEST(Connection, NoEndpoint) {
  promise<exception_ptr> result;
  auto connection =
      Connection::Start("sdafdasfdda", 34523, 500, {[&](exception_ptr eptr, socket::sock::Ptr) {
        ASSERT_TRUE(eptr);
        if (eptr) {
          try {
            std::rethrow_exception(eptr);
          } catch (const std::system_error &e) {
            ASSERT_EQ(ETIMEDOUT, e.code().value());
            result.set_value(nullptr);
          }
        }
      }});
  auto res = result.get_future().get();
  connection->Stop();
}

/**
 * @test Tests timeout
 */
void TimeoutTest() {
  // ToDo use low level socket to listen but no accept?
}

}  // namespace client
}  // namespace tcp
}  // namespace protocol

/// @endcond internal
