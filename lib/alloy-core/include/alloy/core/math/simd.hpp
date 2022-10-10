/*****************************************************************************
 * \file simd.hpp
 *
 * \brief Defines wrapped primitives for SIMD math
 *****************************************************************************/

/*
 The MIT License (MIT)

 Copyright (c) 2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_MATH_SIMD_HPP
#define ALLOY_CORE_MATH_SIMD_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/types.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/containers/span.hpp"
#include "alloy/core/math/math.hpp" // sqrt
#include <cstddef>
#include <algorithm> // std::min

// Detect target platform and include appropriate SIMD support
#if !ALLOY_DISABLE_SIMD
# if defined(__aarch64__)
#   if defined(__ARM_NEON__) && __has_include(<arm_neon.h>)
#     include <arm_neon.h>
#     define ALLOY_SIMD_NEON 1
#   endif
# elif defined(__i386__) || defined(__x86_64__)
#   if defined(__SSE__) && __has_include(<emmintrin.h>)
#     include <emmintrin.h>
#   endif
#   if defined(__SSE2__) && __has_include(<immintrin.h>)
#     include <immintrin.h>
#   endif
#   define ALLOY_SIMD_SSE 1
# endif
#endif

#if defined(ALLOY_DISABLE_SIMD) || !defined(ALLOY_SIMD_NEON) && !defined(ALLOY_SIMD_SSE)
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

#elif defined(__SSE__)

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
    static auto load(span<const T,4> p) noexcept -> simd<T>;

    /// \brief Loads a SIMD value from a possibly unaligned array of values
    ///
    /// \param p a pointer to a sequence of values
    /// \return the simd value
    static auto load_unaligned(span<const T,4> p) noexcept -> simd<T>;

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
    auto store(span<T,4> out) const noexcept -> void;

    /// \brief Stores this SIMD object to the specified unaligned output buffer
    ///
    /// \param out the output buffer to store to
    auto store_unaligned(span<T,4> out) const noexcept -> void;

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

  /// \brief Reinterprets the underlying SIMD object into a different formed
  ///        SIMD object
  ///
  /// In general, this operation should be a no-op and not actually generate
  /// any assembly; it should only change how the data is viewed
  ///
  /// \param v the value to cast
  /// \return the casted value
  template <typename To, typename From>
  auto simd_cast(simd<To> v) noexcept -> simd<From>;

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
  ///        reciprocol of \p v
  ///
  /// \note For SSE, this operation is the same as `simd_reciprocol`. For
  ///       ARM NEON, this expands into an approximation of `1 / v`
  ///
  /// \param v the simd object to compute the reciprocol of
  /// \return the compted reciprocols in a simd object
  auto simd_fast_reciprocol(simd<f32> v) noexcept -> simd<f32>;

  /// \brief Performs a computation for the reciprocol of \p v
  ///
  /// \note For ARM NEON, the precision of this function is increased by adding
  ///       an additional Newton-Raphson iteration to the approximation
  ///       returned from `simd_fast_reciprocol`.
  ///
  /// \param v the simd object to compute the reciprocol of
  /// \return the compted reciprocols in a simd object
  auto simd_reciprocol(simd<f32> v) noexcept -> simd<f32>;

  //---------------------------------------------------------------------------

  /// \brief Computes a fast, and possibly imprecise, sqrt
  ///
  /// \note For SSE, this operation is the same as `simd_sqrt`. For ARM NEON,
  ///       this expands into an approximation of `sqrt`.
  ///
  /// \param v the values to square-root
  /// \return the square-root result in a simd object
  auto simd_fast_sqrt(simd<f32> v) noexcept -> simd<f32>;

  /// \brief Computes a precise sqrt, at a possible performance cost
  ///
  /// \note For ARM NEON, the precision of this function is increased by adding
  ///       an additional Newton-Raphson iteration to the approximation
  ///       returned from `simd_fast_sqrt`.
  ///
  /// \param v the values to square-root
  /// \return the square-root result in a simd object
  auto simd_sqrt(simd<f32> v) noexcept -> simd<f32>;

  //---------------------------------------------------------------------------

  /// \brief Computes the reciprocol sqrt value of \p v using a fast, but
  ///        potentially imprecise, method.
  ///
  /// \note For SSE, this is the same as `simd_reciprocol_sqrt`. For ARM, this
  ///       is a composition of `simd_fast_sqrt` and `simd_fast_reciprocol`,
  ///       which will have an accumulated error.
  ///
  /// \param v the values to compute the reciprocol sqrt for
  /// \return the computed result in a simd object
  auto simd_fast_reciprocol_sqrt(simd<f32> v) noexcept -> simd<f32>;

  /// \brief Computes the reciprocol sqrt value of \p v using a more precies
  ///        method, potentially at a performance impact
  ///
  /// \note For non-SSE architectures, this is a composition of `simd_sqrt` and
  ///       `simd_reciprocol`.
  ///
  /// \param v the values to compute the reciprocol sqrt for
  /// \return the computed result in a simd object
  auto simd_reciprocol_sqrt(simd<f32> f) noexcept -> simd<f32>;

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

  auto simd_dot4(span<const simd<f32>,4> lhs, span<const simd<f32>,4> rhs)
    noexcept -> simd<f32>;

  auto simd_dot4(span<const f32,16> lhs, span<const f32,16> rhs)
    noexcept -> simd<f32>;

  //---------------------------------------------------------------------------

  template <std::size_t N,
            typename = std::enable_if_t<((N % 4) == 0)>>
  auto simd_dot(span<const simd<f32>, N> lhs, span<const simd<f32>, N> rhs)
    noexcept -> std::array<simd<f32>, (N / 4)>;

  template <std::size_t N,
            typename = std::enable_if_t<((N % 4) == 0)>>
  auto simd_dot(span<const f32, N> lhs, span<const f32, N> rhs)
    noexcept -> std::array<simd<f32>, (N / 16)>;

  //---------------------------------------------------------------------------

  template <std::size_t N,
            typename = std::enable_if_t<(N % 16 == 0)>>
  auto simd_dot4_to(
    span<const f32,N> lhs,
    span<const f32,N> rhs,
    span<f32,(N / 4)> out
  ) noexcept -> void;

  template <std::size_t N,
            typename = std::enable_if_t<(N % 16 == 0)>>
  auto simd_dot4_to(
    span<const f32,N> lhs,
    span<const f32,N> rhs,
    span<simd<f32>,(N/16)> out
  ) noexcept -> void;

  template <std::size_t N,
            typename = std::enable_if_t<((N % 4) == 0)>>
  auto simd_dot4_to(
    span<simd<f32>,N> lhs,
    span<simd<f32>,N> rhs,
    span<simd<f32>,N> out
  ) noexcept -> void;

} // namespace alloy::core

//-----------------------------------------------------------------------------
// Loading
//-----------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::load(span<const T, 4> p)
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
    return simd<T>{_mm_load_epi32(compiler::assume_aligned<16u>(p.data()))};
  }
#else
  return set(p[0], p[1], p[2], p[3]);
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::load_unaligned(span<const T, 4> p)
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
    return simd<T>{_mm_loadu_epi32(p.data())};
  }
#else
  return set(p[0], p[1], p[2], p[3]);
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::spread_scalar(T scalar)
  noexcept -> simd<T>
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
    return simd<T>{_mm_set1_epi32(scalar)};
  }
#else
  return set(scalar, scalar, scalar, scalar);
#endif
}

template <typename T>
template <std::size_t Lane>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::spread_from_lane(simd<T> v)
  noexcept -> simd<T>
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
    return simd<T>{_mm_shuffle_epi32(v.value, v.value, _MM_SHUFFLE(Lane, Lane, Lane, Lane))};
  }
#else
  return set(
    v[Lane],
    v[Lane],
    v[Lane],
    v[Lane]
  );
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::set(T v0, T v1, T v2, T v3)
  noexcept -> simd<T>
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
    return simd<T>{_mm_set_ps(v0, v1, v2, v3)};
  } else {
    return simd<T>{_mm_set_epi32(v0, v1, v2, v3)};
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
auto alloy::core::simd<T>::store(span<T,4> out)
  const noexcept -> void
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
    _mm_store_epi32(compiler::assume_aligned<16>(out.data()), value);
  }
#else
  store_unaligned(out);
#endif
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd<T>::store_unaligned(span<T,4> out)
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
    _mm_storeu_epi32(out.data(), value);
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
auto alloy::core::simd<T>::lane()
  const noexcept -> T
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
    return _mm_shuffle_ps(value, value, _MM_SHUFFLE(Lane, Lane, Lane, Lane));
  } else {
    return _mm_shuffle_epi32(value, value, _MM_SHUFFLE(Lane, Lane, Lane, Lane));
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
auto alloy::core::simd_cast(simd<To> v)
  noexcept -> simd<From>
{
  if constexpr (std::is_same_v<To,From>) {
    return v;
  }
#if defined(ALLOY_SIMD_NEON)
  if constexpr (std::is_same_v<To, f32>) {
    if constexpr (std::is_same_v<From, u32>) {
      return simd<To>{vreinterpretq_f32_u32(v.value)};
    } else if constexpr (std::is_same_v<From, s32>) {
      return simd<To>{vreinterpretq_f32_s32(v.value)};
    }
  } else if constexpr (std::is_same_v<To, u32>) {
    if constexpr (std::is_same_v<From, f32>) {
      return simd<To>{vreinterpretq_u32_f32(v.value)};
    } else if constexpr (std::is_same_v<From, s32>) {
      return simd<To>{vreinterpretq_u32_s32(v.value)};
    }
  } else if constexpr (std::is_same_v<To, s32>) {
    if constexpr (std::is_same_v<From, f32>) {
      return simd<To>{vreinterpretq_s32_f32(v.value)};
    } else if constexpr (std::is_same_v<From, u32>) {
      return simd<To>{vreinterpretq_s32_u32(v.value)};
    }
  }
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<To, f32>) {
    return simd<To>{_mm_castsi128_ps(v.value)};
  } else if constexpr (std::is_same_v<To, u32> || std::is_same_v<To, s32>) {
    return simd<To>{_mm_castps_si128(v.value)};
  }
#else
  return simd<To>::set(
    reinterpret_cast<To>(v.template lane<0>()),
    reinterpret_cast<To>(v.template lane<1>()),
    reinterpret_cast<To>(v.template lane<2>()),
    reinterpret_cast<To>(v.template lane<3>())
  );
#endif
}

//-----------------------------------------------------------------------------
// Unary Arithmetic
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::simd_negate(simd<f32> v)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vnegq_f32(v.value)};
#elif defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_sub_ps(_mm_set1_ps(0.0f), v)};
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
auto alloy::core::simd_abs(simd<f32> v)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vabsq_f32(v.value)};
#elif defined(ALLOY_SIMD_SSE)
  const auto mask = _mm_castsi128_ps(_mm_set1_epi32(~(1u<<31u));
  return simd<f32>{_mm_and_ps(mask, v.value)};
#else
  return simd<f32>::set(
    -v.lane<0>(),
    -v.lane<1>(),
    -v.lane<2>(),
    -v.lane<3>()
  );
#endif
}

//-----------------------------------------------------------------------------
// Arithmetic
//-----------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd_add(simd<T> lhs, simd<T> rhs)
  noexcept -> simd<T>
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
auto alloy::core::simd_subtract(simd<T> lhs, simd<T> rhs)
  noexcept -> simd<T>
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
auto alloy::core::simd_multiply(simd<T> lhs, simd<T> rhs)
  noexcept -> simd<T>
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
    return simd<T>{_mm_mul_epu32(lhs.value, rhs.value)};
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
auto alloy::core::simd_fuse_add_multiply(simd<f32> add, simd<f32> lhs, simd<f32> rhs)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vmlaq_f32(add.value, lhs.value, rhs.value)};
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
auto alloy::core::simd_fuse_add_multiply_scalar(simd<f32> add, simd<f32> lhs, f32 rhs)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vmlaq_n_f32(add.value, lhs.value, rhs)};
#else
  return simd_fuse_add_multiply(add, lhs, simd<f32>::spread_scalar(rhs));
#endif
}

//-----------------------------------------------------------------------------
// Comparisons
//-----------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::simd_equal(simd<T> lhs, simd<T> rhs)
  noexcept -> simd<u32>
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
    return simd<u32>{_mm_castps_si128(_mm_cmpeq_epi32(lhs.value, rhs.value))};
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
auto alloy::core::simd_not_equal(simd<T> lhs, simd<T> rhs)
  noexcept -> simd<u32>
{
#if defined(ALLOY_SIMD_NEON)
  // NEON for some reason does not have a `not_equal`, so this is done as a
  // composition of the negation of an equal comparison.
  return simd<u32>{
    vmvnq_u32(simd_equal(lhs.value, rhs.value))
  };
#elif defined(ALLOY_SIMD_SSE)
  if constexpr (std::is_same_v<T,f32>) {
    return simd<u32>{_mm_castps_si128(_mm_cmpneq_ps(lhs.value, rhs.value))};
  } else {
    return simd<u32>{_mm_castps_si128(_mm_cmpneq_epi128(lhs.value, rhs.value))};
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
auto alloy::core::simd_less(simd<T> lhs, simd<T> rhs)
  noexcept -> simd<u32>
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
    return simd<u32>{_mm_castps_si128(_mm_cmplt_epi32(lhs.value, rhs.value))};
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
auto alloy::core::simd_greater(simd<T> lhs, simd<T> rhs)
  noexcept -> simd<u32>
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
    return simd<u32>{_mm_castps_si128(_mm_cmpgt_epi32(lhs.value, rhs.value))};
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
    return simd<u32>{_mm_castps_si128(_mm_cmple_epi32(lhs.value, rhs.value))};
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
    return simd<u32>{_mm_castps_si128(_mm_cmpge_epi32(lhs.value, rhs.value))};
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
auto alloy::core::simd_almost_equal(simd<f32> lhs, simd<f32> rhs, simd<f32> tolerance)
  noexcept -> simd<u32>
{
  return simd_less_equal(simd_abs_difference(lhs, rhs), tolerance);
}

//-----------------------------------------------------------------------------
// Math and Algorithms
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::simd_fast_reciprocol(simd<f32> v)
  noexcept -> simd<f32>
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
auto alloy::core::simd_reciprocol(simd<f32> v)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  // Use a Newton-Raphson iteration to refine the estimated value
  // http://en.wikipedia.org/wiki/Division_algorithm#Newton.E2.80.93Raphson_division
  const auto reciprocal = vrecpeq_f32(v.value);
  return simd<f32>{vmulq_f32(vrecpsq_f32(v.value, reciprocal), reciprocal)};
#else
  return simd_fast_reciprocol(v);
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_fast_sqrt(simd<f32> v)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  return simd<f32>{vsqrtq_f32(v.value)};
#elif defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_sqrt_ps(v.value)};
#else
  return simd<f32>::set(
    sqrt(v.lane<0>()),
    sqrt(v.lane<1>()),
    sqrt(v.lane<2>()),
    sqrt(v.lane<3>())
  );
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_sqrt(simd<f32> v)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_NEON)
  // Approximate a more accurate SQRT
  const auto estimate0 = vrsqrteq_f32(v.value);
  const auto estimate1 = vmulq_f32(vrsqrtsq_f32(v.value, vmulq_f32(estimate0, estimate0)), estimate0);
  const auto estimate2 = vmulq_f32(vrsqrtsq_f32(v.value, vmulq_f32(estimate1, estimate1)), estimate1);
  return simd<f32>{vmulq_f32(v.value, estimate2)};
#else
  return simd_fast_sqrt(v);
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_fast_reciprocol_sqrt(simd<f32> v)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_rsqrt_ps(v.value)};
#else
  return simd_fast_reciprocol(simd_fast_sqrt(v));
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_reciprocol_sqrt(simd<f32> v)
  noexcept -> simd<f32>
{
#if defined(ALLOY_SIMD_SSE)
  return simd<f32>{_mm_rsqrt_ps(v.value)};
#else
  return simd_reciprocol(simd_sqrt(v));
#endif
}

ALLOY_FORCE_INLINE
auto alloy::core::simd_min(simd<f32> lhs, simd<f32> rhs)
  noexcept -> simd<f32>
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
auto alloy::core::simd_max(simd<f32> lhs, simd<f32> rhs)
  noexcept -> simd<f32>
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
auto alloy::core::simd_dot4(span<const simd<f32>,4> lhs, span<const simd<f32>,4> rhs)
  noexcept -> simd<f32>
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
auto alloy::core::simd_dot4(span<const f32,16> lhs, span<const f32,16> rhs)
  noexcept -> simd<f32>
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

template <std::size_t N, typename>
inline
auto alloy::core::simd_dot(span<const simd<f32>, N> lhs, span<const simd<f32>, N> rhs)
  noexcept -> std::array<simd<f32>, (N / 4)>
{
  // Fused multiply-add instructions result in a data-dependency/stall between
  // the result of the FMA instruction, since the result is fed into the next
  // instruction iteration.
  //
  // This is unravelled here to help ensure parallelism between the
  // multiplications and to minimize stalls when waiting on addition
  std::array<simd<f32>, N> intermediate;
  for (auto i = 0u; i < N; ++i) {
    intermediate[i] = simd_multiply(lhs[i], rhs[i]);
  }

  std::array<simd<f32>, (N/4)> dot_result;
  for (auto i = 0u; i < (N/4); ++i) {
    dot_result[i] = simd_add(
      simd_add(intermediate[i], intermediate[i+1]),
      simd_add(intermediate[i+2], intermediate[i+3])
    );
  }
  return dot_result;
}

template <std::size_t N, typename>
inline
auto alloy::core::simd_dot(span<const f32, N> lhs, span<const f32, N> rhs)
  noexcept -> std::array<simd<f32>, (N / 16)>
{
  std::array<simd<f32>, (N/16)> dot_result;

  for (auto i = 0u; i < (N/16); ++i) {
    dot_result[i] = simd_dot4(
      lhs.template first<16>(),
      rhs.template first<16>()
    );
    lhs = lhs.template subspan<16>();
    rhs = rhs.template subspan<16>();
  }

  return dot_result;
}


template <std::size_t N, typename>
inline
auto alloy::core::simd_dot4_to(
  span<const f32,N> lhs,
  span<const f32,N> rhs,
  span<f32,(N/4)> out
) noexcept -> void
{
  constexpr auto n = (N / 4);

  for (auto i = 0u; i < n; i++) {
    const auto result = simd_dot4(
      lhs.template first<16>(),
      rhs.template first<16>()
    );

    result.store(out.template first<4>());

    // Increment
    lhs = lhs.template subsban<16>();
    rhs = rhs.template subsban<16>();
    out = out.template subspan<4>();
  }
}


template <std::size_t N, typename>
inline
auto alloy::core::simd_dot4_to(
  span<const f32,N> lhs,
  span<const f32,N> rhs,
  span<simd<f32>,(N/16)> out
) noexcept -> void
{
  constexpr auto n = (N / 4);

  for (auto i = 0u; i < n; i++) {
    out[i] = simd_dot4(lhs.template first<16>(), rhs.template first<16>());

    // Increment span by 16
    lhs = lhs.template subsban<16>();
    rhs = rhs.template subsban<16>();
  }
}

template <std::size_t N, typename>
inline
auto alloy::core::simd_dot4_to(
  span<simd<f32>,N> lhs,
  span<simd<f32>,N> rhs,
  span<simd<f32>,N> out
) noexcept -> void
{
  for (auto i = 0u; i < N; i++) {
    out[i] = simd_dot4(lhs[i], rhs[i]);
  }
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_MATH_SIMD_HPP */
