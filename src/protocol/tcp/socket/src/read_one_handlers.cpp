/**
 * @file   tcp/socket/src/read_one_handlers.cpp
 * @brief  Definition of some functors to use as a protocol::tcp::socket::ReadOne::StopCondition
 */

#include <protocol/tcp/socket/read_one_handlers.hpp>

namespace protocol {
namespace tcp {
namespace socket {
namespace read_one {
namespace handlers {

ByteCount::ByteCount(size_t bytes)
    : bytes_(bytes) {
}

std::pair<ReadOne::Iterator, bool> ByteCount::operator()(ReadOne::Iterator begin, ReadOne::Iterator end) {
  if (static_cast<size_t>(end - begin - 1) < bytes_) {
    return std::make_pair(begin, false);
  }
  return std::make_pair(begin + bytes_, true);
}

std::pair<ReadOne::Iterator, bool> Substring::operator()(ReadOne::Iterator begin, ReadOne::Iterator end) {
  ReadOne::Iterator i = begin;
  while (true) {
    while ((i != end) && (*i != tail_[0])) ++i;
    const int size = tail_.size();
    if (std::distance(i, end) < size) {
      return std::make_pair(i, false);
    }

    size_t tail_idx;
    for (tail_idx = 1; tail_idx < tail_.size(); ++tail_idx) {
      if (*(i + tail_idx) != tail_[tail_idx]) {
        break;
      }
    }

    if (tail_idx != tail_.size()) {
      ++i;
      continue;
    }

    return std::make_pair(i + tail_.size(), true);
  }
}

}  // namespace handlers
}  // namespace read_one
}  // namespace socket
}  // namespace tcp
}  // namespace protocol
