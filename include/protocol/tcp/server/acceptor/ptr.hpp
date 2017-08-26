#pragma once
/**
 * @file   protocol/tcp/server/acceptor/ptr.hpp
 * @brief  Smart pointer declarations for protocol::tcp::server::Acceptor
 */

#include <boost/shared_ptr.hpp>

namespace protocol {
namespace tcp {
namespace server {
class Acceptor;
namespace acceptor {

/**
 * A mutable Acceptor pointer
 */
using Ptr = boost::shared_ptr<Acceptor>;

}  // namespace acceptor
}  // namespace server
}  // namespace tcp
}  // namespace protocol
