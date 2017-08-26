/**
 * @file   protocol/tcp/socket/src/read_one.cpp
 * @brief  Class definition of protocol::tcp::socket::ReadOne
 */

#include <boost/asio/read_until.hpp>
#include <boost/bind.hpp>
#include <protocol/service/singleton.hpp>
#include <protocol/tcp/socket/read_one.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/yield.hpp>

#include <protocol/tcp/socket/buffer.hpp>

namespace protocol {
namespace tcp {
namespace socket {

namespace ba = boost::asio;
namespace bs = boost::system;

#define BIND(x) boost::bind(&ReadOne::x, shared_from_this())               ///< Helper bind to member method
#define BIND2(x, y, z) boost::bind(&ReadOne::x, shared_from_this(), y, z)  ///< Helper bind to member method

read_one::Ptr ReadOne::Start(socket::sock::Ptr sock, const StopCondition& completion_match_codition,
    const Callback& on_done, const size_t timeout_ms) {
  read_one::Ptr new_(new ReadOne(sock, completion_match_codition, on_done));
  new_->Start(timeout_ms);
  return new_;
}

ReadOne::ReadOne(socket::sock::Ptr sock, const StopCondition& completion_match_codition, const Callback& on_done)
  : stopped_(false),
    sock_(sock),
    buffer_(new Buffer()),
    stop_condition_(completion_match_codition),
    on_done_callback_(on_done) {
  if (!sock_) throw std::invalid_argument("Socket can't be null");
  deadline_ = std::shared_ptr<ba::deadline_timer>(new ba::deadline_timer(sock_->get_io_service()));
}

void ReadOne::Start(const size_t timeout_ms) {
  if (timeout_ms) {
    deadline_->expires_from_now(boost::posix_time::milliseconds(timeout_ms));
    deadline_->async_wait(BIND(OnTimeout));
  }
  operator()(bs::error_code(), 0);
}

void ReadOne::Stop() {
  if (!stopped_) {
    stopped_ = true;
  }
  deadline_->cancel();
}

const sock::Ptr& ReadOne::GetSock() const {
  return sock_;
}

void ReadOne::operator()(bs::error_code ec = bs::error_code(), std::size_t = 0) {
  if (stopped_) {
    return;
  }

  if (ec) {
    on_done_callback_(ec, shared_from_this());
  } else {
    reenter(this) {
      for (;;) {
        yield ba::async_read_until(*sock_, **buffer_, stop_condition_, BIND2(operator(), _1, _2));
        yield service::singleton::Instance().post(boost::bind(on_done_callback_, ec, shared_from_this()));
      }
    }
  }
}

const socket::buffer::Ptr ReadOne::GetBuffer() const {
  return buffer_;
}

void ReadOne::OnTimeout() {
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
