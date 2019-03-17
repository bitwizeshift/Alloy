#include "alloy/core/trigonometry.hpp"

#include <cmath>

//============================================================================
// definitions : static class : trigonometry
//============================================================================

//----------------------------------------------------------------------------
// Trig
//----------------------------------------------------------------------------

alloy::core::real alloy::core::trigonometry::cos( radian rad )
  noexcept
{
  return std::cos( rad.value() );
}

alloy::core::real alloy::core::trigonometry::sin( radian rad )
  noexcept
{
  return std::sin( rad.value() );
}

alloy::core::real alloy::core::trigonometry::tan( radian rad )
  noexcept
{
  return std::tan( rad.value() );
}

//----------------------------------------------------------------------------
// Inverse Trig
//----------------------------------------------------------------------------

alloy::core::radian alloy::core::trigonometry::arccos( core::real f )
  noexcept
{
  return radian{ std::acos(f) };
}

alloy::core::radian alloy::core::trigonometry::arcsin( core::real f )
  noexcept
{
  return radian{ std::asin(f) };
}

alloy::core::radian alloy::core::trigonometry::arctan( core::real f )
  noexcept
{
  return radian{ std::atan(f) };
}

alloy::core::radian alloy::core::trigonometry::arctan2( core::real f1,
                                                        core::real f2 )
  noexcept
{
  return radian{ std::atan2(f1,f2) };
}
