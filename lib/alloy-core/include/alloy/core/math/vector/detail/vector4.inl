#ifndef ALLOY_CORE_MATH_DETAIL_VECTOR_VECTOR4_INL
#define ALLOY_CORE_MATH_DETAIL_VECTOR_VECTOR4_INL

#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR4_HPP
# error "vector4.inl included without first including declaration header vector4.hpp"
#endif

//==============================================================================
// class : vector4
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::vector4()
  noexcept
  : m_data{
      real{0},
      real{0},
      real{0},
      real{0}
    }
{

}

inline constexpr alloy::core::vector4::vector4( real x,
                                                real y,
                                                real z,
                                                real w )
  noexcept
  : m_data{x,y,z,w}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::size_type
  alloy::core::vector4::size()
  const noexcept
{
  return 4;
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::x()
  noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::vector4::const_reference
  alloy::core::vector4::x()
  const noexcept
{
  return m_data[0];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::y()
  noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::vector4::const_reference
  alloy::core::vector4::y()
  const noexcept
{
  return m_data[1];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::z()
  noexcept
{
  return m_data[2];
}

inline constexpr alloy::core::vector4::const_reference
  alloy::core::vector4::z()
  const noexcept
{
  return m_data[2];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::w()
  noexcept
{
  return m_data[3];
}

inline constexpr alloy::core::vector4::const_reference
  alloy::core::vector4::w()
  const noexcept
{
  return m_data[3];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::pointer
  alloy::core::vector4::data()
  noexcept
{
  return m_data;
}

inline constexpr alloy::core::vector4::const_pointer
  alloy::core::vector4::data()
  const noexcept
{
  return m_data;
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::at( index_type n )
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 4 || n < 0 ) {
    throw std::out_of_range("alloy::core::vector4::at: index out of range");
  }
#else
  assert( n < 4 && n >= 0 );
#endif
  return m_data[n];
}

inline constexpr alloy::core::vector4::const_reference
alloy::core::vector4::at( index_type n )
  const
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 4 || n < 0 ) {
    throw std::out_of_range("alloy::core::vector4::at: index out of range");
  }
#else
  assert( n < 4 && n >= 0 );
#endif
  return m_data[n];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::operator[]( index_type n )
  noexcept
{
  assert( n < 4 && n >= 0 );
  return m_data[n];
}

inline constexpr alloy::core::vector4::const_reference
alloy::core::vector4::operator[]( index_type n )
  const noexcept
{
  assert( n < 4 && n >= 0 );
  return m_data[n];
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::vector4::dot( const vector4& other )
  const noexcept
{
  // Use a loop to suggest vectorized math
  auto sum = real{0};

  for (auto i = 0; i < 4; ++i) {
    sum += m_data[i] * other.m_data[i];
  }

  return sum;
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4
  alloy::core::vector4::cross( const vector4& other )
  const noexcept
{
  return vector4{
    (y() * other.z() - z() * other.y()),
    (z() * other.x() - x() * other.z()),
    (x() * other.y() - y() * other.x()),
    0
  };
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::vector4::square_magnitude()
  const noexcept
{
  return dot(*this);
}

inline alloy::core::real
  alloy::core::vector4::magnitude()
  const noexcept
{
  return sqrt( dot(*this) );
}

inline constexpr alloy::core::vector4
  alloy::core::vector4::midpoint( const vector4& rhs )
  const noexcept
{
  return vector4{
    ((x() + rhs.x()) * real{0.5}),
    ((y() + rhs.y()) * real{0.5}),
    ((z() + rhs.z()) * real{0.5}),
    ((w() + rhs.w()) * real{0.5})
  };
}

inline constexpr alloy::core::vector4
  alloy::core::vector4::projection( const vector4& vector )
  const noexcept
{
  const auto multiplier = dot(vector) / dot(*this);

  return vector4{
    multiplier * x(),
    multiplier * y(),
    multiplier * z(),
    multiplier * w()
  };
}

inline constexpr alloy::core::vector4
  alloy::core::vector4::rejection( const vector4& vector )
  const noexcept
{
  return (*this) - projection( vector );
}

inline alloy::core::vector4
  alloy::core::vector4::normalized()
  const noexcept
{
  return vector4{*this}.normalize();
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4
  alloy::core::vector4::inverse()
  const noexcept
{
  return vector4{*this}.invert();
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline alloy::core::vector4&
  alloy::core::vector4::normalize()
  noexcept
{
  const auto mag = magnitude();

  if (mag > real{0})
  {
    const auto mag_inv = real{1} / mag;

    for (auto i = 0; i < 4; ++i) {
      m_data[i] *= mag_inv;
    }
  }

  return (*this);
}

inline constexpr alloy::core::vector4&
  alloy::core::vector4::invert()
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] *= real{-1};
  }

  return (*this);
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

inline constexpr const alloy::core::vector4&
  alloy::core::vector4::operator+()
  const noexcept
{
  return (*this);
}

inline constexpr alloy::core::vector4
  alloy::core::vector4::operator-()
  const noexcept
{
  return inverse();
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4&
  alloy::core::vector4::operator+=( const vector4& rhs )
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] += rhs.m_data[i];
  }
  return (*this);
}

inline constexpr alloy::core::vector4&
  alloy::core::vector4::operator-=( const vector4& rhs )
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] -= rhs.m_data[i];
  }

  return (*this);
}

inline constexpr alloy::core::vector4&
  alloy::core::vector4::operator*=( real scalar )
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] *= scalar;
  }

  return (*this);
}

inline constexpr alloy::core::vector4&
  alloy::core::vector4::operator/=( real scalar )
  noexcept
{
  const auto inv = (real{1} / scalar);

  for (auto i = 0; i < 4; ++i) {
    m_data[i] *= inv;
  }

  return (*this);
}

//==============================================================================
// non-member functions : class : vector4
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4
  alloy::core::operator+( const vector4& lhs, const vector4& rhs )
  noexcept
{
  return vector4(lhs)+=rhs;
}

inline constexpr alloy::core::vector4
  alloy::core::operator-( const vector4& lhs, const vector4& rhs )
  noexcept
{
  return vector4(lhs)-=rhs;
}

inline constexpr alloy::core::vector4
  alloy::core::operator*( const vector4& lhs, real scalar )
  noexcept
{
  return vector4(lhs)*=scalar;
}

inline constexpr alloy::core::vector4
  alloy::core::operator*( real scalar, const vector4& lhs )
  noexcept
{
  return vector4(lhs)*=scalar;
}

inline constexpr alloy::core::vector4
  alloy::core::operator/( const vector4& lhs, real scalar )
  noexcept
{
  return vector4(lhs)/=scalar;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

inline constexpr bool alloy::core::operator==( const vector4& lhs,
                                               const vector4& rhs )
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::operator!=( const vector4& lhs,
                                               const vector4& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//----------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal( const vector4& lhs,
                                                 const vector4& rhs )
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (!almost_equal(rhs[i], rhs[i])) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::almost_equal( const vector4& lhs,
                                                 const vector4& rhs,
                                                 real tolerance )
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (!almost_equal(rhs[i], rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::dot( const vector4& lhs, const vector4& rhs )
  noexcept
{
  return lhs.dot(rhs);
}

inline constexpr alloy::core::vector4
  alloy::core::cross( const vector4& lhs, const vector4& rhs )
  noexcept
{
  return lhs.cross(rhs);
}

inline alloy::core::real
  alloy::core::magnitude( const vector4& vec )
  noexcept
{
  return vec.magnitude();
}

//==============================================================================
// struct : piecewise_compare<vector4>
//==============================================================================

inline constexpr bool
  alloy::core::piecewise_compare<alloy::core::vector4>
  ::operator()( const vector4& lhs, const vector4& rhs )
  noexcept
{
  return (lhs.x() == rhs.x()) ?
           (lhs.y() == rhs.y()) ?
             (lhs.z() == rhs.z()) ?
               (lhs.w() < rhs.w()) :
             (lhs.z() < rhs.z()) :
           (lhs.y() < rhs.y()) :
         (lhs.x() < rhs.x());
}

#endif /* ALLOY_CORE_MATH_DETAIL_VECTOR_VECTOR4_INL */
