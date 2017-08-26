include_directories(inc)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    list(APPEND sources
            src/posix/get_available_port.cpp
            )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    list(APPEND sources
            src/win/get_available_port.cpp
            )
endif()