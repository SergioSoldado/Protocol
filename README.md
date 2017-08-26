# PROTOCOL

#### Contents

 * Introduction
 * Requirements
 * Recommended modules
 * Installation
 * Configuration
 * Troubleshooting
 * FAQ
 * Maintainers

### Introduction

 Protocol is a project that uses boost::asio to implement an API around popular protocols. It uses the concept of a Connection object which encapsulates a socket and buffer and which is used to build other behavourial constructs around to implement protocol logic. Protocol uses boost co-routines, which may save resources and simplify implementation when dealing with a complex protocol logic.
 
 It is also usefull just as a oneshot send/receive e.g. send a command to a remote andpoint and receive a callback with the response data once a certain condition is met.

 At the moment it supports:
  * TCP
    * Server and client connections
    * ReadHandlers which allow you to define buffer patterns which are matches to incomming data that generate callbacks
  * HTTP client
    * GET
    * POST
#### Requirements
This module requires the following packages:
 * C++ >= std11. I believe the only OS dependent component is protocol::utility::GetAvailablePort() which I haven't implemented for any other OS besides POSIX.
 * CMake >= v3.2
 * Boost >= v1.60
 * GTest v1.7.0
 * Doxygen (if you wan't to build the documentation)
   
### Build
```
mkdir build && cd build
cmake .. -DBUILD_DOCUMENTATION=Y
make all doc -j8
```

Running the unit tests:
```
./src/protocol_test
```

### Examples
##### Reading data from a TCP endpoint until we reach the pattern "World/n". This example uses promises to retrieve the result.

```
std::promise<std::pair<bs::error_code, std::string>> read_promise;
  Acceptor acceptor(port, [&](bs::error_code error, Acceptor &, socket::sock::Ptr sock) {
    if (error) {
      read_promise.set_value(std::make_pair(error, std::string()));
      return;
    }
    ReadOne::Start(sock, read_handlers::Substring(std::string("World\n")),
                   [&](bs::error_code ec, socket::read_one::Ptr caller) {
                     read_promise.set_value(std::make_pair(ec, caller->GetBuffer()->ToString()));
                     caller->Stop();  // Only do it once
                   }, 1000);
  });
  
  // Print the message assuming no error occured
  std::cout << read_promise.get_future().get().second << std::endl;
```

##### Likewise sending data to a specific endpoint might look like the following.

```
const std::string message{"Hello World!\n"};
  std::promise<exception_ptr> write_promise;
  auto connection = Connection::Start(host, port, timeout_ms, [&](exception_ptr eptr, socket::sock::Ptr sock) {
    if (eptr) {
      write_promise.set_exception(eptr);
      return;
    }
    auto buf = socket::Buffer::Create(message);
    WriteOne::Start(sock, buf, [&](bs::error_code, socket::write_one::Ptr caller) {
      write_promise.set_value(nullptr);
      caller->Stop();  // Only do it once, otherwise we could set new data in the buffer here and continue streaming
    }, 1000);
  });
```

### Troubleshooting
Running the unit tests randomly results in a segfault. I think this is due to some object teardown issues related to boost::asio::io_service. Running the tests one at a time doesn't seem to cause this problem.

### ToDo
Implement other protocols.
