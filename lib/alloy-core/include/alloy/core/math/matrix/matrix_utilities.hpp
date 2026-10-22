// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/****************************************************************************
 * \file matrix_utilities.hpp
 *
 * \brief This header provides the defintion for various matrix utilities.
 ****************************************************************************/

#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX_UTILITIES_HPP
#define ALLOY_CORE_MATH_MATRIX_MATRIX_UTILITIES_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/matrix/matrix2.hpp"
#include "alloy/core/math/matrix/matrix3.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"

#include "alloy/core/precision/real.hpp"

#include <type_traits> // std::true_type

namespace alloy::core {

  //----------------------------------------------------------------------------
  // Casting
  //----------------------------------------------------------------------------

  inline namespace casts {

    /// \brief Casts a matrix from type From to To
    ///
    /// \tparam To the type to cast to
    /// \param from the matrix being cast from
    /// \return a constructed matrix
    template<typename To, typename From>
    constexpr auto matrix_cast(const From& from) noexcept -> To;

  } // inline namespace casts

  //============================================================================
  // trait : is_matrix
  //============================================================================

  /// \brief Trait to detect whether \p T is a matrix type.
  ///
  /// The result is aliased as `::value`
  template<typename T>
  struct is_matrix : std::false_type{};

  template<>
  struct is_matrix<matrix2> : std::true_type{};
  template<>
  struct is_matrix<matrix3> : std::true_type{};
  template<>
  struct is_matrix<matrix4> : std::true_type{};

  /// \brief Convenience template variable to extract out
  ///        `is_matrix<T>::value`
  template<typename T>
  inline constexpr bool is_matrix_v = is_matrix<T>::value;

} // namespace alloy::core


namespace alloy::core::detail {

  template<typename To, typename From>
  struct matrix_caster;

  //----------------------------------------------------------------------------

  template<>
  struct matrix_caster<matrix2,matrix2>
  {
    static constexpr
    auto cast(const matrix2& from) noexcept -> matrix2
    {
      return from;
    }
  };

  template<>
  struct matrix_caster<matrix3,matrix2>
  {
    static constexpr
    auto cast(const matrix2& from) noexcept -> matrix3
    {
      const real entries[3][3] = {
        {from[0][0], from[0][1], real{0}},
        {from[1][0], from[1][1], real{0}},
        {real{0},    real{0},    real{1}}
      };

      return matrix3{entries};
    }
  };

  template<>
  struct matrix_caster<matrix4,matrix2>
  {
    static constexpr
    auto cast(const matrix2& from) noexcept -> matrix4
    {
      const real entries[4][4] = {
        {from[0][0], from[0][1], real{0}, real{0}},
        {from[1][0], from[1][1], real{0}, real{0}},
        {real{0},    real{0},    real{1}, real{0}},
        {real{0},    real{0},    real{0}, real{1}}
      };

      return matrix4{entries};
    }
  };

  //----------------------------------------------------------------------------

  template<>
  struct matrix_caster<matrix2,matrix3>
  {
    static constexpr
    auto cast(const matrix3& from) noexcept -> matrix2
    {
      const real entries[2][2] = {
        {from[0][0], from[0][1]},
        {from[1][0], from[1][1]}
      };

      return matrix2{entries};
    }
  };

  template<>
  struct matrix_caster<matrix3,matrix3>
  {
    static constexpr
    auto cast(const matrix3& from) noexcept -> matrix3
    {
      return from;
    }
  };

  template<>
  struct matrix_caster<matrix4,matrix3>
  {
    static constexpr
    auto cast(const matrix3& from) noexcept -> matrix4
    {
      const real entries[4][4] = {
        {from[0][0], from[0][1], from[0][2], real{0}},
        {from[1][0], from[1][1], from[1][2], real{0}},
        {from[2][0], from[2][1], from[2][2], real{0}},
        {real{0},    real{0},    real{0},    real{1}}
      };

      return matrix4{entries};
    }
  };

  //----------------------------------------------------------------------------

  template<>
  struct matrix_caster<matrix2,matrix4>
  {
    static constexpr
    auto cast(const matrix4& from) noexcept -> matrix2
    {
      const real entries[2][2] = {
        {from[0][0], from[0][1]},
        {from[1][0], from[1][1]}
      };

      return matrix2{entries};
    }
  };

  template<>
  struct matrix_caster<matrix3,matrix4>
  {
    static constexpr
    auto cast(const matrix4& from) noexcept -> matrix3
    {
      const real entries[3][3] = {
        {from[0][0], from[0][1], from[0][2]},
        {from[1][0], from[1][1], from[1][2]},
        {from[2][0], from[2][1], from[2][2]}
      };

      return matrix3{entries};
    }
  };

  template<>
  struct matrix_caster<matrix4,matrix4>
  {
    static constexpr
    auto cast(const matrix4& from) noexcept -> matrix4
    {
      return from;
    }
  };

} // namespace alloy::core::detail

//------------------------------------------------------------------------------
// Vector Casting
//------------------------------------------------------------------------------

template<typename To, typename From>
inline constexpr
auto alloy::core::casts::matrix_cast(const From& from) noexcept -> To
{
  return detail::matrix_caster<To,From>::cast(from);
}

#endif /* ALLOY_CORE_MATH_MATRIX_MATRIX_UTILITIES_HPP */

