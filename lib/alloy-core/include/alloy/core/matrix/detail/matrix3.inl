#ifndef ALLOY_CORE_DETAIL_MATRIX_MATRIX3_INL
#define ALLOY_CORE_DETAIL_MATRIX_MATRIX3_INL

#ifndef ALLOY_CORE_MATRIX_MATRIX3_HPP
# error "matrix3.inl included without first including declaration header matrix3.hpp"
#endif


//==============================================================================
// class : matrix3<T>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::core::matrix3<T>::matrix3( const vector3& v0,
                                                   const vector3& v1,
                                                   const vector3& v2 )
  noexcept
  : m_matrix {
      v0.x(), v0.y(), v0.z(),
      v1.x(), v1.y(), v1.z(),
      v2.x(), v2.y(), v2.z()
    }
{

}

template<typename T>
inline constexpr alloy::core::matrix3<T>
  ::matrix3( const value_type(&array)[9] )
  noexcept
  : m_matrix {
      array[0], array[1], array[2],
      array[3], array[4], array[5],
      array[6], array[7], array[8]
    }
{

}

template<typename T>
inline constexpr alloy::core::matrix3<T>
  ::matrix3( const value_type(&array)[3][3] )
  noexcept
  : m_matrix {
      array[0][0], array[0][1], array[0][2],
      array[1][0], array[1][1], array[1][2],
      array[2][0], array[2][1], array[2][2]
    }
{

}

template<typename T>
inline constexpr alloy::core::matrix3<T>
  ::matrix3( value_type m00, value_type m01, value_type m02,
             value_type m10, value_type m11, value_type m12,
             value_type m20, value_type m21, value_type m22 )
  noexcept
  : m_matrix {
      m00, m01, m02,
      m10, m11, m12,
      m20, m21, m22
    }
{

}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::matrix3<T>::reference
  alloy::core::matrix3<T>::at( index_type r, index_type c )
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix3::at: index out of range"};
  }
  return get(r,c);
}


template<typename T>
inline constexpr typename alloy::core::matrix3<T>::const_reference
  alloy::core::matrix3<T>::at( index_type r, index_type c )
  const
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix3::at: index out of range"};
  }
  return get(r,c);
}

template<typename T>
inline constexpr typename alloy::core::matrix3<T>::reference
  alloy::core::matrix3<T>::operator()( index_type r, index_type c )
  noexcept
{
  return get(r,c);
}


template<typename T>
inline constexpr typename alloy::core::matrix3<T>::const_reference
  alloy::core::matrix3<T>::operator()( index_type r, index_type c )
  const noexcept
{
  return get(r,c);
}

template<typename T>
inline constexpr alloy::core::vector3
  alloy::core::matrix3<T>::row( index_type r )
  const noexcept
{
  return vector3{ get(r,0), get(r,1), get(r,2) };
}

template<typename T>
inline constexpr alloy::core::vector3
  alloy::core::matrix3<T>::column( index_type c )
  const noexcept
{
  return vector3{ get(0,c), get(1,c), get(2,c) };
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::matrix3<T>::size_type
  alloy::core::matrix3<T>::size()
  const noexcept
{
  return rows * columns;
}

template<typename T>
inline constexpr typename alloy::core::matrix3<T>::pointer
  alloy::core::matrix3<T>::data()
  noexcept
{
  return &get(0,0);
}

template<typename T>
inline constexpr typename alloy::core::matrix3<T>::const_pointer
  alloy::core::matrix3<T>::data()
  const noexcept
{
  return &get(0,0);
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::matrix3<T>::value_type
  alloy::core::matrix3<T>::determinant()
  const noexcept
{
  return (get(0,0)*get(1,1)*get(2,2)) -
         (get(0,0)*get(1,2)*get(2,1)) -
         (get(0,1)*get(1,0)*get(2,2)) +
         (get(0,1)*get(1,2)*get(2,0)) +
         (get(0,2)*get(1,0)*get(2,1)) -
         (get(0,2)*get(1,1)*get(2,0));
}

template<typename T>
inline constexpr typename alloy::core::matrix3<T>::value_type
  alloy::core::matrix3<T>::trace()
  const noexcept
{
  return (get(0,0) + get(1,1) + get(2,2));
}

template<typename T>
inline constexpr alloy::core::matrix3<T>
  alloy::core::matrix3<T>::inverse()
  const noexcept
{
  const auto det = determinant();

  if( det == value_type(0) ) return matrix3<T>::identity;

  const auto inv_det = (1.0 / det);
  return matrix3<T>{
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

template<typename T>
inline constexpr alloy::core::matrix3<T>
  alloy::core::matrix3<T>::transposed()
  const noexcept
{
  return matrix3<T> {
    get(0,0), get(1,0), get(2,0),
    get(0,1), get(1,1), get(2,1),
    get(0,2), get(1,2), get(2,2)
  };
}

template<typename T>
template<typename U>
constexpr alloy::core::vector3
  alloy::core::matrix3<T>::combine( const vector3& vec )
  const noexcept
{
  auto result = vector3();

  for (auto r = 0; r < columns; ++r) {
    auto sum = std::common_type_t<T,U>(0);

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

template<typename T>
inline constexpr alloy::core::matrix3<T>& alloy::core::matrix3<T>::invert()
  noexcept
{
  (*this) = inverse();
  return (*this);
}

template<typename T>
inline constexpr alloy::core::matrix3<T>& alloy::core::matrix3<T>::transpose()
  noexcept
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

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline alloy::core::matrix3<T>&
  alloy::core::matrix3<T>::operator+=( const matrix3<U>& rhs )
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
alloy::core::matrix3<T>&
  alloy::core::matrix3<T>::operator-=(const matrix3<U>& rhs)
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
alloy::core::matrix3<T>&
  alloy::core::matrix3<T>::operator*=(const matrix3<U>& rhs)
  noexcept
{
  auto result = matrix3<T>{};

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {

      auto sum = std::common_type_t<T,U>(0);

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
alloy::core::matrix3<T>&
  alloy::core::matrix3<T>::operator*=(U scalar)
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
alloy::core::matrix3<T>&
  alloy::core::matrix3<T>::operator/=( U scalar )
  noexcept
{
  auto inv = (1.0) / scalar;

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
inline constexpr typename alloy::core::matrix3<T>::reference
  alloy::core::matrix3<T>::get( index_type r, index_type c )
  noexcept
{
  return m_matrix[r][c];
}

template<typename T>
inline constexpr typename alloy::core::matrix3<T>::const_reference
  alloy::core::matrix3<T>::get( index_type r, index_type c )
  const noexcept
{
  return m_matrix[r][c];
}

//==============================================================================
// non-member functions : class : matrix3<T>
//==============================================================================

//--------------------------------------------------------------------------------
// Arithmetic Operators
//--------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr alloy::core::matrix3<std::common_type_t<T,U>>
  alloy::core::operator+( const matrix3<T>& lhs, const matrix3<U>& rhs )
  noexcept
{
  return matrix3<std::common_type_t<T,U>>(lhs)+=rhs;
}

template<typename T, typename U>
inline constexpr alloy::core::matrix3<std::common_type_t<T,U>>
  alloy::core::operator-( const matrix3<T>& lhs, const matrix3<U>& rhs )
  noexcept
{
  return matrix3<std::common_type_t<T,U>>(lhs)-=rhs;
}

template<typename T, typename U>
inline constexpr alloy::core::matrix3<std::common_type_t<T,U>>
  alloy::core::operator*( const matrix3<T>& lhs, const matrix3<U>& rhs )
  noexcept
{
  return matrix3<std::common_type_t<T,U>>(lhs)*=rhs;
}

template<typename T, typename U>
inline constexpr alloy::core::vector3
  alloy::core::operator*( const vector3& lhs, const matrix3<U>& rhs )
  noexcept
{
  return rhs.combine(lhs);
}

template<typename T, typename U>
constexpr alloy::core::matrix3<std::common_type_t<T,U>>
  alloy::core::operator*( T lhs, const matrix3<U>& rhs )
  noexcept
{
  return matrix3<std::common_type_t<T,U>>(rhs) *= lhs;
}

template<typename T, typename U>
constexpr alloy::core::matrix3<std::common_type_t<T,U>>
  alloy::core::operator*( const matrix3<T>& lhs, U rhs )
  noexcept
{
  return matrix3<std::common_type_t<T,U>>(lhs) *= rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::core::operator==( const matrix3<T>& lhs,
                                               const matrix3<U>& rhs )
  noexcept
{
  for (auto r=0;r<matrix3<T>::rows;++r) {
    for (auto c=0;c<matrix3<T>::columns;++c) {
      if (lhs(r,c)!=rhs(r,c)) {
        return false;
      }
    }
  }
  return true;
}

template<typename T, typename U>
inline constexpr bool alloy::core::operator!=( const matrix3<T>& lhs,
                                               const matrix3<U>& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::core::almost_equal( const matrix3<T>& lhs,
                                               const matrix3<U>& rhs )
  noexcept
{
  for (auto r=0;r<matrix3<T>::rows;++r) {
    for (auto c=0;c<matrix3<T>::columns;++c) {
      if (!almost_equal(rhs(r,c),rhs(r,c))) {
        return false;
      }
    }
  }
  return true;
}

template<typename T, typename U, typename Arithmetic, typename>
inline constexpr bool alloy::core::almost_equal( const matrix3<T>& lhs,
                                               const matrix3<U>& rhs,
                                               Arithmetic tolerance )
  noexcept
{
  for (auto r=0;r<matrix3<T>::rows;++r) {
    for (auto c=0;c<matrix3<T>::columns;++c) {
      if (!almost_equal(rhs(r,c),rhs(r,c), tolerance)) {
        return false;
      }
    }
  }
  return true;
}
#endif /* ALLOY_CORE_DETAIL_MATRIX_MATRIX3_INL */
