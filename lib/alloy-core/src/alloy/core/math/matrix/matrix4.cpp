#include "alloy/core/math/matrix/matrix4.hpp"


//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::matrix4::determinant()
  const noexcept -> real
{
  return  get(0,0) *
        ( get(1,1) * get(2,2) * get(3,3) -
          get(1,1) * get(2,3) * get(3,2) -
          get(2,1) * get(1,2) * get(3,3) +
          get(2,1) * get(1,3) * get(3,2) +
          get(3,1) * get(1,2) * get(2,3) -
          get(3,1) * get(1,3) * get(2,2) )
        + get(0,1) *
        (-get(1,0) * get(2,2) * get(3,3) +
          get(1,0) * get(2,3) * get(3,2) +
          get(2,0) * get(1,2) * get(3,3) -
          get(2,0) * get(1,3) * get(3,2) -
          get(3,0) * get(1,2) * get(2,3) +
          get(3,0) * get(1,3) * get(2,2) )
        + get(0,2) *
        ( get(1,0) * get(2,1) * get(3,3) -
          get(1,0) * get(2,3) * get(3,1) -
          get(2,0) * get(1,1) * get(3,3) +
          get(2,0) * get(1,3) * get(3,1) +
          get(3,0) * get(1,1) * get(2,3) -
          get(3,0) * get(1,3) * get(2,1))
        + get(0,3) *
        (-get(1,0) * get(2,1) * get(3,2) +
          get(1,0) * get(2,2) * get(3,1) +
          get(2,0) * get(1,1) * get(3,2) -
          get(2,0) * get(1,2) * get(3,1) -
          get(3,0) * get(1,1) * get(2,2) +
          get(3,0) * get(1,2) * get(2,1));
}

auto alloy::core::matrix4::trace()
  const noexcept -> real
{
  return (get(0,0) + get(1,1) + get(2,2) + get(3,3));
}

auto alloy::core::matrix4::inverse()
  const noexcept -> matrix4
{
  return matrix4{*this}.invert();
}

auto alloy::core::matrix4::transposed()
  const noexcept -> matrix4
{
  return matrix4{
    get(0,0), get(1,0), get(2,0), get(3,0),
    get(0,1), get(1,1), get(2,1), get(3,1),
    get(0,2), get(1,2), get(2,2), get(3,2),
    get(0,3), get(1,3), get(2,3), get(3,3)
  };
}

auto alloy::core::matrix4::combine(const vector4& vec)
  const noexcept -> vector4
{
  auto result = vector4{};

  for(auto r = 0; r < columns; ++r) {
    auto sum = real{0};

    for(auto c = 0; c < rows; ++c) {
      sum += vec[c] * get(r,c);
    }
    result[r] = sum;
  }
  return result;
}


//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::core::matrix4::invert()
  noexcept -> matrix4&
{
  real inv[4][4] = {}; // The resultant matrix

  inv[0][0] = get(1,1) * get(2,2) * get(3,3) -
              get(1,1) * get(2,3) * get(3,2) -
              get(2,1) * get(1,2) * get(3,3) +
              get(2,1) * get(1,3) * get(3,2) +
              get(3,1) * get(1,2) * get(2,3) -
              get(3,1) * get(1,3) * get(2,2);

  inv[1][0] =-get(1,0) * get(2,2) * get(3,3) +
              get(1,0) * get(2,3) * get(3,2) +
              get(2,0) * get(1,2) * get(3,3) -
              get(2,0) * get(1,3) * get(3,2) -
              get(3,0) * get(1,2) * get(2,3) +
              get(3,0) * get(1,3) * get(2,2);

  inv[2][0] = get(1,0) * get(2,1) * get(3,3) -
              get(1,0) * get(2,3) * get(3,1) -
              get(2,0) * get(1,1) * get(3,3) +
              get(2,0) * get(1,3) * get(3,1) +
              get(3,0) * get(1,1) * get(2,3) -
              get(3,0) * get(1,3) * get(2,1);

  inv[3][0] =-get(1,0) * get(2,1) * get(3,2) +
              get(1,0) * get(2,2) * get(3,1) +
              get(2,0) * get(1,1) * get(3,2) -
              get(2,0) * get(1,2) * get(3,1) -
              get(3,0) * get(1,1) * get(2,2) +
              get(3,0) * get(1,2) * get(2,1);

  auto det = get(0,0) * inv[0][0] +
             get(0,1) * inv[1][0] +
             get(0,2) * inv[2][0] +
             get(0,3) * inv[3][0];

  // If determinant is zero, just return the identity matrix
  if (almost_equal(det, real{0})) {
    (*this) = matrix4{
      real{1}, real{0}, real{0}, real{0},
      real{0}, real{1}, real{0}, real{0},
      real{0}, real{0}, real{1}, real{0},
      real{0}, real{0}, real{0}, real{1}
    };

    return (*this);
  }

  inv[0][1] =-get(0,1) * get(2,2) * get(3,3) +
              get(0,1) * get(2,3) * get(3,2) +
              get(2,1) * get(0,2) * get(3,3) -
              get(2,1) * get(0,3) * get(3,2) -
              get(3,1) * get(0,2) * get(2,3) +
              get(3,1) * get(0,3) * get(2,2);

  inv[1][1] = get(0,0) * get(2,2) * get(3,3) -
              get(0,0) * get(2,3) * get(3,2) -
              get(2,0) * get(0,2) * get(3,3) +
              get(2,0) * get(0,3) * get(3,2) +
              get(3,0) * get(0,2) * get(2,3) -
              get(3,0) * get(0,3) * get(2,2);

  inv[2][1] =-get(0,0) * get(2,1) * get(3,3) +
              get(0,0) * get(2,3) * get(3,1) +
              get(2,0) * get(0,1) * get(3,3) -
              get(2,0) * get(0,3) * get(3,1) -
              get(3,0) * get(0,1) * get(2,3) +
              get(3,0) * get(0,3) * get(2,1);

  inv[3][1] = get(0,0) * get(2,1) * get(3,2) -
              get(0,0) * get(2,2) * get(3,1) -
              get(2,0) * get(0,1) * get(3,2) +
              get(2,0) * get(0,2) * get(3,1) +
              get(3,0) * get(0,1) * get(2,2) -
              get(3,0) * get(0,2) * get(2,1);

  inv[0][2] = get(0,1) * get(1,2) * get(3,3) -
              get(0,1) * get(1,3) * get(3,2) -
              get(1,1) * get(0,2) * get(3,3) +
              get(1,1) * get(0,3) * get(3,2) +
              get(3,1) * get(0,2) * get(1,3) -
              get(3,1) * get(0,3) * get(1,2);

  inv[1][2] =-get(0,0) * get(1,2) * get(3,3) +
              get(0,0) * get(1,3) * get(3,2) +
              get(1,0) * get(0,2) * get(3,3) -
              get(1,0) * get(0,3) * get(3,2) -
              get(3,0) * get(0,2) * get(1,3) +
              get(3,0) * get(0,3) * get(1,2);

  inv[2][2] = get(0,0) * get(1,1) * get(3,3) -
              get(0,0) * get(1,3) * get(3,1) -
              get(1,0) * get(0,1) * get(3,3) +
              get(1,0) * get(0,3) * get(3,1) +
              get(3,0) * get(0,1) * get(1,3) -
              get(3,0) * get(0,3) * get(1,1);

  inv[3][2] =-get(0,0) * get(1,1) * get(3,2) +
              get(0,0) * get(1,2) * get(3,1) +
              get(1,0) * get(0,1) * get(3,2) -
              get(1,0) * get(0,2) * get(3,1) -
              get(3,0) * get(0,1) * get(1,2) +
              get(3,0) * get(0,2) * get(1,1);

  inv[0][3] =-get(0,1) * get(1,2) * get(2,3) +
              get(0,1) * get(1,3) * get(2,2) +
              get(1,1) * get(0,2) * get(2,3) -
              get(1,1) * get(0,3) * get(2,2) -
              get(2,1) * get(0,2) * get(1,3) +
              get(2,1) * get(0,3) * get(1,2);

  inv[1][3] = get(0,0) * get(1,2) * get(2,3) -
              get(0,0) * get(1,3) * get(2,2) -
              get(1,0) * get(0,2) * get(2,3) +
              get(1,0) * get(0,3) * get(2,2) +
              get(2,0) * get(0,2) * get(1,3) -
              get(2,0) * get(0,3) * get(1,2);

  inv[2][3] =-get(0,0) * get(1,1) * get(2,3) +
              get(0,0) * get(1,3) * get(2,1) +
              get(1,0) * get(0,1) * get(2,3) -
              get(1,0) * get(0,3) * get(2,1) -
              get(2,0) * get(0,1) * get(1,3) +
              get(2,0) * get(0,3) * get(1,1);

  inv[3][3] = get(0,0) * get(1,1) * get(2,2) -
              get(0,0) * get(1,2) * get(2,1) -
              get(1,0) * get(0,1) * get(2,2) +
              get(1,0) * get(0,2) * get(2,1) +
              get(2,0) * get(0,1) * get(1,2) -
              get(2,0) * get(0,2) * get(1,1);

  const auto inv_det = real{1} / det;

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) = inv[r][c] * inv_det;
    }
  }

  return (*this);
}

auto alloy::core::matrix4::transpose()
  noexcept -> matrix4&
{
  using std::swap;

  // Only need to swap across the diagonal
  for(auto r = 0; r < rows; ++r) {
    for(auto c = 0; c < r; ++c) {
      if(r != c) {
        swap(get(r,c), get(c,r));
      }
    }
  }
  return (*this);
}
