// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/math/trigonometry.hpp"

#include "alloy/core/math/angle/radian.hpp"

#include <cmath>

//============================================================================
// definitions : static class : trigonometry
//============================================================================

//----------------------------------------------------------------------------
// Trig
//----------------------------------------------------------------------------

auto alloy::core::trigonometry::cos(radian rad) noexcept -> float
{
  return std::cos( rad.value() );
}

auto alloy::core::trigonometry::sin(radian rad) noexcept -> float
{
  return std::sin( rad.value() );
}

auto alloy::core::trigonometry::tan(radian rad) noexcept -> float
{
  return std::tan( rad.value() );
}

//----------------------------------------------------------------------------
// Inverse Trig
//----------------------------------------------------------------------------

auto alloy::core::trigonometry::arccos(float f) noexcept -> radian
{
  return radian{ std::acos(f) };
}

auto alloy::core::trigonometry::arcsin(float f) noexcept -> radian
{
  return radian{ std::asin(f) };
}

auto alloy::core::trigonometry::arctan(float f) noexcept -> radian
{
  return radian{ std::atan(f) };
}

auto alloy::core::trigonometry::arctan2(float f1, float f2)
  noexcept -> radian
{
  return radian{ std::atan2(f1,f2) };
}
