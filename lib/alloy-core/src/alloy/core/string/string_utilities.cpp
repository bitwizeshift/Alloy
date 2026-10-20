// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/string/string_utilities.hpp"

#include <charconv>
#include <system_error> // std::errc

namespace alloy::core {
namespace {

  auto to_parse_error(std::errc ec)
    noexcept -> string_utilities::parse_error
  {
    using error_type = string_utilities::parse_error;

    switch (ec) {
      case std::errc::result_out_of_range: {
        return error_type::out_of_range;
      }
      case std::errc::invalid_argument: {
        return error_type::invalid_argument;
      }
      default: {
        return error_type::unknown;
      }
    }
  }

  template <typename T>
  auto to_number(string_view in)
    noexcept -> result<T,string_utilities::parse_error>
  {
    const auto* const first = in.data();
    const char* const last = first + in.size();

    auto out = T{};
    const auto result = std::from_chars(first, last, out);

    if (result.ec != std::errc{}) {
      return fail(to_parse_error(result.ec));
    }

    return out;
  }

} // namespace <anonymous>
} // namespace alloy::core

//-----------------------------------------------------------------------------
// Signed Integer Parsing
//-----------------------------------------------------------------------------

auto alloy::core::string_utilities::to_int8(string_view in)
  noexcept -> result<std::int8_t,parse_error>
{
  return to_number<std::int8_t>(in);
}

auto alloy::core::string_utilities::to_int16(string_view in)
  noexcept -> result<std::int16_t,parse_error>
{
  return to_number<std::int16_t>(in);
}

auto alloy::core::string_utilities::to_int32(string_view in)
  noexcept -> result<std::int32_t,parse_error>
{
  return to_number<std::int32_t>(in);
}

auto alloy::core::string_utilities::to_int64(string_view in)
  noexcept -> result<std::int64_t,parse_error>
{
  return to_number<std::int64_t>(in);
}

//-----------------------------------------------------------------------------
// Unsigned Integer Parsing
//-----------------------------------------------------------------------------

auto alloy::core::string_utilities::to_uint8(string_view in)
  noexcept -> result<std::uint8_t,parse_error>
{
  return to_number<std::uint8_t>(in);
}

auto alloy::core::string_utilities::to_uint16(string_view in)
  noexcept -> result<std::uint16_t,parse_error>
{
  return to_number<std::uint16_t>(in);
}

auto alloy::core::string_utilities::to_uint32(string_view in)
  noexcept -> result<std::uint32_t,parse_error>
{
  return to_number<std::uint32_t>(in);
}

auto alloy::core::string_utilities::to_uint64(string_view in)
  noexcept -> result<std::uint64_t,parse_error>
{
  return to_number<std::uint64_t>(in);
}

//-----------------------------------------------------------------------------
// Floating Point Parsing
//-----------------------------------------------------------------------------

auto alloy::core::string_utilities::to_float(string_view in)
  noexcept -> result<float,parse_error>
{
  return to_number<float>(in);
}

auto alloy::core::string_utilities::to_double(string_view in)
  noexcept -> result<double,parse_error>
{
  return to_number<double>(in);
}

auto alloy::core::string_utilities::to_real(string_view in)
  noexcept -> result<real,parse_error>
{
#if ALLOY_CORE_PRECISION == ALLOY_CORE_PRECISION_FLOAT
    return to_float(in);
#elif ALLOY_CORE_PRECISION == ALLOY_CORE_PRECISION_DOUBLE
    return to_double(in);
#else
# error Uknown precision for 'to_real'
#endif
}

auto alloy::core::get_message(string_utilities::parse_error error)
  noexcept -> zstring_view
{
  using error_type = string_utilities::parse_error;

  switch (error) {
    case error_type::none: {
      return "no error";
    }
    case error_type::invalid_argument: {
      return "Invalid argument";
    }
    case error_type::out_of_range: {
      return "Parsed output exceeds range of receiver type";
    }
    case error_type::unknown: {
      return "An unknown parse error occurred.";
    }
  }
  ALLOY_ASSERT(false, "Invalid error input");
  ALLOY_UNREACHABLE();
}
