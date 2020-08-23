////////////////////////////////////////////////////////////////////////////////
/// \file unaligned.hpp
///
/// \brief This header defines a series of unaligned-memory access utilities
////////////////////////////////////////////////////////////////////////////////

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

#ifndef ALLOY_CORE_MEMORY_UNALIGNED_HPP
#define ALLOY_CORE_MEMORY_UNALIGNED_HPP

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/precision/real.hpp"
#include "alloy/core/containers/span.hpp"
#include "alloy/core/assert.hpp"

#include <cstdint> // std::uint8_t, std::uint16_t
#include <cstring> // std::memcpy
#include <memory>  // std::addressof
#include <type_traits> // std::is_trivially_copyable_v

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class for managing unaligned access
  /////////////////////////////////////////////////////////////////////////////
  struct unaligned
  {
    unaligned() = delete;
    ~unaligned() = delete;

    //-------------------------------------------------------------------------
    // Storing
    //-------------------------------------------------------------------------

    /// \brief Stores a trivially copyable object into \p destination, ignoring
    ///        any alignment requirements
    ///
    /// \pre `destination.size() >= sizeof(val)`
    /// \param destination the buffer to write \p val to
    /// \param val the value to write
    template <typename T>
    static auto store_object(span<std::byte> destination, const T& val) noexcept -> void;

    /// \{
    /// \brief Stores an integral value into the buffer \p destination
    ///
    /// \pre `destination.size() >= sizeof(val)`
    /// \param destination the buffer to write \p val to
    /// \param val the value to write
    static auto store_uint8(span<std::byte> destination, std::uint8_t val) noexcept -> void;
    static auto store_uint16(span<std::byte> destination, std::uint16_t val) noexcept -> void;
    static auto store_uint32(span<std::byte> destination, std::uint32_t val) noexcept -> void;
    static auto store_uint64(span<std::byte> destination, std::uint64_t val) noexcept -> void;
    static auto store_int8(span<std::byte> destination, std::int8_t val) noexcept -> void;
    static auto store_int16(span<std::byte> destination, std::int16_t val) noexcept -> void;
    static auto store_int32(span<std::byte> destination, std::int32_t val) noexcept -> void;
    static auto store_int64(span<std::byte> destination, std::int64_t val) noexcept -> void;
    /// \}

    /// \{
    /// \brief Stores a floating point value into the buffer \p destination
    ///
    /// \pre `destination.size() >= sizeof(val)`
    /// \param destination the buffer to write \p val to
    /// \param val the floating point value to write
    static auto store_float(span<std::byte> destination, float val) noexcept -> void;
    static auto store_double(span<std::byte> destination, double val) noexcept -> void;
    static auto store_real(span<std::byte> destination, real val) noexcept -> void;
    /// \}

    //-------------------------------------------------------------------------
    // Loading
    //-------------------------------------------------------------------------

    /// \brief Loads a trivially copyable object T from the \p source buffer,
    ///        disregarding any alignment requirements
    ///
    /// \pre `source.size() >= sizeof(T)`
    /// \tparam T the type of object to load
    /// \param source the buffer to read from
    /// \return the read value
    template <typename T>
    static auto load_object(span<const std::byte> source) noexcept -> T;

    /// \{
    /// \brief Loads an integral value from the \p source buffer,
    ///        disregarding any alignment requirements
    ///
    /// \pre `source.size() >= sizeof(T)`
    /// \param source the buffer to read from
    /// \return the read integral value
    static auto load_uint8(span<const std::byte> source) noexcept -> std::uint8_t;
    static auto load_uint16(span<const std::byte> source) noexcept -> std::uint16_t;
    static auto load_uint32(span<const std::byte> source) noexcept -> std::uint32_t;
    static auto load_uint64(span<const std::byte> source) noexcept -> std::uint64_t;
    static auto load_int8(span<const std::byte> source) noexcept -> std::int8_t;
    static auto load_int16(span<const std::byte> source) noexcept -> std::int16_t;
    static auto load_int32(span<const std::byte> source) noexcept -> std::int32_t;
    static auto load_int64(span<const std::byte> source) noexcept -> std::int64_t;
    /// \}

    /// \{
    /// \brief Loads a floating-point value from the \p source buffer,
    ///        disregarding any alignment requirements
    ///
    /// \pre `source.size() >= sizeof(T)`
    /// \param source the buffer to read from
    /// \return the read floating point value
    static auto load_float(span<const std::byte> source) noexcept -> float;
    static auto load_double(span<const std::byte> source) noexcept -> double;
    static auto load_real(span<const std::byte> source) noexcept -> real;
    /// \}
  };

} // namespace alloy::core

//-----------------------------------------------------------------------------
// Storing
//-----------------------------------------------------------------------------

template<typename T>
inline auto
  alloy::core::unaligned::store_object(span<std::byte> destination, const T& val)
  noexcept -> void
{
  static_assert(std::is_trivially_copyable_v<T>);

  ALLOY_ASSERT(destination.size() >= sizeof(T));

  std::memcpy(destination.data(), std::addressof(val), sizeof(T));
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::store_uint8(span<std::byte> destination, std::uint8_t val)
  noexcept -> void
{
  store_object(destination, val);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::store_uint16(span<std::byte> destination, std::uint16_t val)
  noexcept -> void
{
  store_object(destination, val);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::store_uint32(span<std::byte> destination, std::uint32_t val)
  noexcept -> void
{
  store_object(destination, val);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::store_uint64(span<std::byte> destination, std::uint64_t val)
  noexcept -> void
{
  store_object(destination, val);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::store_int8(span<std::byte> destination, std::int8_t val)
  noexcept -> void
{
  store_object(destination, val);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::store_int16(span<std::byte> destination, std::int16_t val)
  noexcept -> void
{
  store_object(destination, val);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::store_int32(span<std::byte> destination, std::int32_t val)
  noexcept -> void
{
  store_object(destination, val);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::store_int64(span<std::byte> destination, std::int64_t val)
  noexcept -> void
{
  store_object(destination, val);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::store_float(span<std::byte> destination, float val)
  noexcept -> void
{
  store_object(destination, val);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::store_double(span<std::byte> destination, double val)
  noexcept -> void
{
  store_object(destination, val);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::store_real(span<std::byte> destination, real val)
  noexcept -> void
{
  store_object(destination, val);
}

//-----------------------------------------------------------------------------
// Loading
//-----------------------------------------------------------------------------

template<typename T>
inline auto
  alloy::core::unaligned::load_object(span<const std::byte> source)
  noexcept -> T
{
  static_assert(std::is_trivially_copyable_v<T>);
  static_assert(std::is_default_constructible_v<T>);

  ALLOY_ASSERT(source.size() >= sizeof(T));

  // This could also be conditionally defined to use `aligned_storage` if
  // `result` is not default-constructible; but this seems to be overkill for
  // the intended use of this function.
  // This is something to consider for a future enhancement if this results in
  // problems in practice.

  auto result = T{};
  std::memcpy(std::addressof(result), source.data(), sizeof(T));
  return result;
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::load_uint8(span<const std::byte> source)
  noexcept -> std::uint8_t
{
  return load_object<std::uint8_t>(source);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::load_uint16(span<const std::byte> source)
  noexcept -> std::uint16_t
{
  return load_object<std::uint16_t>(source);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::load_uint32(span<const std::byte> source)
  noexcept -> std::uint32_t
{
  return load_object<std::uint32_t>(source);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::load_uint64(span<const std::byte> source)
  noexcept -> std::uint64_t
{
  return load_object<std::uint64_t>(source);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::load_int8(span<const std::byte> source)
  noexcept -> std::int8_t
{
  return load_object<std::int8_t>(source);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::load_int16(span<const std::byte> source)
  noexcept -> std::int16_t
{
  return load_object<std::int16_t>(source);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::load_int32(span<const std::byte> source)
  noexcept -> std::int32_t
{
  return load_object<std::int32_t>(source);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::load_int64(span<const std::byte> source)
  noexcept -> std::int64_t
{
  return load_object<std::int64_t>(source);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::load_float(span<const std::byte> source)
  noexcept -> float
{
  return load_object<float>(source);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::load_double(span<const std::byte> source)
  noexcept -> double
{
  return load_object<double>(source);
}

ALLOY_FORCE_INLINE auto
  alloy::core::unaligned::load_real(span<const std::byte> source)
  noexcept -> real
{
  return load_object<real>(source);
}

#endif /* ALLOY_CORE_MEMORY_UNALIGNED_HPP */
