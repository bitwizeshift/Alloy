#ifndef ALLOY_CORE_MATH_VECTOR_DETAIL_VECTOR2_INL
#define ALLOY_CORE_MATH_VECTOR_DETAIL_VECTOR2_INL

#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR2_HPP
# error "vector2.inl included without first including declaration header vector2.hpp"
#endif

//==============================================================================
// class : vector2
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::vector2()
  noexcept
  : m_data{
      real{0},
      real{0}
    }
{

}

inline alloy::core::vector2::vector2( real magnitude, radian direction )
  noexcept
  : m_data{
      magnitude * trigonometry::cos(direction),
      magnitude * trigonometry::sin(direction)
    }
{

}

inline constexpr alloy::core::vector2::vector2( real x, real y )
  noexcept
  : m_data{x,y}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::size_type
  alloy::core::vector2::size()
  const noexcept
{
  return 2;
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::reference
  alloy::core::vector2::x()
  noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::vector2::const_reference
  alloy::core::vector2::x()
  const noexcept
{
  return m_data[0];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::reference
  alloy::core::vector2::y()
  noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::vector2::const_reference
  alloy::core::vector2::y()
  const noexcept
{
  return m_data[1];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::pointer
  alloy::core::vector2::data()
  noexcept
{
  return &m_data[0];
}

inline constexpr alloy::core::vector2::const_pointer
  alloy::core::vector2::data()
  const noexcept
{
  return &m_data[0];
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::reference
  alloy::core::vector2::at( index_type n )
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 2 || n < 0 ) {
    throw std::out_of_range("alloy::core::vector2::at: index out of range");
  }
#else
  assert( n < 2 && n >= 0 );
#endif
  return m_data[n];
}

inline constexpr alloy::core::vector2::const_reference
  alloy::core::vector2::at( index_type n )
  const
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 2 || n < 0 ) {
    throw std::out_of_range("alloy::core::vector2::at: index out of range");
  }
#else
  assert( n < 2 && n >= 0 );
#endif
  return m_data[n];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::reference
  alloy::core::vector2::operator[]( index_type n )
  noexcept
{
  assert( n < 2 && n >= 0 );
  return m_data[n];
}

inline constexpr alloy::core::vector2::const_reference
  alloy::core::vector2::operator[]( index_type n )
  const noexcept
{
  assert( n < 2 && n >= 0 );
  return m_data[n];
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::vector2::dot( const vector2& other )
  const noexcept
{
  return (x() * other.x()) + (y() * other.y());
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::vector2::cross( const vector2& other )
  const noexcept
{
  return (x() * other.y()) - (y() * other.x());
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::vector2::square_magnitude()
  const noexcept
{
  return dot(*this);
}

inline alloy::core::real
  alloy::core::vector2::magnitude()
  const noexcept
{
  return sqrt( (x()*x()) + (y()*y()) );
}

inline constexpr alloy::core::vector2
  alloy::core::vector2::midpoint( const vector2& rhs )
  const noexcept
{
  return vector2{
    ((x() + rhs.x()) * real{0.5}),
    ((y() + rhs.y()) * real{0.5})
  };
}

inline constexpr alloy::core::vector2
  alloy::core::vector2::reflection( const vector2& normal )
  const noexcept
{
  return (*this) - (normal * (real{2} * dot(normal)));
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2
  alloy::core::vector2::projection( const vector2& vector )
  const noexcept
{
  const auto multiplier = dot(vector) / dot(*this);

  return vector2{
    multiplier * vector.x(),
    multiplier * vector.y()
  };
}

inline constexpr alloy::core::vector2
  alloy::core::vector2::rejection( const vector2& vector )
  const noexcept
{
  return (*this) - projection(vector);
}


//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2
  alloy::core::vector2::perpendicular()
  const noexcept
{
  return vector2{ -y(), x() };
}

inline alloy::core::vector2
  alloy::core::vector2::normalized()
  const noexcept
{
  const auto mag = magnitude();

  if (mag > real{0}) {
    const auto mag_inv = real{1} / mag;
    return vector2{
      x() * mag_inv,
      y() * mag_inv
    };
  }
  return (*this);
}

inline constexpr alloy::core::vector2
  alloy::core::vector2::inverse()
  const noexcept
{
  return vector2{ -x(), -y() };
}

//------------------------------------------------------------------------------

inline alloy::core::radian
  alloy::core::vector2::angle_between( const vector2& other )
  const noexcept
{
  auto mag_product = magnitude() * other.magnitude();

  if( almost_equal( mag_product, real{0} ) ){
    mag_product = default_tolerance;
  }

  auto f = dot(other) / mag_product;

  f = clamp( f, real{-1.0}, real{1.0} );
  return trigonometry::arccos( f );
}

inline alloy::core::radian
  alloy::core::vector2::angle_to( const vector2& other )
  const noexcept
{
  auto angle = angle_between( other );

  if( cross(other) < real{0} ) {
    return radian_constants::revolution - angle;
  }

  return angle;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline alloy::core::vector2&
  alloy::core::vector2::normalize()
  noexcept
{
  const auto mag = magnitude();

  if (mag > real{0}) {
    const auto mag_inv = real{1} / mag;

    x() *= mag_inv;
    y() *= mag_inv;
  }

  return (*this);
}

inline constexpr alloy::core::vector2&
  alloy::core::vector2::invert()
  noexcept
{
  x() = -x();
  y() = -y();

  return (*this);
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

inline constexpr const alloy::core::vector2&
  alloy::core::vector2::operator+()
  const noexcept
{
  return (*this);
}

inline constexpr alloy::core::vector2
  alloy::core::vector2::operator-()
  const noexcept
{
  return vector2{ -x(), -y() };
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2&
  alloy::core::vector2::operator+=( const vector2& rhs )
  noexcept
{
  x() += rhs.x();
  y() += rhs.y();
  return (*this);
}

inline constexpr alloy::core::vector2&
  alloy::core::vector2::operator-=( const vector2& rhs )
  noexcept
{
  x() -= rhs.x();
  y() -= rhs.y();
  return (*this);
}

inline constexpr alloy::core::vector2&
  alloy::core::vector2::operator*=( real scalar )
  noexcept
{
  x() *= scalar;
  y() *= scalar;
  return (*this);
}

inline constexpr alloy::core::vector2&
  alloy::core::vector2::operator/=( real scalar )
  noexcept
{
  const auto inv = real{1} / scalar;

  x() *= inv;
  y() *= inv;
  return (*this);
}

//==============================================================================
// non-member functions : class : vector2
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2
  alloy::core::operator+( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return vector2{lhs}+=rhs;
}

inline constexpr alloy::core::vector2
  alloy::core::operator-( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return vector2{lhs}-=rhs;
}

inline constexpr alloy::core::vector2
  alloy::core::operator*( const vector2& lhs, real scalar )
  noexcept
{
  return vector2{lhs}*=scalar;
}

inline constexpr alloy::core::vector2
  alloy::core::operator*( real scalar, const vector2& lhs )
  noexcept
{
  return vector2{lhs}*=scalar;
}

inline constexpr alloy::core::vector2
  alloy::core::operator/( const vector2& lhs, real scalar )
  noexcept
{
  return vector2{lhs}/=scalar;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

inline constexpr bool
  alloy::core::operator==( const vector2& lhs, const vector2& rhs )
  noexcept
{
  for (auto i=0; i<2; ++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::operator!=( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

inline constexpr bool
  alloy::core::almost_equal( const vector2& lhs, const vector2& rhs )
  noexcept
{
  for (auto i=0;i<2;++i) {
    if (!almost_equal(rhs[i],rhs[i])) {
      return false;
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::almost_equal( const vector2& lhs,
                             const vector2& rhs,
                             real tolerance )
  noexcept
{
  for (auto i=0;i<2;++i) {
    if (!almost_equal(rhs[i],rhs[i],tolerance)) {
      return false;
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::are_linearly_independent( const vector2& v1,
                                         const vector2& v2 )
  noexcept
{
  // linear-independence tested by checking if the determinant of a produced
  // 2x2 matrix is non-zero

  const auto determinant = v1.x() * v2.y() - v1.y() * v2.x();
  return !almost_equal( real{0}, determinant );
}

//----------------------------------------------------------------------------
// Quantifiers
//----------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::dot( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return lhs.dot(rhs);
}

inline constexpr alloy::core::real
  alloy::core::cross( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return lhs.cross(rhs);
}

inline alloy::core::real
  alloy::core::magnitude( const vector2& vec )
  noexcept
{
  return vec.magnitude();
}

//==============================================================================
// struct : piecewise_compare<vector2>
//==============================================================================

inline constexpr bool
  alloy::core::piecewise_compare<alloy::core::vector2>
  ::operator()( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return (lhs.x() == rhs.x()) ?
           (lhs.y() < rhs.y()) :
         (lhs.x() < rhs.x());
}

#endif /* ALLOY_CORE_MATH_VECTOR_DETAIL_VECTOR2_INL */
