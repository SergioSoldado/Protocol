#pragma once
/**
 * @file   protocol/service/service.hpp
 * @brief  Class declaration of protocol::service::Service
 */

#include <boost/asio/io_service.hpp>
#include <future>
#include <vector>

namespace protocol {
namespace service {

/**
 * Encapsulates boost::asio::io_service instance and runs a configurable number of threads on it.
 */
class Service {
 public:
  /**
   * Start an io service specifying the number of threads to run it on
   * @param num_threads Number of threads to run io_service on
   */
  explicit Service(unsigned int num_threads);

  /**
   * Dtor
   */
  ~Service();

  /**
   * Get a reference to the io_service instance
   * @return A reference to the io_service
   */
  boost::asio::io_service& GetService();

 private:
  Service() = delete;                            ///< Delete default constructor
  Service(Service const&) = delete;              ///< Delete copy construct
  Service(Service&&) = delete;                   ///< Delete move construct
  Service& operator=(Service const&) = delete;   ///< Delete copy assignment
  Service& operator=(Service const&&) = delete;  ///< Delete move assignment

  boost::asio::io_service service_;            ///< The boost::asio::io_service instance
  std::shared_ptr<boost::asio::io_service::work> dummy_work_;   ///< Keeps io_service awake
  std::vector<std::future<void>> io_workers_;  ///< Container for io_service threads
};

}  // namespace service
}  // namespace protocol
