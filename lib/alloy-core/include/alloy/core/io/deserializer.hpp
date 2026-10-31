// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file deserializer.hpp
///
/// \brief This header contains the deserializer customization point, along
///        with the utility for deserializing objects from any reader
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_IO_DESERIALIZER_HPP
#define ALLOY_CORE_IO_DESERIALIZER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/io/reader.hpp"
#include "alloy/core/utilities/result.hpp"

#include <array>        // std::array
#include <cstddef>      // std::byte
#include <cstdint>      // std::uint8_t, std::int8_t, ...
#include <span>         // std::span, std::as_writable_bytes
#include <system_error> // std::error_code
#include <type_traits>  // std::is_trivially_copyable_v, std::is_enum_v, ...

namespace alloy::core {

  //===========================================================================
  // struct : deserializer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A customization point for allowing template specializations to
  ///        enable object deserialization from readers
  ///
  /// This is a user customization point that allows consumers to specialize
  /// this structure for user-defined types, so that deserialize may interpret
  /// how to deserialize these types.
  ///
  /// \tparam T the type to deserialize
  /////////////////////////////////////////////////////////////////////////////
  template <typename T>
  struct deserializer
  {
    /// \brief Reads an arbitrary object from the reader \p source
    ///
    /// \param source the reader to read from
    /// \return the object read on success
    static auto deserialize(reader& source)
      noexcept -> result<T,std::error_code>;
  };

  //===========================================================================
  // non-member functions : struct : deserializer
  //===========================================================================

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Reads an object from the reader \p source
  ///
  /// This uses the deserializer<T> customization point to read arbitrary
  /// types.
  ///
  /// \note Reading objects directly from the reader is meant to not only
  ///       provide a means of reading stronger data-types, but also provide
  ///       a portable API for reading from readers on systems that might
  ///       otherwise have different endiannesses. Using 'deserialize'
  ///       guarantees that integral values read will be written in a
  ///       consistent manner that can be read across systems that might have
  ///       different endiannesses.
  ///
  /// \tparam T the type to read
  /// \param source the reader to read from
  /// \return T on success
  template <typename T>
  auto deserialize(reader& source) noexcept -> result<T,std::error_code>;

} // namespace alloy::core

//=============================================================================
// inline definitions : struct : deserializer<T>
//=============================================================================

template <typename T>
inline
auto alloy::core::deserializer<T>::deserialize(reader& source)
  noexcept -> result<T,std::error_code>
{
  static_assert(
    std::is_trivially_copyable_v<T> &&
    std::is_trivially_default_constructible_v<T>,
    "Only trivially copyable and default-constructible types are "
    "deserializable by default. "
    "Please define a custom specialization instead"
  );

  // Handle 'enum' as a special case; should be treated like integers
  if constexpr (std::is_enum_v<T>) {
    using underlying_type = std::underlying_type_t<T>;

    auto result = deserializer<underlying_type>::deserialize(source);
    if (!result) {
      return fail(result.error());
    }
    return static_cast<T>(*result);
  } else {
    auto object = T{};
    auto result = read_exact(
      source,
      std::as_writable_bytes(std::span{&object, 1u})
    );
    if (!result) {
      return fail(result.error());
    }

    return object;
  }
}

//=============================================================================
// inline definitions : specializations : struct : deserializer
//=============================================================================

//-----------------------------------------------------------------------------
// bool
//-----------------------------------------------------------------------------

template <>
struct alloy::core::deserializer<bool>
{
  static auto deserialize(reader& source)
    noexcept -> result<bool,std::error_code>
  {
    auto bytes = std::array<std::byte,1>{};
    auto result = read_exact(source, bytes);
    if (!result) {
      return fail(result.error());
    }

    // Any non-zero byte is 'true', since reading a byte that is neither 0 nor
    // 1 directly into a 'bool' is undefined behavior
    return bytes[0] != std::byte{0};
  }
};

//-----------------------------------------------------------------------------
// std::uint8_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::deserializer<std::uint8_t>
{
  static auto deserialize(reader& source)
    noexcept -> result<std::uint8_t,std::error_code>
  {
    auto bytes = std::array<std::byte,sizeof(std::uint8_t)>{};
    auto result = read_exact(source, bytes);
    if (!result) {
      return fail(result.error());
    }

    return static_cast<std::uint8_t>(bytes[0]);
  }
};

//-----------------------------------------------------------------------------
// std::int8_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::deserializer<std::int8_t>
{
  static auto deserialize(reader& source)
    noexcept -> result<std::int8_t,std::error_code>
  {
    auto r = deserializer<std::uint8_t>::deserialize(source);

    if (!r) {
      return fail(r.error());
    }
    return static_cast<std::int8_t>(*r);
  }
};

//-----------------------------------------------------------------------------
// std::uint16_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::deserializer<std::uint16_t>
{
  static auto deserialize(reader& source)
    noexcept -> result<std::uint16_t,std::error_code>
  {
    auto bytes = std::array<std::byte,sizeof(std::uint16_t)>{};
    auto result = read_exact(source, bytes);
    if (!result) {
      return fail(result.error());
    }

    // Rebuild the result from the byte sequence
    auto object = std::uint16_t(
      static_cast<std::uint16_t>(bytes[0]) << 8 |
      static_cast<std::uint16_t>(bytes[1])
    );
    return object;
  }
};

//-----------------------------------------------------------------------------
// std::int16_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::deserializer<std::int16_t>
{
  static auto deserialize(reader& source)
    noexcept -> result<std::int16_t,std::error_code>
  {
    auto r = deserializer<std::uint16_t>::deserialize(source);

    if (!r) {
      return fail(r.error());
    }
    return static_cast<std::int16_t>(*r);
  }
};

//-----------------------------------------------------------------------------
// std::uint32_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::deserializer<std::uint32_t>
{
  static auto deserialize(reader& source)
    noexcept -> result<std::uint32_t,std::error_code>
  {
    auto bytes = std::array<std::byte,sizeof(std::uint32_t)>{};
    auto result = read_exact(source, bytes);
    if (!result) {
      return fail(result.error());
    }

    // Rebuild the result from the byte sequence
    auto object = std::uint32_t(
      static_cast<std::uint32_t>(bytes[0]) << 24 |
      static_cast<std::uint32_t>(bytes[1]) << 16 |
      static_cast<std::uint32_t>(bytes[2]) << 8  |
      static_cast<std::uint32_t>(bytes[3])
    );
    return object;
  }
};

//-----------------------------------------------------------------------------
// std::int32_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::deserializer<std::int32_t>
{
  static auto deserialize(reader& source)
    noexcept -> result<std::int32_t,std::error_code>
  {
    auto r = deserializer<std::uint32_t>::deserialize(source);

    if (!r) {
      return fail(r.error());
    }
    return static_cast<std::int32_t>(*r);
  }
};

//-----------------------------------------------------------------------------
// std::uint64_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::deserializer<std::uint64_t>
{
  static auto deserialize(reader& source)
    noexcept -> result<std::uint64_t,std::error_code>
  {
    auto bytes = std::array<std::byte,sizeof(std::uint64_t)>{};
    auto result = read_exact(source, bytes);
    if (!result) {
      return fail(result.error());
    }

    // Rebuild the result from the byte sequence
    auto object = std::uint64_t(
      static_cast<std::uint64_t>(bytes[0]) << 56 |
      static_cast<std::uint64_t>(bytes[1]) << 48 |
      static_cast<std::uint64_t>(bytes[2]) << 40 |
      static_cast<std::uint64_t>(bytes[3]) << 32 |
      static_cast<std::uint64_t>(bytes[4]) << 24 |
      static_cast<std::uint64_t>(bytes[5]) << 16 |
      static_cast<std::uint64_t>(bytes[6]) << 8  |
      static_cast<std::uint64_t>(bytes[7])
    );
    return object;
  }
};

//-----------------------------------------------------------------------------
// std::int64_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::deserializer<std::int64_t>
{
  static auto deserialize(reader& source)
    noexcept -> result<std::int64_t,std::error_code>
  {
    auto r = deserializer<std::uint64_t>::deserialize(source);

    if (!r) {
      return fail(r.error());
    }
    return static_cast<std::int64_t>(*r);
  }
};

//=============================================================================
// inline definitions : non-member functions : struct : deserializer
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <typename T>
inline
auto alloy::core::deserialize(reader& source)
  noexcept -> result<T,std::error_code>
{
  static_assert(
    noexcept(deserializer<T>::deserialize(source)),
    "user-defined deserializer<T>::deserialize requests must be "
    "non-throwing. Please ensure your implementation translates all errors "
    "to an result<T,std::error_code> type and returns them accordingly, then "
    "mark the function 'noexcept'."
  );

  return deserializer<T>::deserialize(source);
}

#endif /* ALLOY_CORE_IO_DESERIALIZER_HPP */
