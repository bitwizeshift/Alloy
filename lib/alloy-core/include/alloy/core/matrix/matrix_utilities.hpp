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
#ifndef ALLOY_CORE_MATRIX_MATRIX_UTILITIES_HPP
#define ALLOY_CORE_MATRIX_MATRIX_UTILITIES_HPP

#include "matrix2.hpp"
#include "matrix3.hpp"
#include "matrix4.hpp"

namespace alloy::core {

  //----------------------------------------------------------------------------
  // Casting
  //----------------------------------------------------------------------------

  inline namespace casts {

    template<typename To, typename From>
    constexpr To matrix_cast(const From& from) noexcept;

  } // inline namespace casts

  //============================================================================
  // struct : matrix2_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of matrix2 constants
  //////////////////////////////////////////////////////////////////////////////
  struct matrix2_constants
  {
    //--------------------------------------------------------------------------
    // Public Constants
    //--------------------------------------------------------------------------

    static inline constexpr auto zero = matrix2{
      real{0}, real{0},
      real{0}, real{0}
    };
    static inline constexpr auto identity = matrix2{
      real{1}, real{0},
      real{0}, real{1},
    };
  };

  //============================================================================
  // struct : matrix3_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of matrix3 constants
  //////////////////////////////////////////////////////////////////////////////
  struct matrix3_constants
  {
    //--------------------------------------------------------------------------
    // Public Constants
    //--------------------------------------------------------------------------

    static inline constexpr auto zero = matrix3{
      real{0}, real{0}, real{0},
      real{0}, real{0}, real{0},
      real{0}, real{0}, real{0}
    };
    static inline constexpr auto identity = matrix3{
      real{1}, real{0}, real{0},
      real{0}, real{1}, real{0},
      real{0}, real{0}, real{1}
    };
  };

  //============================================================================
  // struct : matrix4_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of matrix4 constants
  //////////////////////////////////////////////////////////////////////////////
  struct matrix4_constants
  {
    //--------------------------------------------------------------------------
    // Public Constants
    //--------------------------------------------------------------------------

    static inline constexpr auto zero = matrix4{
      real{0}, real{0}, real{0}, real{0},
      real{0}, real{0}, real{0}, real{0},
      real{0}, real{0}, real{0}, real{0},
      real{0}, real{0}, real{0}, real{0}
    };
    static inline constexpr auto identity = matrix4{
      real{1}, real{0}, real{0}, real{0},
      real{0}, real{1}, real{0}, real{0},
      real{0}, real{0}, real{1}, real{0},
      real{0}, real{0}, real{0}, real{1}
    };
  };

  //============================================================================
  // aliases
  //============================================================================

  using mat2_constants = matrix2_constants;
  using mat3_constants = matrix3_constants;
  using mat4_constants = matrix4_constants;

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

#include "detail/matrix_utilities.inl"

#endif /* ALLOY_CORE_MATRIX_MATRIX_UTILITIES_HPP */