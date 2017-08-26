#pragma once
/**
 * @file  protocol/tcp/client/http.hpp
 * @brief Free method declarations for protocol::tcp::client::http
 */

#include <string>
#include <vector>

namespace protocol {
namespace tcp {
namespace client {
namespace http {

using Headers = std::vector<std::pair<std::string, std::string>>;  ///< HTTP header type

/**
 * Retrieves the HTML header value which has the given key
 * @param headers HTML headers object
 * @param name Field name
 * @return Reference to header entry value
 * @throws Exception if key is not found
 */
PROTOCOL_DLL_PUBLIC std::string& GetHeaderField(Headers& headers, const std::string &name);

}  // namespace http
}  // namespace client
}  // namespace tcp
}  // namespace protocol

