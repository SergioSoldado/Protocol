/**
 * @file   protocol/utility/src/posix/get_available_port.cpp
 * @brief  Get an available port number
 */

#include <stdexcept>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol/utility/get_available_port.hpp"

namespace protocol {
namespace utility {

uint16_t GetAvailablePort() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Socket error");
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = 0;
  if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    throw std::runtime_error("No port available");
  }

  socklen_t len = sizeof(serv_addr);
  if (getsockname(sock, (struct sockaddr *)&serv_addr, &len) == -1) {
    throw std::runtime_error("Error getting socket info");
  }

  uint16_t port = serv_addr.sin_port;

  if (close(sock) < 0) {
    throw std::runtime_error("Error closing port");
  }

  return port;
}

}  // namespace utility
}  // namespace protocol
