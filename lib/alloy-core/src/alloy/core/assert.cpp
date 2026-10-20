// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/assert.hpp"

#include <cstdio>      // std::printf
#include <exception>   // std::terminate
#include <atomic>      // std::atomic
#include <cstdlib>     // std::abort
#include <array>       // std::array

namespace alloy::core {
namespace {

  [[noreturn]]
  void default_assert_handler(const char* message) noexcept
  {
    std::fprintf(stderr, "%s", message);
    std::fflush(stderr);
    std::terminate();
  }

  auto s_assert_handler = std::atomic<assert_handler_fn*>{&default_assert_handler};

} // namespace <anonymous>
} // namespace alloy::core

alloy::core::assert_handler_fn*
  alloy::core::set_assert_handler(assert_handler_fn* handler)
  noexcept
{
  if (handler == nullptr) {
    handler = &default_assert_handler;
  }

  return s_assert_handler.exchange(handler);
}

[[noreturn]] ALLOY_COLD
void alloy::core::detail::assert_internal(const char* condition,
                                          const char* message,
                                          const char* file_name,
                                          std::size_t line,
                                          const char* function_name)
  noexcept
{
  auto buffer = std::array<char,512>{};

  if (message == nullptr) {
    std::snprintf(buffer.data(), buffer.size() - 1,
      "[assertion] %s (%zu)::%s\n"
      "            assertion failure: condition '%s' failed\n",
      file_name,
      line,
      function_name,
      condition
    );
  } else {
    std::snprintf(buffer.data(), buffer.size() - 1,
      "[assertion] %s (%zu)::%s\n"
      "            assertion failure: condition '%s' failed with message: "
      "'%s'\n",
      file_name,
      line,
      function_name,
      condition,
      message
    );
  }

  (*s_assert_handler.load())(buffer.data());
  std::abort();
}

