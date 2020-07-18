/*****************************************************************************
 * \file pointer_util.hpp
 *
 * \brief This header contains various utilities for general pointer_util
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_EXTRA_ALLOCATORS_POINTER_UTIL_HPP
#define ALLOY_EXTRA_ALLOCATORS_POINTER_UTIL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/assert.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/aligned_storage.hpp"

#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <cstdint>     // std::uint8_t, std::uint16_t, etc
#include <cstring>     // std::memcpy
#include <memory>      // std::addressof
#include <type_traits> // std::is_trivially_copyable

namespace alloy::extra {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class containing pointer utilities for aligning and
  ///        storing to pointers
  /////////////////////////////////////////////////////////////////////////////
  struct pointer_util
  {
    ~pointer_util() = delete;

    //-------------------------------------------------------------------------
    // Alignment Checking
    //-------------------------------------------------------------------------

    /// \brief Determines whether a given value is a power of two (used in
    ///        alignment checks)
    ///
    /// \param x the value to check
    /// \return \c true if the value is a power-of-two
    static constexpr bool is_power_of_two(std::size_t x) noexcept;

    /// \brief Gets the alignment of the pointer
    ///
    /// \param ptr the pointer
    /// \return the alignment of the pointer
    static std::size_t align_of(const void* ptr) noexcept;

    //-------------------------------------------------------------------------
    // Offset Calculation
    //-------------------------------------------------------------------------

    /// \brief Calculates the forward-offset required to align \p ptr to
    ///        \p align alignment
    ///
    /// \param p The pointer to align
    /// \param align The alignment to calculate
    /// \return the amount of bytes required to align to the next aligned
    ///         address
    static std::size_t align_forward_offset(const void* p,
                                            std::size_t align) noexcept;

    /// \brief Calculates the forward-offset required to align \p ptr to
    ///        \p align alignment
    ///
    /// \param p The pointer to align
    /// \param align The alignment to calculate
    /// \param offset The amount of offset to leave prior to alignment
    /// \return the amount of bytes required to align to the next aligned
    ///         address
    static std::size_t align_forward_offset(const void* p,
                                            std::size_t align,
                                            std::size_t offset) noexcept;

    /// \brief Calculates the backward-offset required to align \p ptr to
    ///        \p align alignment
    ///
    /// \param ptr The pointer to align
    /// \param align The alignment to calculate
    /// \return the amount of bytes required to align to the next aligned
    ///         address
    static std::size_t align_backward_offset(const void* ptr,
                                             std::size_t align) noexcept;

    /// \brief Calculates the backward-offset required to align \p ptr to
    ///        \p align alignment
    ///
    /// \param ptr The pointer to align
    /// \param align The alignment to calculate
    /// \param offset The amount of offset to leave prior to alignment
    /// \return the amount of bytes required to align to the next aligned
    ///         address
    static std::size_t align_backward_offset(const void* ptr,
                                             std::size_t align,
                                             std::size_t offset) noexcept;

    //-------------------------------------------------------------------------
    // Align
    //-------------------------------------------------------------------------

    /// \{
    /// \brief Aligns memory to a higher memory address at an alignment boundary
    ///        of \p align
    ///
    /// \param ptr the pointer to align
    /// \param align the alignment
    /// \param adjust [out] the amount the pointer is allocated by
    /// \return the aligned pointer
    static void* align_forward(void* ptr,
                               std::size_t align,
                               core::not_null<std::size_t*> adjust) noexcept;
    static void* align_forward(void* ptr,
                               std::size_t align) noexcept;
    /// \}

    //-------------------------------------------------------------------------

    /// \{
    /// \brief Aligns memory to a lower memory address at an alignment boundary
    ///        of \p align
    ///
    /// \param p the pointer to align
    /// \param align the alignment
    /// \param adjust [out] the amount the pointer is allocated by
    /// \return the aligned pointer
    static void* align_backward(void* p,
                                std::size_t align,
                                core::not_null<std::size_t*> adjust) noexcept;
    static void* align_backward(void* p,
                                std::size_t align) noexcept;
    /// \}

    //-------------------------------------------------------------------------
    // Align with Offset
    //-------------------------------------------------------------------------

    /// \{
    /// \brief Aligns memory to a higher memory address at an alignment boundary
    ///        of \p align, offset by \p offset
    ///
    /// \param p the pointer to align
    /// \param align the alignment
    /// \param offset the amount to offset the alignment
    /// \param adjust [out] the amount the pointer is allocated by
    /// \return the aligned, offset pointer
    static void* offset_align_forward(void* p,
                                      std::size_t align,
                                      std::size_t offset,
                                      core::not_null<std::size_t*> adjust) noexcept;
    static void* offset_align_forward(void* p,
                                      std::size_t align,
                                      std::size_t offset) noexcept;
    /// \}

    //-------------------------------------------------------------------------

    /// \{
    /// \brief Aligns memory to a lower memory address at an alignment boundary
    ///        of \p align, offset by \p offset
    ///
    /// \param p the pointer to align
    /// \param align the alignment
    /// \param offset the amount to offset the alignment
    /// \param adjust [out] the amount the pointer is allocated by
    /// \return the aligned, offset pointer.
    static void* offset_align_backward(void* p,
                                       std::size_t align,
                                       std::size_t offset,
                                       core::not_null<std::size_t*> adjust) noexcept;
    static void* offset_align_backward(void* p,
                                       std::size_t align,
                                       std::size_t offset) noexcept;
    /// \}

    //-------------------------------------------------------------------------
    // Deltas
    //-------------------------------------------------------------------------

    /// \brief Calculates the distance between two pointers
    ///
    /// The distance between to pointers is the absolute value of the difference
    /// between two pointers
    ///
    /// \param lhs the left pointer
    /// \param rhs the right pointer
    /// \return the distance
    static std::size_t distance(const void* lhs, const void* rhs) noexcept;

    /// \brief Calculates the difference between two pointers
    ///
    /// The difference is identical to \c lhs - \c rhs
    ///
    /// \param lhs the left pointer
    /// \param rhs the right pointer
    /// \return the result of \c lhs - \c rhs
    static std::ptrdiff_t difference(const void* lhs,
                                     const void* rhs) noexcept;

    /// \{
    /// \brief Adjusts a pointer \p p by \p bytes
    ///
    /// \param p the pointer to adjust
    /// \param bytes the bytes to adjust by
    /// \return the pointer adjusted by \p bytes
    static void* advance(void* p, std::ptrdiff_t bytes) noexcept;
    static const void* advance(const void* p, std::ptrdiff_t bytes) noexcept;
    /// \}

    //-------------------------------------------------------------------------
    // Storing
    //-------------------------------------------------------------------------

    /// \brief Stores an arbitrary type \p T into unaligned memory
    ///
    /// \pre \c std::is_trivially_copyable<T>::value is \c true
    ///
    /// \param p the memory to store into
    /// \param val the value to store
    template<typename T>
    static void store_unaligned(void* p, const T& val) noexcept;

    //-------------------------------------------------------------------------
    // Loading
    //-------------------------------------------------------------------------

    /// \brief Loads an arbitrary type \p T from unaligned memory
    ///
    /// \note Only types that don't contain pointers that point inside the
    ///       loaded type can be safely loaded. Otherwise, the pointers will be
    ///       pointing to potentially unsafe memory. Ideally, this function
    ///       should only be used to load fundamental types, or simple
    ///       pod/aggregate types.
    ///
    /// \pre \c std::is_trivially_copyable<T>::value is \c true
    ///
    /// \param p pointer to the memory to load from
    /// \return the type \p T
    template<typename T>
    static T load_unaligned(const void* p) noexcept;

    //-------------------------------------------------------------------------

    /// \brief Loads an unaligned std::int8_t from the memory pointed at
    ///        by \p p
    ///
    /// \param p pointer to the memory to load from
    /// \return the loaded std::int8_t
    static std::int8_t load_unaligned_int8(const void* p) noexcept;

    /// \brief Loads an unaligned std::uint8_t from the memory pointed at
    ///        by \p p
    ///
    /// \param p pointer to the memory to load from
    /// \return the loaded std::uint8_t
    static std::uint8_t load_unaligned_uint8(const void* p) noexcept;

    //-------------------------------------------------------------------------

    /// \brief Loads an unaligned std::int16_t from the memory pointed at
    ///        by \p p
    ///
    /// \param p pointer to the memory to load from
    /// \return the loaded std::int16_t
    static std::int16_t load_unaligned_int16(const void* p) noexcept;

    /// \brief Loads an unaligned std::uint16_t from the memory pointed at
    ///        by \p p
    ///
    /// \param p pointer to the memory to load from
    /// \return the loaded std::uint16_t
    static std::uint16_t load_unaligned_uint16(const void* p) noexcept;

    //-------------------------------------------------------------------------

    /// \brief Loads an unaligned std::int32_t from the memory pointed at
    ///        by \p p
    ///
    /// \param p pointer to the memory to load from
    /// \return the loaded std::int32_t
    static std::int32_t load_unaligned_int32(const void* p) noexcept;

    /// \brief Loads an unaligned std::uint32_t from the memory pointed at
    ///        by \p p
    ///
    /// \param p pointer to the memory to load from
    /// \return the loaded std::uint32_t
    static std::uint32_t load_unaligned_uint32(const void* p) noexcept;

    //-------------------------------------------------------------------------

    /// \brief Loads an unaligned std::int64_t from the memory pointed at
    ///        by \p p
    ///
    /// \param p pointer to the memory to load from
    /// \return the loaded std::int64_t
    static std::int64_t load_unaligned_int64(const void* p) noexcept;

    /// \brief Loads an unaligned std::uint64_t from the memory pointed at
    ///        by \p p
    ///
    /// \param p pointer to the memory to load from
    /// \return the loaded std::uint64_t
    static std::uint64_t load_unaligned_uint64(const void* p) noexcept;

    //-------------------------------------------------------------------------

    /// \brief Loads an unaligned float from the memory pointed at
    ///        by \p p
    ///
    /// \param p pointer to the memory to load from
    /// \return the loaded float
    static float load_unaligned_float(const void* p) noexcept;

    /// \brief Loads an unaligned double from the memory pointed at
    ///        by \p p
    ///
    /// \param p pointer to the memory to load from
    /// \return the loaded double
    static double load_unaligned_double(const void* p) noexcept;
  };

} // namespace alloy::extra

//-----------------------------------------------------------------------------
// Alignment Checking
//-----------------------------------------------------------------------------

inline constexpr bool
  alloy::extra::pointer_util::is_power_of_two(std::size_t x)
  noexcept
{
  return (x && !(x & (x-1)));
}


inline std::size_t alloy::extra::pointer_util::align_of(const void* ptr)
  noexcept
{
  const auto address = reinterpret_cast<std::uintptr_t>(ptr);

  return static_cast<std::size_t>(address & ((~address) + 1));
}

//-----------------------------------------------------------------------------
// Offset Calculation
//-----------------------------------------------------------------------------

inline std::size_t
  alloy::extra::pointer_util::align_forward_offset(const void* p,
                                                  std::size_t align)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "p cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  return align_forward_offset(p, align, 0u);
}


inline std::size_t
  alloy::extra::pointer_util::align_forward_offset(const void* p,
                                                   std::size_t align,
                                                   std::size_t offset)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "p cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  const auto address    = reinterpret_cast<std::uintptr_t>(p) + offset;
  const auto adjustment = align - (address & (align - 1u));

  return (adjustment == align) ? 0u : adjustment;
}


//-----------------------------------------------------------------------------


inline std::size_t
  alloy::extra::pointer_util::align_backward_offset(const void* p,
                                                    std::size_t align)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  return align_backward_offset(p, align, 0u);
}


inline std::size_t
  alloy::extra::pointer_util::align_backward_offset(const void* p,
                                                    std::size_t align,
                                                    std::size_t offset)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  const auto address = reinterpret_cast<std::uintptr_t>(p) - offset;
  const auto adjustment = (address & (align - 1));

  return (adjustment == align) ? 0u : adjustment;
}


//-----------------------------------------------------------------------------
// Align
//-----------------------------------------------------------------------------


inline void*
  alloy::extra::pointer_util::align_forward(void* p,
                                            std::size_t align,
                                            core::not_null<std::size_t*> adjust)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  auto* result = align_forward(p,align);
  const auto new_address = reinterpret_cast<std::uintptr_t>(result);
  const auto old_address = reinterpret_cast<std::uintptr_t>(p);

  *adjust = static_cast<std::size_t>(new_address - old_address);

  return result;
}


inline void* alloy::extra::pointer_util::align_forward(void* p,
                                                       std::size_t align)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  const auto compliment  = ~static_cast<std::uintptr_t>(align-1);
  const auto old_address = reinterpret_cast<std::uintptr_t>(p);
  const auto new_address = (old_address + align - 1) & compliment;

  return reinterpret_cast<void*>(new_address);
}


//-----------------------------------------------------------------------------


inline void*
  alloy::extra::pointer_util::align_backward(void* p,
                                             std::size_t align,
                                             core::not_null<std::size_t*> adjust)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  auto* result = align_backward(p, align);
  const auto old_address = reinterpret_cast<std::uintptr_t>(p);
  const auto new_address = reinterpret_cast<std::uintptr_t>(result);

  *adjust = static_cast<std::size_t>(old_address - new_address);

  return result;
}


inline void* alloy::extra::pointer_util::align_backward(void* p,
                                                        std::size_t align)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  const auto compliment  = ~static_cast<std::uintptr_t>(align-1);
  const auto old_address = reinterpret_cast<std::uintptr_t>(p);
  const auto new_address = old_address & compliment;

  return reinterpret_cast<void*>(new_address);
}


//-----------------------------------------------------------------------------
// Align with Offset
//-----------------------------------------------------------------------------


inline void*
  alloy::extra::pointer_util::offset_align_forward(void* p,
                                                   std::size_t align,
                                                   std::size_t offset,
                                                   core::not_null<std::size_t*> adjust )
    noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  auto* result = offset_align_forward(p, align, offset);

  const auto old_address = reinterpret_cast<std::uintptr_t>(p);
  const auto new_address = reinterpret_cast<std::uintptr_t>(result);

  *adjust = static_cast<std::size_t>(new_address - old_address);
  return result;
}


inline void* alloy::extra::pointer_util::offset_align_forward(void* p,
                                                              std::size_t align,
                                                              std::size_t offset )
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  const auto compliment = ~static_cast<std::uintptr_t>(align-1);
  const auto mask = static_cast<std::uintptr_t>(compliment - offset);

  const auto old_address = reinterpret_cast<std::uintptr_t>(p);
  const auto stepped_address = old_address + offset + align - 1;
  const auto new_address = stepped_address & mask;

  return reinterpret_cast<void*>(new_address);
}


//-----------------------------------------------------------------------------


inline void*
  alloy::extra::pointer_util::offset_align_backward(void* p,
                                                    std::size_t align,
                                                    std::size_t offset,
                                                    core::not_null<std::size_t*> adjust)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  auto* result = offset_align_backward( p, align, offset );

  const auto old_address = reinterpret_cast<std::uintptr_t>(p);
  const auto new_address = reinterpret_cast<std::uintptr_t>(result);

  *adjust = static_cast<std::size_t>(old_address - new_address);
  return result;
}


inline void*
  alloy::extra::pointer_util::offset_align_backward(void* p,
                                                    std::size_t align,
                                                    std::size_t offset)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(is_power_of_two(align), "Alignment must be a power of 2");

  const auto compliment  = ~static_cast<std::uintptr_t>(align-1);
  const auto old_address = reinterpret_cast<std::uintptr_t>(p);
  const auto stepped_address = old_address + offset;
  const auto new_address = (stepped_address & compliment) - offset;

  return reinterpret_cast<void*>(new_address);
}


//-----------------------------------------------------------------------------
// Deltas
//-----------------------------------------------------------------------------


inline std::size_t
  alloy::extra::pointer_util::distance(const void* lhs, const void* rhs)
  noexcept
{
  ALLOY_ASSERT(lhs != nullptr, "Pointers cannot be null");
  ALLOY_ASSERT(rhs != nullptr, "Pointers cannot be null");

  if (lhs > rhs) {
    return static_cast<std::size_t>(difference(lhs, rhs));
  }
  return static_cast<std::size_t>(difference(rhs, lhs));
}


inline std::ptrdiff_t
  alloy::extra::pointer_util::difference(const void* lhs, const void* rhs)
  noexcept
{
  ALLOY_ASSERT(lhs != nullptr, "Pointers cannot be null");
  ALLOY_ASSERT(rhs != nullptr, "Pointers cannot be null");

  return static_cast<const std::byte*>(lhs) - static_cast<const std::byte*>(rhs);
}


inline void*
  alloy::extra::pointer_util::advance(void* p, std::ptrdiff_t bytes)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return static_cast<std::byte*>(p) + bytes;
}


inline const void*
  alloy::extra::pointer_util::advance(const void* p, std::ptrdiff_t bytes)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return static_cast<const std::byte*>(p) + bytes;
}


//-----------------------------------------------------------------------------
// Storing
//-----------------------------------------------------------------------------


template<typename T>
inline void alloy::extra::pointer_util::store_unaligned(void* p, const T& val)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  static_assert(std::is_trivially_copyable<T>::value);

  const auto* const val_ptr  = std::addressof(val);

  std::memcpy(p, val_ptr, sizeof(T));
}


//-----------------------------------------------------------------------------
// Loading
//-----------------------------------------------------------------------------


template<typename T>
inline T alloy::extra::pointer_util::load_unaligned(const void* p)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  static_assert(std::is_trivially_copyable<T>::value);

  core::aligned_storage<sizeof(T),alignof(T)> result;

  std::memcpy(&result, p, sizeof(T));

  // Type is trivially copyable, so just copy the result out
  return *reinterpret_cast<T*>(&result);
}


//-----------------------------------------------------------------------------


inline std::int8_t
  alloy::extra::pointer_util::load_unaligned_int8(const void* p)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return load_unaligned<std::int8_t>(p);
}


inline std::uint8_t
  alloy::extra::pointer_util::load_unaligned_uint8(const void* p)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return load_unaligned<std::uint8_t>(p);
}


//-----------------------------------------------------------------------------


inline std::int16_t
  alloy::extra::pointer_util::load_unaligned_int16(const void* p)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return load_unaligned<std::int16_t>(p);
}


inline std::uint16_t
  alloy::extra::pointer_util::load_unaligned_uint16(const void* p)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return load_unaligned<std::uint16_t>(p);
}


//-----------------------------------------------------------------------------


inline std::int32_t
  alloy::extra::pointer_util::load_unaligned_int32(const void* p)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return load_unaligned<std::int32_t>(p);
}


inline std::uint32_t
  alloy::extra::pointer_util::load_unaligned_uint32(const void* p)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return load_unaligned<std::uint32_t>(p);
}


//-----------------------------------------------------------------------------


inline std::int64_t
  alloy::extra::pointer_util::load_unaligned_int64(const void* p)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return load_unaligned<std::int64_t>(p);
}


inline std::uint64_t
  alloy::extra::pointer_util::load_unaligned_uint64(const void* p)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return load_unaligned<std::uint64_t>(p);
}


//-----------------------------------------------------------------------------


inline float
  alloy::extra::pointer_util::load_unaligned_float(const void* p)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return load_unaligned<float>(p);
}


inline double
  alloy::extra::pointer_util::load_unaligned_double(const void* p)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  return load_unaligned<double>(p);
}


#endif /* ALLOY_EXTRA_ALLOCATORS_POINTER_UTIL_HPP */
