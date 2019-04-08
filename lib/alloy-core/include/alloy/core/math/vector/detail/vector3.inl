#ifndef ALLOY_CORE_MATH_VECTOR_DETAIL_VECTOR3_INL
#define ALLOY_CORE_MATH_VECTOR_DETAIL_VECTOR3_INL

#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR3_HPP
# error "vector3.inl included without first including declaration header vector3.hpp"
#endif

//==============================================================================
// class : vector3
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3::vector3()
  noexcept
  : m_data{
      real{0},
      real{0},
      real{0}
    }
{

}

inline constexpr alloy::core::vector3::vector3( real x, real y, real z )
  noexcept
  : m_data{x,y,z}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3::size_type
  alloy::core::vector3::size()
  const noexcept
{
  return 3;
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3::reference
  alloy::core::vector3::x()
  noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::vector3::const_reference
  alloy::core::vector3::x()
  const noexcept
{
  return m_data[0];
}

//----------------------------------------------------------------------------

inline constexpr alloy::core::vector3::reference
  alloy::core::vector3::y()
  noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::vector3::const_reference
  alloy::core::vector3::y()
  const noexcept
{
  return m_data[1];
}

//----------------------------------------------------------------------------

inline constexpr alloy::core::vector3::reference
  alloy::core::vector3::z()
  noexcept
{
  return m_data[2];
}

inline constexpr alloy::core::vector3::const_reference
  alloy::core::vector3::z()
  const noexcept
{
  return m_data[2];
}

//----------------------------------------------------------------------------

inline constexpr alloy::core::vector3::pointer
  alloy::core::vector3::data()
  noexcept
{
  return m_data;
}

inline constexpr alloy::core::vector3::const_pointer
  alloy::core::vector3::data()
  const noexcept
{
  return m_data;
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3::reference
  alloy::core::vector3::at( index_type n )
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 3 || n < 0 ) {
    throw std::out_of_range("alloy::core::vector3::at: index out of range");
  }
#else
  assert( n < 3 && n >= 0 );
#endif
  return m_data[n];
}

inline constexpr alloy::core::vector3::const_reference
  alloy::core::vector3::at( index_type n )
  const
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 3 || n < 0 ) {
    throw std::out_of_range("alloy::core::vector3::at: index out of range");
  }
#else
  assert( n < 3 && n >= 0 );
#endif
  return m_data[n];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3::reference
  alloy::core::vector3::operator[]( index_type n )
  noexcept
{
  assert( n < 3 && n >= 0 );
  return m_data[n];
}

inline constexpr alloy::core::vector3::const_reference
alloy::core::vector3::operator[]( index_type n )
  const noexcept
{
  assert( n < 3 && n >= 0 );
  return m_data[n];
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::vector3::dot( const vector3& other )
  const noexcept
{
  return (x() * other.x()) + (y() * other.y()) + (z() * other.z());
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3
  alloy::core::vector3::cross( const vector3& other )
  const noexcept
{
  return vector3{
    (y() * other.z() - z() * other.y()),
    (z() * other.x() - x() * other.z()),
    (x() * other.y() - y() * other.x())
  };
}

//------------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::vector3::magnitude()
  const noexcept
{
  return sqrt( (x()*x()) + (y()*y()) + (z()*z()) );
}

inline constexpr alloy::core::vector3
  alloy::core::vector3::midpoint( const vector3& rhs )
  const noexcept
{
  return vector3{
    ((x() + rhs.x()) * 0.5f),
    ((y() + rhs.y()) * 0.5f),
    ((z() + rhs.z()) * 0.5f)
  };
}

inline constexpr alloy::core::vector3
  alloy::core::vector3::reflection( const vector3& normal )
  const noexcept
{
  return (*this) - ((2*dot(normal)) * normal);
}

inline constexpr alloy::core::vector3
  alloy::core::vector3::projection( const vector3& vector )
  const noexcept
{
  const auto multiplier = dot(vector) / dot(*this);

  return vector3{
    multiplier * x(),
    multiplier * y(),
    multiplier * z()
  };
}

inline constexpr alloy::core::vector3
  alloy::core::vector3::rejection( const vector3& vector )
  const noexcept
{
  return (*this) - projection(vector);
}


inline alloy::core::vector3
  alloy::core::vector3::perpendicular()
  const noexcept
{
  constexpr auto unit_x = vector3{ real{1}, real{0}, real{0} };
  constexpr auto unit_y = vector3{ real{0}, real{1}, real{0} };

  // Try perpendicular to x-axis
  // if result is 0, try y-axis
  auto perp = cross( unit_x );

  if (almost_equal( perp.dot(perp), 0 )) {
    perp = cross( unit_y );
  }

  return perp.normalize();
}


inline alloy::core::vector3
  alloy::core::vector3::normalized()
  const noexcept
{
  const auto square_mag = dot(*this);
  if (square_mag > real{0}) {
    // TODO(bitwize): consider replacing with inv_sqrt eventually?
    const auto mag_inv = real{1} / sqrt(square_mag);
    return vector3{
      x() * mag_inv,
      y() * mag_inv,
      z() * mag_inv
    };
  }
  return (*this);
}

inline constexpr alloy::core::vector3
  alloy::core::vector3::inverse()
  const noexcept
{
  return vector3{ -x(), -y(), -z() };
}

inline alloy::core::radian
  alloy::core::vector3::angle_between( const vector3& other )
  const noexcept
{
  auto mag_product = magnitude() * other.magnitude();

  if (almost_equal( mag_product, real{0} )){
    mag_product = default_tolerance;
  }

  auto f = dot(other) / mag_product;

  f = clamp( f, real{-1.0}, real{1.0} );
  return trigonometry::arccos( f );
}

inline alloy::core::radian
  alloy::core::vector3::angle_to( const vector3& other )
  const noexcept
{
  const auto angle = angle_between( other );

  // if (cross(other) < real{0}) {
  //   return radian_constants::revolution - angle;
  // }

  return angle;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline alloy::core::vector3&
  alloy::core::vector3::normalize()
  noexcept
{
  const auto mag = magnitude();

  if (mag > real{0}){
    const auto mag_inv = real{1} / mag;

    x() *= mag_inv;
    y() *= mag_inv;
    z() *= mag_inv;
  }

  return (*this);
}

inline constexpr alloy::core::vector3&
  alloy::core::vector3::invert()
  noexcept
{
  x() = -x();
  y() = -y();
  z() = -z();

  return (*this);
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

inline constexpr const alloy::core::vector3&
  alloy::core::vector3::operator+()
  const noexcept
{
  return (*this);
}

inline constexpr alloy::core::vector3
  alloy::core::vector3::operator-()
  const noexcept
{
  return vector3{ -x(), -y(), -z() };
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3&
  alloy::core::vector3::operator+=( const vector3& rhs )
  noexcept
{
  x() += rhs.x();
  y() += rhs.y();
  z() += rhs.z();
  return (*this);
}

inline constexpr alloy::core::vector3&
  alloy::core::vector3::operator-=( const vector3& rhs )
  noexcept
{
  x() -= rhs.x();
  y() -= rhs.y();
  z() -= rhs.z();
  return (*this);
}

inline constexpr alloy::core::vector3&
  alloy::core::vector3::operator*=( real scalar )
  noexcept
{
  x() *= scalar;
  y() *= scalar;
  z() *= scalar;
  return (*this);
}

inline constexpr alloy::core::vector3&
  alloy::core::vector3::operator/=( real scalar )
  noexcept
{
  const auto inv = real{1} / scalar;

  x() *= inv;
  y() *= inv;
  z() *= inv;
  return (*this);
}

//==============================================================================
// non-member functions : class : vector3
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3
  alloy::core::operator+( const vector3& lhs, const vector3& rhs )
  noexcept
{
  return vector3{lhs}+=rhs;
}

inline constexpr alloy::core::vector3
  alloy::core::operator-( const vector3& lhs, const vector3& rhs )
  noexcept
{
  return vector3{lhs}-=rhs;
}

inline constexpr alloy::core::vector3
  alloy::core::operator*( const vector3& lhs, real scalar )
  noexcept
{
  return vector3{lhs}*=scalar;
}

inline constexpr alloy::core::vector3
  alloy::core::operator*( real scalar, const vector3& lhs )
  noexcept
{
  return vector3{lhs}*=scalar;
}

inline constexpr alloy::core::vector3
  alloy::core::operator/( const vector3& lhs, real scalar )
  noexcept
{
  return vector3{lhs}/=scalar;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

inline constexpr bool alloy::core::operator==( const vector3& lhs,
                                               const vector3& rhs )
  noexcept
{
  for (auto i=0;i<3;++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::operator!=( const vector3& lhs,
                                               const vector3& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal( const vector3& lhs,
                                                 const vector3& rhs )
  noexcept
{
  for (auto i=0;i<3;++i) {
    if (!almost_equal(rhs[i],rhs[i])) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::almost_equal( const vector3& lhs,
                                                 const vector3& rhs,
                                                 real tolerance )
  noexcept
{
  for (auto i=0;i<3;++i) {
    if (!almost_equal(rhs[i],rhs[i],tolerance)) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------

inline constexpr bool
  alloy::core::are_linearly_independent( const vector3& v1,
                                         const vector3& v2,
                                         const vector3& v3 )
  noexcept
{
  // linear-independence tested by checking if the determinant of a produced
  // 3x3 matrix is non-zero
  const vector3 matrix[3] = {v1,v2,v3};

  const auto determinant = (matrix[0][0] * matrix[1][1] * matrix[2][2]) -
                           (matrix[0][0] * matrix[1][2] * matrix[2][1]) -
                           (matrix[0][1] * matrix[1][0] * matrix[2][2]) +
                           (matrix[0][1] * matrix[1][2] * matrix[2][0]) +
                           (matrix[0][2] * matrix[1][0] * matrix[2][1]) -
                           (matrix[0][2] * matrix[1][1] * matrix[2][0]);

  return !almost_equal( real{0}, determinant );
}



//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::dot( const vector3& lhs, const vector3& rhs )
  noexcept
{
  return lhs.dot(rhs);
}

inline constexpr alloy::core::vector3
  alloy::core::cross( const vector3& lhs, const vector3& rhs )
  noexcept
{
  return lhs.cross(rhs);
}

inline alloy::core::real
  alloy::core::magnitude( const vector3& vec )
  noexcept
{
  return vec.magnitude();
}

//==============================================================================
// struct : piecewise_compare<vector3>
//==============================================================================

inline constexpr bool
  alloy::core::piecewise_compare<alloy::core::vector3>
  ::operator()( const vector3& lhs, const vector3& rhs )
  noexcept
{
  return (lhs.x() == rhs.x()) ?
           (lhs.y() == rhs.y()) ?
             (lhs.z() < rhs.z()) :
           (lhs.y() < rhs.y()) :
         (lhs.x() < rhs.x());
}

#endif /* ALLOY_CORE_MATH_VECTOR_DETAIL_VECTOR3_INL */
