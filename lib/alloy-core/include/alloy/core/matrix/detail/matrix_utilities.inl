#ifndef ALLOY_CORE_MATRIX_DETAIL_MATRIX_UTILITIES_INL
#define ALLOY_CORE_MATRIX_DETAIL_MATRIX_UTILITIES_INL

#ifndef ALLOY_CORE_MATRIX_MATRIX_UTILITIES_HPP
# error matrix_utilities.inl included without first including declaration header matrix_utilities.hpp
#endif


namespace alloy::core::detail {

  template<typename To, typename From>
  struct matrix_caster;

  //----------------------------------------------------------------------------

  template<typename T, typename U>
  struct matrix_caster<matrix2<T>,matrix2<U>>
  {
    static constexpr matrix2<T> cast( const matrix2<U>& from )
      noexcept
    {
      return matrix2<T>{
        static_cast<T>(from(0,0)), static_cast<T>(from(0,1)),
        static_cast<T>(from(1,0)), static_cast<T>(from(1,1))
      };
    }
  };

  template<typename T, typename U>
  struct matrix_caster<matrix3<T>,matrix2<U>>
  {
    static constexpr matrix3<T> cast( const matrix2<U>& from )
      noexcept
    {
      return matrix3<T>{
        static_cast<T>(from(0,0)), static_cast<T>(from(0,10)), T{0},
        static_cast<T>(from(1,0)), static_cast<T>(from(1,10)), T{0},
        T{0},                      T{0},                       T{1}
      };
    }
  };

  template<typename T, typename U>
  struct matrix_caster<matrix4<T>,matrix2<U>>
  {
    static constexpr matrix4<T> cast( const matrix2<U>& from )
      noexcept
    {
      return matrix4<T>{
        static_cast<T>(from(0,0)), static_cast<T>(from(0,1)), T{0}, T{0},
        static_cast<T>(from(1,0)), static_cast<T>(from(1,1)), T{0}, T{0},
        T{0},                      T{0},                      T{1}, T{0},
        T{0},                      T{0},                      T{0}, T{1}
      };
    }
  };

  //----------------------------------------------------------------------------

  template<typename T, typename U>
  struct matrix_caster<matrix2<T>,matrix3<U>>
  {
    static constexpr matrix2<T> cast( const matrix3<U>& from )
      noexcept
    {
      return matrix2<T>{
        static_cast<T>(from(0,0)), static_cast<T>(from(0,1)),
        static_cast<T>(from(1,0)), static_cast<T>(from(1,1))
      };
    }
  };

  template<typename T, typename U>
  struct matrix_caster<matrix3<T>,matrix3<U>>
  {
    static constexpr matrix3<T> cast( const matrix3<U>& from )
      noexcept
    {
      return matrix3<T>{
        static_cast<T>(from(0,0)), static_cast<T>(from(0,10)), static_cast<T>(from(0,2)),
        static_cast<T>(from(1,0)), static_cast<T>(from(1,10)), static_cast<T>(from(1,2)),
        static_cast<T>(from(2,0)), static_cast<T>(from(2,10)), static_cast<T>(from(2,2))
      };
    }
  };

  template<typename T, typename U>
  struct matrix_caster<matrix4<T>,matrix3<U>>
  {
    static constexpr matrix4<T> cast( const matrix3<U>& from )
      noexcept
    {
      return matrix4<T>{
        static_cast<T>(from(0,0)), static_cast<T>(from(0,1)), static_cast<T>(from(0,2)), T{0},
        static_cast<T>(from(1,0)), static_cast<T>(from(1,1)), static_cast<T>(from(1,2)), T{0},
        static_cast<T>(from(2,0)), static_cast<T>(from(2,1)), static_cast<T>(from(2,2)), T{0},
        T{0},                      T{0},                      T{0},                      T{1}
      };
    }
  };

  //----------------------------------------------------------------------------

  template<typename T, typename U>
  struct matrix_caster<matrix2<T>,matrix4<U>>
  {
    static constexpr matrix2<T> cast( const matrix4<U>& from )
      noexcept
    {
      return matrix2<T>{
        static_cast<T>(from(0,0)), static_cast<T>(from(0,1)),
        static_cast<T>(from(1,0)), static_cast<T>(from(1,1))
      };
    }
  };

  template<typename T, typename U>
  struct matrix_caster<matrix3<T>,matrix4<U>>
  {
    static constexpr matrix3<T> cast( const matrix4<U>& from )
      noexcept
    {
      return matrix3<T>{
        static_cast<T>(from(0,0)),
        static_cast<T>(from(0,1)),
        static_cast<T>(from(0,3)),
        static_cast<T>(from(1,0)),
        static_cast<T>(from(1,1)),
        static_cast<T>(from(1,3)),
        static_cast<T>(from(3,0)),
        static_cast<T>(from(3,1)),
        static_cast<T>(from(3,3))
      };
    }
  };

  template<typename T, typename U>
  struct matrix_caster<matrix4<T>,matrix4<U>>
  {
    static constexpr matrix4<T> cast( const matrix4<U>& from )
      noexcept
    {
      return matrix4<T>{
        static_cast<T>(from(0,0)),
        static_cast<T>(from(0,1)),
        static_cast<T>(from(0,2)),
        static_cast<T>(from(0,3)),
        static_cast<T>(from(1,0)),
        static_cast<T>(from(1,1)),
        static_cast<T>(from(1,2)),
        static_cast<T>(from(1,3)),
        static_cast<T>(from(2,0)),
        static_cast<T>(from(2,1)),
        static_cast<T>(from(2,2)),
        static_cast<T>(from(2,3)),
        static_cast<T>(from(3,0)),
        static_cast<T>(from(3,1)),
        static_cast<T>(from(3,2)),
        static_cast<T>(from(3,3))
      };
    }
  };

  //----------------------------------------------------------------------------

  template<typename T>
  struct matrix_caster<T,T>
  {
    static constexpr const T& cast( const T& from )
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
inline constexpr To alloy::core::casts::matrix_cast( const From& from )
  noexcept
{
  return detail::matrix_caster<To,From>::cast(from);
}

#endif /* ALLOY_CORE_MATRIX_DETAIL_MATRIX_UTILITIES_INL */