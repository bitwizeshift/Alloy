// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/math/vector/vector4.hpp"

#include "alloy/core/math/math.hpp"

//=============================================================================
// class : vector4
//=============================================================================

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

auto alloy::core::vector4::magnitude() const noexcept -> float
{
  return sqrt( dot(*this) );
}
//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

auto alloy::core::vector4::normalize() noexcept -> vector4&
{
  const auto square_mag = square_magnitude();

  if (almost_equal(square_mag, 1.0f)) {
    return (*this);
  }

  if (square_mag > 0.0f) {
    const auto mag_inv = 1.0f / sqrt(square_mag);

    for (auto i = 0; i < 4; ++i) {
      m_data[i] *= mag_inv;
    }
  }

  return (*this);
}
