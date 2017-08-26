/**
 * @file   protocol/service/src/singleton.cpp
 * @brief  Singleton definition for protocol::service::singleton
 */

#include <mutex>
#include <protocol/service/singleton.hpp>
#include "protocol/service/service.hpp"

namespace protocol {
namespace service {
namespace singleton {

static Service* service = nullptr;

static std::mutex& GetMutex() {
  static std::mutex m_;
  return m_;
}

void TearDown() {
  GetMutex().lock();
  if (nullptr != service) {
    delete service;
    service = nullptr;
  }
  GetMutex().unlock();
}


boost::asio::io_service& Instance() {
  static const unsigned int kNumServiceThreads = 5;
  GetMutex().lock();
  if (nullptr == service) {
    service = new Service(kNumServiceThreads);
  }
  if (service->GetService().stopped()) {
    delete service;
    service = new Service(kNumServiceThreads);
  }
  GetMutex().unlock();

  return service->GetService();
}

}  // namespace singleton
}  // namespace service
}  // namespace protocol
