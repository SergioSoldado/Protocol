#pragma once
/**
 * @file   protocol/tcp/socket/write_one.hpp
 * @brief  Class declaration of protocol::tcp::socket::WriteOne
 */

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <functional>
#include <memory>
#include <vector>
#include <boost/asio/coroutine.hpp>

#include <protocol/tcp/socket/write_one/ptr.hpp>
#include <protocol/tcp/socket/ptr.hpp>
#include <protocol/tcp/socket/buffer/ptr.hpp>

namespace protocol {
namespace tcp {
namespace socket {

/**
 * Performs a write operation on a socket.
 */
class PROTOCOL_DLL_PUBLIC WriteOne : public boost::enable_shared_from_this<WriteOne>, boost::asio::coroutine {
 public:
  using Callback = std::function<void(boost::system::error_code, write_one::Ptr caller)>;  ///< Callback type

  /**
   * Create a WriteOne instance and return a shared pointer to it
   * @param sock       Input socket in the open state
   * @param buffer     Buffer to send
   * @param on_done    Callback called upon completion or when an error condition occurs
   * @param timeout_ms Timeout value in milliseconds, a value of 0 implies no timeout
   * @return A shared pointer of a newly created WriteOne instance
   */
  static write_one::Ptr Start(sock::Ptr sock, buffer::Ptr buffer, const Callback& on_done, const size_t timeout_ms);

  /**
   * Private constructor for this class
   * @param sock    Input socket in the open state
   * @param buffer  Buffer to send
   * @param on_done Callback called upon completion or when an error condition occurs
   */
  WriteOne(sock::Ptr sock, buffer::Ptr buffer, const Callback& on_done);

  /**
   * Stop the operation and close the socket
   */
  void Stop();

  /**
   * Get the underlying socket
   * @return socket
   */
  const sock::Ptr& GetSock() const;

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
  buffer::Ptr write_buffer_;                               ///< Output buffer
  std::shared_ptr<boost::asio::deadline_timer> deadline_;  ///< Deadline timer
  Callback on_done_callback_;                              ///< Client callback
};

}  // namespace socket
}  // namespace tcp
}  // namespace protocol
