#pragma once
/**
 * @file   protocol/tcp/socket/read_one.hpp
 * @brief  Class definition of protocol::tcp::socket::ReadOne
 */

#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <functional>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>
#include <boost/asio/coroutine.hpp>

#include <protocol/tcp/socket/read_one/ptr.hpp>
#include <protocol/tcp/socket/ptr.hpp>
#include <protocol/tcp/socket/buffer/ptr.hpp>

namespace protocol {
namespace tcp {
namespace socket {

/**
 * Performs a read operation on a socket until a given stop condition is met.
 */
class PROTOCOL_DLL_PUBLIC ReadOne : public boost::enable_shared_from_this<ReadOne>, boost::asio::coroutine {
 public:
  using Callback = std::function<void(boost::system::error_code, read_one::Ptr)>;              ///< Callback type
  using Iterator = boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type>;  ///< Alias for iterator
  using StopCondition = std::function<std::pair<Iterator, bool>(Iterator, Iterator)>;          ///< Match condition type

  /**
   * Create a ReadOne instance and starts the asynchronous operation
   * @param sock            Input socket in the open state
   * @param stop_condition  Functor that determines when operation has completed
   * @param on_done         Callback called when operation has completed
   * @param timeout_ms      Timeout value in milliseconds, a value of 0 implies no timeout
   * @return A shared_ptr to a newly created ReadOne instance
   */
  static read_one::Ptr Start(
      sock::Ptr sock, const StopCondition& stop_condition, const Callback& on_done, const size_t timeout_ms);

  /**
   * Private constructor for this class
   * @param sock            Input socket in the open state
   * @param stop_condition  Functor that determines when operation has completed
   * @param on_done         Callback called when operation has completed
   */
  ReadOne(sock::Ptr sock, const StopCondition& stop_condition, const Callback& on_done);

  /**
   * Stop the operation
   */
  void Stop();

  /**
   * Get the underlying socket
   * @return socket
   */
  const sock::Ptr& GetSock() const;

  /**
   * Get the input buffer
   * @return buffer
   */
  const socket::buffer::Ptr GetBuffer() const;

 private:
  /**
   * Start the operation with the given timeout value
   * @param timeout_ms Timeout value in milliseconds, a value of 0 implies no timeout
   */
  void Start(const size_t timeout_ms);

  /**
   * Performs most of the logical operations of this class using boost::asio::coroutine
   * @param ec Error code
   * @param n  Number of bytes written
   */
  void operator()(boost::system::error_code ec, std::size_t bytes);

  /**
   * Handler for timer
   */
  void OnTimeout();

 private:
  bool stopped_;                                           ///< Control variable
  sock::Ptr sock_;                                         ///< Network socket resource
  buffer::Ptr buffer_;                                     ///< Input buffer
  StopCondition stop_condition_;                           ///< Condition to consider read operation complete
  std::shared_ptr<boost::asio::deadline_timer> deadline_;  ///< Deadline timer
  Callback on_done_callback_;                              ///< Client callback
};

}  // namespace socket
}  // namespace tcp
}  // namespace protocol
