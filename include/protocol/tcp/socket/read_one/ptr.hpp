#pragma once
/**
 * @file   protocol/tcp/socket/read_one/ptr.hpp
 * @brief  Smart pointer declarations for protocol::tcp::socket::ReadOne
 */

#include <boost/shared_ptr.hpp>

namespace protocol {
namespace tcp {
namespace socket {
class ReadOne;
namespace read_one {

/**
 * A mutable ReadOne pointer
 */
using Ptr = boost::shared_ptr<ReadOne>;

}  // namespace read_one
}  // namespace socket
}  // namespace tcp
}  // namespace protocol
