#pragma once
/**
 * @file  protocol/service/singleton.hpp
 * @brief Free method declarations for protocol::service::singleton
 */

#include <boost/asio/io_service.hpp>

namespace protocol {
namespace service {
namespace singleton {

/**
 * Get an instance of boost::asio::io_service
 * @return instance of boost::asio::io_service
 */
PROTOCOL_DLL_PUBLIC boost::asio::io_service& Instance();

/**
 * Stops and deletes io_service instance
 */
PROTOCOL_DLL_PUBLIC void TearDown();

}  // namespace singleton
}  // namespace service
}  // namespace protocol
