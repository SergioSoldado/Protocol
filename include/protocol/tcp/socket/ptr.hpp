#pragma once
/**
 * @file   protocol/tcp/socket/ptr.hpp
 * @brief  Smart pointer declarations for boost::asio::ip::tcp::socket
 */

#include <boost/asio/ip/tcp.hpp>
#include <memory>

namespace protocol {
namespace tcp {
namespace socket {
namespace sock {

using Ptr = std::shared_ptr<boost::asio::ip::tcp::socket>;  ///< Shared pointer to boost asio socket type

}  // namespace sock
}  // namespace socket
}  // namespace tcp
}  // namespace protocol
