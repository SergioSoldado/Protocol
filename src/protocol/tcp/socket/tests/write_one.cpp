/**
 * @cond   internal
 * @file   tests/test_write_one.cpp
 * @brief  Unit tests for protocol::tcp::WriteOne
 */

#include <stdexcept>

#include <gtest/gtest.h>

#include <protocol/tcp/socket/write_one.hpp>

namespace protocol {
namespace tcp {
namespace socket {

namespace ba = boost::asio;
namespace bs = boost::system;

/**
 * @test Tests the basic concepts
 */
TEST(WriteOne, Basics) {
  sock::Ptr sock;
  buffer::Ptr data;
  ASSERT_THROW(
      WriteOne::Start(sock, data, [](bs::error_code, write_one::Ptr) {}, 1000), std::invalid_argument);
}

}  // namespace socket
}  // namespace tcp
}  // namespace protocol

/// @endcond internal
