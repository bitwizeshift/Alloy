#ifndef ALLOY_CORE_MATH_MATRIX_DETAIL_MATRIX4_INL
#define ALLOY_CORE_MATH_MATRIX_DETAIL_MATRIX4_INL

#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX4_HPP
# error "matrix4.inl included without first including declaration header matrix4.hpp"
#endif

//==============================================================================
// class : matrix4
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix4
  ::matrix4()
  noexcept
  : m_matrix{
    real{0}, real{0}, real{0}, real{0},
    real{0}, real{0}, real{0}, real{0},
    real{0}, real{0}, real{0}, real{0},
    real{0}, real{0}, real{0}, real{0}
  }
{

}

inline constexpr alloy::core::matrix4
  ::matrix4( const vector4& v0,
             const vector4& v1,
             const vector4& v2,
             const vector4& v3 )
  noexcept
  : m_matrix{
      v0.x(), v0.y(), v0.z(), v0.w(),
      v1.x(), v1.y(), v1.z(), v1.w(),
      v2.x(), v2.y(), v2.z(), v2.w(),
      v3.x(), v3.y(), v3.z(), v3.w()
    }
{

}

inline constexpr alloy::core::matrix4
  ::matrix4( const real(&array)[16] )
  noexcept
  : m_matrix {
      array[0],  array[1],  array[2],  array[3],
      array[4],  array[5],  array[6],  array[7],
      array[8],  array[9],  array[10], array[11],
      array[12], array[13], array[14], array[15]
    }
{

}

inline constexpr alloy::core::matrix4
  ::matrix4( const real(&array)[4][4] )
  noexcept
  : m_matrix {
      array[0][0], array[0][1], array[0][2], array[0][3],
      array[1][0], array[1][1], array[1][2], array[1][3],
      array[2][0], array[2][1], array[2][2], array[2][3],
      array[3][0], array[3][1], array[3][2], array[3][3]
    }
{

}

inline constexpr alloy::core::matrix4
  ::matrix4( real m00, real m01, real m02, real m03,
             real m10, real m11, real m12, real m13,
             real m20, real m21, real m22, real m23,
             real m30, real m31, real m32, real m33 )
  noexcept
  : m_matrix {
      m00, m01, m02, m03,
      m10, m11, m12, m13,
      m20, m21, m23, m23,
      m30, m31, m32, m33
    }
{

}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix4::reference
  alloy::core::matrix4::at( index_type c, index_type r )
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix4::at: index out of range"};
  }
  return get(r,c);
}


inline constexpr alloy::core::matrix4::const_reference
  alloy::core::matrix4::at( index_type c, index_type r )
  const
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix4::at: index out of range"};
  }
  return get(r,c);
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix4::reference
  alloy::core::matrix4::get( index_type r, index_type c )
  noexcept
{
  return m_matrix[r][c];
}

inline constexpr alloy::core::matrix4::const_reference
  alloy::core::matrix4::get( index_type r, index_type c )
  const noexcept
{
  return m_matrix[r][c];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4
  alloy::core::matrix4::row( index_type r )
  const noexcept
{
  return vector4{ get(r,0), get(r,1), get(r,2), get(r,3) };
}

inline constexpr alloy::core::vector4
  alloy::core::matrix4::column( index_type c )
  const noexcept
{
  return vector4{ get(0,c), get(1,c), get(2,c), get(3,c) };
}

//----------------------------------------------------------------------------
// Observers
//----------------------------------------------------------------------------

inline constexpr alloy::core::matrix4::size_type
  alloy::core::matrix4::size()
  const noexcept
{
  return rows * columns;
}

inline constexpr alloy::core::matrix4::pointer
  alloy::core::matrix4::data()
  noexcept
{
  return &get(0,0);
}

inline constexpr alloy::core::matrix4::const_pointer
  alloy::core::matrix4::data()
  const noexcept
{
  return &get(0,0);
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::matrix4::determinant()
  const noexcept
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

inline constexpr alloy::core::real
  alloy::core::matrix4::trace()
  const noexcept
{
  return (get(0,0) + get(1,1) + get(2,2) + get(3,3));
}

inline constexpr alloy::core::matrix4
  alloy::core::matrix4::inverse()
  const noexcept
{
  return matrix4{*this}.invert();
}

inline constexpr alloy::core::matrix4
  alloy::core::matrix4::transposed()
  const noexcept
{
  return matrix4{
    get(0,0), get(1,0), get(2,0), get(3,0),
    get(0,1), get(1,1), get(2,1), get(3,1),
    get(0,2), get(1,2), get(2,2), get(3,2),
    get(0,3), get(1,3), get(2,3), get(3,3)
  };
}

constexpr alloy::core::vector4
  alloy::core::matrix4::combine( const vector4& vec )
  const noexcept
{
  auto result = vector4{};

  for( auto r = 0; r < columns; ++r ) {
    auto sum = real{0};

    for( auto c = 0; c < rows; ++c ) {
      sum += vec[c] * get(r,c);
    }
    result[r] = sum;
  }
  return result;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix4&
  alloy::core::matrix4::invert()
  noexcept
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
  if (det == real{0}) {
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

inline constexpr alloy::core::matrix4&
  alloy::core::matrix4::transpose()
  noexcept
{
  using std::swap;

  // Only need to swap across the diagonal
  for( auto r = 0; r < rows; ++r ) {
    for( auto c = 0; c < r; ++c ) {
      if( r != c ) {
        swap( get(r,c), get(c,r) );
      }
    }
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline alloy::core::matrix4&
  alloy::core::matrix4::operator+=( const matrix4& rhs )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) += rhs.get(r,c);
    }
  }
  return (*this);
}

alloy::core::matrix4&
  alloy::core::matrix4::operator-=( const matrix4& rhs )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) -= rhs.get(r,c);
    }
  }
  return (*this);
}

alloy::core::matrix4&
  alloy::core::matrix4::operator*=( const matrix4& rhs )
  noexcept
{
  auto result = matrix4{};

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      auto sum = real{0};

      for (auto i = 0; i < rows; ++i) {
        sum += (get(i,c) * rhs.get(r,i));
      }
      result.get(r,c) = sum;
    }
  }
  // Copy result in
  (*this) = result;
  return (*this);
}

alloy::core::matrix4&
  alloy::core::matrix4::operator*=( real scalar )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) *= scalar;
    }
  }
  return (*this);
}

alloy::core::matrix4&
  alloy::core::matrix4::operator/=( real scalar )
  noexcept
{
  const auto inv = real{1} / scalar;

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) *= inv;
    }
  }
  return (*this);
}

//==============================================================================
// non-member functions : class : matrix4
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline alloy::core::matrix4
  alloy::core::operator+( const matrix4& lhs, const matrix4& rhs )
  noexcept
{
  return matrix4{lhs} += rhs;
}

inline alloy::core::matrix4
  alloy::core::operator-( const matrix4& lhs, const matrix4& rhs )
  noexcept
{
  return matrix4{lhs} -= rhs;
}

inline alloy::core::matrix4
  alloy::core::operator*( const matrix4& lhs, const matrix4& rhs )
  noexcept
{
  return matrix4{lhs} *= rhs;
}

inline alloy::core::vector4
  alloy::core::operator*( const vector4& lhs, const matrix4& rhs )
  noexcept
{
  return rhs.combine(lhs);
}

inline alloy::core::matrix4
  alloy::core::operator*( real lhs, const matrix4& rhs )
  noexcept
{
  return matrix4{rhs} *= lhs;
}

inline alloy::core::matrix4
  alloy::core::operator*( const matrix4& lhs, real rhs )
  noexcept
{
  return matrix4{lhs} *= rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

inline constexpr bool
  alloy::core::operator==( const matrix4& lhs, const matrix4& rhs )
  noexcept
{
  for(auto r = 0; r < matrix4::rows; ++r) {
    for(auto c = 0; c < matrix4::columns; ++c) {
      if( lhs.get(r,c) != rhs.get(r,c) ) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::operator!=( const matrix4& lhs, const matrix4& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//----------------------------------------------------------------------------

inline constexpr bool
  alloy::core::almost_equal( const matrix4& lhs, const matrix4& rhs )
  noexcept
{
  for(auto r = 0; r < matrix4::rows; ++r) {
    for(auto c = 0; c < matrix4::columns; ++c) {
      if( !almost_equal(rhs.get(r,c), rhs.get(r,c)) ) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::almost_equal( const matrix4& lhs,
                             const matrix4& rhs,
                             real tolerance )
  noexcept
{
  for(auto r = 0; r < matrix4::rows; ++r) {
    for(auto c = 0; c < matrix4::columns; ++c) {
      if( !almost_equal(rhs.get(r,c), rhs.get(r,c), tolerance) ) {
        return false;
      }
    }
  }
  return true;
}
#endif /* ALLOY_CORE_MATH_MATRIX_DETAIL_MATRIX4_INL */
