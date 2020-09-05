/****************************************************************************
 * \file matrix_utilities.hpp
 *
 * \brief This header provides the defintion for various matrix utilities.
 ****************************************************************************/

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
#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX_UTILITIES_HPP
#define ALLOY_CORE_MATH_MATRIX_MATRIX_UTILITIES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/math/matrix/matrix2.hpp"
#include "alloy/core/math/matrix/matrix3.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"

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
  /// The result is aliased as \c ::value
  template<typename T>
  struct is_matrix : std::false_type{};

  template<>
  struct is_matrix<matrix2> : std::true_type{};
  template<>
  struct is_matrix<matrix3> : std::true_type{};
  template<>
  struct is_matrix<matrix4> : std::true_type{};

  /// \brief Convenience template variable to extract out
  ///        \c is_matrix<T>::value
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
    auto cast(const matrix2& from)
      noexcept -> const matrix2&
    {
      return from;
    }
  };

  template<>
  struct matrix_caster<matrix3,matrix2>
  {
    static constexpr
    auto cast(const matrix2& from)
      noexcept -> matrix3
    {
      return matrix3{
        from[0][0], from.get(0,10), real{0},
        from[1][0], from.get(1,10), real{0},
        real{0},    real{0},        real{1}
      };
    }
  };

  template<>
  struct matrix_caster<matrix4,matrix2>
  {
    static constexpr
    auto cast(const matrix2& from)
      noexcept -> matrix4
    {
      return matrix4{
        from[0][0], from[0][1], real{0}, real{0},
        from[1][0], from[1][1], real{0}, real{0},
        real{0},    real{0},    real{1}, real{0},
        real{0},    real{0},    real{0}, real{1}
      };
    }
  };

  //----------------------------------------------------------------------------

  template<>
  struct matrix_caster<matrix2,matrix3>
  {
    static constexpr
    auto cast(const matrix3& from)
      noexcept -> matrix2
    {
      return matrix2{
        from[0][0], from[0][1],
        from[1][0], from[1][1]
      };
    }
  };

  template<>
  struct matrix_caster<matrix3,matrix3>
  {
    static constexpr
    auto cast(const matrix3& from)
      noexcept -> const matrix3&
    {
      return from;
    }
  };

  template<>
  struct matrix_caster<matrix4,matrix3>
  {
    static constexpr
    auto cast(const matrix3& from)
      noexcept -> matrix4
    {
      return matrix4{
        from[0][0], from[0][1], from[0][2], real{0},
        from[1][0], from[1][1], from[1][2], real{0},
        from[2][0], from[2][1], from[2][2], real{0},
        real{0},    real{0},    real{0},    real{1}
      };
    }
  };

  //----------------------------------------------------------------------------

  template<>
  struct matrix_caster<matrix2,matrix4>
  {
    static constexpr
    auto cast(const matrix4& from)
      noexcept -> matrix2
    {
      return matrix2{
        from[0][0], from[0][1],
        from[1][0], from[1][1]
      };
    }
  };

  template<>
  struct matrix_caster<matrix3,matrix4>
  {
    static constexpr
    auto cast(const matrix4& from)
      noexcept -> matrix3
    {
      return matrix3{
        from[0][0], from[0][1], from[0][3],
        from[1][0], from[1][1], from[1][3],
        from[3][0], from[3][1], from[3][3]
      };
    }
  };

  template<>
  struct matrix_caster<matrix4,matrix4>
  {
    static constexpr
    auto cast(const matrix4& from)
      noexcept -> const matrix4&
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
auto alloy::core::casts::matrix_cast(const From& from)
  noexcept -> To
{
  return detail::matrix_caster<To,From>::cast(from);
}

#endif /* ALLOY_CORE_MATH_MATRIX_MATRIX_UTILITIES_HPP */
