// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/string/string_tokenizer.hpp"

#include "alloy/core/string/zstring_view.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/intrinsics.hpp"

auto alloy::core::get_message(tokenizer_error error) noexcept -> zstring_view
{
  switch (error) {
    case tokenizer_error::none: {
      return "no error";
    }
    case tokenizer_error::out_of_tokens: {
      return "out of tokens";
    }
  }
  ALLOY_ASSERT(false, "Invalid error input");
  ALLOY_UNREACHABLE();
}
