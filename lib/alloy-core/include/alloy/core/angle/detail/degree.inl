#ifndef ALLOY_CORE_ANGLE_DEGREE_INL
#define ALLOY_CORE_ANGLE_DEGREE_INL

#ifndef ALLOY_CORE_ANGLE_DEGREE_HPP
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
// Literals
//==============================================================================

inline constexpr alloy::core::degree
  alloy::core::literals::operator ""_deg( long double x )
  noexcept
{
  return degree{ static_cast<real>(x) };
}

#endif /* ALLOY_CORE_ANGLE_DEGREE_INL */