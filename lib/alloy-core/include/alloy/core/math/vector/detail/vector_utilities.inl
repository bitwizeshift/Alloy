#ifndef ALLOY_CORE_MATH_VECTOR_DETAIL_VECTOR_UTILITIES_INL
#define ALLOY_CORE_MATH_VECTOR_DETAIL_VECTOR_UTILITIES_INL

#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR_UTILITIES_HPP
# error "vector_utilities.inl included without first including declaration header vector_utilities.hpp"
#endif

namespace alloy::core::detail {

  template<typename To, typename From>
  struct vector_caster;

  //----------------------------------------------------------------------------

  template<>
  struct vector_caster<vector2,vector2>
  {
    static constexpr const vector2& cast( const vector2& from )
      noexcept
    {
      return from;
    }
  };

  template<>
  struct vector_caster<vector3,vector2>
  {
    static constexpr vector3 cast( const vector2& from )
      noexcept
    {
      return vector3{
        from.x(),
        from.y(),
        real{0}
      };
    }
  };

  template<>
  struct vector_caster<vector4,vector2>
  {
    static constexpr vector4 cast( const vector2& from )
      noexcept
    {
      return vector4{
        from.x(),
        from.y(),
        real{0},
        real{0}
      };
    }
  };

  //----------------------------------------------------------------------------

  template<>
  struct vector_caster<vector2,vector3>
  {
    static constexpr vector2 cast( const vector3& from )
      noexcept
    {
      return vector2{
        from.x(),
        from.y()
      };
    }
  };

  template<>
  struct vector_caster<vector3,vector3>
  {
    static constexpr const vector3& cast( const vector3& from )
      noexcept
    {
      return from;
    }
  };

  template<>
  struct vector_caster<vector4,vector3>
  {
    static constexpr vector4 cast( const vector3& from )
      noexcept
    {
      return vector4{
        from.x(),
        from.y(),
        from.z(),
        real{0}
      };
    }
  };

  //----------------------------------------------------------------------------

  template<>
  struct vector_caster<vector2,vector4>
  {
    static constexpr vector2 cast( const vector4& from )
      noexcept
    {
      return vector2{
        from.x(),
        from.y()
      };
    }
  };

  template<>
  struct vector_caster<vector3,vector4>
  {
    static constexpr vector3 cast( const vector4& from )
      noexcept
    {
      return vector3{
        from.x(),
        from.y(),
        from.z()
      };
    }
  };

  template<>
  struct vector_caster<vector4,vector4>
  {
    static constexpr const vector4& cast( const vector4& from )
      noexcept
    {
      return from;
    }
  };

} // namespace alloy::core::detail

//------------------------------------------------------------------------------
// Vector Casting
//------------------------------------------------------------------------------

template<typename To, typename From>
inline constexpr To
  alloy::core::casts::vector_cast( const From& from )
  noexcept
{
  return detail::vector_caster<To,From>::cast(from);
}

//==============================================================================
// trait : vector_traits<T>
//==============================================================================

template<typename T>
inline constexpr alloy::core::real
  alloy::core::vector_traits<T>::x( const T& vec )
  noexcept
{
  if constexpr ( T{}.size() >= 1u ) {
    return vec[0];
  } else {
    return real{0};
  }
}

template<typename T>
inline constexpr alloy::core::real
  alloy::core::vector_traits<T>::y( const T& vec )
  noexcept
{
  if constexpr ( T{}.size() >= 2u ) {
    return vec[1];
  } else {
    return real{0};
  }
}

template<typename T>
inline constexpr alloy::core::real
  alloy::core::vector_traits<T>::z( const T& vec )
  noexcept
{
  if constexpr ( T{}.size() >= 3u ) {
    return vec[2];
  } else {
    return real{0};
  }
}

template<typename T>
inline constexpr alloy::core::real
  alloy::core::vector_traits<T>::w( const T& vec )
  noexcept
{
  if constexpr ( T{}.size() >= 4u ) {
    return vec[3];
  } else {
    return real{0};
  }
}

#endif /* ALLOY_CORE_MATH_VECTOR_DETAIL_VECTOR_UTILITIES_INL */