/*****************************************************************************
 * \file math_constants.hpp
 *
 * \brief This header defines mathematical constants required elsewhere
 *        within the alloy engine.
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
#ifndef ALLOY_CORE_MATH_MATH_CONSTANTS_HPP
#define ALLOY_CORE_MATH_MATH_CONSTANTS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"

namespace alloy::core {

  //===========================================================================
  // static class : constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A pure static-class that contains functions for receiving
  ///        mathematical constants.
  /////////////////////////////////////////////////////////////////////////////
  struct math_constants
  {
    math_constants() = delete;
    ~math_constants() = delete;

    inline static constexpr auto pi = 3.1415926535897932384626433832795028_real;
    inline static constexpr auto half_pi = pi / 2;
    inline static constexpr auto two_pi = 2 * pi;

    inline static constexpr auto tau = two_pi;
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MATH_MATH_CONSTANTS_HPP */
