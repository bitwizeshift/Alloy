////////////////////////////////////////////////////////////////////////////////
/// \file frustum.cpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

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

#include "alloy/core/geometry/frustum.hpp"

auto alloy::core::frustum::contains(const point& p)
  const noexcept -> bool
{
  for (const auto& plane : m_planes) {
    if (plane.distance_to(p) < 0) {
      return false;
    }
  }
  return true;
}

auto alloy::core::frustum::make(const plane& left, const plane& right,
                                const plane& top,  const plane& bottom,
                                const plane& near, const plane& far)
  -> frustum
{
  ALLOY_ALWAYS_ASSERT(left.normal().dot(right.normal()) < 0);
  ALLOY_ALWAYS_ASSERT(top.normal().dot(bottom.normal()) < 0);
  ALLOY_ALWAYS_ASSERT(near.normal().dot(far.normal()) < 0);

  return make_unchecked(left, right, top, bottom, near, far);
}

