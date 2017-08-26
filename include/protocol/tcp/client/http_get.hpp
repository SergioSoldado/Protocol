#pragma once
/**
 * @file  protocol/tcp/client/http_get.hpp
 * @brief  Class declaration of protocol::tcp::client::http::HTTPGet
 */

#include <functional>
#include <exception>
#include <memory>
#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/coroutine.hpp>

#include <protocol/tcp/client/http.hpp>
#include <protocol/tcp/client/http_get/ptr.hpp>
#include <protocol/tcp/socket/ptr.hpp>
#include <protocol/tcp/socket/buffer/ptr.hpp>

namespace protocol {
namespace tcp {
namespace client {
namespace http {

/**
 * Performs an HTTP Get method on an existing TCP connection.
 */
class PROTOCOL_DLL_PUBLIC HTTPGet : public boost::enable_shared_from_this<HTTPGet>,
boost::asio::coroutine {
 public:
  using Callback = std::function<void(std::exception_ptr, http_get::Ptr caller)>;  ///< Callback type

  /**
   * Create an HTTPGet instance and return a shared pointer to it
   * @param sock    Input socket in the open state
   * @param path    Remote host end-point to perform HTTP requests on
   * @param on_done Callback handler
   * @return A shared pointer of a newly created HTTPGet instance
   */
  static http_get::Ptr Create(protocol::tcp::socket::sock::Ptr sock, std::string path, const Callback& on_done);

  /**
   * Public constructor for this class
   * @copydoc Create
   */
  HTTPGet(protocol::tcp::socket::sock::Ptr sock, std::string path, const Callback& on_done);

  /**
   * Start the operation with the given timeout value
   * @param timeout_ms Timeout value in milliseconds, a value of 0 implies no timeout
   */
  void Start(const size_t timeout_ms);

  /**
   * Stop the operation and close the socket
   */
  void Stop();

  /**
   * Get the underlying socket
   * @return socket
   */
  const protocol::tcp::socket::sock::Ptr GetSock() const;

  /**
   * Getter for status_code_
   * @return HTTP response status
   */
  int GetStatus() const;

  /**
   * Getter for headers_
   * @return HTTP response headers
   */
  const Headers& GetHeaders() const;

  /**
   * Getter for body_
   * @return HTTP response body
   */
  const std::string& GetBody() const;

  /**
   * Sets the HTML request headers
   * @param requestHeaders Request headers to set
   */
  void SetRequestHeaders(const Headers& requestHeaders);

 private:
  /**
   * Performs the logical operations of this class using boost::asio::coroutine
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
  protocol::tcp::socket::sock::Ptr sock_;                  ///< Network socket resource
  protocol::tcp::socket::buffer::Ptr buffer_;              ///< Input/Output buffer
  std::shared_ptr<boost::asio::deadline_timer> deadline_;  ///< Deadline timer
  Callback on_done_callback_;                              ///< Client callback
  std::string path_;                                       ///< Remote host endpoint
  Headers request_headers_;                                ///< Request header fields
  int status_code_;                                        ///< Response status code
  Headers headers_;                                        ///< Response header fields
  std::string body_;                                       ///< Response body
};

}  // namespace http
}  // namespace client
}  // namespace tcp
}  // namespace protocol

