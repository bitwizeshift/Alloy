/*****************************************************************************
 * \file direction_light.hpp
 *
 * \todo description
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew ALLOY_CORE_MODEL_ All rights reserved.

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
#ifndef ALLOY_CORE_MODEL_LIGHT_DIRECTION_LIGHT_HPP
#define ALLOY_CORE_MODEL_LIGHT_DIRECTION_LIGHT_HPP

#include "alloy/render/light/light.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/model/color.hpp"

namespace alloy::render {

  //============================================================================
  // struct : direction_light
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A light that shines in a specific direction, but has no specific
  ///        source location
  //////////////////////////////////////////////////////////////////////////////
  struct direction_light
  {
    core::vector3 direction;
    core::color color;
    light::attenuation attenuation;
    light::intensity intensity;
  };

} // namespace alloy::render

#endif /* ALLOY_CORE_MODEL_LIGHT_DIRECTION_LIGHT_HPP */
