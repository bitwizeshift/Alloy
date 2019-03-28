#ifndef ALLOY_CORE_MATH_MATRIX_DETAIL_MATRIX2_INL
#define ALLOY_CORE_MATH_MATRIX_DETAIL_MATRIX2_INL

#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX2_HPP
# error "matrix2.inl included without first including declaration header matrix2.hpp"
#endif

//==============================================================================
// class : matrix2
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix2
  ::matrix2()
  noexcept
  : m_matrix {
      real{0}, real{0},
      real{0}, real{0}
    }
{

}

inline constexpr alloy::core::matrix2
  ::matrix2( const vector2& v0, const vector2& v1 )
  noexcept
  : m_matrix {
      v0.x(), v0.y(),
      v1.x(), v1.y()
    }
{

}

inline constexpr alloy::core::matrix2
  ::matrix2( const real(&array)[4] )
  noexcept
  : m_matrix {
      array[0], array[1],
      array[2], array[3]
    }
{

}

inline constexpr alloy::core::matrix2
  ::matrix2( const real(&array)[2][2] )
  noexcept
  : m_matrix {
      array[0][0], array[0][1],
      array[1][0], array[1][1]
    }
{

}

inline constexpr alloy::core::matrix2
  ::matrix2( real m00, real m01,
             real m10, real m11 )
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

inline constexpr alloy::core::matrix2::reference
  alloy::core::matrix2::at( index_type r, index_type c )
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix2::at: index out of range"};
  }
  return get(r,c);
}


inline constexpr alloy::core::matrix2::const_reference
  alloy::core::matrix2::at( index_type r, index_type c )
  const
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix2::at: index out of range"};
  }
  return get(r,c);
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix2::reference
  alloy::core::matrix2::get( index_type r, index_type c )
  noexcept
{
  return m_matrix[r][c];
}

inline constexpr alloy::core::matrix2::const_reference
  alloy::core::matrix2::get( index_type r, index_type c )
  const noexcept
{
  return m_matrix[r][c];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2
  alloy::core::matrix2::row( index_type r )
  const noexcept
{
  return vector2{ get(r,0), get(r,1) };
}

inline constexpr alloy::core::vector2
  alloy::core::matrix2::column( index_type c )
  const noexcept
{
  return vector2{ get(0,c), get(1,c) };
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix2::size_type
  alloy::core::matrix2::size()
  const noexcept
{
  return rows*columns;
}

inline constexpr alloy::core::matrix2::pointer
  alloy::core::matrix2::data()
  noexcept
{
  return &get(0,0);
}

inline constexpr alloy::core::matrix2::const_pointer
  alloy::core::matrix2::data()
  const noexcept
{
  return &get(0,0);
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::matrix2::determinant()
  const noexcept
{
  return (get(0,0) * get(1,1)) - (get(0,1) * get(1,0));
}

inline constexpr alloy::core::real
  alloy::core::matrix2::trace()
  const noexcept
{
  return (get(0,0) + get(1,1));
}

inline constexpr alloy::core::matrix2
  alloy::core::matrix2::inverse()
  const noexcept
{
  const auto det = determinant();

  if (det == real{0}) {
    return matrix2{};
  }

  const auto inv_det = (real{1} / det);
  return matrix2{
     get(1,1) * inv_det, -get(1,0) * inv_det,
    -get(0,1) * inv_det,  get(0,0) * inv_det
  };
}

inline constexpr alloy::core::matrix2
  alloy::core::matrix2::transposed()
  const noexcept
{
  return matrix2{
    get(0,0), get(1,0),
    get(0,1), get(1,1)
  };
}

constexpr alloy::core::vector2
  alloy::core::matrix2::combine( const vector2& vec )
  const noexcept
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

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix2&
  alloy::core::matrix2::invert()
  noexcept
{
  (*this) = inverse();
  return (*this);
}

inline alloy::core::matrix2&
  alloy::core::matrix2::transpose()
  noexcept
{
  using std::swap;

  swap(get(0,1),get(1,0));
  return (*this);
}


//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline alloy::core::matrix2&
  alloy::core::matrix2::operator+=( const matrix2& rhs )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) += rhs.get(r,c);
    }
  }
  return (*this);
}

inline alloy::core::matrix2&
  alloy::core::matrix2::operator-=( const matrix2& rhs )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) -= rhs.get(r,c);
    }
  }
  return (*this);
}

inline alloy::core::matrix2&
  alloy::core::matrix2::operator*=( const matrix2& rhs )
  noexcept
{
  auto result = matrix2{};

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {

      auto sum = real{0};

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

inline alloy::core::matrix2&
  alloy::core::matrix2::operator*=( real scalar )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) *= scalar;
    }
  }
  return (*this);
}

inline alloy::core::matrix2&
  alloy::core::matrix2::operator/=( real scalar )
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
// non-member functions : class : matrix2
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline alloy::core::matrix2
  alloy::core::operator+( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  return matrix2{lhs} += rhs;
}

inline alloy::core::matrix2
  alloy::core::operator-( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  return matrix2{lhs} -= rhs;
}

inline alloy::core::matrix2
  alloy::core::operator*( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  return matrix2{lhs} *= rhs;
}

inline alloy::core::vector2
  alloy::core::operator*( const vector2& lhs, const matrix2& rhs )
  noexcept
{
  return rhs.combine(lhs);
}

inline alloy::core::matrix2
  alloy::core::operator*( real lhs, const matrix2& rhs )
  noexcept
{
  return matrix2{rhs} *= lhs;
}

inline alloy::core::matrix2
  alloy::core::operator*( const matrix2& lhs, real rhs )
  noexcept
{
  return matrix2{lhs} *= rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

inline constexpr bool
  alloy::core::operator==( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  for (auto r = 0; r < matrix2::rows; ++r) {
    for (auto c = 0; c < matrix2::columns; ++c) {
      if (lhs.get(r,c) != rhs.get(r,c)) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::operator!=( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

inline constexpr bool
  alloy::core::almost_equal( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  for (auto r = 0; r < matrix2::rows; ++r) {
    for (auto c = 0; c < matrix2::columns; ++c) {
      if (!almost_equal(rhs.get(r,c), rhs.get(r,c))) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::almost_equal( const matrix2& lhs,
                             const matrix2& rhs,
                             real tolerance )
  noexcept
{
  for (auto r = 0; r < matrix2::rows; ++r) {
    for (auto c = 0; c < matrix2::columns; ++c) {
      if (!almost_equal(rhs.get(r,c), rhs.get(r,c), tolerance)) {
        return false;
      }
    }
  }
  return true;
}

#endif /* ALLOY_CORE_MATH_MATRIX_DETAIL_MATRIX2_INL */
