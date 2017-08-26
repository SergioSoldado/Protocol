#pragma once
/**
 * @file   protocol/tcp/client/connection/ptr.hpp
 * @brief  Smart pointer declaration for protocol::tcp::client::Connection
 */

#include <boost/shared_ptr.hpp>

namespace protocol {
namespace tcp {
namespace client {

class Connection;

namespace connection {

/**
 * A mutable Connection pointer
 */
using Ptr = boost::shared_ptr<Connection>;

}  // namespace connection
}  // namespace client
}  // namespace tcp
}  // namespace protocol
