#ifndef ALLOY_CORE_MATH_DETAIL_MATRIX_MATRIX3_INL
#define ALLOY_CORE_MATH_DETAIL_MATRIX_MATRIX3_INL

#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX3_HPP
# error "matrix3.inl included without first including declaration header matrix3.hpp"
#endif


//==============================================================================
// class : matrix3
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix3
  ::matrix3()
  noexcept
  : m_matrix {
    real{0}, real{0}, real{0},
    real{0}, real{0}, real{0},
    real{0}, real{0}, real{0}
  }
{

}

inline constexpr alloy::core::matrix3
  ::matrix3( const vector3& v0, const vector3& v1, const vector3& v2 )
  noexcept
  : m_matrix {
      v0.x(), v0.y(), v0.z(),
      v1.x(), v1.y(), v1.z(),
      v2.x(), v2.y(), v2.z()
    }
{

}

inline constexpr alloy::core::matrix3
  ::matrix3( const real(&array)[9] )
  noexcept
  : m_matrix {
      array[0], array[1], array[2],
      array[3], array[4], array[5],
      array[6], array[7], array[8]
    }
{

}

inline constexpr alloy::core::matrix3
  ::matrix3( const real(&array)[3][3] )
  noexcept
  : m_matrix {
      array[0][0], array[0][1], array[0][2],
      array[1][0], array[1][1], array[1][2],
      array[2][0], array[2][1], array[2][2]
    }
{

}

inline constexpr alloy::core::matrix3
  ::matrix3( real m00, real m01, real m02,
             real m10, real m11, real m12,
             real m20, real m21, real m22 )
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

inline constexpr typename alloy::core::matrix3::reference
  alloy::core::matrix3::at( index_type r, index_type c )
{
  if( c >= columns || c < 0 || r >= rows || r < 0 ) {
    throw std::out_of_range{"matrix3::at: index out of range"};
  }
  return get(r,c);
}

inline constexpr typename alloy::core::matrix3::const_reference
  alloy::core::matrix3::at( index_type r, index_type c )
  const
{
  if( c >= columns || c < 0 || r >= rows || r < 0 ) {
    throw std::out_of_range{"matrix3::at: index out of range"};
  }
  return get(r,c);
}

//------------------------------------------------------------------------------

inline constexpr typename alloy::core::matrix3::reference
  alloy::core::matrix3::get( index_type r, index_type c )
  noexcept
{
  return m_matrix[r][c];
}

inline constexpr typename alloy::core::matrix3::const_reference
  alloy::core::matrix3::get( index_type r, index_type c )
  const noexcept
{
  return m_matrix[r][c];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3
  alloy::core::matrix3::row( index_type r )
  const noexcept
{
  return vector3{ get(r,0), get(r,1), get(r,2) };
}

inline constexpr alloy::core::vector3
  alloy::core::matrix3::column( index_type c )
  const noexcept
{
  return vector3{ get(0,c), get(1,c), get(2,c) };
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr typename alloy::core::matrix3::size_type
  alloy::core::matrix3::size()
  const noexcept
{
  return rows * columns;
}

inline constexpr typename alloy::core::matrix3::pointer
  alloy::core::matrix3::data()
  noexcept
{
  return &get(0,0);
}

inline constexpr typename alloy::core::matrix3::const_pointer
  alloy::core::matrix3::data()
  const noexcept
{
  return &get(0,0);
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr typename alloy::core::real
  alloy::core::matrix3::determinant()
  const noexcept
{
  return (get(0,0)*get(1,1)*get(2,2)) -
         (get(0,0)*get(1,2)*get(2,1)) -
         (get(0,1)*get(1,0)*get(2,2)) +
         (get(0,1)*get(1,2)*get(2,0)) +
         (get(0,2)*get(1,0)*get(2,1)) -
         (get(0,2)*get(1,1)*get(2,0));
}

inline constexpr typename alloy::core::real
  alloy::core::matrix3::trace()
  const noexcept
{
  return (get(0,0) + get(1,1) + get(2,2));
}

inline constexpr alloy::core::matrix3
  alloy::core::matrix3::inverse()
  const noexcept
{
  const auto det = determinant();

  if (det == real{0}) {
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

inline constexpr alloy::core::matrix3
  alloy::core::matrix3::transposed()
  const noexcept
{
  return matrix3{
    get(0,0), get(1,0), get(2,0),
    get(0,1), get(1,1), get(2,1),
    get(0,2), get(1,2), get(2,2)
  };
}

constexpr alloy::core::vector3
  alloy::core::matrix3::combine( const vector3& vec )
  const noexcept
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

inline constexpr alloy::core::matrix3&
  alloy::core::matrix3::invert()
  noexcept
{
  (*this) = inverse();
  return (*this);
}

inline constexpr alloy::core::matrix3&
  alloy::core::matrix3::transpose()
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

inline alloy::core::matrix3&
  alloy::core::matrix3::operator+=( const matrix3& rhs )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) += rhs.get(r,c);
    }
  }
  return (*this);
}

inline alloy::core::matrix3&
  alloy::core::matrix3::operator-=( const matrix3& rhs )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) -= rhs.get(r,c);
    }
  }
  return (*this);
}

inline alloy::core::matrix3&
  alloy::core::matrix3::operator*=( const matrix3& rhs )
  noexcept
{
  auto result = matrix3{};

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

inline alloy::core::matrix3&
  alloy::core::matrix3::operator*=( real scalar )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) *= scalar;
    }
  }
  return (*this);
}

inline alloy::core::matrix3&
  alloy::core::matrix3::operator/=( real scalar )
  noexcept
{
  auto inv = real{1} / scalar;

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) *= inv;
    }
  }
  return (*this);
}

//==============================================================================
// non-member functions : class : matrix3
//==============================================================================

//--------------------------------------------------------------------------------
// Arithmetic Operators
//--------------------------------------------------------------------------------

inline alloy::core::matrix3
  alloy::core::operator+( const matrix3& lhs, const matrix3& rhs )
  noexcept
{
  return matrix3{lhs} += rhs;
}

inline alloy::core::matrix3
  alloy::core::operator-( const matrix3& lhs, const matrix3& rhs )
  noexcept
{
  return matrix3{lhs} -= rhs;
}

inline alloy::core::matrix3
  alloy::core::operator*( const matrix3& lhs, const matrix3& rhs )
  noexcept
{
  return matrix3{lhs} *= rhs;
}

inline alloy::core::vector3
  alloy::core::operator*( const vector3& lhs, const matrix3& rhs )
  noexcept
{
  return rhs.combine(lhs);
}

inline alloy::core::matrix3
  alloy::core::operator*( real lhs, const matrix3& rhs )
  noexcept
{
  return matrix3{rhs} *= lhs;
}

inline alloy::core::matrix3
  alloy::core::operator*( const matrix3& lhs, real rhs )
  noexcept
{
  return matrix3{lhs} *= rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

inline constexpr bool
  alloy::core::operator==( const matrix3& lhs, const matrix3& rhs )
  noexcept
{
  for (auto r = 0; r < matrix3::rows; ++r) {
    for (auto c = 0; c < matrix3::columns; ++c) {
      if (lhs.get(r,c) != rhs.get(r,c)) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::operator!=( const matrix3& lhs, const matrix3& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

inline constexpr bool
  alloy::core::almost_equal( const matrix3& lhs, const matrix3& rhs )
  noexcept
{
  for (auto r = 0; r < matrix3::rows; ++r) {
    for (auto c = 0; c < matrix3::columns; ++c) {
      if (!almost_equal(rhs.get(r,c), rhs.get(r,c))) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::almost_equal( const matrix3& lhs,
                             const matrix3& rhs,
                             real tolerance )
  noexcept
{
  for (auto r = 0; r < matrix3::rows; ++r) {
    for (auto c = 0; c < matrix3::columns; ++c) {
      if (!almost_equal(rhs.get(r,c), rhs.get(r,c), tolerance)) {
        return false;
      }
    }
  }
  return true;
}
#endif /* ALLOY_CORE_MATH_DETAIL_MATRIX_MATRIX3_INL */
