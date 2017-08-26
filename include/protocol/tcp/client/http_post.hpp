#pragma once
/**
 * @file   protocol/tcp/client/http_post.hpp
 * @brief  Class declaration of protocol::tcp::client::http::HTTPPost
 */

#include <exception>
#include <functional>
#include <memory>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/coroutine.hpp>

#include <protocol/tcp/client/http.hpp>
#include <protocol/tcp/client/http_post/ptr.hpp>
#include <protocol/tcp/socket/ptr.hpp>
#include <protocol/tcp/socket/buffer/ptr.hpp>

namespace protocol {
namespace tcp {
namespace client {
namespace http {

/**
 * Performs an HTTP Post method on an existing TCP connection.
 */
class PROTOCOL_DLL_PUBLIC HTTPPost : public boost::enable_shared_from_this<HTTPPost>,
boost::asio::coroutine {
 public:
  using Callback = std::function<void(std::exception_ptr, http_post::Ptr caller)>;  ///< Callback type

  /**
   * Create an HTTPPost instance and return a shared pointer to it
   * @param sock    Input socket in the open state
   * @param path    Remote host end-point to perform HTTP requests on
   * @param on_done Callback handler
   * @param headers HTTP headers to include in request
   * @param body    HTTP body to include in request
   * @return A shared pointer of a newly created HTTPPost instance
   */
  static http_post::Ptr Create(protocol::tcp::socket::sock::Ptr sock, std::string path, const Callback& on_done,
                               Headers headers, std::string body);

  /**
   * Public constructor for this class
   * @copydoc Create
   */
  HTTPPost(protocol::tcp::socket::sock::Ptr sock, std::string path, const Callback& on_done, Headers headers,
           std::string body);

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
   * Get the raw boost buffer
   * @return buffer
   */
  protocol::tcp::socket::buffer::Ptr GetBuffer();

  /**
   * Get the user specified post request body
   * @return Request body
   */
  const std::string& GetRequestBody() const;

  /**
   * Get the user specified post request headers
   * @return Request custom headers
   */
  const Headers& GetRequestHeaders() const;

  /**
   * Get the endpoint path
   * @return The endpoint path
   */
  const std::string& GetPath() const;

  /**
   * Getter for response_body_
   * @return response_body_
   */
  const std::string& GetResponseContent() const;

  /**
   * Getter for response_headers_
   * @return response_headers_
   */
  const Headers& GetResponseHeaders() const;

  /**
   * Getter for response_status_code_
   * @return response_status_code_
   */
  int GetResponseStatusCode() const;

  /**
   * Setter for request_body_
   * @param body Value to set
   */
  void SetRequestBody(std::string body);

  /**
   * Setter for request_headers_
   * @param headers Value to set
   */
  void SetRequestHeaders(Headers headers);

  /**
   * Setter for on_done_callback_
   * @param callback Set callback method
   */
  void SetOnDoneCallback(const Callback& callback);

 private:
  /**
   * Performs the logical operations of this class using boost::asio::coroutine
   * @param ec    Error code
   * @param bytes Number of bytes written
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
  std::string request_content_;                            ///< Request body

  int response_status_code_;                               ///< Response status code
  Headers response_headers_;                               ///< Response header fields
  std::string response_content_;                           ///< Response body
};

}  // namespace http
}  // namespace client
}  // namespace tcp
}  // namespace protocol
