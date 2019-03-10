#ifndef ALLOY_MATH_MATRIX_DETAIL_MATRIX4_INL
#define ALLOY_MATH_MATRIX_DETAIL_MATRIX4_INL

#ifndef ALLOY_MATH_MATRIX_MATRIX4_HPP
# error "matrix4.inl included without first including declaration header matrix4.hpp"
#endif

//==============================================================================
// class : matrix4<T>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::math::matrix4<T>::matrix4( const vector4<T>& v0,
                                                   const vector4<T>& v1,
                                                   const vector4<T>& v2,
                                                   const vector4<T>& v3 )
  noexcept
  : m_matrix {
      v0.x(), v0.y(), v0.z(), v0.w(),
      v1.x(), v1.y(), v1.z(), v1.w(),
      v2.x(), v2.y(), v2.z(), v2.w(),
      v3.x(), v3.y(), v3.z(), v3.w()
    }
{

}

template<typename T>
inline constexpr alloy::math::matrix4<T>
  ::matrix4( const value_type(&array)[16] )
  noexcept
  : m_matrix {
      array[0],  array[1],  array[2],  array[3],
      array[4],  array[5],  array[6],  array[7],
      array[8],  array[9],  array[10], array[11],
      array[12], array[13], array[14], array[15]
    }
{

}

template<typename T>
inline constexpr alloy::math::matrix4<T>
  ::matrix4( const value_type(&array)[4][4] )
  noexcept
  : m_matrix {
      array[0][0], array[0][1], array[0][2], array[0][3],
      array[1][0], array[1][1], array[1][2], array[1][3],
      array[2][0], array[2][1], array[2][2], array[2][3],
      array[3][0], array[3][1], array[3][2], array[3][3]
    }
{

}

template<typename T>
inline constexpr alloy::math::matrix4<T>
  ::matrix4( value_type m00, value_type m01, value_type m02, value_type m03,
             value_type m10, value_type m11, value_type m12, value_type m13,
             value_type m20, value_type m21, value_type m22, value_type m23,
             value_type m30, value_type m31, value_type m32, value_type m33 )
  noexcept
  : m_matrix {
      m00, m01, m02, m03,
      m10, m11, m12, m13,
      m20, m21, m23, m23,
      m30, m31, m32, m33
    }
{

}

template<typename T>
inline constexpr alloy::math::matrix4<T>
  ::matrix4( value_type m00, value_type m01, value_type m02,
             value_type m10, value_type m11, value_type m12,
             value_type m20, value_type m21, value_type m22  )
  noexcept
  : m_matrix{
      m00,  m01,  T{0}, m02,
      m10,  m11,  T{0}, m12,
      T{0}, T{0}, T{1}, T{0},
      m20,  m21,  T{0}, m22
    }
{

}

template<typename T>
inline constexpr alloy::math::matrix4<T>::matrix4( const vector3<T>& v0,
                                                   const vector3<T>& v1,
                                                   const vector3<T>& v2 )
  noexcept
  : m_matrix{
      v0.x(), v0.y(), T{0}, v0.z(),
      v1.x(), v1.y(), T{0}, v1.z(),
      T{0},   T{0},   T{1}, T{0},
      v2.x(), v2.y(), T{0}, v2.z()
    }
{

}

//----------------------------------------------------------------------------
// Element Access
//----------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::matrix4<T>::reference
  alloy::math::matrix4<T>::at( index_type c, index_type r )
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix4::at: index out of range"};
  }
  return get(r,c);
}


template<typename T>
inline constexpr typename alloy::math::matrix4<T>::const_reference
  alloy::math::matrix4<T>::at( index_type c, index_type r )
  const
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix4::at: index out of range"};
  }
  return get(r,c);
}

//----------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::matrix4<T>::reference
  alloy::math::matrix4<T>::operator()( index_type r, index_type c )
  noexcept
{
  return get(r,c);
}


template<typename T>
inline constexpr typename alloy::math::matrix4<T>::const_reference
  alloy::math::matrix4<T>::operator()( index_type r, index_type c )
  const noexcept
{
  return get(r,c);
}

template<typename T>
inline constexpr alloy::math::vector4<T>
  alloy::math::matrix4<T>::row( index_type r )
  const noexcept
{
  return { get(r,0), get(r,1), get(r,2), get(r,3) };
}

template<typename T>
inline constexpr alloy::math::vector4<T>
  alloy::math::matrix4<T>::column( index_type c )
  const noexcept
{
  return { get(0,c), get(1,c), get(2,c), get(3,c) };
}

//----------------------------------------------------------------------------
// Observers
//----------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::matrix4<T>::size_type
  alloy::math::matrix4<T>::size()
  const noexcept
{
  return rows * columns;
}

template<typename T>
inline constexpr typename alloy::math::matrix4<T>::pointer
  alloy::math::matrix4<T>::data()
  noexcept
{
  return &get(0,0);
}

template<typename T>
inline constexpr typename alloy::math::matrix4<T>::const_pointer
  alloy::math::matrix4<T>::data()
  const noexcept
{
  return &get(0,0);
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::matrix4<T>::value_type
  alloy::math::matrix4<T>::determinant()
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

template<typename T>
inline constexpr typename alloy::math::matrix4<T>::value_type
  alloy::math::matrix4<T>::trace()
  const noexcept
{
  return (get(0,0) + get(1,1) + get(2,2) + get(3,3));
}

template<typename T>
inline constexpr alloy::math::matrix4<T>
  alloy::math::matrix4<T>::inverse()
  const noexcept
{
  return matrix4<T>{*this}.invert();
}

template<typename T>
inline constexpr alloy::math::matrix4<T>
  alloy::math::matrix4<T>::transposed()
  const noexcept
{
  return matrix4<T> {
    get(0,0), get(1,0), get(2,0),
    get(0,1), get(1,1), get(2,1),
    get(0,2), get(1,2), get(2,2)
  };
}

template<typename T>
template<typename U>
constexpr alloy::math::vector4<std::common_type_t<T,U>>
  alloy::math::matrix4<T>::combine( const vector4<U>& vec )
  const noexcept
{
  auto result = vector4<std::common_type_t<T,U>>{};

  for( auto r = 0; r < columns; ++r ) {
    auto sum = std::common_type_t<T,U>{0};

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

template<typename T>
inline constexpr alloy::math::matrix4<T>& alloy::math::matrix4<T>::invert()
  noexcept
{
  value_type inv[4][4]; // The resultant matrix

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

  T det = get(0,0) * inv[0][0] +
          get(0,1) * inv[1][0] +
          get(0,2) * inv[2][0] +
          get(0,3) * inv[3][0];

  // If determinant is zero, just return the identity matrix
  if (det == T{0}) {
    (*this) = matrix4_constants<T>::identity;
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

  const auto inv_det = T{1} / det;

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) = inv[r][c] * inv_det;
    }
  }

  return (*this);
}

template<typename T>
inline constexpr alloy::math::matrix4<T>& alloy::math::matrix4<T>::transpose()
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

template<typename T>
template<typename U>
inline alloy::math::matrix4<T>&
  alloy::math::matrix4<T>::operator+=( const matrix4<U>& rhs )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) += rhs.get(r,c);
    }
  }
  return (*this);
}

template<typename T>
template<typename U>
alloy::math::matrix4<T>&
  alloy::math::matrix4<T>::operator-=(const matrix4<U>& rhs)
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) -= rhs.get(r,c);
    }
  }
  return (*this);
}

template<typename T>
template<typename U>
alloy::math::matrix4<T>&
  alloy::math::matrix4<T>::operator*=(const matrix4<U>& rhs)
  noexcept
{
  auto result = matrix4<T>{};

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      auto sum = std::common_type_t<T,U>{0};

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

template<typename T>
template<typename U>
alloy::math::matrix4<T>&
  alloy::math::matrix4<T>::operator*=(U scalar)
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) *= scalar;
    }
  }
  return (*this);
}

template<typename T>
template<typename U>
alloy::math::matrix4<T>&
  alloy::math::matrix4<T>::operator/=( U scalar )
  noexcept
{
  const auto inv = T{1} / scalar;

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) *= inv;
    }
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Private Member Functions
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::matrix4<T>::reference
  alloy::math::matrix4<T>::get( index_type r, index_type c )
  noexcept
{
  return m_matrix[r][c];
}

template<typename T>
inline constexpr typename alloy::math::matrix4<T>::const_reference
  alloy::math::matrix4<T>::get( index_type r, index_type c )
  const noexcept
{
  return m_matrix[r][c];
}

//==============================================================================
// non-member functions : class : matrix4<T>
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr alloy::math::matrix4<std::common_type_t<T,U>>
  alloy::math::operator+( const matrix4<T>& lhs, const matrix4<U>& rhs )
  noexcept
{
  return matrix4<std::common_type_t<T,U>>(lhs)+=rhs;
}

template<typename T, typename U>
inline constexpr alloy::math::matrix4<std::common_type_t<T,U>>
  alloy::math::operator-( const matrix4<T>& lhs, const matrix4<U>& rhs )
  noexcept
{
  return matrix4<std::common_type_t<T,U>>(lhs)-=rhs;
}

template<typename T, typename U>
inline constexpr alloy::math::matrix4<std::common_type_t<T,U>>
  alloy::math::operator*( const matrix4<T>& lhs, const matrix4<U>& rhs )
  noexcept
{
  return matrix4<std::common_type_t<T,U>>(lhs)*=rhs;
}

template<typename T, typename U>
inline constexpr alloy::math::vector4<std::common_type_t<T,U>>
  alloy::math::operator*( const vector4<T>& lhs, const matrix4<U>& rhs )
  noexcept
{
  return rhs.combine(lhs);
}

template<typename T, typename U, typename>
constexpr alloy::math::matrix4<std::common_type_t<T,U>>
  alloy::math::operator*( T lhs, const matrix4<U>& rhs )
  noexcept
{
  return matrix4<std::common_type_t<T,U>>(rhs) *= lhs;
}

template<typename T, typename U, typename>
constexpr alloy::math::matrix4<std::common_type_t<T,U>>
  alloy::math::operator*( const matrix4<T>& lhs, U rhs )
  noexcept
{
  return matrix4<std::common_type_t<T,U>>(lhs) *= rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::math::operator==( const matrix4<T>& lhs,
                                               const matrix4<U>& rhs )
  noexcept
{
  for(auto r=0;r<matrix4<T>::rows;++r) {
    for(auto c=0;c<matrix4<T>::columns;++c) {
      if( lhs(r,c) != rhs(r,c) ) {
        return false;
      }
    }
  }
  return true;
}

template<typename T, typename U>
inline constexpr bool alloy::math::operator!=( const matrix4<T>& lhs,
                                               const matrix4<U>& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//----------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::math::almost_equal( const matrix4<T>& lhs,
                                                 const matrix4<U>& rhs )
  noexcept
{
  for(auto r=0;r<matrix4<T>::rows;++r) {
    for(auto c=0;c<matrix4<T>::columns;++c) {
      if( !almost_equal(rhs(r,c),rhs(r,c)) ) {
        return false;
      }
    }
  }
  return true;
}

template<typename T, typename U, typename Arithmetic, typename>
inline constexpr bool alloy::math::almost_equal( const matrix4<T>& lhs,
                                                 const matrix4<U>& rhs,
                                                 Arithmetic tolerance )
  noexcept
{
  for(auto r=0;r<matrix4<T>::rows;++r) {
    for(auto c=0;c<matrix4<T>::columns;++c) {
      if( !almost_equal(rhs(r,c),rhs(r,c), tolerance) ) {
        return false;
      }
    }
  }
  return true;
}
#endif /* ALLOY_MATH_MATRIX_DETAIL_MATRIX4_INL */
