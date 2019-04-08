#include "alloy/core/geometry/line_segment.hpp"
#include "alloy/core/math/math.hpp"

//==============================================================================
// class : line_segment
//==============================================================================

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

alloy::core::real alloy::core::line_segment::magnitude()
  const noexcept
{
  return sqrt(square_magnitude());
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

alloy::core::point alloy::core::line_segment::point_at_percent( real percent )
  const noexcept
{
  percent = saturate(percent);

  return m_start + (percent * direction());
}
