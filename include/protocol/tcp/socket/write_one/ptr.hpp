#pragma once
/**
 * @file   protocol/tcp/socket/write_one/ptr.hpp
 * @brief  Smart pointer declarations for protocol::tcp::socket::WriteOne
 */

#include <boost/shared_ptr.hpp>

namespace protocol {
namespace tcp {
namespace socket {
class WriteOne;
namespace write_one {

/**
 * A mutable WriteOne pointer
 */
using Ptr = boost::shared_ptr<WriteOne>;

}  // namespace write_one
}  // namespace socket
}  // namespace tcp
}  // namespace protocol
