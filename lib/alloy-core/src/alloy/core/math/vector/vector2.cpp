#include "alloy/core/math/vector/vector2.hpp"

//==============================================================================
// class : vector2
//==============================================================================

//------------------------------------------------------------------------------
// Static Constants
//------------------------------------------------------------------------------

const alloy::core::vector2 alloy::core::vector2::zero = {
  alloy::core::real{0},
  alloy::core::real{0}
};
const alloy::core::vector2 alloy::core::vector2::unit_x = {
  alloy::core::real{1},
  alloy::core::real{0}
};
const alloy::core::vector2 alloy::core::vector2::unit_y = {
  alloy::core::real{0},
  alloy::core::real{1}
};

const alloy::core::vector2 alloy::core::vector2::neg_unit_x
  = -alloy::core::vector2::unit_x;
const alloy::core::vector2 alloy::core::vector2::neg_unit_y
  = -alloy::core::vector2::unit_y;
