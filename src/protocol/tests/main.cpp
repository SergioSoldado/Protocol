/**
 * @cond   internal
 * @file   protocol/tests/main.cpp
 * @brief  Unit tests entry point for protocol::
 */

#include <gtest/gtest.h>

/**
 * The binary entry point for C++ unit tests
 * @param argc the argument count
 * @param argv the argument values
 * @returns the executable status code
 */
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return ret;
}

/// @endcond
