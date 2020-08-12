/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

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

