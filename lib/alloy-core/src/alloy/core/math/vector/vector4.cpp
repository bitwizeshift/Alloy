#include "alloy/core/math/vector/vector4.hpp"

//=============================================================================
// class : vector4
//=============================================================================

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

auto alloy::core::vector4::magnitude()
  const noexcept -> real
{
  return sqrt( dot(*this) );
}
//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

auto alloy::core::vector4::normalize()
  noexcept -> vector4&
{
  const auto square_mag = square_magnitude();

  if (almost_equal(square_mag, real{1})) {
    return (*this);
  }

  if (square_mag > real{0}) {
    const auto mag_inv = real{1} / sqrt(square_mag);

    for (auto i = 0; i < 4; ++i) {
      m_data[i] *= mag_inv;
    }
  }

  return (*this);
}
