#ifndef ALLOY_CORE_PRECISION_DETAIL_HALF_INL
#define ALLOY_CORE_PRECISION_DETAIL_HALF_INL

#ifndef ALLOY_CORE_PRECISION_HALF_HPP
# error "half.inl included without first including declaration header half.hpp"
#endif

//==============================================================================
// definitions : non-member functions : alias : half
//==============================================================================

inline bool alloy::core::is_nan( const half& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

inline bool alloy::core::is_finite( const half& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

inline bool alloy::core::is_infinite( const half& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

inline bool alloy::core::is_normal( const half& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

inline bool alloy::core::is_subnormal( const half& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

//==============================================================================
// definitions : literals
//==============================================================================

// inline constexpr alloy::core::half
//   alloy::core::literals::precision_literals::operator""_h(long double in)
//   noexcept
// {
//   return half{in};
// }
//
// inline constexpr alloy::core::half
//   alloy::core::literals::precision_literals::operator""_half(long double in)
//   noexcept
// {
//   return half{in};
// }

#endif /* ALLOY_CORE_PRECISION_DETAIL_HALF_INL */