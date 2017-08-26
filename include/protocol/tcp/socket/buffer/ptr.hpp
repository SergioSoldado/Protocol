#pragma once
/**
 * @file   protocol/tcp/socket/buffer/ptr.hpp
 * @brief  Smart pointer declaration for protocol::tcp::socket::Buffer
 */

#include <boost/shared_ptr.hpp>

namespace protocol {
namespace tcp {
namespace socket {
class Buffer;
namespace buffer {

/**
 * A mutable Buffer pointer
 */
using Ptr = boost::shared_ptr<Buffer>;

}  // namespace buffer
}  // namespace socket
}  // namespace tcp
}  // namespace protocol
