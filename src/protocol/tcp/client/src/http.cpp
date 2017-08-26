/**
 * @file   protocol/tcp/client/src/http.cpp
 * @brief  Free method definitions for protocol::tcp::client::http
 */

#include <stdexcept>
#include <algorithm>
#include <utility>

#include <protocol/tcp/client/http.hpp>

namespace protocol {
namespace tcp {
namespace client {
namespace http {

using std::pair;
using std::runtime_error;
using std::string;

std::string& GetHeaderField(Headers& headers, const std::string& name) {
  std::string key_ = name + ":";
  auto it = std::find_if(headers.begin(), headers.end(), [&key_](const pair<string, string>& element) {
    return element.first == key_;
  });
  if (headers.end() != it) {
    return it->second;
  }
  throw std::runtime_error("Couldn't find field:" + name);
}

}  // namespace http
}  // namespace client
}  // namespace tcp
}  // namespace protocol
