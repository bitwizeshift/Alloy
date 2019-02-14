#ifndef ALLOY_CORE_PRECISION_DETAIL_REAL_INL
#define ALLOY_CORE_PRECISION_DETAIL_REAL_INL

#ifndef ALLOY_CORE_PRECISION_REAL_HPP
# error "real.inl included without first including declaration header real.hpp"
#endif

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

#endif /* ALLOY_CORE_PRECISION_DETAIL_REAL_INL */