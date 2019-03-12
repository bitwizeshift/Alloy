#ifndef ALLOY_CORE_PRECISION_DETAIL_REAL_INL
#define ALLOY_CORE_PRECISION_DETAIL_REAL_INL

#ifndef ALLOY_CORE_PRECISION_REAL_HPP
# error "real.inl included without first including declaration header real.hpp"
#endif

//==============================================================================
// definitions : non-member functions : alias : real
//==============================================================================

inline bool alloy::core::is_nan( real f )
  noexcept
{
  return std::isnan(f);
}

inline bool alloy::core::is_finite( real f )
  noexcept
{
  return std::isfinite( f );
}

inline bool alloy::core::is_infinite( real f )
  noexcept
{
  return std::isinf( f );
}

inline bool alloy::core::is_normal( real f )
  noexcept
{
  return std::isnormal( f );
}

inline bool alloy::core::is_subnormal( real f )
  noexcept
{
  return std::fpclassify(f) == FP_SUBNORMAL;
}

//==============================================================================
// definitions : literals
//==============================================================================

inline constexpr alloy::core::real
  alloy::core::literals::precision_literals::operator""_r(long double in)
  noexcept
{
  return static_cast<real>(in);
}

inline constexpr alloy::core::real
  alloy::core::literals::precision_literals::operator""_real(long double in)
  noexcept
{
  return static_cast<real>(in);
}

#endif /* ALLOY_CORE_PRECISION_DETAIL_REAL_INL */