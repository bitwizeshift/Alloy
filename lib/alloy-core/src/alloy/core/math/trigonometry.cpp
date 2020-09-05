#include "alloy/core/math/trigonometry.hpp"

#include <cmath>

//============================================================================
// definitions : static class : trigonometry
//============================================================================

//----------------------------------------------------------------------------
// Trig
//----------------------------------------------------------------------------

auto alloy::core::trigonometry::cos(radian rad)
  noexcept -> real
{
  return std::cos( rad.value() );
}

auto alloy::core::trigonometry::sin(radian rad)
  noexcept -> real
{
  return std::sin( rad.value() );
}

auto alloy::core::trigonometry::tan(radian rad)
  noexcept -> real
{
  return std::tan( rad.value() );
}

//----------------------------------------------------------------------------
// Inverse Trig
//----------------------------------------------------------------------------

auto alloy::core::trigonometry::arccos(core::real f)
  noexcept -> radian
{
  return radian{ std::acos(f) };
}

auto alloy::core::trigonometry::arcsin(core::real f)
  noexcept -> radian
{
  return radian{ std::asin(f) };
}

auto alloy::core::trigonometry::arctan(core::real f)
  noexcept -> radian
{
  return radian{ std::atan(f) };
}

auto alloy::core::trigonometry::arctan2(core::real f1, core::real f2)
  noexcept -> radian
{
  return radian{ std::atan2(f1,f2) };
}
