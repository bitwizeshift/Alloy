// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file simd.hpp
 *
 * \brief Defines wrapped primitives for SIMD math
 *****************************************************************************/

#ifndef ALLOY_CORE_MATH_SIMD_HPP
#define ALLOY_CORE_MATH_SIMD_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/types.hpp"

#include <algorithm> // IWYU pragma: keep
#include <array>
#include <bit>       // IWYU pragma: keep
#include <cmath>     // IWYU pragma: keep
#include <cstddef>
#include <span>
#include <type_traits>

// Detect target platform and include appropriate SIMD support. Defining
// ALLOY_DISABLE_SIMD to a non-zero value forces the portable fallback.
#if !defined(ALLOY_DISABLE_SIMD) || !ALLOY_DISABLE_SIMD
# if defined(__ARM_NEON) || defined(_M_ARM64)
#   include <arm_neon.h>
#   define ALLOY_SIMD_NEON 1
# elif defined(__SSE2__) || defined(_M_X64) || \
       (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
#   include <immintrin.h>
#   define ALLOY_SIMD_SSE 1
# endif
#endif

#if !defined(ALLOY_SIMD_NEON) && !defined(ALLOY_SIMD_SSE)
# define ALLOY_SIMD_NONE 1
#endif

// SSE's __m128 contains a 'may-alias' attribute on GCC that does not propagate
// to the type when used in a template. Aliasing is not used in this context,
// and so functionally this is not an issue -- so we suppress the error.
ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wignored-attributes)

namespace alloy::core {

  // Sources for all instruction information:
  //
  // Arm NEON:
  // * https://developer.arm.com/architectures/instruction-sets/intrinsics/
  //
  // x86_64 SSE:
  // * https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
  //

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A small helper type around a normal `std::array` that also ensures
  ///        proper over-alignment of elements
  /////////////////////////////////////////////////////////////////////////////
  template <typename T, std::size_t Size, std::size_t Align = alignof(T)>
  struct alignas(Align) aligned_array : std::array<T, Size> {};

  /// \brief The normal alignment for average SIMD instructions.
  ///
  /// \note Some SIMD instruction sets may require greater alignment; this is
  ///       only for the base-set
  inline constexpr auto simd_alignment = 16u;

  /// \brief A convenience alias for the lowest alignment required for most
  ///        SIMD operations in engines
  template <typename T, std::size_t Size>
  using simd_aligned_array = aligned_array<T, Size, simd_alignment>;

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A small "trait"-like type that is specialized per SIMD T type to
  ///        expose a `::type` alias of the underlying SIMD type.
  /////////////////////////////////////////////////////////////////////////////
  template <typename T>
  struct simd_type {};

  /// \brief An alias that expands into the underlying SIMD intrinsic type.
  ///
  /// A `simd<T>` type should always become the underlying native SIMD type
  /// directly, meaning it should be interoperable with the underlying intrinsic
  /// APIs.
  template <typename T>
  using simd_type_t = typename simd_type<T>::type;

#if defined(ALLOY_SIMD_NEON)

  template <>
  struct simd_type<f32>
  {
    using type = ::float32x4_t;
  };

  template <>
  struct simd_type<u32>
  {
    using type = ::uint32x4_t;
  };

  template <>
  struct simd_type<s32>
  {
    using type = ::int32x4_t;
  };

#elif defined(ALLOY_SIMD_SSE)

  template <>
  struct simd_type<f32>
  {
    using type = __m128;
  };

  template <>
  struct simd_type<s32>
  {
    using type = __m128i;
  };

  template <>
  struct simd_type<u32>
  {
    // SSE has no unsigned equivalent
    using type = __m128i;
  };

#else

  template <>
  struct simd_type<f32>
  {
    using type = aligned_array<f32, 4u, 16u>;
  };

  template <>
  struct simd_type<s32>
  {
    using type = aligned_array<s32, 4u, 16u>;
  };

  template <>
  struct simd_type<u32>
  {
    using type = aligned_array<u32, 4u, 16u>;
  };

#endif

  template <typename T>
  struct simd
  {
    static_assert(
      std::is_same_v<T,f32> || std::is_same_v<T,s32> || std::is_same_v<T,u32>
    );

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------

    using element_type = T;
    using simd_type    = simd_type_t<T>;

    //-------------------------------------------------------------------------
    // Static Factories
    //-------------------------------------------------------------------------

    /// \brief Loads a SIMD value from an already aligned array of values
    ///
    /// \pre `p` is aligned to a 16-byte boundary
    ///
    /// \param p a pointer to a sequence of aligned values
    /// \return the simd value
    static auto load(std::span<const T,4> p) noexcept -> simd<T>;

    /// \brief Loads a SIMD value from a possibly unaligned array of values
    ///
    /// \param p a pointer to a sequence of values
    /// \return the simd value
    static auto load_unaligned(std::span<const T,4> p) noexcept -> simd<T>;

    /// \brief Sets a single scalar \p f into all simd lanes
    ///
    /// \param f the value to load
    /// \return the simd value
    static auto spread_scalar(T scalar) noexcept -> simd<T>;

    /// \brief Loads a single \p lane from \p v into all simd lanes
    ///
    /// \tparam Lane the lane vlaue to select
    /// \param v the simd value to select a lane from
    /// \return the simd float
    template <std::size_t Lane>
    static auto spread_from_lane(simd<T> v) noexcept -> simd<T>;

    /// \brief Constructs a SIMD value from the 4 specified components
    ///
    /// \param v0 the first lane value
    /// \param v1 the second lane value
    /// \param v2 the third lane value
    /// \param v3 the fourth lane value
    static auto set(T v0, T v1, T v2, T v3) noexcept -> simd<T>;

    //-------------------------------------------------------------------------
    // Storing
    //-------------------------------------------------------------------------

    /// \brief Stores this SIMD object to the specified 16-byte aligned output
    ///        buffer
    ///
    /// \pre \p out is aligned to a 16-byte boundary
    /// \param out the output buffer to store to
    auto store(std::span<T,4> out) const noexcept -> void;

    /// \brief Stores this SIMD object to the specified unaligned output buffer
    ///
    /// \param out the output buffer to store to
    auto store_unaligned(std::span<T,4> out) const noexcept -> void;

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------

    /// \brief Gets the specified `Lane` of data from this SIMD object
    ///
    /// \tparam Lane the lane
    /// \return the data in the given lane
    template <std::size_t Lane>
    auto lane() const noexcept -> T;

    //-------------------------------------------------------------------------
    // Public Members
    //-------------------------------------------------------------------------

    simd_type value;
  };

  //---------------------------------------------------------------------------
  // Casting
  //---------------------------------------------------------------------------

  /// \brief Reinterprets the bits of the SIMD value \p v as a SIMD value of
  ///        \p To
  ///
  /// In general, this operation should be a no-op and not actually generate
  /// any assembly; it should only change how the data is viewed
  ///
  /// \tparam To the element type to reinterpret the lanes as
  /// \param v the value to cast
  /// \return the casted value
  template <typename To, typename From>
  auto simd_cast(simd<From> v) noexcept -> simd<To>;

  //---------------------------------------------------------------------------
  // Unary Arithmetic
  //---------------------------------------------------------------------------

  /// \brief Negates the state of the SIMD vector \p v
  ///
  /// \param v the value to negate
  /// \return the negated vector
  auto simd_negate(simd<f32> v) noexcept -> simd<f32>;

  /// \brief Computes the absolute value o the SIMD vector \p v
  ///
  /// \param v the value to get the absolute value of
  /// \return a vector containing only the absolute value of each lane
  auto simd_abs(simd<f32> v) noexcept -> simd<f32>;

  //---------------------------------------------------------------------------
  // Arithmetic
  //---------------------------------------------------------------------------

  /// \brief Adds two SIMD values together
  ///
  /// This computes `lhs + rhs` and returns the result
  ///
  /// \param lhs the left argument to add
  /// \param rhs the right argument to add
  /// \return the result
  template <typename T>
  auto simd_add(simd<T> lhs, simd<T> rhs) noexcept -> simd<T>;

  /// \brief Subtracts two SIMD values
  ///
  /// This computes `lhs - rhs` and returns the result
  ///
  /// \param lhs the left argument
  /// \param rhs the right argument to subtract from \p lhs
  /// \return the result
  template <typename T>
  auto simd_subtract(simd<T> lhs, simd<T> rhs) noexcept -> simd<T>;

  /// \brief Multiplies two SIMD values
  ///
  /// This computes `lhs * rhs` and returns the result
  ///
  /// \param lhs the left value to multiply
  /// \param rhs the right value to multiply
  /// \return the result
  template <typename T>
  auto simd_multiply(simd<T> lhs, simd<T> rhs) noexcept -> simd<T>;

  /// \brief Multiplies the SIMD value \p v by a spread \p scalar
  ///
  /// This computes `lhs * rhs` where `rhs` is a SIMD vector containing
  /// `[scalar, scalar, scalar, scalar]` and returns the result.
  ///
  /// \param v the SIMD vector to multiply
  /// \param scalar the scalar to multiply each lane of \p v by
  /// \return the result
  auto simd_multiply_scalar(simd<f32> v, f32 scalar)
    noexcept -> simd<f32>;

  /// \brief Divides two SIMD values
  ///
  /// This computes `lhs / rhs` and returns the result
  ///
  /// \param lhs the left value to divide by \p rhs
  /// \param rhs the right value
  /// \return the result
  auto simd_divide(simd<f32> lhs, simd<f32> rhs) noexcept -> simd<f32>;

  /// \brief Computes the absolute difference between two SIMD values
  ///
  /// This is effectively equivalent to calling `simd_abs(simd_sub(lhs, rhs))`,
  /// and may in fact by identical if a platform lacks an explicit instruction
  /// for this.
  ///
  /// \param lhs the left value
  /// \param rhs the right value
  /// \return the absolute difference
  auto simd_abs_difference(simd<f32> lhs, simd<f32> rhs) noexcept -> simd<f32>;

  //---------------------------------------------------------------------------
  // Fused Arithmetic
  //---------------------------------------------------------------------------

  auto simd_fuse_add_multiply(simd<f32> add, simd<f32> lhs, simd<f32> rhs)
    noexcept -> simd<f32>;

  auto simd_fuse_add_multiply_scalar(simd<f32> add, simd<f32> lhs, f32 rhs)
    noexcept -> simd<f32>;

  //---------------------------------------------------------------------------
  // Comparisons
  //---------------------------------------------------------------------------

  template <typename T>
  auto simd_equal(simd<T> lhs, simd<T> rhs) noexcept -> simd<u32>;

  template <typename T>
  auto simd_not_equal(simd<T> lhs, simd<T> rhs) noexcept -> simd<u32>;

  template <typename T>
  auto simd_less(simd<T> lhs, simd<T> rhs) noexcept -> simd<u32>;

  template <typename T>
  auto simd_greater(simd<T> lhs, simd<T> rhs) noexcept -> simd<u32>;

  template <typename T>
  auto simd_less_equal(simd<T> lhs, simd<T> rhs) noexcept -> simd<u32>;

  template <typename T>
  auto simd_greater_equal(simd<T> lhs, simd<T> rhs) noexcept -> simd<u32>;

  /// \brief Performs an equality comparison relative to a scalar tolerance
  ///        value.
  ///
  /// \param lhs the left simd object
  /// \param rhs the right simd object
  /// \param tolerance the tolerance value to compare against
  /// \return the reuslt of the comparison, in a simd object
  auto simd_almost_equal(simd<f32> lhs, simd<f32> rhs, f32 tolerance) noexcept -> simd<u32>;

  /// \brief Performs an equality comparison relative to a simd tolerance pack.
  ///
  /// \param lhs the left simd object
  /// \param rhs the right simd object
  /// \param tolerance the tolerance value in a simd object to compare against
  /// \return the reuslt of the comparison, in a simd object
  auto simd_almost_equal(simd<f32> lhs, simd<f32> rhs, simd<f32> tolerance) noexcept -> simd<u32>;

  //---------------------------------------------------------------------------
  // Math and Algorithms
  //---------------------------------------------------------------------------

  /// \brief Performs a fast, and possibly imprecise, calculation for the
  ///        reciprocal of \p v
  ///
  /// \note This is the hardware estimate of the reciprocal, which is accurate
  ///       to about 12 bits on SSE, and 8 bits on NEON.
  ///
  /// \param v the simd object to compute the reciprocal of
  /// \return the compted reciprocals in a simd object
  auto simd_fast_reciprocal(simd<f32> v) noexcept -> simd<f32>;

  /// \brief Performs a computation for the reciprocal of \p v
  ///
  /// This is computed with a full-precision division.
  ///
  /// \param v the simd object to compute the reciprocal of
  /// \return the compted reciprocals in a simd object
  auto simd_reciprocal(simd<f32> v) noexcept -> simd<f32>;

  //---------------------------------------------------------------------------

  /// \brief Computes a fast, and possibly imprecise, sqrt
  ///
  /// \note Both NEON and SSE have a full-precision square root instruction,
  ///       so this is currently the same as `simd_sqrt`.
  ///
  /// \param v the values to square-root
  /// \return the square-root result in a simd object
  auto simd_fast_sqrt(simd<f32> v) noexcept -> simd<f32>;

  /// \brief Computes a precise sqrt, at a possible performance cost
  ///
  /// \param v the values to square-root
  /// \return the square-root result in a simd object
  auto simd_sqrt(simd<f32> v) noexcept -> simd<f32>;

  //---------------------------------------------------------------------------

  /// \brief Computes the reciprocal sqrt value of \p v using a fast, but
  ///        potentially imprecise, method.
  ///
  /// \note This is the hardware estimate of the reciprocal square root, which
  ///       is accurate to about 12 bits on SSE, and 8 bits on NEON.
  ///
  /// \param v the values to compute the reciprocal sqrt for
  /// \return the computed result in a simd object
  auto simd_fast_reciprocal_sqrt(simd<f32> v) noexcept -> simd<f32>;

  /// \brief Computes the reciprocal sqrt value of \p v using a more precies
  ///        method, potentially at a performance impact
  ///
  /// \note This is a composition of `simd_sqrt` and `simd_reciprocal`.
  ///
  /// \param v the values to compute the reciprocal sqrt for
  /// \return the computed result in a simd object
  auto simd_reciprocal_sqrt(simd<f32> f) noexcept -> simd<f32>;

  //---------------------------------------------------------------------------

  /// \brief Computes the minimum value of each lane for \p lhs and \p rhs
  ///
  /// \param lhs the left simd value
  /// \param rhs the right simd value
  /// \return a simd object holding the min value from each lanes
  auto simd_min(simd<f32> lhs, simd<f32> rhs) noexcept -> simd<f32>;

  /// \brief Computes the maximum value of each lane for \p lhs and \p rhs
  ///
  /// \param lhs the left simd value
  /// \param rhs the right simd value
  /// \return a simd object holding the max value from each lanes
  auto simd_max(simd<f32> lhs, simd<f32> rhs) noexcept -> simd<f32>;

  //---------------------------------------------------------------------------

  // The dot products below operate on 4-component vectors in a
  // structure-of-arrays layout: a group of 4 `simd<f32>` holds the x, y, z,
  // and w components of 4 vectors, one vector per lane. Lane i of a result
  // is the dot product of the i-th vectors of the two groups. A group of 16
  // `f32` is the same layout, unpacked into the four components in order.

  /// \brief Computes the dot products of the 4 vectors in \p lhs and \p rhs
  ///
  /// \param lhs the components of the left vectors
  /// \param rhs the components of the right vectors
  /// \return the dot product of each pair of vectors, one per lane
  auto simd_dot4(std::span<const simd<f32>,4> lhs, std::span<const simd<f32>,4> rhs)
    noexcept -> simd<f32>;

  /// \brief Computes the dot products of the 4 vectors in \p lhs and \p rhs
  ///
  /// \pre \p lhs and \p rhs are aligned to a 16-byte boundary
  ///
  /// \param lhs the components of the left vectors
  /// \param rhs the components of the right vectors
  /// \return the dot product of each pair of vectors, one per lane
  auto simd_dot4(std::span<const f32,16> lhs, std::span<const f32,16> rhs)
    noexcept -> simd<f32>;

  //---------------------------------------------------------------------------

  /// \brief Computes the dot products of each group of 4 vectors in \p lhs
  ///        and \p rhs
  ///
  /// \param lhs the groups of left vectors
  /// \param rhs the groups of right vectors
  /// \return the dot products of each group
  template <std::size_t N>
    requires ((N % 4) == 0)
  auto simd_dot(std::span<const simd<f32>, N> lhs, std::span<const simd<f32>, N> rhs)
    noexcept -> std::array<simd<f32>, (N / 4)>;

  /// \brief Computes the dot products of each group of 4 vectors in \p lhs
  ///        and \p rhs
  ///
  /// \pre \p lhs and \p rhs are aligned to a 16-byte boundary
  ///
  /// \param lhs the groups of left vectors
  /// \param rhs the groups of right vectors
  /// \return the dot products of each group
  template <std::size_t N>
    requires ((N % 16) == 0)
  auto simd_dot(std::span<const f32, N> lhs, std::span<const f32, N> rhs)
    noexcept -> std::array<simd<f32>, (N / 16)>;

  //---------------------------------------------------------------------------

  /// \brief Computes the dot products of each group of 4 vectors in \p lhs
  ///        and \p rhs into \p out
  ///
  /// \pre \p lhs, \p rhs, and \p out are aligned to a 16-byte boundary
  ///
  /// \param lhs the groups of left vectors
  /// \param rhs the groups of right vectors
  /// \param out the dot products, 4 for each group
  template <std::size_t N>
    requires (N % 16 == 0)
  auto simd_dot4_to(
    std::span<const f32,N> lhs,
    std::span<const f32,N> rhs,
    std::span<f32,(N / 4)> out
  ) noexcept -> void;

  /// \brief Computes the dot products of each group of 4 vectors in \p lhs
  ///        and \p rhs into \p out
  ///
  /// \pre \p lhs and \p rhs are aligned to a 16-byte boundary
  ///
  /// \param lhs the groups of left vectors
  /// \param rhs the groups of right vectors
  /// \param out the dot products of each group
  template <std::size_t N>
    requires (N % 16 == 0)
  auto simd_dot4_to(
    std::span<const f32,N> lhs,
    std::span<const f32,N> rhs,
    std::span<simd<f32>,(N/16)> out
  ) noexcept -> void;

  /// \brief Computes the dot products of each group of 4 vectors in \p lhs
  ///        and \p rhs into \p out
  ///
  /// \param lhs the groups of left vectors
  /// \param rhs the groups of right vectors
  /// \param out the dot products of each group
  template <std::size_t N>
    requires ((N % 4) == 0)
  auto simd_dot4_to(
    std::span<const simd<f32>,N> lhs,
    std::span<const simd<f32>,N> rhs,
    std::span<simd<f32>,(N/4)> out
  ) noexcept -> void;

} // namespace alloy::core

//-----------------------------------------------------------------------------
// Loading
//-----------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::load(std::span<const T, 4> p)
  noexcept -> simd<T>
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{vld1q_f32(compiler::assume_aligned<16u>(p.data()))};
  } else if constexpr (std::is_same_v<T, s32>) {
    return simd<T>{vld1q_s32(compiler::assume_aligned<16u>(p.data()))};
  } else {
    return simd<T>{vld1q_u32(compiler::assume_aligned<16u>(p.data()))};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{_mm_load_ps(compiler::assume_aligned<16u>(p.data()))};
  } else {
    const auto* const data = compiler::assume_aligned<16u>(p.data());
    return simd<T>{_mm_load_si128(reinterpret_cast<const __m128i*>(data))};
  }
#else
  return set(p[0], p[1], p[2], p[3]);
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::load_unaligned(std::span<const T, 4> p)
  noexcept -> simd<T>
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{vld1q_f32(p.data())};
  } else if constexpr (std::is_same_v<T, s32>) {
    return simd<T>{vld1q_s32(p.data())};
  } else {
    return simd<T>{vld1q_u32(p.data())};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{_mm_loadu_ps(p.data())};
  } else {
    return simd<T>{_mm_loadu_si128(reinterpret_cast<const __m128i*>(p.data()))};
  }
#else
  return set(p[0], p[1], p[2], p[3]);
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::spread_scalar(T scalar) noexcept -> simd<T>
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{vld1q_dup_f32(&scalar)};
  } else if constexpr (std::is_same_v<T, s32>) {
    return simd<T>{vld1q_dup_s32(&scalar)};
  } else {
    return simd<T>{vld1q_dup_u32(&scalar)};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{_mm_set1_ps(scalar)};
  } else {
    return simd<T>{_mm_set1_epi32(static_cast<int>(scalar))};
  }
#else
  return set(scalar, scalar, scalar, scalar);
#endif
}

template <typename T>
template <std::size_t Lane>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::spread_from_lane(simd<T> v) noexcept -> simd<T>
{
  static_assert(Lane <= 3u);

#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{vdupq_laneq_f32(v.value, Lane)};
  } else if constexpr (std::is_same_v<T, s32>) {
    return simd<T>{vdupq_laneq_s32(v.value, Lane)};
  } else {
    return simd<T>{vdupq_laneq_u32(v.value, Lane)};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{_mm_shuffle_ps(v.value, v.value, _MM_SHUFFLE(Lane, Lane, Lane, Lane))};
  } else {
    return simd<T>{_mm_shuffle_epi32(v.value, _MM_SHUFFLE(Lane, Lane, Lane, Lane))};
  }
#else
  return set(
    v.value[Lane],
    v.value[Lane],
    v.value[Lane],
    v.value[Lane]
  );
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::set(T v0, T v1, T v2, T v3) noexcept -> simd<T>
{
#if defined(ALLOY_SIMD_NEON)
  const auto data = aligned_array<T, 4u, 16u>{{
    v0, v1, v2, v3
  }};
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{vld1q_f32(compiler::assume_aligned<16u>(data.data()))};
  } else if constexpr (std::is_same_v<T, s32>) {
    return simd<T>{vld1q_s32(compiler::assume_aligned<16u>(data.data()))};
  } else {
    return simd<T>{vld1q_u32(compiler::assume_aligned<16u>(data.data()))};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{_mm_setr_ps(v0, v1, v2, v3)};
  } else {
    return simd<T>{_mm_setr_epi32(
      static_cast<int>(v0),
      static_cast<int>(v1),
      static_cast<int>(v2),
      static_cast<int>(v3)
    )};
  }
#else
  return simd<T>{{v0, v1, v2, v3}};
#endif
}

//-----------------------------------------------------------------------------
// Storing
//-----------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::store(std::span<T,4> out) const noexcept -> void
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T, f32>) {
    vst1q_f32(compiler::assume_aligned<16>(out.data()), value);
  } else if constexpr (std::is_same_v<T, s32>) {
    vst1q_s32(compiler::assume_aligned<16>(out.data()), value);
  } else if constexpr (std::is_same_v<T, u32>) {
    vst1q_u32(compiler::assume_aligned<16>(out.data()), value);
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T, f32>) {
    _mm_store_ps(compiler::assume_aligned<16>(out.data()), value);
  } else {
    auto* const data = compiler::assume_aligned<16>(out.data());
    _mm_store_si128(reinterpret_cast<__m128i*>(data), value);
  }
#else
  store_unaligned(out);
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::store_unaligned(std::span<T,4> out)
  const noexcept -> void
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T, f32>) {
    vst1q_f32(out.data(), value);
  } else if constexpr (std::is_same_v<T, s32>) {
    vst1q_s32(out.data(), value);
  } else if constexpr (std::is_same_v<T, u32>) {
    vst1q_u32(out.data(), value);
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T, f32>) {
    _mm_storeu_ps(out.data(), value);
  } else {
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out.data()), value);
  }
#else
  // In the absence of a better mechanism, unpack each lane independently
  out[0] = lane<0>();
  out[1] = lane<1>();
  out[2] = lane<2>();
  out[3] = lane<3>();
#endif
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

template <typename T>
template <std::size_t Lane>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::lane() const noexcept -> T
{
  static_assert(Lane <= 3u);

#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T, f32>) {
    return vgetq_lane_f32(value, Lane);
  } else if constexpr (std::is_same_v<T, s32>) {
    return vgetq_lane_s32(value, Lane);
  } else {
    return vgetq_lane_u32(value, Lane);
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T, f32>) {
    return _mm_cvtss_f32(
      _mm_shuffle_ps(value, value, _MM_SHUFFLE(Lane, Lane, Lane, Lane))
    );
  } else {
    return static_cast<T>(_mm_cvtsi128_si32(
      _mm_shuffle_epi32(value, _MM_SHUFFLE(Lane, Lane, Lane, Lane))
    ));
  }
#elif defined(ALLOY_SIMD_NONE)
  return value[Lane];
#endif
}

//-----------------------------------------------------------------------------
// Casting
//-----------------------------------------------------------------------------

template <typename To, typename From>
ALLOY_FORCE_INLINE
auto alloy::core::simd_cast(simd<From> v) noexcept -> simd<To>
{
  if constexpr (std::is_same_v<To,From>) {
    return v;
  } else {
#if defined(ALLOY_SIMD_NEON)
    if constexpr (std::is_same_v<To, f32> && std::is_same_v<From, u32>) {
      return simd<To>{vreinterpretq_f32_u32(v.value)};
    } else if constexpr (std::is_same_v<To, f32>) {
      return simd<To>{vreinterpretq_f32_s32(v.value)};
    } else if constexpr (std::is_same_v<To, u32> && std::is_same_v<From, f32>) {
      return simd<To>{vreinterpretq_u32_f32(v.value)};
    } else if constexpr (std::is_same_v<To, u32>) {
      return simd<To>{vreinterpretq_u32_s32(v.value)};
    } else if constexpr (std::is_same_v<From, f32>) {
      return simd<To>{vreinterpretq_s32_f32(v.value)};
    } else {
      return simd<To>{vreinterpretq_s32_u32(v.value)};
    }
#elif defined(ALLOY_SIMD_SSE)
    if constexpr (std::is_same_v<To, f32>) {
      return simd<To>{_mm_castsi128_ps(v.value)};
    } else if constexpr (std::is_same_v<From, f32>) {
      return simd<To>{_mm_castps_si128(v.value)};
    } else {
      // Signed and unsigned lanes share the same __m128i representation
      return simd<To>{v.value};
    }
#else
    return simd<To>::set(
      std::bit_cast<To>(v.template lane<0>()),
      std::bit_cast<To>(v.template lane<1>()),
      std::bit_cast<To>(v.template lane<2>()),
      std::bit_cast<To>(v.template lane<3>())
    );
#endif
  }
}

//-----------------------------------------------------------------------------
// Unary Arithmetic
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::simd_negate(simd<f32> v) noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vnegq_f32(v.value)};
#elif defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_sub_ps(_mm_set1_ps(0.0f), v.value)};
#else
  return simd<f32>::set(
    -v.lane<0>(),
    -v.lane<1>(),
    -v.lane<2>(),
    -v.lane<3>()
  );
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_abs(simd<f32> v) noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vabsq_f32(v.value)};
#elif defined(ALLOY_SIMD_SSE)
  // Clearing the sign bit of an IEEE-754 float produces its absolute value
  const auto mask = _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff));
  return simd<f32>{_mm_and_ps(mask, v.value)};
#else
  return simd<f32>::set(
    std::abs(v.lane<0>()),
    std::abs(v.lane<1>()),
    std::abs(v.lane<2>()),
    std::abs(v.lane<3>())
  );
#endif
}

//-----------------------------------------------------------------------------
// Arithmetic
//-----------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd_add(simd<T> lhs, simd<T> rhs) noexcept -> simd<T>
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{vaddq_f32(lhs.value, rhs.value)};
  } else if constexpr (std::is_same_v<T, s32>) {
    return simd<T>{vaddq_s32(lhs.value, rhs.value)};
  } else {
    return simd<T>{vaddq_u32(lhs.value, rhs.value)};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<T>{_mm_add_ps(lhs.value, rhs.value)};
  } else {
    return simd<T>{_mm_add_epi32(lhs.value, rhs.value)};
  }
#else
  return simd<T>::set(
    lhs.template lane<0>() + rhs.template lane<0>(),
    lhs.template lane<1>() + rhs.template lane<1>(),
    lhs.template lane<2>() + rhs.template lane<2>(),
    lhs.template lane<3>() + rhs.template lane<3>()
  );
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd_subtract(simd<T> lhs, simd<T> rhs) noexcept -> simd<T>
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<f32>{vsubq_f32(lhs.value, rhs.value)};
  } else if constexpr (std::is_same_v<T, s32>) {
    return simd<s32>{vsubq_s32(lhs.value, rhs.value)};
  } else {
    return simd<u32>{vsubq_u32(lhs.value, rhs.value)};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<f32>{_mm_sub_ps(lhs.value, rhs.value)};
  } else {
    return simd<T>{_mm_sub_epi32(lhs.value, rhs.value)};
  }
#else
  return simd<T>::set(
    lhs.template lane<0>() - rhs.template lane<0>(),
    lhs.template lane<1>() - rhs.template lane<1>(),
    lhs.template lane<2>() - rhs.template lane<2>(),
    lhs.template lane<3>() - rhs.template lane<3>()
  );
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd_multiply(simd<T> lhs, simd<T> rhs) noexcept -> simd<T>
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<f32>{vmulq_f32(lhs.value, rhs.value)};
  } else if constexpr (std::is_same_v<T, s32>) {
    return simd<s32>{vmulq_s32(lhs.value, rhs.value)};
  } else {
    return simd<u32>{vmulq_u32(lhs.value, rhs.value)};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T, f32>) {
    return simd<f32>{_mm_mul_ps(lhs.value, rhs.value)};
  } else {
#if defined(__SSE4_1__)
    return simd<T>{_mm_mullo_epi32(lhs.value, rhs.value)};
#else
    // SSE2 can only multiply the even lanes, into 64-bit products. The odd
    // lanes are shifted into the even positions to be multiplied separately,
    // and the low 32 bits of every product, which are the same for signed
    // and unsigned lanes, are interleaved back together.
    const auto even = _mm_mul_epu32(lhs.value, rhs.value);
    const auto odd = _mm_mul_epu32(
      _mm_srli_epi64(lhs.value, 32),
      _mm_srli_epi64(rhs.value, 32)
    );
    return simd<T>{_mm_unpacklo_epi32(
      _mm_shuffle_epi32(even, _MM_SHUFFLE(0, 0, 2, 0)),
      _mm_shuffle_epi32(odd, _MM_SHUFFLE(0, 0, 2, 0))
    )};
#endif
  }
#else
  return simd<T>::set(
    lhs.template lane<0>() * rhs.template lane<0>(),
    lhs.template lane<1>() * rhs.template lane<1>(),
    lhs.template lane<2>() * rhs.template lane<2>(),
    lhs.template lane<3>() * rhs.template lane<3>()
  );
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_multiply_scalar(simd<f32> v, f32 scalar)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vmulq_n_f32(v.value, scalar)};
#elif defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_mul_ps(v.value, _mm_set1_ps(scalar))};
#else
  return simd<f32>::set(
    v.lane<0>() * scalar,
    v.lane<1>() * scalar,
    v.lane<2>() * scalar,
    v.lane<3>() * scalar
  );
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_divide(simd<f32> lhs, simd<f32> rhs)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vdivq_f32(lhs.value, rhs.value)};
#elif defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_div_ps(lhs.value, rhs.value)};
#else
  return simd<f32>::set(
    lhs.lane<0>() / rhs.lane<0>(),
    lhs.lane<1>() / rhs.lane<1>(),
    lhs.lane<2>() / rhs.lane<2>(),
    lhs.lane<3>() / rhs.lane<3>()
  );
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_abs_difference(simd<f32> lhs, simd<f32> rhs)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vabdq_f32(lhs.value, rhs.value)};
#else
  return simd_abs(simd_subtract(lhs, rhs));
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_fuse_add_multiply(
  simd<f32> add,
  simd<f32> lhs,
  simd<f32> rhs
) noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vfmaq_f32(add.value, lhs.value, rhs.value)};
#elif defined(__FMA__)
  return simd<f32>{_mm_fmadd_ps(lhs.value, rhs.value, add.value)};
#elif defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_add_ps(add.value, _mm_mul_ps(lhs.value, rhs.value))};
#else
  return simd<f32>::set(
    lhs.lane<0>() * rhs.lane<0>() + add.lane<0>(),
    lhs.lane<1>() * rhs.lane<1>() + add.lane<1>(),
    lhs.lane<2>() * rhs.lane<2>() + add.lane<2>(),
    lhs.lane<3>() * rhs.lane<3>() + add.lane<3>()
  );
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_fuse_add_multiply_scalar(
  simd<f32> add,
  simd<f32> lhs,
  f32 rhs
) noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vfmaq_n_f32(add.value, lhs.value, rhs)};
#else
  return simd_fuse_add_multiply(add, lhs, simd<f32>::spread_scalar(rhs));
#endif
}

#if defined(ALLOY_SIMD_SSE)
namespace alloy::core::detail {

  /// \brief Gets the lanes of \p lhs and \p rhs in a form that SSE2's signed
  ///        integer comparisons order correctly
  ///
  /// SSE2 only compares signed integers. Flipping the sign bit of unsigned
  /// lanes maps them onto the signed range in the same order.
  template <typename T>
  ALLOY_FORCE_INLINE
  auto simd_signed_order(simd<T> lhs, simd<T> rhs)
    noexcept -> std::array<__m128i, 2>
  {
    if constexpr (std::is_same_v<T, u32>) {
      const auto bias = _mm_set1_epi32(static_cast<int>(0x80000000u));
      return {{_mm_xor_si128(lhs.value, bias), _mm_xor_si128(rhs.value, bias)}};
    } else {
      return {{lhs.value, rhs.value}};
    }
  }

} // namespace alloy::core::detail
#endif

//-----------------------------------------------------------------------------
// Comparisons
//-----------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd_equal(simd<T> lhs, simd<T> rhs) noexcept -> simd<u32>
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{vceqq_f32(lhs.value, rhs.value)};
  } else if constexpr (std::is_same_v<T,s32>) {
    return simd<u32>{vceqq_s32(lhs.value, rhs.value)};
  } else {
    return simd<u32>{vceqq_u32(lhs.value, rhs.value)};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{_mm_castps_si128(_mm_cmpeq_ps(lhs.value, rhs.value))};
  } else {
    return simd<u32>{_mm_cmpeq_epi32(lhs.value, rhs.value)};
  }
#else
  return simd<u32>::set(
    u32{0xffffffff} * (lhs.template lane<0>() == rhs.template lane<0>()),
    u32{0xffffffff} * (lhs.template lane<1>() == rhs.template lane<1>()),
    u32{0xffffffff} * (lhs.template lane<2>() == rhs.template lane<2>()),
    u32{0xffffffff} * (lhs.template lane<3>() == rhs.template lane<3>())
  );
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd_not_equal(simd<T> lhs, simd<T> rhs) noexcept -> simd<u32>
{
#if defined(ALLOY_SIMD_NEON)
  // NEON for some reason does not have a `not_equal`, so this is done as a
  // composition of the negation of an equal comparison.
  return simd<u32>{
    vmvnq_u32(simd_equal(lhs, rhs).value)
  };
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{_mm_castps_si128(_mm_cmpneq_ps(lhs.value, rhs.value))};
  } else {
    const auto equal = _mm_cmpeq_epi32(lhs.value, rhs.value);
    return simd<u32>{_mm_xor_si128(equal, _mm_set1_epi32(-1))};
  }
#else
  return simd<u32>::set(
    u32{0xffffffff} * (lhs.template lane<0>() != rhs.template lane<0>()),
    u32{0xffffffff} * (lhs.template lane<1>() != rhs.template lane<1>()),
    u32{0xffffffff} * (lhs.template lane<2>() != rhs.template lane<2>()),
    u32{0xffffffff} * (lhs.template lane<3>() != rhs.template lane<3>())
  );
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd_less(simd<T> lhs, simd<T> rhs) noexcept -> simd<u32>
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{vcltq_f32(lhs.value, rhs.value)};
  } else if constexpr (std::is_same_v<T,s32>) {
    return simd<u32>{vcltq_s32(lhs.value, rhs.value)};
  } else {
    return simd<u32>{vcltq_u32(lhs.value, rhs.value)};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{_mm_castps_si128(_mm_cmplt_ps(lhs.value, rhs.value))};
  } else {
    const auto [l, r] = detail::simd_signed_order(lhs, rhs);
    return simd<u32>{_mm_cmplt_epi32(l, r)};
  }
#else
  return simd<u32>::set(
    u32{0xffffffff} * (lhs.template lane<0>() < rhs.template lane<0>()),
    u32{0xffffffff} * (lhs.template lane<1>() < rhs.template lane<1>()),
    u32{0xffffffff} * (lhs.template lane<2>() < rhs.template lane<2>()),
    u32{0xffffffff} * (lhs.template lane<3>() < rhs.template lane<3>())
  );
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd_greater(simd<T> lhs, simd<T> rhs) noexcept -> simd<u32>
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{vcgtq_f32(lhs.value, rhs.value)};
  } else if constexpr (std::is_same_v<T,s32>) {
    return simd<u32>{vcgtq_s32(lhs.value, rhs.value)};
  } else {
    return simd<u32>{vcgtq_u32(lhs.value, rhs.value)};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{_mm_castps_si128(_mm_cmpgt_ps(lhs.value, rhs.value))};
  } else {
    const auto [l, r] = detail::simd_signed_order(lhs, rhs);
    return simd<u32>{_mm_cmpgt_epi32(l, r)};
  }
#else
  return simd<u32>::set(
    u32{0xffffffff} * (lhs.template lane<0>() > rhs.template lane<0>()),
    u32{0xffffffff} * (lhs.template lane<1>() > rhs.template lane<1>()),
    u32{0xffffffff} * (lhs.template lane<2>() > rhs.template lane<2>()),
    u32{0xffffffff} * (lhs.template lane<3>() > rhs.template lane<3>())
  );
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd_less_equal(simd<T> lhs, simd<T> rhs)
  noexcept -> simd<u32>
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{vcleq_f32(lhs.value, rhs.value)};
  } else if constexpr (std::is_same_v<T,s32>) {
    return simd<u32>{vcleq_s32(lhs.value, rhs.value)};
  } else {
    return simd<u32>{vcleq_u32(lhs.value, rhs.value)};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{_mm_castps_si128(_mm_cmple_ps(lhs.value, rhs.value))};
  } else {
    // SSE2 has no integer <=, so this is the complement of >
    const auto [l, r] = detail::simd_signed_order(lhs, rhs);
    const auto greater = _mm_cmpgt_epi32(l, r);
    return simd<u32>{_mm_xor_si128(greater, _mm_set1_epi32(-1))};
  }
#else
  return simd<u32>::set(
    u32{0xffffffff} * (lhs.template lane<0>() <= rhs.template lane<0>()),
    u32{0xffffffff} * (lhs.template lane<1>() <= rhs.template lane<1>()),
    u32{0xffffffff} * (lhs.template lane<2>() <= rhs.template lane<2>()),
    u32{0xffffffff} * (lhs.template lane<3>() <= rhs.template lane<3>())
  );
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd_greater_equal(simd<T> lhs, simd<T> rhs)
  noexcept -> simd<u32>
{
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{vcgeq_f32(lhs.value, rhs.value)};
  } else if constexpr (std::is_same_v<T,s32>) {
    return simd<u32>{vcgeq_s32(lhs.value, rhs.value)};
  } else {
    return simd<u32>{vcgeq_u32(lhs.value, rhs.value)};
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{_mm_castps_si128(_mm_cmpge_ps(lhs.value, rhs.value))};
  } else {
    // SSE2 has no integer >=, so this is the complement of <
    const auto [l, r] = detail::simd_signed_order(lhs, rhs);
    const auto less = _mm_cmplt_epi32(l, r);
    return simd<u32>{_mm_xor_si128(less, _mm_set1_epi32(-1))};
  }
#else
  return simd<u32>::set(
    u32{0xffffffff} * (lhs.template lane<0>() >= rhs.template lane<0>()),
    u32{0xffffffff} * (lhs.template lane<1>() >= rhs.template lane<1>()),
    u32{0xffffffff} * (lhs.template lane<2>() >= rhs.template lane<2>()),
    u32{0xffffffff} * (lhs.template lane<3>() >= rhs.template lane<3>())
  );
#endif
}

//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::simd_almost_equal(simd<f32> lhs, simd<f32> rhs, f32 tolerance)
  noexcept -> simd<u32>
{
  return simd_almost_equal(lhs, rhs, simd<f32>::spread_scalar(tolerance));
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_almost_equal(
  simd<f32> lhs,
  simd<f32> rhs,
  simd<f32> tolerance
) noexcept -> simd<u32>
{
  return simd_less_equal(simd_abs_difference(lhs, rhs), tolerance);
}

//-----------------------------------------------------------------------------
// Math and Algorithms
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::simd_fast_reciprocal(simd<f32> v) noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vrecpeq_f32(v.value)};
#elif defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_rcp_ps(v.value)};
#else
  return simd<f32>::set(
    1.0f / v.lane<0>(),
    1.0f / v.lane<1>(),
    1.0f / v.lane<2>(),
    1.0f / v.lane<3>()
  );
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_reciprocal(simd<f32> v) noexcept -> simd<f32>
{
  return simd_divide(simd<f32>::spread_scalar(1.0f), v);
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_fast_sqrt(simd<f32> v) noexcept -> simd<f32>
{
  return simd_sqrt(v);
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_sqrt(simd<f32> v) noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vsqrtq_f32(v.value)};
#elif defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_sqrt_ps(v.value)};
#else
  return simd<f32>::set(
    std::sqrt(v.lane<0>()),
    std::sqrt(v.lane<1>()),
    std::sqrt(v.lane<2>()),
    std::sqrt(v.lane<3>())
  );
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_fast_reciprocal_sqrt(simd<f32> v) noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vrsqrteq_f32(v.value)};
#elif defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_rsqrt_ps(v.value)};
#else
  return simd_reciprocal_sqrt(v);
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_reciprocal_sqrt(simd<f32> v) noexcept -> simd<f32>
{
  return simd_reciprocal(simd_sqrt(v));
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_min(simd<f32> lhs, simd<f32> rhs) noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vminq_f32(lhs.value, rhs.value)};
#elif defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_min_ps(lhs.value, rhs.value)};
#else
  return simd<f32>::set(
    std::min(lhs.lane<0>(), rhs.lane<0>()),
    std::min(lhs.lane<1>(), rhs.lane<1>()),
    std::min(lhs.lane<2>(), rhs.lane<2>()),
    std::min(lhs.lane<3>(), rhs.lane<3>())
  );
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_max(simd<f32> lhs, simd<f32> rhs) noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vmaxq_f32(lhs.value, rhs.value)};
#elif defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_max_ps(lhs.value, rhs.value)};
#else
  return simd<f32>::set(
    std::max(lhs.lane<0>(), rhs.lane<0>()),
    std::max(lhs.lane<1>(), rhs.lane<1>()),
    std::max(lhs.lane<2>(), rhs.lane<2>()),
    std::max(lhs.lane<3>(), rhs.lane<3>())
  );
#endif
}

inline
auto alloy::core::simd_dot4(
  std::span<const simd<f32>,4> lhs,
  std::span<const simd<f32>,4> rhs
) noexcept -> simd<f32>
{
  // Fused multiply-add instructions result in a data-dependency/stall between
  // the result of the FMA instruction, since the result is fed into the next
  // instruction.
  // This is unravelled here to help ensure parallelism between the
  // multiplications and to minimize stalls.
  const auto r0 = simd_multiply(lhs[0], rhs[0]);
  const auto r1 = simd_multiply(lhs[1], rhs[1]);
  const auto r2 = simd_multiply(lhs[2], rhs[2]);
  const auto r3 = simd_multiply(lhs[3], rhs[3]);

  const auto r01 = simd_add(r0, r1);
  const auto r23 = simd_add(r2, r3);

  return simd_add(r01, r23);
}

inline
auto alloy::core::simd_dot4(
  std::span<const f32,16> lhs,
  std::span<const f32,16> rhs
) noexcept -> simd<f32>
{
  const auto lhs_array = std::array<simd<f32>,4>{{
    simd<f32>::load(lhs.subspan<0,4u>()),
    simd<f32>::load(lhs.subspan<4u,4u>()),
    simd<f32>::load(lhs.subspan<8u,4u>()),
    simd<f32>::load(lhs.subspan<12u,4u>()),
  }};
  const auto rhs_array = std::array<simd<f32>,4>{{
    simd<f32>::load(rhs.subspan<0,4u>()),
    simd<f32>::load(rhs.subspan<4u,4u>()),
    simd<f32>::load(rhs.subspan<8u,4u>()),
    simd<f32>::load(rhs.subspan<12u,4u>()),
  }};

  return simd_dot4(lhs_array, rhs_array);
}

//-----------------------------------------------------------------------------

template <std::size_t N>
  requires ((N % 4) == 0)
inline
auto alloy::core::simd_dot(
  std::span<const simd<f32>, N> lhs,
  std::span<const simd<f32>, N> rhs
) noexcept -> std::array<simd<f32>, (N / 4)>
{
  // Fused multiply-add instructions result in a data-dependency/stall between
  // the result of the FMA instruction, since the result is fed into the next
  // instruction iteration.
  //
  // This is unravelled here to help ensure parallelism between the
  // multiplications and to minimize stalls when waiting on addition
  auto intermediate = std::array<simd<f32>, N>{};
  for (auto i = 0u; i < N; ++i) {
    intermediate[i] = simd_multiply(lhs[i], rhs[i]);
  }

  auto dot_result = std::array<simd<f32>, (N / 4)>{};
  for (auto i = 0u; i < (N / 4); ++i) {
    const auto group = 4u * i;
    dot_result[i] = simd_add(
      simd_add(intermediate[group], intermediate[group + 1]),
      simd_add(intermediate[group + 2], intermediate[group + 3])
    );
  }
  return dot_result;
}

template <std::size_t N>
  requires ((N % 16) == 0)
inline
auto alloy::core::simd_dot(
  std::span<const f32, N> lhs,
  std::span<const f32, N> rhs
) noexcept -> std::array<simd<f32>, (N / 16)>
{
  auto dot_result = std::array<simd<f32>, (N / 16)>{};

  for (auto i = 0u; i < (N / 16); ++i) {
    dot_result[i] = simd_dot4(
      std::span<const f32, 16>{lhs.data() + (16u * i), 16u},
      std::span<const f32, 16>{rhs.data() + (16u * i), 16u}
    );
  }

  return dot_result;
}


template <std::size_t N>
  requires (N % 16 == 0)
inline
auto alloy::core::simd_dot4_to(
  std::span<const f32,N> lhs,
  std::span<const f32,N> rhs,
  std::span<f32,(N/4)> out
) noexcept -> void
{
  for (auto i = 0u; i < (N / 16); ++i) {
    const auto result = simd_dot4(
      std::span<const f32, 16>{lhs.data() + (16u * i), 16u},
      std::span<const f32, 16>{rhs.data() + (16u * i), 16u}
    );

    result.store(std::span<f32, 4>{out.data() + (4u * i), 4u});
  }
}


template <std::size_t N>
  requires (N % 16 == 0)
inline
auto alloy::core::simd_dot4_to(
  std::span<const f32,N> lhs,
  std::span<const f32,N> rhs,
  std::span<simd<f32>,(N/16)> out
) noexcept -> void
{
  for (auto i = 0u; i < (N / 16); ++i) {
    out[i] = simd_dot4(
      std::span<const f32, 16>{lhs.data() + (16u * i), 16u},
      std::span<const f32, 16>{rhs.data() + (16u * i), 16u}
    );
  }
}

template <std::size_t N>
  requires ((N % 4) == 0)
inline
auto alloy::core::simd_dot4_to(
  std::span<const simd<f32>,N> lhs,
  std::span<const simd<f32>,N> rhs,
  std::span<simd<f32>,(N/4)> out
) noexcept -> void
{
  for (auto i = 0u; i < (N / 4); ++i) {
    out[i] = simd_dot4(
      std::span<const simd<f32>, 4>{lhs.data() + (4u * i), 4u},
      std::span<const simd<f32>, 4>{rhs.data() + (4u * i), 4u}
    );
  }
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_MATH_SIMD_HPP */
