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

    static constexpr core::real half_pi() noexcept;
    static constexpr core::real pi() noexcept;
    static constexpr core::real two_pi() noexcept;
    static constexpr auto tau = &math_constants::two_pi;
  };

} // namespace alloy::core

inline constexpr alloy::core::real alloy::core::math_constants::half_pi()
  noexcept
{
  return pi() / 2.0_real;
}

inline constexpr alloy::core::real alloy::core::math_constants::pi()
  noexcept
{
  return 3.14159265358979323846264338327950288419716939937510_real;
}

inline constexpr alloy::core::real alloy::core::math_constants::two_pi()
  noexcept
{
  return 2.0_real * pi();
}

#endif /* ALLOY_CORE_MATH_MATH_CONSTANTS_HPP */