#ifndef ALLOY_MATH_VECTOR_DETAIL_VECTOR3_INL
#define ALLOY_MATH_VECTOR_DETAIL_VECTOR3_INL

#ifndef ALLOY_MATH_VECTOR_VECTOR3_HPP
# error "vector3.inl included without first including declaration header vector3.hpp"
#endif

//==============================================================================
// class : vector3<T>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::math::vector3<T>::vector3( value_type x,
                                                   value_type y,
                                                   value_type z )
  noexcept
  : m_data{x,y,z}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector3<T>::size_type
  alloy::math::vector3<T>::size()
  const noexcept
{
  return 3;
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector3<T>::reference
  alloy::math::vector3<T>::x()
  noexcept
{
  return m_data[0];
}

template<typename T>
inline constexpr typename alloy::math::vector3<T>::const_reference
  alloy::math::vector3<T>::x()
  const noexcept
{
  return m_data[0];
}

//----------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector3<T>::reference
  alloy::math::vector3<T>::y()
  noexcept
{
  return m_data[1];
}

template<typename T>
inline constexpr typename alloy::math::vector3<T>::const_reference
  alloy::math::vector3<T>::y()
  const noexcept
{
  return m_data[1];
}

//----------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector3<T>::reference
  alloy::math::vector3<T>::z()
  noexcept
{
  return m_data[2];
}

template<typename T>
inline constexpr typename alloy::math::vector3<T>::const_reference
  alloy::math::vector3<T>::z()
  const noexcept
{
  return m_data[2];
}

//----------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector3<T>::pointer
  alloy::math::vector3<T>::data()
  noexcept
{
  return m_data;
}

template<typename T>
inline constexpr typename alloy::math::vector3<T>::const_pointer
  alloy::math::vector3<T>::data()
  const noexcept
{
  return m_data;
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector3<T>::reference
  alloy::math::vector3<T>::at( index_type n )
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 3 || n < 0 ) {
    throw std::out_of_range("alloy::math::vector3<T>::at: index out of range");
  }
#else
  assert( n < 3 && n >= 0 );
#endif
  return m_data[n];
}

template<typename T>
inline constexpr typename alloy::math::vector3<T>::const_reference
alloy::math::vector3<T>::at( index_type n )
  const
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 3 || n < 0 ) {
    throw std::out_of_range("alloy::math::vector3<T>::at: index out of range");
  }
#else
  assert( n < 3 && n >= 0 );
#endif
  return m_data[n];
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector3<T>::reference
  alloy::math::vector3<T>::operator[]( index_type n )
  noexcept
{
  assert( n < 3 && n >= 0 );
  return m_data[n];
}

template<typename T>
inline constexpr typename alloy::math::vector3<T>::const_reference
alloy::math::vector3<T>::operator[]( index_type n )
  const noexcept
{
  assert( n < 3 && n >= 0 );
  return m_data[n];
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline constexpr std::common_type_t<T,U>
  alloy::math::vector3<T>::dot( const vector3<U>& other )
  const noexcept
{
  return (x() * other.x()) + (y() * other.y()) + (z() * other.z());
}

//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline constexpr alloy::math::vector3<std::common_type_t<T,U>>
  alloy::math::vector3<T>::cross( const vector3<U>& other )
  const noexcept
{
  return {
    (y() * other.z() - z() * other.y()),
    (z() * other.x() - x() * other.z()),
    (x() * other.y() - y() * other.x())
  };
}

//------------------------------------------------------------------------------

template<typename T>
inline typename alloy::core::real
  alloy::math::vector3<T>::magnitude()
  const noexcept
{
  return sqrt( (x()*x()) + (y()*y()) + (z()*z()) );
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector3<std::common_type_t<T,U>>
  alloy::math::vector3<T>::midpoint( const vector3<U>& rhs )
  const noexcept
{
  return {
    ((x() + rhs.x()) * 0.5f),
    ((y() + rhs.y()) * 0.5f),
    ((z() + rhs.z()) * 0.5f)
  };
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector3<std::common_type_t<T,U>>
  alloy::math::vector3<T>::reflection( const vector3<U>& normal )
  const noexcept
{
  return (*this) - ((2*dot(normal)) * normal);
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector3<std::common_type_t<T,U>>
  alloy::math::vector3<T>::projection( const vector3<U>& vector )
  const noexcept
{
  const auto multiplier = dot(vector) / dot(*this);

  return {
    multiplier * x(),
    multiplier * y(),
    multiplier * z()
  };
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector3<std::common_type_t<T,U>>
  alloy::math::vector3<T>::rejection( const vector3<U>& vector )
  const noexcept
{
  return (*this) - projection(vector);
}


template<typename T>
inline constexpr alloy::math::vector3<T>
  alloy::math::vector3<T>::perpendicular()
  const noexcept
{
  // Try perpendicular to x-axis
  // if result is 0, try y-axis
  auto perp = cross( vector3_constants<T>::unit_x );

  if( almost_equal( perp.dot(perp), 0 ) ) {
    perp = cross( vector3_constants<T>::unit_y );
  }

  return perp.normalize();
}


template<typename T>
inline alloy::math::vector3<T>
  alloy::math::vector3<T>::normalized()
  const noexcept
{
  const auto square_mag = dot(*this);
  if( square_mag > 0 ){
    // TODO(bitwize): ocnsider replacing with inv_sqrt eventually?
    const auto mag_inv = core::real{1} / sqrt(square_mag);
    return {
      x() * mag_inv,
      y() * mag_inv,
      z() * mag_inv
    };
  }
  return (*this);
}

template<typename T>
inline constexpr alloy::math::vector3<T>
  alloy::math::vector3<T>::inverse()
  const noexcept
{
  return { -x(), -y(), -z() };
}

template<typename T>
template<typename U>
alloy::math::radian
  alloy::math::vector3<T>::angle_between( const vector3<U>& other )
  const noexcept
{
  auto mag_product = magnitude() * other.magnitude();

  if (almost_equal( mag_product, 0 )){
    mag_product = default_tolerance;
  }

  auto f = dot(other) / mag_product;

  f = clamp( f, -1.0, 1.0 );
  return trigonometry::arccos( f );
}

template<typename T>
template<typename U>
alloy::math::radian
  alloy::math::vector3<T>::angle_to( const vector3<U>& other )
  const noexcept
{
  auto angle = angle_between( other );

  if (cross(other) < T{0})
    return radian_constants::revolution - angle;

  return angle;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::math::vector3<T>& alloy::math::vector3<T>::normalize()
  noexcept
{
  const auto mag = magnitude();

  if( mag > 0 ){
    const auto mag_inv = core::real{1} / mag;

    x() *= mag_inv;
    y() *= mag_inv;
    z() *= mag_inv;
  }

  return (*this);
}

template<typename T>
inline constexpr alloy::math::vector3<T>& alloy::math::vector3<T>::invert()
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

template<typename T>
inline constexpr const alloy::math::vector3<T>&
  alloy::math::vector3<T>::operator+()
  const noexcept
{
  return (*this);
}

template<typename T>
inline constexpr alloy::math::vector3<T>
  alloy::math::vector3<T>::operator-()
  const noexcept
{
  return vector3<T>( -x(), -y(), -z() );
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline constexpr alloy::math::vector3<T>&
  alloy::math::vector3<T>::operator+=( const vector3<U>& rhs )
  noexcept
{
  x() += rhs.x();
  y() += rhs.y();
  z() += rhs.z();
  return (*this);
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector3<T>&
  alloy::math::vector3<T>::operator-=( const vector3<U>& rhs )
  noexcept
{
  x() -= rhs.x();
  y() -= rhs.y();
  z() -= rhs.z();
  return (*this);
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector3<T>&
  alloy::math::vector3<T>::operator*=( U scalar )
  noexcept
{
  x() *= scalar;
  y() *= scalar;
  z() *= scalar;
  return (*this);
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector3<T>&
  alloy::math::vector3<T>::operator/=( U scalar )
  noexcept
{
  const auto inv = core::real{1} / scalar;

  x() *= inv;
  y() *= inv;
  z() *= inv;
  return (*this);
}

//==============================================================================
// non-member functions : class : vector3<T>
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr alloy::math::vector3<std::common_type_t<T,U>>
  alloy::math::operator+( const vector3<T>& lhs, const vector3<U>& rhs )
  noexcept
{
  return vector3<std::common_type_t<T,U>>{lhs}+=rhs;
}

template<typename T, typename U>
inline constexpr alloy::math::vector3<std::common_type_t<T,U>>
  alloy::math::operator-( const vector3<T>& lhs, const vector3<U>& rhs )
  noexcept
{
  return vector3<std::common_type_t<T,U>>{lhs}-=rhs;
}

template<typename T, typename U, typename>
inline constexpr alloy::math::vector3<std::common_type_t<T,U>>
  alloy::math::operator*( const vector3<T>& lhs, U scalar )
  noexcept
{
  return vector3<std::common_type_t<T,U>>{lhs}*=scalar;
}

template<typename T, typename U, typename>
inline constexpr alloy::math::vector3<std::common_type_t<T,U>>
  alloy::math::operator*( T scalar, const vector3<U>& lhs )
  noexcept
{
  return vector3<std::common_type_t<T,U>>{lhs}*=scalar;
}

template<typename T, typename U, typename>
inline constexpr alloy::math::vector3<std::common_type_t<T,U>>
  alloy::math::operator/( const vector3<T>& lhs, U scalar )
  noexcept
{
  return vector3<std::common_type_t<T,U>>{lhs}/=scalar;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::math::operator==( const vector3<T>& lhs,
                                               const vector3<U>& rhs )
  noexcept
{
  for(auto i=0;i<3;++i) {
    if( lhs[i]!=rhs[i] ) return false;
  }
  return true;
}

template<typename T, typename U>
inline constexpr bool alloy::math::operator!=( const vector3<T>& lhs,
                                               const vector3<U>& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::math::almost_equal( const vector3<T>& lhs,
                                                 const vector3<U>& rhs )
  noexcept
{
  for(auto i=0;i<3;++i) {
    if( !almost_equal(rhs[i],rhs[i]) ) return false;
  }
  return true;
}

template<typename T, typename U, typename Arithmetic, typename>
inline constexpr bool alloy::math::almost_equal( const vector3<T>& lhs,
                                                 const vector3<U>& rhs,
                                                 Arithmetic tolerance )
  noexcept
{
  for(auto i=0;i<3;++i) {
    if( !almost_equal(rhs[i],rhs[i],tolerance) ) return false;
  }
  return true;
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr std::common_type_t<T,U>
  alloy::math::dot( const vector3<T>& lhs, const vector3<U>& rhs )
  noexcept
{
  return lhs.dot(rhs);
}

template<typename T, typename U>
inline constexpr alloy::math::vector3<std::common_type_t<T,U>>
  alloy::math::cross( const vector3<T>& lhs, const vector3<U>& rhs )
  noexcept
{
  return lhs.cross(rhs);
}

template<typename T>
typename alloy::core::real
  alloy::math::magnitude( const vector3<T>& vec )
  noexcept
{
  return vec.magnitude();
}

#endif /* ALLOY_MATH_VECTOR_DETAIL_VECTOR3_INL */
