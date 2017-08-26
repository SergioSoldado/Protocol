/**
 * @file   protocol/service/src/service.cpp
 * @brief  Class definition of protocol::service::Service
 */

#include <iostream>
#include <thread>
#include <vector>
#include "protocol/service/service.hpp"

namespace protocol {
namespace service {

Service::Service(unsigned int num_threads) :
    service_(),
    dummy_work_(std::shared_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(service_))) {
  for (size_t i = 0; i < num_threads; ++i) {
    io_workers_.push_back(std::async(std::launch::async, [&]() { service_.run(); }));
  }
}

Service::~Service() {
  dummy_work_.reset();
//  for (auto & worker : io_workers_) {
//    worker.get();
//  }
  service_.stop();
}

boost::asio::io_service& Service::GetService() {
  return service_;
}

}  // namespace service
}  // namespace protocol
