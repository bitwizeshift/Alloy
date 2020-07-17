////////////////////////////////////////////////////////////////////////////////
/// \file vector4_constants.hpp
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

#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR4_CONSTANTS_HPP
#define ALLOY_CORE_MATH_VECTOR_VECTOR4_CONSTANTS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/math/vector/vector4.hpp"

namespace alloy::core {

  //===========================================================================
  // static class : vector4_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief This static class contains all constants relating to the vector4
  ///        type as inline constexpr values
  /////////////////////////////////////////////////////////////////////////////
  struct vector4_constants
  {
    vector4_constants() = delete;
    ~vector4_constants() = delete;

    //-------------------------------------------------------------------------

    /// \brief A zero-valued vector4
    inline static constexpr auto zero = vector4{
      real{0}, real{0}, real{0}, real{0}
    };

    //-------------------------------------------------------------------------

    /// \brief A vector representing the X unit-vector
    inline static constexpr auto unit_x = vector4{
      real{1}, real{0}, real{0}, real{0}
    };

    /// \brief A vector representing the Y unit-vector
    inline static constexpr auto unit_y = vector4{
      real{0}, real{1}, real{0}, real{0}
    };

    /// \brief A vector representing the Z unit-vector
    inline static constexpr auto unit_z = vector4{
      real{0}, real{0}, real{1}, real{0}
    };

    /// \brief A vector representing the W unit-vector
    inline static constexpr auto unit_w = vector4{
      real{0}, real{0}, real{0}, real{1}
    };

    //-------------------------------------------------------------------------

    /// \brief A vector representing the negative X unit-vector
    inline static constexpr auto neg_unit_x = -unit_x;

    /// \brief A vector representing the negative Y unit-vector
    inline static constexpr auto neg_unit_y = -unit_y;

    /// \brief A vector representing the negative Z unit-vector
    inline static constexpr auto neg_unit_z = -unit_z;

    /// \brief A vector representing the negative W unit-vector
    inline static constexpr auto neg_unit_w = -unit_w;
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR4_CONSTANTS_HPP */
