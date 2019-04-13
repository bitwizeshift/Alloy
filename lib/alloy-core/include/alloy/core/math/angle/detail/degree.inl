#ifndef ALLOY_CORE_MATH_ANGLE_DEGREE_INL
#define ALLOY_CORE_MATH_ANGLE_DEGREE_INL

#ifndef ALLOY_CORE_MATH_ANGLE_DEGREE_HPP
# error "degree.inl included without first including declaration header degree.hpp"
#endif

//==============================================================================
// struct : degree_unit
//==============================================================================

inline constexpr alloy::core::real alloy::core::degree_unit::revolution()
  noexcept
{
  return static_cast<real>(360);
}

//==============================================================================
// non-member functions : class : degree
//==============================================================================

//------------------------------------------------------------------------------
// Utilities : Casting
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr alloy::core::degree
  alloy::core::to_degree( basic_angle<AngleUnit> angle )
  noexcept
{
  return angle_cast<degree>(angle);
}

//==============================================================================
// Literals
//==============================================================================

inline constexpr alloy::core::degree
  alloy::core::literals::operator ""_deg( long double x )
  noexcept
{
  return degree{ static_cast<real>(x) };
}

#endif /* ALLOY_CORE_MATH_ANGLE_DEGREE_INL */