// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/math/matrix/matrix3.hpp"

#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector3.hpp"

#include <utility>


//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::matrix3::determinant() const noexcept -> float
{
  return (get(0,0)*get(1,1)*get(2,2)) -
         (get(0,0)*get(1,2)*get(2,1)) -
         (get(0,1)*get(1,0)*get(2,2)) +
         (get(0,1)*get(1,2)*get(2,0)) +
         (get(0,2)*get(1,0)*get(2,1)) -
         (get(0,2)*get(1,1)*get(2,0));
}

auto alloy::core::matrix3::trace() const noexcept -> float
{
  return (get(0,0) + get(1,1) + get(2,2));
}

auto alloy::core::matrix3::inverse() const noexcept -> matrix3
{
  const auto det = determinant();

  if (almost_equal(det,0.0f)) {
    return matrix3{
      1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 1.0f
    };
  }

  const auto inv_det = (1.0f / det);

  return matrix3{
    (get(1,1)*get(2,2) - get(1,2)*get(2,1)) * inv_det,
    (get(0,2)*get(2,1) - get(0,1)*get(2,2)) * inv_det,
    (get(0,1)*get(1,2) - get(0,2)*get(1,1)) * inv_det,

    (get(1,2)*get(2,0) - get(1,0)*get(2,2)) * inv_det,
    (get(0,0)*get(2,2) - get(0,2)*get(2,0)) * inv_det,
    (get(0,2)*get(1,0) - get(0,0)*get(1,2)) * inv_det,

    (get(1,0)*get(2,1) - get(1,1)*get(2,0)) * inv_det,
    (get(0,1)*get(2,0) - get(0,0)*get(2,1)) * inv_det,
    (get(0,0)*get(1,1) - get(0,1)*get(1,0)) * inv_det
  };
}

auto alloy::core::matrix3::transposed() const noexcept -> matrix3
{
  return matrix3{
    get(0,0), get(1,0), get(2,0),
    get(0,1), get(1,1), get(2,1),
    get(0,2), get(1,2), get(2,2)
  };
}

auto alloy::core::matrix3::combine(const vector3& vec) const noexcept -> vector3
{
  auto result = vector3{};

  for (auto c = 0; c < columns; ++c) {
    for (auto r = 0; r < rows; ++r) {
      result[r] += get(r,c) * vec[c];
    }
  }
  return result;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::core::matrix3::transpose() noexcept -> matrix3&
{
  using std::swap;

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < r; ++c) {
      if (r != c) {
        swap(get(r,c),get(c,r));
      }
    }
  }
  return (*this);
}
