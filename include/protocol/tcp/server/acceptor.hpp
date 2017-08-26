#pragma once
/**
 * @file   protocol/tcp/server/acceptor.hpp
 * @brief  Class declaration of protocol::tcp::server::Acceptor
 */

#include <boost/asio/ip/tcp.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <functional>

#include <protocol/tcp/socket/ptr.hpp>

namespace protocol {
namespace tcp {
namespace server {

/**
 * Bind to the specified endpoint and listens for incoming connection.
 * Once a client is connected performs a callback handing over the newly created socket which holds the client
 * connection.
 */
class PROTOCOL_DLL_PUBLIC Acceptor : boost::noncopyable {
 public:
  /**
   * Callback performed when a new connection is established or an error occurs.
   * The handler should be non-blocking.
   */
  using Callback = std::function<void(boost::system::error_code, Acceptor &, socket::sock::Ptr)>;

  /**
   * Acceptor constructor
   * @param port          Service port to bind to
   * @param on_new_client Client callback (should be non-blocking)
   */
  Acceptor(uint16_t port, Callback on_new_client);

  /**
   * Dtor
   */
  ~Acceptor();

  /**
   * Stop task scheduling and close acceptor
   */
  void Stop();

 private:
  /**
   * Handle an incoming connection
   * @param ec     Error code
   * @param socket Socket that holds connection
   */
  void HandleAccept(const boost::system::error_code &ec, socket::sock::Ptr socket);

 private:
  bool stopped_;                             ///< Control variable
  boost::asio::ip::tcp::acceptor acceptor_;  ///< TCP listener/acceptor
  Callback on_new_client_;                   ///< Client callback
};

}  // namespace server
}  // namespace tcp
}  // namespace protocol
