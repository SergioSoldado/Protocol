/**
 * @cond   internal
 * @file   tests/test_read_one.cpp
 * @brief  Unit tests for protocol::tcp::ReadOne
 */

#include <stdexcept>

#include <gtest/gtest.h>

#include <protocol/tcp/socket/read_one.hpp>

namespace protocol {
namespace tcp {
namespace socket {

namespace ba = boost::asio;
namespace bs = boost::system;

/**
 * @test Tests the basic concepts
 */
void BasicsTest() {
  sock::Ptr sock;
  ASSERT_THROW(
      ReadOne::Start(sock, [](ReadOne::Iterator, ReadOne::Iterator end) { return std::make_pair(end, true); },
                     [](bs::error_code, read_one::Ptr) {}, 1000),
      std::invalid_argument);
}

}  // namespace socket
}  // namespace tcp
}  // namespace protocol

/// @endcond internal
