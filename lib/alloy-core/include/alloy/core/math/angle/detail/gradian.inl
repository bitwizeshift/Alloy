#ifndef ALLOY_CORE_MATH_ANGLE_GRADIAN_INL
#define ALLOY_CORE_MATH_ANGLE_GRADIAN_INL

#ifndef ALLOY_CORE_MATH_ANGLE_GRADIAN_HPP
# error "gradian.inl included without first including declaration header gradian.hpp"
#endif

//==============================================================================
// struct : gradian_unit
//==============================================================================

inline constexpr alloy::core::real alloy::core::gradian_unit::revolution()
  noexcept
{
  return static_cast<real>(400);
}

//==============================================================================
// Literals
//==============================================================================

inline constexpr alloy::core::gradian
  alloy::core::literals::operator ""_grad( long double x )
  noexcept
{
  return gradian{ static_cast<real>(x) };
}

#endif /* ALLOY_CORE_MATH_ANGLE_GRADIAN_INL */