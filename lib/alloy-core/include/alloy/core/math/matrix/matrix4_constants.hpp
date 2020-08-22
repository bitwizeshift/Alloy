////////////////////////////////////////////////////////////////////////////////
/// \file matrix4_constants.hpp
///
/// \brief TODO(Bitwize): Add description
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

#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX4_CONSTANTS_HPP
#define ALLOY_CORE_MATH_MATRIX_MATRIX4_CONSTANTS_HPP

#include "alloy/core/math/matrix/matrix4.hpp"

namespace alloy::core {

  //===========================================================================
  // static class : matrix4_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class used for storing matrix constants
  /////////////////////////////////////////////////////////////////////////////
  struct matrix4_constants
  {
    matrix4_constants() = delete;
    ~matrix4_constants() = delete;

    inline static constexpr auto identity = matrix4{
      real{1}, real{0}, real{0}, real{0},
      real{0}, real{1}, real{0}, real{0},
      real{0}, real{0}, real{1}, real{0},
      real{0}, real{0}, real{0}, real{1},
    };
  };
} // namespace alloy::core

#endif /* ALLOY_CORE_MATH_MATRIX_MATRIX4_CONSTANTS_HPP */
