/**
 * @cond   internal
 * @file   tests/test_read_one.cpp
 * @brief  Unit tests for protocol::tcp::Buffer
 */

#include <iostream>

#include <gtest/gtest.h>

#include <protocol/tcp/socket/buffer.hpp>

namespace protocol {
namespace tcp {
namespace socket {

namespace ba = boost::asio;

/**
 * @test Tests the basic concepts
 */
TEST(Buffer, Basics) {
  std::string str("Hello world!");
  Buffer::Create(str);
  Buffer::Create("Hello world!");
  Buffer::Create(12345);
}

/**
 * @test Tests the ostream concepts
 */
TEST(Buffer, OstreamTest) {
  std::string str("Hello\r\nworld!\r\n12345");
  auto buffer = Buffer::Create(str);
  std::ostringstream ss;
  ss << *buffer;
  ASSERT_TRUE(str == ss.str());
}

/**
 * @test Tests the istream concepts
 */
TEST(Buffer, IstreamTest) {
  std::string str("Hello\r\nworld!\r\n12345");
  auto buffer = Buffer::Create(str);
  std::string hello, world;
  int number;
  *buffer >> hello;
  ASSERT_TRUE("Hello" == hello);
  *buffer >> world;
  ASSERT_TRUE("world!" == world);
  *buffer >> number;
  ASSERT_TRUE(12345 == number);
}

}  // namespace socket
}  // namespace tcp
}  // namespace protocol

/// @endcond internal
