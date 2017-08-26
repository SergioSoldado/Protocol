#pragma once
/**
 * @file   protocol/utility/get_available_port.hpp
 * @brief  Get an available port number
 */

#include <cstdint>

namespace protocol {
namespace utility {

/**
 * Get an available port number
 * @return Port number
 * @throws std::runtime_error
 */
uint16_t GetAvailablePort();

}  // namespace utility
}  // namespace protocol
