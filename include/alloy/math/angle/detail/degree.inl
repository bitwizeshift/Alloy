#ifndef ALLOY_MATH_ANGLE_DEGREE_INL
#define ALLOY_MATH_ANGLE_DEGREE_INL

#ifndef ALLOY_MATH_ANGLE_DEGREE_HPP
# error "degree.inl included without first including declaration header degree.hpp"
#endif

//==============================================================================
// struct : degree_unit
//==============================================================================

inline constexpr alloy::core::real alloy::math::degree_unit::revolution()
  noexcept
{
  return static_cast<core::real>(360);
}

//==============================================================================
// Literals
//==============================================================================

inline constexpr alloy::math::degree
  alloy::math::literals::operator ""_deg( long double x )
  noexcept
{
  return degree{ static_cast<core::real>(x) };
}

#endif /* ALLOY_MATH_ANGLE_DEGREE_INL */