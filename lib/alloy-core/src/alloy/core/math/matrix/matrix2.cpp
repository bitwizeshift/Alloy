// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/math/matrix/matrix2.hpp"

#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector2.hpp"

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::matrix2::determinant() const noexcept -> float
{
  return (get(0,0) * get(1,1)) - (get(0,1) * get(1,0));
}

auto alloy::core::matrix2::trace() const noexcept -> float
{
  return (get(0,0) + get(1,1));
}

auto alloy::core::matrix2::inverse() const noexcept -> matrix2
{
  const auto det = determinant();

  if (almost_equal(det, 0.0f)) {
    return matrix2{
      1.0f, 0.0f,
      0.0f, 1.0f
    };
  }

  const auto inv_det = (1.0f / det);

  return matrix2{
     get(1,1) * inv_det, -get(0,1) * inv_det,
    -get(1,0) * inv_det,  get(0,0) * inv_det
  };
}

auto alloy::core::matrix2::transposed() const noexcept -> matrix2
{
  return matrix2{
    get(0,0), get(1,0),
    get(0,1), get(1,1)
  };
}

auto alloy::core::matrix2::combine(const vector2& vec) const noexcept -> vector2
{
  auto result = vector2{};

  for (auto c = 0; c < columns; ++c) {
    for (auto r = 0; r < rows; ++r) {
      result[r] += get(r,c) * vec[c];
    }
  }
  return result;
}
