#include "alloy/math/trigonometry.hpp"

#include <cmath>

//============================================================================
// definitions : static class : trigonometry
//============================================================================

//----------------------------------------------------------------------------
// Trig
//----------------------------------------------------------------------------

alloy::core::real alloy::math::trigonometry::cos( radian rad )
  noexcept
{
  return std::cos( rad.value() );
}

alloy::core::real alloy::math::trigonometry::sin( radian rad )
  noexcept
{
  return std::sin( rad.value() );
}

alloy::core::real alloy::math::trigonometry::tan( radian rad )
  noexcept
{
  return std::tan( rad.value() );
}

//----------------------------------------------------------------------------
// Inverse Trig
//----------------------------------------------------------------------------

alloy::math::radian alloy::math::trigonometry::arccos( core::real f )
  noexcept
{
  return radian{ std::acos(f) };
}

alloy::math::radian alloy::math::trigonometry::arcsin( core::real f )
  noexcept
{
  return radian{ std::asin(f) };
}

alloy::math::radian alloy::math::trigonometry::arctan( core::real f )
  noexcept
{
  return radian{ std::atan(f) };
}

alloy::math::radian alloy::math::trigonometry::arctan2( core::real f1,
                                                        core::real f2 )
  noexcept
{
  return radian{ std::atan2(f1,f2) };
}
