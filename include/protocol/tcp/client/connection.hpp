#pragma once
/**
 * @file   protocol/tcp/client/connection.hpp
 * @brief  Class definition of protocol::tcp::client::Connection
 */

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <cstdint>
#include <cstddef>
#include <functional>
#include <exception>
#include <string>

#include <protocol/tcp/client/connection/ptr.hpp>
#include <protocol/tcp/socket/ptr.hpp>

namespace protocol {
namespace tcp {
namespace client {

/**
 * Asynchronously establishes a TCP connection to a specified address within the given timeout.
 */
class PROTOCOL_DLL_PUBLIC Connection : public boost::enable_shared_from_this<Connection>, boost::noncopyable {
 public:
  using Callback = std::function<void(std::exception_ptr, socket::sock::Ptr)>;  ///< Callback type

  /**
   * Create an instance of Connection
   * @param uri        Address to connect to
   * @param port       Port to connect to
   * @param timeout_ms Timeout for establishing the connection. A value of 0 implies no timeout
   * @param on_done    Callback to return result asynchronously
   * @return  A shared pointer of a newly created Connection instance
   */
  static connection::Ptr Start(std::string uri, const uint16_t port, const size_t timeout_ms, const Callback& on_done);

  /**
   * Dtor
   */
  ~Connection();

  /**
   * Stop all tasks
   */
  void Stop();

 private:
  /**
   * Ctor
   * @param on_done  Callback reference
   */
  explicit Connection(const Callback& on_done);

  /**
   * Starts the connection establishment operation
   * @param uri        Address to connect to
   * @param port       Port to connect to
   * @param timeout_ms Timeout for establishing the connection. A value of 0 implies no timeout
   */
  void Start(std::string uri, const uint16_t port, const size_t timeout_ms);

  /**
   * Handler for deadline task
   */
  void HandleDeadline();

  /**
   * Handler for connection task
   * @param ec                Error code in case an error occurs
   * @param endpoint_iterator An iterator with the available endpoints
   */
  void HandleConnect(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

  /**
   * Connection resolver handler
   * @param ec                Error code in case an error occurs
   * @param endpoint_iterator An iterator with the available endpoints
   */
  void HandleResolve(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

 private:
  bool stopped_;                             ///< State control
  boost::asio::deadline_timer deadline_;     ///< Deadline timer
  socket::sock::Ptr sock_;                   ///< Network socket resource
  boost::asio::ip::tcp::resolver resolver_;  ///< Host resolver
  Callback on_done_;                         ///< Client callback
};

}  // namespace client
}  // namespace tcp
}  // namespace protocol
