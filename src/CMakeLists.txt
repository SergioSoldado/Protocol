include_directories(
        protocol/service/inc
        protocol/utility/inc
)

set(protocol_src
        protocol/service/src/service.cpp
        protocol/service/src/singleton.cpp

        protocol/tcp/client/src/connection.cpp
        protocol/tcp/client/src/http.cpp
        protocol/tcp/client/src/http_get.cpp
        protocol/tcp/client/src/http_post.cpp

        protocol/tcp/server/src/acceptor.cpp

        protocol/tcp/socket/src/buffer.cpp
        protocol/tcp/socket/src/read_one.cpp
        protocol/tcp/socket/src/read_one_handlers.cpp
        protocol/tcp/socket/src/write_one.cpp

        protocol/utility/src/${PLATFORM}/get_available_port.cpp
        )

set(test_src
        protocol/tcp/client/tests/connection.cpp
        protocol/tcp/server/tests/acceptor.cpp
        protocol/tcp/socket/tests/buffer.cpp
        protocol/tcp/socket/tests/read_one.cpp
        protocol/tcp/socket/tests/write_one.cpp
        protocol/tests/server_client.cpp
        protocol/tests/write_read.cpp
        protocol/tests/main.cpp
        protocol/tests/exampl0.cpp
        )

add_library(protocol SHARED ${protocol_src})
target_link_libraries(protocol pthread ${Boost_LIBRARIES})

add_executable(protocol_test ${protocol_src} ${test_src})
target_link_libraries(protocol_test pthread cppunit gtest ${Boost_LIBRARIES})
add_test(ProtocolTest protocol_test)
