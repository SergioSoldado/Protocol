#pragma once
/**
 * @file   protocol/tcp/socket/buffer.hpp
 * @brief  Class declaration of protocol::tcp::socket::Buffer
 */

#include <boost/asio/streambuf.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <sstream>
#include <ostream>
#include <istream>

#include <protocol/tcp/socket/buffer/ptr.hpp>

namespace protocol {
namespace tcp {
namespace socket {

/**
 * Wrapper class around boost:asio::streambuf
 */
class PROTOCOL_DLL_PUBLIC Buffer : public boost::enable_shared_from_this<Buffer> {
 public:
  /**
   * Create a Buffer instance and initialise it
   * @param val Value to stream to buffer
   * @return A Buffer::ptr object
   */
  template <typename T>
  static buffer::Ptr Create(const T& val) {
    buffer::Ptr new_ = buffer::Ptr(new Buffer());
    *new_ << val;
    return new_;
  }

  /**
   * Default ctor for Buffer
   */
  Buffer() : buffer_(), os_(&buffer_), is_(&buffer_) {}

  /**
   * Insert operator overload to read data from buffer
   * @param out    Output stream to write contents from buffer
   * @param buffer Buffer object to read from
   * @return A reference to the output stream
   */
  friend std::ostream& operator<<(std::ostream& out, protocol::tcp::socket::Buffer& buffer) {
    boost::asio::streambuf &ref = buffer.buffer_;
    out << &ref;
    return out;
  }

  /**
   * Insert operator overload to write data to buffer
   * @param val Object to read data from
   * @return A reference to the internal output stream
   */
  template <typename T>
  std::ostream& operator<<(const T& val) {
    os_ << val;
    return os_;
  }

  /**
   * Extract operator overload to read data to buffer
   * @param val Object to write data to
   * @return A reference to the internal input stream
   */
  template <typename T>
  std::istream& operator>>(T& val) {
    is_ >> val;
    return is_;
  }

  /**
   * Dereference operator overload
   * @return the streambuf object reference
   */
  boost::asio::streambuf& operator*() {
    return buffer_;
  }

  /**
   * Get the buffer input stream
   * @return Input stream
   */
  std::istream& GetIStream() {
    return is_;
  }

  /**
   * Get the buffer output stream
   * @return Output stream
   */
  std::ostream& GetOStream() {
    return os_;
  }

  /**
   * Empty the buffer
   */
  void Reset() {
    buffer_.consume(buffer_.size());
  }

  /**
   * Get a string created with the buffer's contents. Doesn't consume the buffer
   * @return String with buffer contents
   */
  const std::string ToString() const {
    return {boost::asio::buffers_begin(buffer_.data()), boost::asio::buffers_end(buffer_.data())};
  }

 private:
  boost::asio::streambuf buffer_;  ///<  The boost stream buffer
  std::ostream os_;                ///< Output stream for buffer_
  std::istream is_;                ///< Input stream for buffer_
};

/**
 * Insert operator declaration
 * @param out    Output stream to write contents from buffer
 * @param buffer Buffer object to read from
 * @return A reference to the output stream
 */
PROTOCOL_DLL_PUBLIC std::ostream& operator<<(std::ostream& out, protocol::tcp::socket::Buffer& buffer);

}  // namespace socket
}  // namespace tcp
}  // namespace protocol
