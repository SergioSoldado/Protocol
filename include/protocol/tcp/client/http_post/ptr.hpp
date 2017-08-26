#pragma once
/**
 * @file   protocol/tcp/client/http_get/ptr.hpp
 * @brief  Smart pointer declaration for protocol::tcp::client::http::HTTPPost
 */

#include <boost/shared_ptr.hpp>

namespace protocol {
namespace tcp {
namespace client {
namespace http {

class HTTPPost;

namespace http_post {

/**
 * A mutable HTTPPost pointer
 */
using Ptr = boost::shared_ptr<HTTPPost>;

}  // namespace http_post
}  // namespace http
}  // namespace client
}  // namespace tcp
}  // namespace protocol
