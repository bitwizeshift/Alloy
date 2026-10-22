// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file serializer.hpp
///
/// \brief This header contains the serializer customization point, along with
///        the utility for serializing objects to any writer
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_IO_SERIALIZER_HPP
#define ALLOY_CORE_IO_SERIALIZER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/io/writer.hpp"

#include "alloy/core/utilities/result.hpp"

#include <array>        // std::array
#include <cstddef>      // std::byte
#include <cstdint>      // std::uint8_t, std::int8_t, ...
#include <span>         // std::span, std::as_bytes
#include <system_error> // std::error_code
#include <type_traits>  // std::is_trivially_copyable_v, std::is_enum_v, ...

namespace alloy::core {

  //===========================================================================
  // struct : serializer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A customization point for allowing template specializations to
  ///        enable object serialization to writers
  ///
  /// This is a user customization point that allows consumers to specialize
  /// this structure for user-defined types, so that serialize may interpret
  /// how to serialize these types.
  ///
  /// \tparam T the type to serialize
  /////////////////////////////////////////////////////////////////////////////
  template <typename T>
  struct serializer
  {
    /// \brief Writes the arbitrary object \p in to the writer \p destination
    ///
    /// \param destination the writer to write to
    /// \param in the value to write
    /// \return `void` on success
    static auto serialize(writer& destination, const T& in)
      noexcept -> result<void,std::error_code>;
  };

  //===========================================================================
  // non-member functions : struct : serializer
  //===========================================================================

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Writes an object to the writer \p destination
  ///
  /// This uses the serializer<T> customization point to write arbitrary
  /// types.
  ///
  /// \note Writing objects directly to the writer is meant to not only
  ///       provide a means of writing stronger data-types, but also provide
  ///       a portable API for writing to writers on systems that might
  ///       otherwise have different endiannesses. Using 'serialize'
  ///       guarantees that integral values will be written in a consistent
  ///       manner that can be read across systems that might have different
  ///       endiannesses.
  ///
  /// \param destination the writer to write to
  /// \param v the value to write
  /// \return void on success
  template <typename T>
  auto serialize(writer& destination, const T& v)
    noexcept -> result<void,std::error_code>;

} // namespace alloy::core

//=============================================================================
// inline definitions : struct : serializer<T>
//=============================================================================

template <typename T>
inline
auto alloy::core::serializer<T>::serialize(writer& destination, const T& in)
  noexcept -> result<void,std::error_code>
{
  static_assert(
    std::is_trivially_copyable_v<T>,
    "Only trivially copyable types are serializable by default. "
    "Please define a custom specialization instead"
  );

  // Handle 'enum' as a special case; should be treated like integers
  if constexpr (std::is_enum_v<T>) {
    using underlying_type = std::underlying_type_t<T>;

    return serializer<underlying_type>::serialize(
      destination,
      static_cast<underlying_type>(in)
    );
  } else {
    return write_all(destination, std::as_bytes(std::span{&in, 1u}));
  }
}

//=============================================================================
// inline definitions : specializations : struct : serializer
//=============================================================================

//-----------------------------------------------------------------------------
// bool
//-----------------------------------------------------------------------------

template <>
struct alloy::core::serializer<bool>
{
  static auto serialize(writer& destination, bool in)
    noexcept -> result<void,std::error_code>
  {
    // Written as a single 0 or 1 byte, since the object representation of
    // 'bool' is implementation-defined
    const auto bytes = std::array<std::byte,1>{
      in ? std::byte{1} : std::byte{0},
    };
    return write_all(destination, bytes);
  }
};

//-----------------------------------------------------------------------------
// std::uint8_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::serializer<std::uint8_t>
{
  static auto serialize(writer& destination, std::uint8_t in)
    noexcept -> result<void,std::error_code>
  {
    const auto bytes = std::array<std::byte,sizeof(std::uint8_t)>{
      static_cast<std::byte>(in),
    };
    return write_all(destination, bytes);
  }
};

//-----------------------------------------------------------------------------
// std::int8_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::serializer<std::int8_t>
{
  static auto serialize(writer& destination, std::int8_t in)
    noexcept -> result<void,std::error_code>
  {
    return serializer<std::uint8_t>::serialize(
      destination,
      static_cast<std::uint8_t>(in)
    );
  }
};

//-----------------------------------------------------------------------------
// std::uint16_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::serializer<std::uint16_t>
{
  static auto serialize(writer& destination, std::uint16_t in)
    noexcept -> result<void,std::error_code>
  {
    // Create an array of the byte-sequence in a consistent order
    const auto bytes = std::array<std::byte,sizeof(std::uint16_t)>{
      static_cast<std::byte>((0xff00 & in) >> 8),
      static_cast<std::byte>((0x00ff & in)),
    };
    return write_all(destination, bytes);
  }
};

//-----------------------------------------------------------------------------
// std::int16_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::serializer<std::int16_t>
{
  static auto serialize(writer& destination, std::int16_t in)
    noexcept -> result<void,std::error_code>
  {
    return serializer<std::uint16_t>::serialize(
      destination,
      static_cast<std::uint16_t>(in)
    );
  }
};

//-----------------------------------------------------------------------------
// std::uint32_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::serializer<std::uint32_t>
{
  static auto serialize(writer& destination, std::uint32_t in)
    noexcept -> result<void,std::error_code>
  {
    // Create an array of the byte-sequence in a consistent order
    const auto bytes = std::array<std::byte,sizeof(std::uint32_t)>{
      static_cast<std::byte>((0xff000000 & in) >> 24),
      static_cast<std::byte>((0x00ff0000 & in) >> 16),
      static_cast<std::byte>((0x0000ff00 & in) >> 8),
      static_cast<std::byte>((0x000000ff & in)),
    };
    return write_all(destination, bytes);
  }
};

//-----------------------------------------------------------------------------
// std::int32_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::serializer<std::int32_t>
{
  static auto serialize(writer& destination, std::int32_t in)
    noexcept -> result<void,std::error_code>
  {
    return serializer<std::uint32_t>::serialize(
      destination,
      static_cast<std::uint32_t>(in)
    );
  }
};

//-----------------------------------------------------------------------------
// std::uint64_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::serializer<std::uint64_t>
{
  static auto serialize(writer& destination, std::uint64_t in)
    noexcept -> result<void,std::error_code>
  {
    // Create an array of the byte-sequence in a consistent order
    const auto bytes = std::array<std::byte,sizeof(std::uint64_t)>{
      static_cast<std::byte>((0xff00000000000000 & in) >> 56),
      static_cast<std::byte>((0x00ff000000000000 & in) >> 48),
      static_cast<std::byte>((0x0000ff0000000000 & in) >> 40),
      static_cast<std::byte>((0x000000ff00000000 & in) >> 32),
      static_cast<std::byte>((0x00000000ff000000 & in) >> 24),
      static_cast<std::byte>((0x0000000000ff0000 & in) >> 16),
      static_cast<std::byte>((0x000000000000ff00 & in) >> 8),
      static_cast<std::byte>((0x00000000000000ff & in)),
    };
    return write_all(destination, bytes);
  }
};

//-----------------------------------------------------------------------------
// std::int64_t
//-----------------------------------------------------------------------------

template <>
struct alloy::core::serializer<std::int64_t>
{
  static auto serialize(writer& destination, std::int64_t in)
    noexcept -> result<void,std::error_code>
  {
    return serializer<std::uint64_t>::serialize(
      destination,
      static_cast<std::uint64_t>(in)
    );
  }
};

//=============================================================================
// inline definitions : non-member functions : struct : serializer
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <typename T>
inline
auto alloy::core::serialize(writer& destination, const T& v)
  noexcept -> result<void,std::error_code>
{
  static_assert(
    noexcept(serializer<T>::serialize(destination, v)),
    "user-defined serializer<T>::serialize requests must be non-throwing. "
    "Please ensure your implementation translates all errors to an "
    "result<void,std::error_code> type and returns them accordingly, then "
    "mark the function 'noexcept'."
  );

  return serializer<T>::serialize(destination, v);
}

#endif /* ALLOY_CORE_IO_SERIALIZER_HPP */
