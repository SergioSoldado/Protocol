/**
 * @cond   internal
 * @file   tests/write_read.cpp
 * @brief  Unit tests for protocol::tcp::Acceptor and protocol::tcp::Connection
 */

#include <chrono>
#include <exception>
#include <iostream>
#include <future>
#include <ostream>
#include <thread>

#include <boost/system/system_error.hpp>
#include <gtest/gtest.h>

#include <protocol/service/singleton.hpp>
#include <protocol/tcp/client/connection.hpp>
#include <protocol/tcp/server/acceptor.hpp>
#include <protocol/tcp/socket/read_one.hpp>
#include <protocol/tcp/socket/read_one_handlers.hpp>
#include <protocol/tcp/socket/write_one.hpp>
#include <protocol/tcp/socket/buffer.hpp>

#include "protocol/utility/get_available_port.hpp"

namespace protocol {
namespace tcp {

using std::exception_ptr;
using std::vector;

using socket::WriteOne;
using socket::ReadOne;
using server::Acceptor;
using client::Connection;

namespace sc = std::chrono;
namespace ba = boost::asio;
namespace bs = boost::system;

namespace read_handlers = ::protocol::tcp::socket::read_one::handlers;

class WriteRead : public ::testing::Test {
 protected:
  WriteRead() {
  }

  virtual ~WriteRead() {

  }

  virtual void SetUp() {
    std::cout << "SETUP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    service::singleton::TearDown();
  }

  virtual void TearDown() {
    std::cout << "TEARDOWN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    service::singleton::TearDown();
  }

};

/**
 * @test Tests the basic concepts
 */
TEST_F(WriteRead, WriteReadLambda) {
  uint16_t port = utility::GetAvailablePort();
  std::promise<std::pair<bs::error_code, std::string>> acceptor_promise;
  socket::sock::Ptr server_sock;
  Acceptor acceptor(port, {[&](bs::error_code ec, Acceptor &, socket::sock::Ptr sock) {
    if (ec) {
      acceptor_promise.set_value(std::make_pair(ec, std::string()));
    } else {
      ASSERT_TRUE(sock.get());
      server_sock = sock;
      acceptor_promise.set_value(std::make_pair(ec, std::string()));
    }
  }});

  std::promise<exception_ptr> connect_promise;
  socket::sock::Ptr client_sock;
  auto connection = Connection::Start("127.0.0.1", port, 200, {[&](exception_ptr eptr, socket::sock::Ptr sock) {
    if (eptr) {
      connect_promise.set_exception(eptr);
    } else {
      ASSERT_TRUE(sock.get());
      client_sock = sock;
      connect_promise.set_value(nullptr);
    }
  }});

  auto acceptor_fut = acceptor_promise.get_future();
  auto connect_fut = connect_promise.get_future();
  ASSERT_TRUE(std::future_status::ready == acceptor_fut.wait_for(sc::milliseconds(500)));
  ASSERT_TRUE(std::future_status::ready ==  connect_fut.wait_for(sc::milliseconds(500)));
  acceptor_fut.get();
  ASSERT_NO_THROW(connect_fut.get());

  std::promise<std::pair<bs::error_code, std::string>> read_promise;
  auto read = ReadOne::Start(server_sock, read_handlers::Substring(std::string("\n")),
                 [&](bs::error_code ec, socket::read_one::Ptr caller) {
                   if (ec) {
                     read_promise.set_value(std::make_pair(ec, std::string()));
                   } else {
                     read_promise.set_value(std::make_pair(ec, caller->GetBuffer()->ToString()));
                   }
//                   caller->Stop();
                 }, 1000);

  std::string write_message{"Hello World!\n"};
  std::promise<std::pair<bs::error_code, std::string>> write_promise;
  auto buf = socket::Buffer::Create(write_message);
  socket::write_one::Ptr write =
      WriteOne::Start(client_sock, buf, [&](bs::error_code ec, socket::write_one::Ptr caller) {
        write_promise.set_value(std::make_pair(ec, std::string()));
//        caller->Stop();
        (void)caller;
      }, 1000);

  auto write_fut = write_promise.get_future();
  auto read_fut = read_promise.get_future();
  ASSERT_EQ(std::future_status::ready, write_fut.wait_for(sc::milliseconds(500)));
  ASSERT_EQ(std::future_status::ready, read_fut.wait_for(sc::milliseconds(500)));
  std::pair<bs::error_code, std::string> read_result = read_fut.get();
  ASSERT_EQ(write_message, read_result.second);
  std::cout << "Read message is " << read_result.second << std::endl;

  acceptor.Stop();
}

/**
 * @test Tests operations using std::future
 */
TEST_F(WriteRead, WriteReadFuture) {
  uint16_t port = utility::GetAvailablePort();
  std::promise<std::pair<bs::error_code, std::string>> read_promise;
  Acceptor acceptor(port, [&](bs::error_code ec, Acceptor &, socket::sock::Ptr sock) {
    if (ec) {
      read_promise.set_value(std::make_pair(ec, std::string()));
    } else {
      ASSERT_TRUE(sock.get());
      ReadOne::Start(sock, read_handlers::Substring(std::string("\n")),
                     [&](bs::error_code ec, socket::read_one::Ptr caller) {
                       if (ec) {
                         read_promise.set_value(std::make_pair(ec, std::string()));
//                         caller->Stop();
                       } else {
                         read_promise.set_value(std::make_pair(ec, caller->GetBuffer()->ToString()));
                       }
                     }, 1000);
    }
//    acceptor.Stop();
  });

  std::string write_message{"Hello World!\n"};
  std::promise<exception_ptr> write_promise;
  auto connection = Connection::Start("127.0.0.1", port, 500, [&](exception_ptr eptr, socket::sock::Ptr sock) {
    if (eptr) {
      write_promise.set_exception(eptr);
    } else {
      ASSERT_TRUE(sock);
      auto buf = socket::Buffer::Create(write_message);
      WriteOne::Start(sock, buf, [&](bs::error_code, socket::write_one::Ptr caller) {
        write_promise.set_value(nullptr);
        caller->Stop();
      }, 1000);
    }
  });

  auto write_fut = write_promise.get_future();
  auto read_fut = read_promise.get_future();
  ASSERT_EQ(std::future_status::ready, write_fut.wait_for(sc::milliseconds(500)));
  ASSERT_EQ(std::future_status::ready, read_fut.wait_for(sc::milliseconds(500)));
  auto write_result = write_fut.get();
  ASSERT_TRUE(!write_result);
  std::pair<bs::error_code, std::string> read_result = read_fut.get();
  ASSERT_EQ(write_message, read_result.second);

  acceptor.Stop();
}

}  // namespace tcp
}  // namespace protocol

/// @endcond internal
