#ifndef ALLOY_MATH_ANGLE_GRADIAN_INL
#define ALLOY_MATH_ANGLE_GRADIAN_INL

#ifndef ALLOY_MATH_ANGLE_GRADIAN_HPP
# error "gradian.inl included without first including declaration header gradian.hpp"
#endif

//==============================================================================
// struct : gradian_unit
//==============================================================================

inline constexpr alloy::core::real alloy::math::gradian_unit::revolution()
  noexcept
{
  return static_cast<core::real>(400);
}

//==============================================================================
// Literals
//==============================================================================

inline constexpr alloy::math::gradian
  alloy::math::literals::operator ""_grad( long double x )
  noexcept
{
  return gradian{ static_cast<core::real>(x) };
}

#endif /* ALLOY_MATH_ANGLE_GRADIAN_INL */