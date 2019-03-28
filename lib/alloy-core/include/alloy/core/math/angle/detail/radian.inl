#ifndef ALLOY_CORE_MATH_ANGLE_RADIAN_INL
#define ALLOY_CORE_MATH_ANGLE_RADIAN_INL

#ifndef ALLOY_CORE_MATH_ANGLE_RADIAN_HPP
# error "radian.inl included without first including declaration header radian.hpp"
#endif

//==============================================================================
// struct : radian_unit
//==============================================================================

inline constexpr alloy::core::real alloy::core::radian_unit::revolution()
  noexcept
{
  return static_cast<real>(math_constants::two_pi());
}

//==============================================================================
// Literals
//==============================================================================

inline constexpr alloy::core::radian
  alloy::core::literals::operator ""_rad( long double x )
  noexcept
{
  return radian{ static_cast<real>(x) };
}

#endif /* ALLOY_CORE_MATH_ANGLE_RADIAN_INL */