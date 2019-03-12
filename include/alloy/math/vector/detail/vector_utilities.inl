#ifndef ALLOY_MATH_VECTOR_DETAIL_VECTOR_UTILITIES_INL
#define ALLOY_MATH_VECTOR_DETAIL_VECTOR_UTILITIES_INL

#ifndef ALLOY_MATH_VECTOR_VECTOR_UTILITIES_HPP
# error "vector_utilities.inl included without first including declaration header vector_utilities.hpp"
#endif

namespace alloy::math::detail {

  template<typename To, typename From>
  struct vector_caster;

  //----------------------------------------------------------------------------

  template<typename T, typename U>
  struct vector_caster<vector2<T>,vector2<U>>
  {
    static constexpr vector2<T> cast( const vector2<U>& from )
      noexcept
    {
      return vector2<T>{
        static_cast<T>(from.x()),
        static_cast<T>(from.y())
      };
    }
  };

  template<typename T, typename U>
  struct vector_caster<vector3<T>,vector2<U>>
  {
    static constexpr vector3<T> cast( const vector2<U>& from )
      noexcept
    {
      return vector3<T>{
        static_cast<T>(from.x()),
        static_cast<T>(from.y()),
        T{0}
      };
    }
  };

  template<typename T, typename U>
  struct vector_caster<vector4<T>,vector2<U>>
  {
    static constexpr vector4<T> cast( const vector2<U>& from )
      noexcept
    {
      return vector4<T>{
        static_cast<T>(from.x()),
        static_cast<T>(from.y()),
        T{0},
        T{0}
      };
    }
  };

  //----------------------------------------------------------------------------

  template<typename T, typename U>
  struct vector_caster<vector2<T>,vector3<U>>
  {
    static constexpr vector2<T> cast( const vector3<U>& from )
      noexcept
    {
      return vector2<T>{
        static_cast<T>(from.x()),
        static_cast<T>(from.y())
      };
    }
  };

  template<typename T, typename U>
  struct vector_caster<vector3<T>,vector3<U>>
  {
    static constexpr vector3<T> cast( const vector3<U>& from )
      noexcept
    {
      return vector3<T>{
        static_cast<T>(from.x()),
        static_cast<T>(from.y()),
        static_cast<T>(from.z())
      };
    }
  };

  template<typename T, typename U>
  struct vector_caster<vector4<T>,vector3<U>>
  {
    static constexpr vector4<T> cast( const vector3<U>& from )
      noexcept
    {
      return vector4<T>{
        static_cast<T>(from.x()),
        static_cast<T>(from.y()),
        static_cast<T>(from.z()),
        T{0}
      };
    }
  };

  //----------------------------------------------------------------------------

  template<typename T, typename U>
  struct vector_caster<vector2<T>,vector4<U>>
  {
    static constexpr vector2<T> cast( const vector4<U>& from )
      noexcept
    {
      return vector2<T>{
        static_cast<T>(from.x()),
        static_cast<T>(from.y())
      };
    }
  };

  template<typename T, typename U>
  struct vector_caster<vector3<T>,vector4<U>>
  {
    static constexpr vector3<T> cast( const vector4<U>& from )
      noexcept
    {
      return vector3<T>{
        static_cast<T>(from.x()),
        static_cast<T>(from.y()),
        static_cast<T>(from.z())
      };
    }
  };

  template<typename T, typename U>
  struct vector_caster<vector4<T>,vector4<U>>
  {
    static constexpr vector4<T> cast( const vector4<U>& from )
      noexcept
    {
      return vector4<T>{
        static_cast<T>(from.x()),
        static_cast<T>(from.y()),
        static_cast<T>(from.z()),
        static_cast<T>(from.w())
      };
    }
  };

  //----------------------------------------------------------------------------

  template<typename T>
  struct vector_caster<T,T>
  {
    static constexpr const T& cast( const T& from )
      noexcept
    {
      return from;
    }
  };
} // namespace alloy::math::detail

//------------------------------------------------------------------------------
// Vector Casting
//------------------------------------------------------------------------------

template<typename To, typename From>
inline constexpr To alloy::math::casts::vector_cast( const From& from )
  noexcept
{
  return detail::vector_caster<To,From>::cast(from);
}

#endif /* ALLOY_MATH_VECTOR_DETAIL_VECTOR_UTILITIES_INL */