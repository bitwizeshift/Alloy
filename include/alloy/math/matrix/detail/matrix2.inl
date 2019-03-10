#ifndef ALLOY_MATH_MATRIX_DETAIL_MATRIX2_INL
#define ALLOY_MATH_MATRIX_DETAIL_MATRIX2_INL

#ifndef ALLOY_MATH_MATRIX_MATRIX2_HPP
# error "matrix2.inl included without first including declaration header matrix2.hpp"
#endif

//==============================================================================
// class : matrix2<T>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::math::matrix2<T>::matrix2( const vector2<T>& v0,
                                                   const vector2<T>& v1 )
  noexcept
  : m_matrix {
      v0.x(), v0.y(),
      v1.x(), v1.y()
    }
{

}

template<typename T>
inline constexpr alloy::math::matrix2<T>
  ::matrix2( const value_type(&array)[4] )
  noexcept
  : m_matrix {
      array[0], array[1],
      array[2], array[3]
    }
{

}

template<typename T>
inline constexpr alloy::math::matrix2<T>
  ::matrix2( const value_type(&array)[2][2] )
  noexcept
  : m_matrix {
      array[0][0], array[0][1],
      array[1][0], array[1][1]
    }
{

}

template<typename T>
inline constexpr alloy::math::matrix2<T>
  ::matrix2( value_type m00, value_type m01,
             value_type m10, value_type m11 )
  noexcept
  : m_matrix {
      m00, m01,
      m10, m11
    }
{

}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::matrix2<T>::reference
  alloy::math::matrix2<T>::at( index_type r, index_type c )
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix2::at: index out of range"};
  }
  return get(r,c);
}


template<typename T>
inline constexpr typename alloy::math::matrix2<T>::const_reference
  alloy::math::matrix2<T>::at( index_type r, index_type c )
  const
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix2::at: index out of range"};
  }
  return get(r,c);
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::matrix2<T>::reference
  alloy::math::matrix2<T>::operator()( index_type r, index_type c )
  noexcept
{
  return get(r,c);
}

template<typename T>
inline constexpr typename alloy::math::matrix2<T>::const_reference
  alloy::math::matrix2<T>::operator()( index_type r, index_type c )
  const noexcept
{
  return get(r,c);
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::math::vector2<T>
  alloy::math::matrix2<T>::row( index_type r )
  const noexcept
{
  return vector2<T>{ get(r,0), get(r,1) };
}

template<typename T>
inline constexpr alloy::math::vector2<T>
  alloy::math::matrix2<T>::column( index_type c )
  const noexcept
{
  return vector2<T>{ get(0,c), get(1,c) };
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::matrix2<T>::size_type
  alloy::math::matrix2<T>::size()
  const noexcept
{
  return rows*columns;
}

template<typename T>
inline constexpr typename alloy::math::matrix2<T>::pointer
  alloy::math::matrix2<T>::data()
  noexcept
{
  return std::addressof(get(0,0));
}

template<typename T>
inline constexpr typename alloy::math::matrix2<T>::const_pointer
  alloy::math::matrix2<T>::data()
  const noexcept
{
  return std::addressof(get(0,0));
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::matrix2<T>::value_type
  alloy::math::matrix2<T>::determinant()
  const noexcept
{
  return (get(0,0) * get(1,1)) - (get(0,1) * get(1,0));
}

template<typename T>
inline constexpr typename alloy::math::matrix2<T>::value_type
  alloy::math::matrix2<T>::trace()
  const noexcept
{
  return (get(0,0) + get(1,1));
}

template<typename T>
inline constexpr alloy::math::matrix2<T>
  alloy::math::matrix2<T>::inverse()
  const noexcept
{
  const auto det = determinant();

  if (det == T{0}) {
    return matrix2_constants<T>::identity;
  }

  const auto inv_det = (T{1} / det);
  return matrix2<T> {
     get(1,1) * inv_det, -get(1,0) * inv_det,
    -get(0,1) * inv_det,  get(0,0) * inv_det
  };
}

template<typename T>
inline constexpr alloy::math::matrix2<T>
  alloy::math::matrix2<T>::transposed()
  const noexcept
{
  return matrix2<T> {
    get(0,0), get(1,0),
    get(0,1), get(1,1)
  };
}

template<typename T>
template<typename U>
constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::matrix2<T>::combine( const vector2<U>& vec )
  const noexcept
{
  auto result = vector2<std::common_type_t<T,U>>{};

  for (auto r = 0; r < columns; ++r) {
    auto sum = std::common_type_t<T,U>{0};

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
inline constexpr alloy::math::matrix2<T>& alloy::math::matrix2<T>::invert()
  noexcept
{
  (*this) = inverse();
  return (*this);
}

template<typename T>
inline constexpr alloy::math::matrix2<T>& alloy::math::matrix2<T>::transpose()
  noexcept
{
  using std::swap;

  swap(get(0,1),get(1,0));
  return (*this);
}


//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline alloy::math::matrix2<T>&
  alloy::math::matrix2<T>::operator+=( const matrix2<U>& rhs )
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
alloy::math::matrix2<T>&
  alloy::math::matrix2<T>::operator-=( const matrix2<U>& rhs )
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
alloy::math::matrix2<T>&
  alloy::math::matrix2<T>::operator*=( const matrix2<U>& rhs )
  noexcept
{
  auto result = matrix2<T>{};

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {

      auto sum = std::common_type_t<T,U>(0);

      for (auto i = 0; i < columns; ++i) {
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
alloy::math::matrix2<T>&
  alloy::math::matrix2<T>::operator*=( U scalar )
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
alloy::math::matrix2<T>&
  alloy::math::matrix2<T>::operator/=( U scalar )
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
inline constexpr typename alloy::math::matrix2<T>::reference
  alloy::math::matrix2<T>::get( index_type r, index_type c )
  noexcept
{
  return m_matrix[r][c];
}

template<typename T>
inline constexpr typename alloy::math::matrix2<T>::const_reference
  alloy::math::matrix2<T>::get( index_type r, index_type c )
  const noexcept
{
  return m_matrix[r][c];
}

//==============================================================================
// non-member functions : class : matrix2<T>
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr alloy::math::matrix2<std::common_type_t<T,U>>
  alloy::math::operator+( const matrix2<T>& lhs, const matrix2<U>& rhs )
  noexcept
{
  return matrix2<std::common_type_t<T,U>>{lhs}+=rhs;
}

template<typename T, typename U>
inline constexpr alloy::math::matrix2<std::common_type_t<T,U>>
  alloy::math::operator-( const matrix2<T>& lhs, const matrix2<U>& rhs )
  noexcept
{
  return matrix2<std::common_type_t<T,U>>{lhs}-=rhs;
}

template<typename T, typename U>
inline constexpr alloy::math::matrix2<std::common_type_t<T,U>>
  alloy::math::operator*( const matrix2<T>& lhs, const matrix2<U>& rhs )
  noexcept
{
  return matrix2<std::common_type_t<T,U>>{lhs}*=rhs;
}

template<typename T, typename U>
inline constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::operator*( const vector2<T>& lhs, const matrix2<U>& rhs )
  noexcept
{
  return rhs.combine(lhs);
}

template<typename T, typename U, std::enable_if_t<std::is_arithmetic<T>::value>*>
constexpr alloy::math::matrix2<std::common_type_t<T,U>>
  alloy::math::operator*( T lhs, const matrix2<U>& rhs )
  noexcept
{
  return matrix2<std::common_type_t<T,U>>{rhs} *= lhs;
}

template<typename T, typename U, std::enable_if_t<std::is_arithmetic<U>::value>*>
constexpr alloy::math::matrix2<std::common_type_t<T,U>>
  alloy::math::operator*( const matrix2<T>& lhs, U rhs )
  noexcept
{
  return matrix2<std::common_type_t<T,U>>{lhs} *= rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::math::operator==( const matrix2<T>& lhs,
                                               const matrix2<U>& rhs )
  noexcept
{
  for (auto r=0;r<matrix2<T>::rows;++r) {
    for (auto c=0;c<matrix2<T>::columns;++c) {
      if (lhs(r,c)!=rhs(r,c)) {
        return false;
      }
    }
  }
  return true;
}

template<typename T, typename U>
inline constexpr bool alloy::math::operator!=( const matrix2<T>& lhs,
                                               const matrix2<U>& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::math::almost_equal( const matrix2<T>& lhs,
                                                 const matrix2<U>& rhs )
  noexcept
{
  for (auto r=0;r<matrix2<T>::rows;++r) {
    for (auto c=0;c<matrix2<T>::columns;++c) {
      if (!almost_equal(rhs(r,c),rhs(r,c))) {
        return false;
      }
    }
  }
  return true;
}

template<typename T, typename U, typename Arithmetic, typename>
inline constexpr bool alloy::math::almost_equal( const matrix2<T>& lhs,
                                                 const matrix2<U>& rhs,
                                                 Arithmetic tolerance )
  noexcept
{
  for (auto r=0;r<matrix2<T>::rows;++r) {
    for (auto c=0;c<matrix2<T>::columns;++c) {
      if (!almost_equal(rhs(r,c),rhs(r,c), tolerance)) {
        return false;
      }
    }
  }
  return true;
}

#endif /* ALLOY_MATH_MATRIX_DETAIL_MATRIX2_INL */
