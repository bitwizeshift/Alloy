///////////////////////////////////////////////////////////////////////////////
/// \file casts.hpp
///
/// \todo(Bitwize): Document this
///////////////////////////////////////////////////////////////////////////////

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
#ifndef ALLOY_CORE_UTILITIES_CASTS_HPP
#define ALLOY_CORE_UTILITIES_CASTS_HPP

#include "alloy/core/assert.hpp"
#include "alloy/core/intrinsics.hpp"

#include <type_traits> // std::declval, std::enable_if_t, etc
#include <utility>     // std::forward
#include <cstring>     // std::memcpy
#include <limits>      // std::numeric_limits

namespace alloy::core {
  inline namespace casts {

    //=========================================================================
    // non-member functions : casts
    //=========================================================================

    /// \brief Performs an implicit cast between two types, such that the cast
    ///        is actually performed using the implicit language casting
    ///
    /// This can be used to signal cases that would otherwise produce warnings
    /// when type promotions are performed.
    ///
    /// \note This cast does not participate in overload resolution unless
    ///       std::is_implicitly_convertible<From,To>::value is true
    ///
    /// \tparam To the type to convert to
    /// \param from the type being converted
    template <typename To,
              typename From,
              typename = std::enable_if_t<std::is_convertible<From, To>::value>>
    constexpr auto implicit_cast(From&& from)
      noexcept(std::is_nothrow_constructible<To, From>::value) -> To;

    /// \brief Performs a cast that is known to potentially cause narrowing
    ///
    /// This cast will not prevent the narrowing from occurring; this simply
    /// exists as a way to semantically mark up in code that there may be some
    /// form of narrowing at this point.
    ///
    /// \note This cast does not participate in overload resolution unless
    ///       std::is_integral<To>::value and std::is_integral<From>::value are
    ///       both true.
    ///
    /// \tparam To the type to cast to
    /// \param from the value to cast
    /// \return the casted value
    template <typename To,
              typename From,
              typename = std::enable_if_t<std::is_integral<To>::value &&
                                          std::is_integral<From>::value>>
    constexpr auto narrow_cast(From from) noexcept -> To;

    /// \brief Performs a bounded casting safely between numeric types.
    ///
    /// This function will assert if the value being casted does not properly
    /// fit into the value being converted to.
    /// Calling this method has the same form as calling static_cast
    ///
    /// \note This cast does not participate in overload resolution unless
    ///       std::is_integral<To>::value and std::is_integral<From>::value are
    ///       both true.
    ///
    /// \tparam To the type to cast to
    /// \param from the value to cast
    /// \return the casted value
    template <typename To,
              typename From,
              typename = std::enable_if_t<std::is_integral<To>::value &&
                                          std::is_integral<From>::value>>
    constexpr auto checked_narrow_cast(From from) noexcept -> To;

    /// \brief Statically casts a pointer if the pointer is dynamically
    ///        castable to that type, asserting otherwise.
    ///
    /// \note This will only assert in debug builds
    ///
    /// \note This cast does not participate in overload resolution unless
    ///       std::is_pointer<To>::value and std::is_pointer<From>::value are
    ///       both true
    ///
    /// \tparam To the type to cast to
    /// \param ptr the pointer to cast
    /// \return the statically casted pointer
    template <typename To,
              typename From,
              typename = std::enable_if_t<std::is_pointer<To>::value &&
                                          std::is_pointer<From>::value>>
    constexpr auto pointer_cast(From ptr) noexcept -> To;

    /// \brief Performs a cast between types by copying bytes from \p from to
    ///        the new type.
    ///
    /// \note This cast does not participate in overload resolution unless
    ///       std::is_trivially_copyable<To>::value and
    ///       std::is_trivially_copyable<From>::value are both true
    ///
    /// \tparam To the type to cast to
    /// \param from the value to convert
    /// \return the converted value
    template <typename To,
              typename From,
              typename = std::enable_if_t<
                (sizeof(To) == sizeof(From)) &&
                std::is_trivially_copyable<From>::value &&
                std::is_trivially_copyable<To>::value>>
    auto bit_cast(const From& from) noexcept -> To;

    //-------------------------------------------------------------------------
    // Signed / Unsigned Conversions
    //-------------------------------------------------------------------------

    /// \{
    /// \brief Performs an explicit cast to the signed variant of the input.
    ///
    /// If the input is an enum, it is first converted to the underlying
    /// integral type first before being converted.
    /// If the type is already signed, no conversion occurs
    ///
    /// \param from the integral type being converted
    /// \return a signed integral
    template <typename Integral,
              typename = std::enable_if_t<std::is_integral<Integral>::value>>
    constexpr auto as_signed(Integral from)
      noexcept -> std::make_signed_t<Integral>;
    template <typename Enum,
              typename = std::enable_if_t<std::is_enum<Enum>::value>>
    constexpr auto as_signed(Enum from)
      noexcept -> std::make_signed_t<std::underlying_type_t<Enum>>;
    /// \}

    //-------------------------------------------------------------------------

    /// \{
    /// \brief Performs an explicit cast to the unsigned variant of the input.
    ///
    /// If the input is an enum, it is first converted to the underlying
    /// integral type first before being converted.
    /// If the type is already unsigned, no conversion occurs
    ///
    /// \param from the integral type being converted
    /// \return an unsigned integral
    template <typename Integral,
              typename = std::enable_if_t<std::is_integral<Integral>::value>>
    constexpr auto as_unsigned(Integral from)
      noexcept -> std::make_unsigned_t<Integral>;
    template <typename Enum,
              typename = std::enable_if_t<std::is_enum<Enum>::value>>
    constexpr auto as_unsigned(Enum from)
      noexcept -> std::make_unsigned_t<std::underlying_type_t<Enum>>;
    /// \}

  } // inline namespace casts
} // namespace alloy::core

//=============================================================================
// definitions : non-member functions : casts
//=============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wconversion)
ALLOY_COMPILER_CLANG_DIAGNOSTIC_IGNORE(-Wimplicit-int-conversion)
ALLOY_COMPILER_MSVC_DIAGNOSTIC_IGNORE(4244) // ignore loss-of-precision warning

template<typename To, typename From, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::casts::implicit_cast(From&& from)
  noexcept(std::is_nothrow_constructible<To, From>::value) -> To
{
  return std::forward<From>(from);
}
ALLOY_COMPILER_DIAGNOSTIC_POP()

template<typename To, typename From, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::casts::narrow_cast(From from)
  noexcept -> To
{
  return static_cast<To>(from);
}

template<typename To, typename From, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::casts::checked_narrow_cast(From from)
  noexcept -> To
{
  auto to = static_cast<To>(from);

  ALLOY_ASSERT(
    static_cast<From>(to) == from,
    "narrow_cast: narrowing error"
  );
  if constexpr (std::is_signed_v<To> != std::is_signed_v<From>) {
    ALLOY_ASSERT(
      (to < To{}) == (from < From{}),
      "narrow_cast: narrowing error"
    );
  }

  return to;
}

template <typename To, typename From, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::casts::pointer_cast(From ptr)
  noexcept -> To
{
  ALLOY_ASSERT(
    static_cast<To>(ptr) == dynamic_cast<To>(ptr),
    "pointer_cast: destination type is not reachable through dynamic_cast"
  );

  return static_cast<To>(ptr);
}

template<typename To, typename From, typename>
ALLOY_FORCE_INLINE
auto alloy::core::casts::bit_cast(const From& from)
  noexcept -> To
{
    To to;
    std::memcpy(&to, &from, sizeof(To));
    return to;
}

//-----------------------------------------------------------------------------
// Signed / Unsigned Conversions
//-----------------------------------------------------------------------------

template <typename Integral, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::casts::as_signed(Integral from)
  noexcept -> std::make_signed_t<Integral>
{
  using signed_type = std::make_signed_t<Integral>;

  return static_cast<signed_type>(from);
}

template <typename Enum, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::casts::as_signed(Enum from)
  noexcept -> std::make_signed_t<std::underlying_type_t<Enum>>
{
  using integral_type = std::underlying_type_t<Enum>;
  using signed_type = std::make_signed_t<integral_type>;

  return static_cast<signed_type>(from);
}

//-----------------------------------------------------------------------------

template <typename Integral, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::casts::as_unsigned(Integral from)
  noexcept -> std::make_unsigned_t<Integral>
{
  using unsigned_type = std::make_unsigned_t<Integral>;

  return static_cast<unsigned_type>(from);
}

template <typename Enum, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::casts::as_unsigned(Enum from)
  noexcept -> std::make_unsigned_t<std::underlying_type_t<Enum>>
{
  using integral_type = std::underlying_type_t<Enum>;
  using unsigned_type = std::make_unsigned_t<integral_type>;

  return static_cast<unsigned_type>(from);
}

#endif /* ALLOY_CORE_UTILITIES_CASTS_HPP */
