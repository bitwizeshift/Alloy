#ifndef ALLOY_MATH_ANGLE_DETAIL_BASIC_ANGLE_INL
#define ALLOY_MATH_ANGLE_DETAIL_BASIC_ANGLE_INL

#ifndef ALLOY_MATH_ANGLE_BASIC_ANGLE_HPP
# error "basic_angle.inl included without first including declaration header basic_angle.hpp"
#endif

//==============================================================================
// class : basic_angle<AngleUnit>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr alloy::math::basic_angle<AngleUnit>
  ::basic_angle()
  noexcept
  : m_angle{0}
{

}

template<typename AngleUnit>
inline constexpr alloy::math::basic_angle<AngleUnit>
  ::basic_angle( core::real angle )
  noexcept
  : m_angle{angle}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr alloy::core::real
  alloy::math::basic_angle<AngleUnit>::value()
  const noexcept
{
  return m_angle;
}

template<typename AngleUnit>
inline constexpr alloy::core::real
  alloy::math::basic_angle<AngleUnit>::revolutions()
  const noexcept
{
  return m_angle / AngleUnit::revolution();
}

template<typename AngleUnit>
inline alloy::math::basic_angle<AngleUnit>
  alloy::math::basic_angle<AngleUnit>::constrained()
  const noexcept
{
  const auto angle = std::fmod(m_angle, AngleUnit::revolution());

  if(angle < 0) angle += AngleUnit::revolution();

  return basic_angle{ core::real(angle) };
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

template<typename AngleUnit>
constexpr alloy::math::basic_angle<AngleUnit>
  alloy::math::basic_angle<AngleUnit>::operator+()
  const noexcept
{
  return (*this);
}

template<typename AngleUnit>
constexpr alloy::math::basic_angle<AngleUnit>
  alloy::math::basic_angle<AngleUnit>::operator-()
  const noexcept
{
  return basic_angle{-m_angle};
}

//------------------------------------------------------------------------------
// Compound Assignment
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline alloy::math::basic_angle<AngleUnit>&
  alloy::math::basic_angle<AngleUnit>::operator+=( const basic_angle& rhs )
  noexcept
{
  m_angle += rhs.m_angle;
  return (*this);
}

template<typename AngleUnit>
inline alloy::math::basic_angle<AngleUnit>&
  alloy::math::basic_angle<AngleUnit>::operator-=( const basic_angle& rhs )
  noexcept
{
  m_angle -= rhs.m_angle;
  return (*this);
}

template<typename AngleUnit>
inline alloy::math::basic_angle<AngleUnit>&
  alloy::math::basic_angle<AngleUnit>::operator*=( core::real rhs )
  noexcept
{
  m_angle *= rhs;
  return (*this);
}

template<typename AngleUnit>
inline alloy::math::basic_angle<AngleUnit>&
  alloy::math::basic_angle<AngleUnit>::operator/=( core::real rhs )
  noexcept
{
  m_angle /= rhs;
  return (*this);
}

//==============================================================================
// non-member functions : class : basic_angle<AngleUnit>
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr alloy::math::basic_angle<AngleUnit>
  alloy::math::operator+( const basic_angle<AngleUnit>& lhs,
                          const basic_angle<AngleUnit>& rhs )
  noexcept
{
  return basic_angle<AngleUnit>{ lhs.value() + rhs.value() };
}

template<typename AngleUnit>
inline constexpr alloy::math::basic_angle<AngleUnit>
  alloy::math::operator-( const basic_angle<AngleUnit>& lhs,
                          const basic_angle<AngleUnit>& rhs )
  noexcept
{
  return basic_angle<AngleUnit>{ lhs.value() - rhs.value() };
}

template<typename AngleUnit>
inline constexpr alloy::math::basic_angle<AngleUnit>
  alloy::math::operator*( const basic_angle<AngleUnit>& lhs, core::real rhs )
  noexcept
{
  return basic_angle<AngleUnit>{ lhs.value() * rhs };
}

template<typename AngleUnit>
inline constexpr alloy::math::basic_angle<AngleUnit>
  alloy::math::operator*( core::real lhs, const basic_angle<AngleUnit>& rhs )
  noexcept
{
  return basic_angle<AngleUnit>{ rhs.value() * lhs };
}

template<typename AngleUnit>
inline constexpr alloy::math::basic_angle<AngleUnit>
  alloy::math::operator/( const basic_angle<AngleUnit>& lhs, core::real rhs )
  noexcept
{
  return basic_angle<AngleUnit>{ lhs.value() / rhs };
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr bool
  alloy::math::operator==( const basic_angle<AngleUnit>& lhs,
                           const basic_angle<AngleUnit>& rhs )
  noexcept
{
  return lhs.value() == rhs.value();
}

template<typename AngleUnit>
inline constexpr bool
  alloy::math::operator!=( const basic_angle<AngleUnit>& lhs,
                           const basic_angle<AngleUnit>& rhs )
  noexcept
{
  return !(lhs==rhs);
}

template<typename AngleUnit>
inline constexpr bool
  alloy::math::operator<( const basic_angle<AngleUnit>& lhs,
                          const basic_angle<AngleUnit>& rhs )
  noexcept
{
  return lhs.value() < rhs.value();
}

template<typename AngleUnit>
inline constexpr bool
  alloy::math::operator>( const basic_angle<AngleUnit>& lhs,
                          const basic_angle<AngleUnit>& rhs )
  noexcept
{
  return lhs.value() > rhs.value();
}

template<typename AngleUnit>
inline constexpr bool
  alloy::math::operator<=( const basic_angle<AngleUnit>& lhs,
                           const basic_angle<AngleUnit>& rhs )
  noexcept
{
  return lhs.value() <= rhs.value();
}

template<typename AngleUnit>
inline constexpr bool
  alloy::math::operator>=( const basic_angle<AngleUnit>& lhs,
                           const basic_angle<AngleUnit>& rhs )
  noexcept
{
  return lhs.value() >= rhs.value();
}

//----------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr bool
  alloy::math::almost_equal( const basic_angle<AngleUnit>& lhs,
                             const basic_angle<AngleUnit>& rhs )
  noexcept
{
  return almost_equal( lhs.value(), rhs.value() );
}

template<typename AngleUnit>
inline constexpr bool
  alloy::math::almost_equal( const basic_angle<AngleUnit>& lhs,
                             const basic_angle<AngleUnit>& rhs,
                             core::real tolerance )
  noexcept
{
  return almost_equal( lhs.value(), rhs.value(), tolerance );
}

//-----------------------------------------------------------------------------
// Utilities : Math Functions
//-----------------------------------------------------------------------------

template<typename AngleUnit>
alloy::math::basic_angle<AngleUnit>
  alloy::math::round( basic_angle<AngleUnit> angle )
  noexcept
{
  return basic_angle<AngleUnit>{ std::round(angle.value()) };
}

template<typename AngleUnit>
alloy::math::basic_angle<AngleUnit>
  alloy::math::ceil( basic_angle<AngleUnit> angle )
  noexcept
{
  return basic_angle<AngleUnit>{ std::ceil(angle.value()) };
}

template<typename AngleUnit>
alloy::math::basic_angle<AngleUnit>
  alloy::math::floor( basic_angle<AngleUnit> angle )
  noexcept
{
  return basic_angle<AngleUnit>{ std::floor(angle.value()) };
}

template<typename AngleUnit>
alloy::math::basic_angle<AngleUnit>
  alloy::math::trunc( basic_angle<AngleUnit> angle )
  noexcept
{
  return basic_angle<AngleUnit>{ std::trunc(angle.value()) };
}

//------------------------------------------------------------------------

template<typename AngleUnit>
alloy::math::basic_angle<AngleUnit>
  alloy::math::abs( basic_angle<AngleUnit> angle )
  noexcept
{
  return basic_angle<AngleUnit>{ std::abs(angle.value()) };
}

//============================================================================
// casts : class : basic_angle<AngleUnit>
//============================================================================

namespace alloy::math::detail {
  // case: From != To
  template<typename AngleUnitTo, typename AngleUnitFrom>
  inline constexpr basic_angle<AngleUnitTo>
    angle_cast( basic_angle<AngleUnitFrom> from )
    noexcept
  {
    using From = AngleUnitFrom;
    using To   = AngleUnitTo;

    constexpr auto factor = To::revolution() / From::revolution();

    return basic_angle{ from.value() * factor };
  }

  // case: From == To
  template<typename AngleUnit>
  inline constexpr const basic_angle<AngleUnit>
    angle_cast( basic_angle<AngleUnit> from )
    noexcept
  {
    return from;
  }
}

template<typename To, typename From>
inline constexpr To alloy::math::casts::angle_cast( From from )
{
  return detail::angle_cast<typename To::angle_unit>( from );
}

#endif /* ALLOY_MATH_ANGLE_DETAIL_BASIC_ANGLE_INL */