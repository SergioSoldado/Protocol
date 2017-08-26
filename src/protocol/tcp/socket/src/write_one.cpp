/**
 * @file   protocol/tcp/socket/src/write_one.cpp
 * @brief  Class definition of protocol::tcp::socket::WriteOne
 */

#include <boost/asio/write.hpp>
#include <boost/bind.hpp>

#include <protocol/service/singleton.hpp>
#include <protocol/tcp/socket/write_one.hpp>
#include <protocol/tcp/socket/buffer.hpp>

#include <boost/asio/coroutine.hpp>
#include <boost/asio/yield.hpp>

namespace protocol {
namespace tcp {
namespace socket {

namespace ba = boost::asio;
namespace bs = boost::system;
using std::vector;

#define BIND(x) boost::bind(&WriteOne::x, shared_from_this())               ///< Helper bind to member method
#define BIND2(x, y, z) boost::bind(&WriteOne::x, shared_from_this(), y, z)  ///< Helper bind to member method

write_one::Ptr WriteOne::Start(
    socket::sock::Ptr sock, buffer::Ptr buffer, const Callback& on_done, const size_t timeout_ms) {
  write_one::Ptr new_(new WriteOne(sock, buffer, on_done));
  new_->Start(timeout_ms);
  return new_;
}

WriteOne::WriteOne(socket::sock::Ptr sock, buffer::Ptr buffer, const Callback& on_done)
  : stopped_(false), sock_(sock), write_buffer_(buffer), on_done_callback_(on_done) {
  if (!sock_) throw std::invalid_argument("Socket can't be null");
  deadline_ = std::shared_ptr<ba::deadline_timer>(new ba::deadline_timer(sock_->get_io_service()));
}

void WriteOne::Start(const size_t timeout_ms) {
  if (timeout_ms) {
    deadline_->expires_from_now(boost::posix_time::milliseconds(timeout_ms));
    deadline_->async_wait(BIND(OnTimeout));
  }
  operator()(bs::error_code(), 0);
}

void WriteOne::Stop() {
  if (!stopped_) {
    stopped_ = true;
  }
  deadline_->cancel();
}

const sock::Ptr& WriteOne::GetSock() const {
  return sock_;
}

void WriteOne::operator()(bs::error_code ec = bs::error_code(), std::size_t = 0) {
  if (stopped_) {
    return;
  }

  if (ec) {
    on_done_callback_(ec, shared_from_this());
  } else {
    reenter(this) {
      for (;;) {
        yield ba::async_write(*sock_, **write_buffer_, BIND2(operator(), _1, _2));
        yield service::singleton::Instance().post(boost::bind(on_done_callback_, ec, shared_from_this()));
      }
    }
  }
}

void WriteOne::OnTimeout() {
  if (stopped_) {
    return;
  }

  if (deadline_->expires_at() <= ba::deadline_timer::traits_type::now()) {
    Stop();
    on_done_callback_(bs::error_code(bs::errc::timed_out, bs::system_category()), shared_from_this());
  }
}

}  // namespace socket
}  // namespace tcp
}  // namespace protocol
