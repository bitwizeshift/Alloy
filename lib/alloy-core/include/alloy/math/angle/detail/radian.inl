#ifndef ALLOY_MATH_ANGLE_RADIAN_INL
#define ALLOY_MATH_ANGLE_RADIAN_INL

#ifndef ALLOY_MATH_ANGLE_RADIAN_HPP
# error "radian.inl included without first including declaration header radian.hpp"
#endif

//==============================================================================
// struct : radian_unit
//==============================================================================

inline constexpr alloy::core::real alloy::math::radian_unit::revolution()
  noexcept
{
  return static_cast<core::real>(constants::two_pi());
}

//==============================================================================
// Literals
//==============================================================================

inline constexpr alloy::math::radian
  alloy::math::literals::operator ""_rad( long double x )
  noexcept
{
  return radian{ static_cast<core::real>(x) };
}

#endif /* ALLOY_MATH_ANGLE_RADIAN_INL */