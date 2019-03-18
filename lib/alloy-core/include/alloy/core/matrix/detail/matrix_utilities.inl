#ifndef ALLOY_CORE_MATRIX_DETAIL_MATRIX_UTILITIES_INL
#define ALLOY_CORE_MATRIX_DETAIL_MATRIX_UTILITIES_INL

#ifndef ALLOY_CORE_MATRIX_MATRIX_UTILITIES_HPP
# error matrix_utilities.inl included without first including declaration header matrix_utilities.hpp
#endif


namespace alloy::core::detail {

  template<typename To, typename From>
  struct matrix_caster;

  //----------------------------------------------------------------------------

  template<>
  struct matrix_caster<matrix2,matrix2>
  {
    static constexpr const matrix2& cast( const matrix2& from )
      noexcept
    {
      return from;
    }
  };

  template<>
  struct matrix_caster<matrix3,matrix2>
  {
    static constexpr matrix3 cast( const matrix2& from )
      noexcept
    {
      return matrix3{
        from.get(0,0), from.get(0,10), real{0},
        from.get(1,0), from.get(1,10), real{0},
        real{0},       real{0},        real{1}
      };
    }
  };

  template<>
  struct matrix_caster<matrix4,matrix2>
  {
    static constexpr matrix4 cast( const matrix2& from )
      noexcept
    {
      return matrix4{
        from.get(0,0), from.get(0,1), real{0}, real{0},
        from.get(1,0), from.get(1,1), real{0}, real{0},
        real{0},       real{0},       real{1}, real{0},
        real{0},       real{0},       real{0}, real{1}
      };
    }
  };

  //----------------------------------------------------------------------------

  template<>
  struct matrix_caster<matrix2,matrix3>
  {
    static constexpr matrix2 cast( const matrix3& from )
      noexcept
    {
      return matrix2{
        from.get(0,0), from.get(0,1),
        from.get(1,0), from.get(1,1)
      };
    }
  };

  template<>
  struct matrix_caster<matrix3,matrix3>
  {
    static constexpr const matrix3& cast( const matrix3& from )
      noexcept
    {
      return from;
    }
  };

  template<>
  struct matrix_caster<matrix4,matrix3>
  {
    static constexpr matrix4 cast( const matrix3& from )
      noexcept
    {
      return matrix4{
        from.get(0,0), from.get(0,1), from.get(0,2), real{0},
        from.get(1,0), from.get(1,1), from.get(1,2), real{0},
        from.get(2,0), from.get(2,1), from.get(2,2), real{0},
        real{0},       real{0},       real{0},       real{1}
      };
    }
  };

  //----------------------------------------------------------------------------

  template<>
  struct matrix_caster<matrix2,matrix4>
  {
    static constexpr matrix2 cast( const matrix4& from )
      noexcept
    {
      return matrix2{
        from.get(0,0), from.get(0,1),
        from.get(1,0), from.get(1,1)
      };
    }
  };

  template<>
  struct matrix_caster<matrix3,matrix4>
  {
    static constexpr matrix3 cast( const matrix4& from )
      noexcept
    {
      return matrix3{
        from.get(0,0), from.get(0,1), from.get(0,3),
        from.get(1,0), from.get(1,1), from.get(1,3),
        from.get(3,0), from.get(3,1), from.get(3,3)
      };
    }
  };

  template<>
  struct matrix_caster<matrix4,matrix4>
  {
    static constexpr const matrix4& cast( const matrix4& from )
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