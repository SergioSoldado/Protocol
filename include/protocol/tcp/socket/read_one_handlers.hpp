#pragma once
/**
 * @file   protocol/tcp/socket/read_one_handlers.hpp
 * @brief  Declaration of some functors to use as a protocol::tcp::socket::ReadOne::StopCondition
 */

#include <cstdio>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <protocol/tcp/socket/read_one.hpp>

namespace protocol {
namespace tcp {
namespace socket {
namespace read_one {
namespace handlers {

/**
 * Match based on number of bytes
 */
struct PROTOCOL_DLL_PUBLIC ByteCount {
  /**
   * Default ctor
   * @param bytes Number of bytes to match
   */
  explicit ByteCount(size_t bytes);

  /**
   * The match algorithm
   * @param begin Start of search range
   * @param end   End of search range
   * @return The first member of the return value is an iterator marking one-past-the-end of the bytes that have been
   * consumed by the match function. The second member of the return value is true if a match has been found, false
   * otherwise.
   */
  std::pair<ReadOne::Iterator, bool> operator()(ReadOne::Iterator begin, ReadOne::Iterator end);

  size_t bytes_;  ///< Number of bytes to read
};

/**
 * Match based on matching a string from the end of the buffer
 */
struct PROTOCOL_DLL_PUBLIC Substring {
  /**
   * Default ctor
   * @param val Value to extract a string that we will search for
   */
  template <typename T>
  explicit Substring(const T& val)
      : tail_(val.begin(), val.end()) {
    if (tail_.empty()) throw std::runtime_error("Token can't be empty");
  }

  /**
   * The match algorithm
   * @param begin Start of search range
   * @param end   End of search range
   * @return The first member of the return value is an iterator marking one-past-the-end of the bytes that have been
   * consumed by the match function. The second member of the return value is true if a match has been found, false
   * otherwise.
   */
  std::pair<ReadOne::Iterator, bool> operator()(ReadOne::Iterator begin, ReadOne::Iterator end);

  std::vector<char> tail_;  ///< String to match
};

}  // namespace handlers
}  // namespace read_one
}  // namespace socket
}  // namespace tcp
}  // namespace protocol
