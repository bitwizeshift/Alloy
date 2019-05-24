#include "alloy/core/math/vector/vector4.hpp"

//==============================================================================
// class : vector4
//==============================================================================

//------------------------------------------------------------------------------
// Static Constants
//------------------------------------------------------------------------------

const alloy::core::vector4 alloy::core::vector4::zero = {
  alloy::core::real{0},
  alloy::core::real{0},
  alloy::core::real{0},
  alloy::core::real{0}
};
const alloy::core::vector4 alloy::core::vector4::unit_x = {
  alloy::core::real{1},
  alloy::core::real{0},
  alloy::core::real{0},
  alloy::core::real{0}
};
const alloy::core::vector4 alloy::core::vector4::unit_y = {
  alloy::core::real{0},
  alloy::core::real{1},
  alloy::core::real{0},
  alloy::core::real{0}
};
const alloy::core::vector4 alloy::core::vector4::unit_z = {
  alloy::core::real{0},
  alloy::core::real{0},
  alloy::core::real{1},
  alloy::core::real{0}
};
const alloy::core::vector4 alloy::core::vector4::unit_w = {
  alloy::core::real{0},
  alloy::core::real{0},
  alloy::core::real{0},
  alloy::core::real{1}
};

const alloy::core::vector4 alloy::core::vector4::neg_unit_x
  = -alloy::core::vector4::unit_x;
const alloy::core::vector4 alloy::core::vector4::neg_unit_y
  = -alloy::core::vector4::unit_y;
const alloy::core::vector4 alloy::core::vector4::neg_unit_z
  = -alloy::core::vector4::unit_z;
const alloy::core::vector4 alloy::core::vector4::neg_unit_w
  = -alloy::core::vector4::unit_w;
