#ifndef ALLOY_CORE_PRECISION_DETAIL_FIXED_INL
#define ALLOY_CORE_PRECISION_DETAIL_FIXED_INL

#ifndef ALLOY_CORE_PRECISION_FIXED_HPP
# error "fixed.inl included without first including declaration header fixed.hpp"
#endif

//==============================================================================
// definitions : non-member functions : alias : basic_fixed
//==============================================================================

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline bool
  alloy::core::is_nan( const basic_fixed<MantissaBits,ExponentBits>& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline bool
  alloy::core::is_finite( const basic_fixed<MantissaBits,ExponentBits>& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline bool
  alloy::core::is_infinite( const basic_fixed<MantissaBits,ExponentBits>& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline bool
  alloy::core::is_normal( const basic_fixed<MantissaBits,ExponentBits>& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline bool
  alloy::core::is_subnormal( const basic_fixed<MantissaBits,ExponentBits>& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

//==============================================================================
// definitions : literals
//==============================================================================

// TODO(bitwizeshift) determine what reasonable literals can be provided
//                    for fixed-precision type. Probably match the aliases?

#endif /* ALLOY_CORE_PRECISION_DETAIL_FIXED_INL */