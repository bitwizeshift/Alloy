#ifndef ALLOY_MATH_VECTOR_DETAIL_VECTOR2_INL
#define ALLOY_MATH_VECTOR_DETAIL_VECTOR2_INL

#ifndef ALLOY_MATH_VECTOR_VECTOR2_HPP
# error "vector2.inl included without first including declaration header vector2.hpp"
#endif

//==============================================================================
// class : vector2<T>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template<typename T>
inline alloy::math::vector2<T>::vector2( value_type magnitude,
                                         radian direction )
  noexcept
  : m_data {
      static_cast<T>( magnitude * trigonometry::cos(direction) ),
      static_cast<T>( magnitude * trigonometry::sin(direction) )
    }
{

}

template<typename T>
inline constexpr alloy::math::vector2<T>::vector2( value_type x,
                                                   value_type y )
  noexcept
  : m_data{x,y}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector2<T>::size_type
  alloy::math::vector2<T>::size()
  const noexcept
{
  return 2;
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector2<T>::reference
  alloy::math::vector2<T>::x()
  noexcept
{
  return m_data[0];
}

template<typename T>
inline constexpr typename alloy::math::vector2<T>::const_reference
  alloy::math::vector2<T>::x()
  const noexcept
{
  return m_data[0];
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector2<T>::reference
  alloy::math::vector2<T>::y()
  noexcept
{
  return m_data[1];
}

template<typename T>
inline constexpr typename alloy::math::vector2<T>::const_reference
  alloy::math::vector2<T>::y()
  const noexcept
{
  return m_data[1];
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector2<T>::pointer
  alloy::math::vector2<T>::data()
  noexcept
{
  return m_data;
}

template<typename T>
inline constexpr typename alloy::math::vector2<T>::const_pointer
  alloy::math::vector2<T>::data()
  const noexcept
{
  return m_data;
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector2<T>::reference
  alloy::math::vector2<T>::at( index_type n )
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 2 || n < 0 ) {
    throw std::out_of_range("alloy::math::vector2<T>::at: index out of range");
  }
#else
  assert( n < 2 && n >= 0 );
#endif
  return m_data[n];
}

template<typename T>
inline constexpr typename alloy::math::vector2<T>::const_reference
alloy::math::vector2<T>::at( index_type n )
  const
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 2 || n < 0 ) {
    throw std::out_of_range("alloy::math::vector2<T>::at: index out of range");
  }
#else
  assert( n < 2 && n >= 0 );
#endif
  return m_data[n];
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::math::vector2<T>::reference
  alloy::math::vector2<T>::operator[]( index_type n )
  noexcept
{
  assert( n < 2 && n >= 0 );
  return m_data[n];
}

template<typename T>
inline constexpr typename alloy::math::vector2<T>::const_reference
alloy::math::vector2<T>::operator[]( index_type n )
  const noexcept
{
  assert( n < 2 && n >= 0 );
  return m_data[n];
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline constexpr std::common_type_t<T,U>
  alloy::math::vector2<T>::dot( const vector2<U>& other )
  const noexcept
{
  return (x() * other.x()) + (y() * other.y());
}

//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline constexpr std::common_type_t<T,U>
  alloy::math::vector2<T>::cross( const vector2<U>& other )
  const noexcept
{
  return (x() * other.y()) - (y() * other.x());
}

//------------------------------------------------------------------------------

template<typename T>
inline typename alloy::core::real
  alloy::math::vector2<T>::magnitude()
  const noexcept
{
  return sqrt( (x()*x()) + (y()*y()) );
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::vector2<T>::midpoint( const vector2<U>& rhs )
  const noexcept
{
  return {
    ((x() + rhs.x()) * 0.5f),
    ((y() + rhs.y()) * 0.5f)
  };
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::vector2<T>::reflection( const vector2<U>& normal )
  const noexcept
{
  return (*this) - (normal * (2 * dot(normal)));
}

//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::vector2<T>::projection( const vector2<U>& vector )
  const noexcept
{
  const auto multiplier = dot(vector) / dot(*this);

  return {multiplier*vector.x(),multiplier*vector.y()};
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::vector2<T>::rejection( const vector2<U>& vector )
  const noexcept
{
  return (*this) - projection(vector);
}


//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::math::vector2<T>
  alloy::math::vector2<T>::perpendicular()
  const noexcept
{
  return vector2{ -y(), x() };
}


template<typename T>
inline alloy::math::vector2<T>
  alloy::math::vector2<T>::normalized()
  const noexcept
{
  const auto mag = magnitude();
  if( mag > 0 ){
    const auto mag_inv = core::real{1} / mag;
    return vector2{ x() * mag_inv, y() * mag_inv };
  }
  return (*this);
}

template<typename T>
inline constexpr alloy::math::vector2<T>
  alloy::math::vector2<T>::inverse()
  const noexcept
{
  return vector2<T>( -x(), -y() );
}

//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline alloy::math::radian
  alloy::math::vector2<T>::angle_between( const vector2<U>& other )
  const noexcept
{
  auto mag_product = magnitude() * other.magnitude();

  if( almost_equal( mag_product, 0 ) ){
    mag_product = default_tolerance;
  }

  auto f = dot(other) / mag_product;

  f = clamp( f, -1.0, 1.0 );
  return arccos( f );
}

template<typename T>
template<typename U>
inline alloy::math::radian
  alloy::math::vector2<T>::angle_to( const vector2<U>& other )
  const noexcept
{
  auto angle = angle_between( other );

  if( cross(other) < T{0} ) {
    return radian_constants::revolution - angle;
  }

  return angle;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::math::vector2<T>& alloy::math::vector2<T>::normalize()
  noexcept
{
  const auto mag = magnitude();

  if( mag > 0 ){
    const auto mag_inv = core::real{1} / mag;

    x() *= mag_inv;
    y() *= mag_inv;
  }

  return (*this);
}

template<typename T>
inline constexpr alloy::math::vector2<T>& alloy::math::vector2<T>::invert()
  noexcept
{
  x() = -x();
  y() = -y();

  return (*this);
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

template<typename T>
inline constexpr const alloy::math::vector2<T>& alloy::math::vector2<T>::operator+()
  const noexcept
{
  return (*this);
}

template<typename T>
inline constexpr alloy::math::vector2<T> alloy::math::vector2<T>::operator-()
  const noexcept
{
  return vector2<T>{ -x(), -y() };
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline constexpr alloy::math::vector2<T>&
  alloy::math::vector2<T>::operator+=( const vector2<U>& rhs )
  noexcept
{
  x() += rhs.x();
  y() += rhs.y();
  return (*this);
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector2<T>&
  alloy::math::vector2<T>::operator-=( const vector2<U>& rhs )
  noexcept
{
  x() -= rhs.x();
  y() -= rhs.y();
  return (*this);
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector2<T>&
  alloy::math::vector2<T>::operator*=( U scalar )
  noexcept
{
  x() *= scalar;
  y() *= scalar;
  return (*this);
}

template<typename T>
template<typename U>
inline constexpr alloy::math::vector2<T>&
  alloy::math::vector2<T>::operator/=( U scalar )
  noexcept
{
  const auto inv = core::real{1} / scalar;

  x() *= inv;
  y() *= inv;
  return (*this);
}

//==============================================================================
// non-member functions : class : vector2<T>
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::operator+( const vector2<T>& lhs, const vector2<U>& rhs )
  noexcept
{
  return vector2<std::common_type_t<T,U>>(lhs)+=rhs;
}

template<typename T, typename U>
inline constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::operator-( const vector2<T>& lhs, const vector2<U>& rhs )
  noexcept
{
  return vector2<std::common_type_t<T,U>>(lhs)-=rhs;
}

template<typename T, typename U, typename>
inline constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::operator*( const vector2<T>& lhs, U scalar )
  noexcept
{
  return vector2<std::common_type_t<T,U>>(lhs)*=scalar;
}

template<typename T, typename U, typename>
inline constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::operator*( T scalar, const vector2<U>& lhs )
  noexcept
{
  return vector2<std::common_type_t<T,U>>(lhs)*=scalar;
}

template<typename T, typename U, typename>
inline constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::operator/( const vector2<T>& lhs, U scalar )
  noexcept
{
  return vector2<std::common_type_t<T,U>>(lhs)/=scalar;
}


//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::math::operator==( const vector2<T>& lhs,
                                               const vector2<U>& rhs )
  noexcept
{
  for(auto i=0;i<2;++i) {
    if( lhs[i]!=rhs[i] ) {
      return false;
    }
  }
  return true;
}

template<typename T, typename U>
inline constexpr bool alloy::math::operator!=( const vector2<T>& lhs,
                                               const vector2<U>& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::math::almost_equal( const vector2<T>& lhs,
                                                 const vector2<U>& rhs )
  noexcept
{
  for(auto i=0;i<2;++i) {
    if( !almost_equal(rhs[i],rhs[i]) ) {
      return false;
    }
  }
  return true;
}

template<typename T, typename U, typename Arithmetic, typename>
inline constexpr bool alloy::math::almost_equal( const vector2<T>& lhs,
                                                 const vector2<U>& rhs,
                                                 Arithmetic tolerance )
  noexcept
{
  for(auto i=0;i<2;++i) {
    if( !almost_equal(rhs[i],rhs[i],tolerance) ) return false;
  }
  return true;
}

//----------------------------------------------------------------------------
// Quantifiers
//----------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr std::common_type_t<T,U>
  alloy::math::dot( const vector2<T>& lhs, const vector2<U>& rhs )
  noexcept
{
  return lhs.dot(rhs);
}

template<typename T, typename U>
inline constexpr alloy::math::vector2<std::common_type_t<T,U>>
  alloy::math::cross( const vector2<T>& lhs, const vector2<U>& rhs )
  noexcept
{
  return lhs.cross(rhs);
}

template<typename T>
typename alloy::core::real
  alloy::math::magnitude( const vector2<T>& vec )
  noexcept
{
  return vec.magnitude();
}

#endif /* ALLOY_MATH_VECTOR_DETAIL_VECTOR2_INL */
