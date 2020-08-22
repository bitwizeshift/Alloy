#include "alloy/core/math/matrix/matrix2.hpp"

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::matrix2::determinant()
  const noexcept -> real
{
  return (get(0,0) * get(1,1)) - (get(0,1) * get(1,0));
}

auto alloy::core::matrix2::trace()
  const noexcept -> real
{
  return (get(0,0) + get(1,1));
}

auto alloy::core::matrix2::inverse()
  const noexcept -> matrix2
{
  const auto det = determinant();

  if (almost_equal(det, real{0})) {
    return matrix2{
      real{1}, real{0},
      real{0}, real{1}
    };
  }

  const auto inv_det = (real{1} / det);

  return matrix2{
     get(1,1) * inv_det, -get(1,0) * inv_det,
    -get(0,1) * inv_det,  get(0,0) * inv_det
  };
}

auto alloy::core::matrix2::transposed()
  const noexcept -> matrix2
{
  return matrix2{
    get(0,0), get(1,0),
    get(0,1), get(1,1)
  };
}

auto alloy::core::matrix2::combine(const vector2& vec)
  const noexcept -> vector2
{
  auto result = vector2{};

  for (auto r = 0; r < columns; ++r) {
    auto sum = real{0};

    for (auto c = 0; c < rows; ++c) {
      sum += vec[c] * get(r,c);
    }
    result[r] = sum;
  }
  return result;
}
