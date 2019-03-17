#ifndef ALLOY_CORE_DETAIL_VECTOR_VECTOR4_INL
#define ALLOY_CORE_DETAIL_VECTOR_VECTOR4_INL

#ifndef ALLOY_CORE_VECTOR_VECTOR4_HPP
# error "vector4.inl included without first including declaration header vector4.hpp"
#endif

//==============================================================================
// class : vector4<T>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::core::vector4<T>::vector4( value_type x,
                                                   value_type y,
                                                   value_type z,
                                                   value_type w )
  noexcept
  : m_data{x,y,z,w}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::vector4<T>::size_type
  alloy::core::vector4<T>::size()
  const noexcept
{
  return 4;
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::vector4<T>::reference
  alloy::core::vector4<T>::x()
  noexcept
{
  return m_data[0];
}

template<typename T>
inline constexpr typename alloy::core::vector4<T>::const_reference
  alloy::core::vector4<T>::x()
  const noexcept
{
  return m_data[0];
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::vector4<T>::reference
  alloy::core::vector4<T>::y()
  noexcept
{
  return m_data[1];
}

template<typename T>
inline constexpr typename alloy::core::vector4<T>::const_reference
  alloy::core::vector4<T>::y()
  const noexcept
{
  return m_data[1];
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::vector4<T>::reference
  alloy::core::vector4<T>::z()
  noexcept
{
  return m_data[2];
}

template<typename T>
inline constexpr typename alloy::core::vector4<T>::const_reference
  alloy::core::vector4<T>::z()
  const noexcept
{
  return m_data[2];
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::vector4<T>::reference
  alloy::core::vector4<T>::w()
  noexcept
{
  return m_data[3];
}

template<typename T>
inline constexpr typename alloy::core::vector4<T>::const_reference
  alloy::core::vector4<T>::w()
  const noexcept
{
  return m_data[3];
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::vector4<T>::pointer
  alloy::core::vector4<T>::data()
  noexcept
{
  return m_data;
}

template<typename T>
inline constexpr typename alloy::core::vector4<T>::const_pointer
  alloy::core::vector4<T>::data()
  const noexcept
{
  return m_data;
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::vector4<T>::reference
  alloy::core::vector4<T>::at( index_type n )
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 4 || n < 0 ) {
    throw std::out_of_range("alloy::core::vector4<T>::at: index out of range");
  }
#else
  assert( n < 4 && n >= 0 );
#endif
  return m_data[n];
}

template<typename T>
inline constexpr typename alloy::core::vector4<T>::const_reference
alloy::core::vector4<T>::at( index_type n )
  const
{
#if ALLOY_ENABLE_EXCEPTIONS
  if( n >= 4 || n < 0 ) {
    throw std::out_of_range("alloy::core::vector4<T>::at: index out of range");
  }
#else
  assert( n < 4 && n >= 0 );
#endif
  return m_data[n];
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::vector4<T>::reference
  alloy::core::vector4<T>::operator[]( index_type n )
  noexcept
{
  assert( n < 4 && n >= 0 );
  return m_data[n];
}

template<typename T>
inline constexpr typename alloy::core::vector4<T>::const_reference
alloy::core::vector4<T>::operator[]( index_type n )
  const noexcept
{
  assert( n < 4 && n >= 0 );
  return m_data[n];
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline constexpr std::common_type_t<T,U>
  alloy::core::vector4<T>::dot( const vector4<U>& other )
  const noexcept
{
  // Use a loop to suggest vectorized math
  auto sum = std::common_type_t<T,U>(0);

  for( auto i = 0; i < 4; ++i ) {
    sum += m_data[i] * other.m_data[i];
  }

  return sum;
}

//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline constexpr alloy::core::vector4<std::common_type_t<T,U>>
  alloy::core::vector4<T>::cross( const vector4<U>& other )
  const noexcept
{
  return {
    (y() * other.z() - z() * other.y()),
    (z() * other.x() - x() * other.z()),
    (x() * other.y() - y() * other.x()),
    0
  };
}

//------------------------------------------------------------------------------

template<typename T>
inline alloy::core::real
  alloy::core::vector4<T>::magnitude()
  const noexcept
{
  return sqrt( dot(*this) );
}

template<typename T>
template<typename U>
inline constexpr alloy::core::vector4<std::common_type_t<T,U>>
  alloy::core::vector4<T>::midpoint( const vector4<U>& rhs )
  const noexcept
{
  return {
    ((x() + rhs.x()) * 0.5f),
    ((y() + rhs.y()) * 0.5f),
    ((z() + rhs.z()) * 0.5f),
    ((w() + rhs.w()) * 0.5f)
  };
}

template<typename T>
template<typename U>
inline constexpr alloy::core::vector4<std::common_type_t<T,U>>
  alloy::core::vector4<T>::projection( const vector4<U>& vector )
  const noexcept
{
  const auto multiplier = dot(vector) / dot(*this);

  return {
    multiplier * x(),
    multiplier * y(),
    multiplier * z(),
    multiplier * w()
  };
}

template<typename T>
template<typename U>
inline constexpr alloy::core::vector4<std::common_type_t<T,U>>
  alloy::core::vector4<T>::rejection( const vector4<U>& vector )
  const noexcept
{
  return (*this) - projection( vector );
}

template<typename T>
inline alloy::core::vector4<T>
  alloy::core::vector4<T>::normalized()
  const noexcept
{
  return vector4<T>{*this}.normalize();
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::core::vector4<T>
  alloy::core::vector4<T>::inverse()
  const noexcept
{
  return vector4<T>{*this}.invert();
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::core::vector4<T>& alloy::core::vector4<T>::normalize()
  noexcept
{
  const auto mag = magnitude();

  if (mag > 0)
  {
    const auto mag_inv = real{1} / mag;

    for (auto i = 0; i < 4; ++i) {
      m_data[i] *= mag_inv;
    }
  }

  return (*this);
}

template<typename T>
inline constexpr alloy::core::vector4<T>& alloy::core::vector4<T>::invert()
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] *= -1;
  }

  return (*this);
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

template<typename T>
inline constexpr const alloy::core::vector4<T>&
  alloy::core::vector4<T>::operator+()
  const noexcept
{
  return (*this);
}

template<typename T>
inline constexpr alloy::core::vector4<T>
  alloy::core::vector4<T>::operator-()
  const noexcept
{
  return inverse();
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

template<typename T>
template<typename U>
inline constexpr alloy::core::vector4<T>&
  alloy::core::vector4<T>::operator+=( const vector4<U>& rhs )
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] += rhs.m_data[i];
  }
  return (*this);
}

template<typename T>
template<typename U>
inline constexpr alloy::core::vector4<T>&
  alloy::core::vector4<T>::operator-=( const vector4<U>& rhs )
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] -= rhs.m_data[i];
  }

  return (*this);
}

template<typename T>
template<typename U>
inline constexpr alloy::core::vector4<T>&
  alloy::core::vector4<T>::operator*=( U scalar )
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] *= scalar;
  }

  return (*this);
}

template<typename T>
template<typename U>
inline constexpr alloy::core::vector4<T>&
  alloy::core::vector4<T>::operator/=( U scalar )
  noexcept
{
  const auto inv = (real{1} / scalar);

  for (auto i = 0; i < 4; ++i) {
    m_data[i] *= inv;
  }

  return (*this);
}

//==============================================================================
// non-member functions : class : vector4<T>
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr alloy::core::vector4<std::common_type_t<T,U>>
  alloy::core::operator+( const vector4<T>& lhs, const vector4<U>& rhs )
  noexcept
{
  return vector4<std::common_type_t<T,U>>(lhs)+=rhs;
}

template<typename T, typename U>
inline constexpr alloy::core::vector4<std::common_type_t<T,U>>
  alloy::core::operator-( const vector4<T>& lhs, const vector4<U>& rhs )
  noexcept
{
  return vector4<std::common_type_t<T,U>>(lhs)-=rhs;
}

template<typename T, typename U, typename>
inline constexpr alloy::core::vector4<std::common_type_t<T,U>>
  alloy::core::operator*( const vector4<T>& lhs, U scalar )
  noexcept
{
  return vector4<std::common_type_t<T,U>>(lhs)*=scalar;
}

template<typename T, typename U, typename>
inline constexpr alloy::core::vector4<std::common_type_t<T,U>>
  alloy::core::operator*( T scalar, const vector4<U>& lhs )
  noexcept
{
  return vector4<std::common_type_t<T,U>>(lhs)*=scalar;
}

template<typename T, typename U, typename>
inline constexpr alloy::core::vector4<std::common_type_t<T,U>>
  alloy::core::operator/( const vector4<T>& lhs, U scalar )
  noexcept
{
  return vector4<std::common_type_t<T,U>>(lhs)/=scalar;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::core::operator==( const vector4<T>& lhs,
                                               const vector4<U>& rhs )
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

template<typename T, typename U>
inline constexpr bool alloy::core::operator!=( const vector4<T>& lhs,
                                               const vector4<U>& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//----------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool alloy::core::almost_equal( const vector4<T>& lhs,
                                                 const vector4<U>& rhs )
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (!almost_equal(rhs[i], rhs[i])) {
      return false;
    }
  }
  return true;
}

template<typename T, typename U, typename Arithmetic, typename>
inline constexpr bool alloy::core::almost_equal( const vector4<T>& lhs,
                                                 const vector4<U>& rhs,
                                                 Arithmetic tolerance )
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

template<typename T, typename U>
inline constexpr std::common_type_t<T,U>
  alloy::core::dot( const vector4<T>& lhs, const vector4<U>& rhs )
  noexcept
{
  return lhs.dot(rhs);
}

template<typename T, typename U>
inline constexpr alloy::core::vector4<std::common_type_t<T,U>>
  alloy::core::cross( const vector4<T>& lhs, const vector4<U>& rhs )
  noexcept
{
  return lhs.cross(rhs);
}

template<typename T>
typename alloy::core::vector4<T>::value_type
  alloy::core::magnitude( const vector4<T>& vec )
  noexcept
{
  return vec.magnitude();
}

#endif /* ALLOY_CORE_DETAIL_VECTOR_VECTOR4_INL */
