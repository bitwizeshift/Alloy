#include "alloy/core/math/matrix/matrix3.hpp"


//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::matrix3::determinant()
  const noexcept -> real
{
  return (get(0,0)*get(1,1)*get(2,2)) -
         (get(0,0)*get(1,2)*get(2,1)) -
         (get(0,1)*get(1,0)*get(2,2)) +
         (get(0,1)*get(1,2)*get(2,0)) +
         (get(0,2)*get(1,0)*get(2,1)) -
         (get(0,2)*get(1,1)*get(2,0));
}

auto alloy::core::matrix3::trace()
  const noexcept -> real
{
  return (get(0,0) + get(1,1) + get(2,2));
}

auto alloy::core::matrix3::inverse()
  const noexcept -> matrix3
{
  const auto det = determinant();

  if (almost_equal(det,real{0})) {
    return matrix3{
      real{1}, real{0}, real{0},
      real{0}, real{1}, real{0},
      real{0}, real{0}, real{1}
    };
  }

  const auto inv_det = (real{1} / det);

  return matrix3{
    (get(1,1)*get(2,2) - get(1,2)*get(2,1)) * inv_det,
    (get(1,2)*get(2,0) - get(1,0)*get(2,2)) * inv_det,
    (get(1,0)*get(2,1) - get(1,1)*get(2,0)) * inv_det,

    (get(0,2)*get(2,1) - get(0,1)*get(2,2)) * inv_det,
    (get(0,0)*get(2,2) - get(0,2)*get(2,0)) * inv_det,
    (get(0,1)*get(2,0) - get(0,0)*get(2,1)) * inv_det,

    (get(0,1)*get(1,2) - get(0,2)*get(1,1)) * inv_det,
    (get(0,2)*get(1,0) - get(0,0)*get(1,2)) * inv_det,
    (get(0,0)*get(1,1) - get(0,1)*get(1,0)) * inv_det
  };
}

auto alloy::core::matrix3::transposed()
  const noexcept -> matrix3
{
  return matrix3{
    get(0,0), get(1,0), get(2,0),
    get(0,1), get(1,1), get(2,1),
    get(0,2), get(1,2), get(2,2)
  };
}

auto alloy::core::matrix3::combine(const vector3& vec)
  const noexcept -> vector3
{
  auto result = vector3{};

  for (auto r = 0; r < columns; ++r) {
    auto sum = real{0};

    for (auto c = 0; c < rows; ++c) {
      sum += vec[c] * get(r,c);
    }
    result[r] = sum;
  }
  return result;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::core::matrix3::transpose()
  noexcept -> matrix3&
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
