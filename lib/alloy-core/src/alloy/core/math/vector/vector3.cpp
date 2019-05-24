#include "alloy/core/math/vector/vector3.hpp"

//==============================================================================
// class : vector3
//==============================================================================

//------------------------------------------------------------------------------
// Static Constants
//------------------------------------------------------------------------------

const alloy::core::vector3 alloy::core::vector3::zero = {
  alloy::core::real{0},
  alloy::core::real{0},
  alloy::core::real{0}
};
const alloy::core::vector3 alloy::core::vector3::unit_x = {
  alloy::core::real{1},
  alloy::core::real{0},
  alloy::core::real{0}
};
const alloy::core::vector3 alloy::core::vector3::unit_y = {
  alloy::core::real{0},
  alloy::core::real{1},
  alloy::core::real{0}
};
const alloy::core::vector3 alloy::core::vector3::unit_z = {
  alloy::core::real{0},
  alloy::core::real{0},
  alloy::core::real{1}
};

const alloy::core::vector3 alloy::core::vector3::neg_unit_x
  = -alloy::core::vector3::unit_x;
const alloy::core::vector3 alloy::core::vector3::neg_unit_y
  = -alloy::core::vector3::unit_y;
const alloy::core::vector3 alloy::core::vector3::neg_unit_z
  = -alloy::core::vector3::unit_z;
