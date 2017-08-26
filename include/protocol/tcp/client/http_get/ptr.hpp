#pragma once
/**
 * @file   protocol/tcp/client/http_get/ptr.hpp
 * @brief  Smart pointer declaration for protocol::tcp::client::http::HTTPGet
 */

#include <boost/shared_ptr.hpp>

namespace protocol {
namespace tcp {
namespace client {
namespace http {

class HTTPGet;

namespace http_get {

/**
 * A mutable HTTPGet pointer
 */
using Ptr = boost::shared_ptr<HTTPGet>;

}  // namespace http_get
}  // namespace http
}  // namespace client
}  // namespace tcp
}  // namespace protocol
