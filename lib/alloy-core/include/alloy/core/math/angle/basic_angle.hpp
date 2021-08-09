/*****************************************************************************
 * \file basic_angle.hpp
 *
 * \todo description
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#ifndef ALLOY_CORE_MATH_ANGLE_BASIC_ANGLE_HPP
#define ALLOY_CORE_MATH_ANGLE_BASIC_ANGLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/math/math.hpp" // core::almost_equal
#include "alloy/core/intrinsics.hpp" // ALLOY_COMPILER_GCC_DIAGNOSTIC_IGNORE

#include <cmath>       // std::fmod
#include <type_traits> // std::true_type / std::false_type

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \concept{AngleUnit}
  ///
  /// \brief This concept defines the required interface and semantics
  ///        expected of an "angle_unit"
  ///
  /// The \c AngleUnit concept is used to identify the unit definition of one
  /// full revolution value for a given angle. This allows consuming code,
  /// e.g. the \c basic_angle type, to calculate revolutions, wrapping, and
  /// angle conversions in a generic manner.
  ///
  /// For type \c A to be \c AngleUnit, it must satisfy the following:
  ///
  /// **Provided**
  ///
  /// - \c A - an AngleUnit type
  /// - \c r - an instance of \ref core::real
  ///
  /// the following expressions must be well-formed with the expected
  /// side-effects:
  ///
  /// \code
  /// r = AngleUnit::one_revolution();
  /// \endcode
  /// \c one_revolution() is a static function which returns the numeric
  /// representation of the value of a single revolution in that angular unit.
  ///
  /// This function must be marked \c constexpr, and must not \c throw an
  /// exception.
  //////////////////////////////////////////////////////////////////////////////

#if 0
  namespace exemplar {
    struct AngleUnit
    {
      // Returns the value of one full revolution (e.g. 3.14 for radian)
      static constexpr core::real one_revolution() noexcept; // NOLINT
    };
  } // namespace exemplar
#endif

  //============================================================================
  // class : basic_angle<AngleUnit>
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Represents an angle as a generic base type
  ///
  /// The basic_angle class is provided as a utility for creating other angles,
  /// through the use of the \ref AngleUnit template parameter.
  ///
  /// \tparam AngleUnit the angle unit which specifies the value of one full
  ///                   revolution as a \ref real
  //////////////////////////////////////////////////////////////////////////////
  template<typename AngleUnit>
  class basic_angle
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using angle_unit = AngleUnit;

    //--------------------------------------------------------------------------
    // Public Static Members
    //--------------------------------------------------------------------------
  public:

    static inline constexpr auto comparison_tolerance = default_tolerance;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default constructs a basic_angle.
    ///
    /// The angle defaults to 0 basic_angles.
    constexpr basic_angle() noexcept;

    /// \brief Constructs a basic_angle from the given floating point value
    ///
    /// \param angle the value of the basic_angle angle
    constexpr explicit basic_angle(real angle) noexcept;

    /// \brief Copy-constructs a basic_angle from another basic_angle
    ///
    /// \param other the other basic_angle to copy
    constexpr basic_angle(const basic_angle& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a basic_angle from another basic_angle
    ///
    /// \param other the other basic_angle to copy
    auto operator=(const basic_angle& other) noexcept -> basic_angle& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the value decimal value of the angle
    ///
    /// \return the value of the angle
    constexpr auto value() const noexcept -> real;

    /// \brief Counts the number of revolutions for the current
    ///        \ref basic_angle
    ///
    /// \return the number of revolutions
    constexpr auto revolutions() const noexcept -> real;

    /// \brief Constrains the \ref basic_angle between \c 0 and
    ///        \c AngleUnit::revolution()
    ///
    /// \return the constrained \ref basic_angle
    auto constrained() const noexcept -> basic_angle;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    constexpr auto operator+() const noexcept -> basic_angle;
    constexpr auto operator-() const noexcept -> basic_angle;

    //--------------------------------------------------------------------------
    // Compound Assignment
    //--------------------------------------------------------------------------
  public:

    auto operator+=(const basic_angle& rhs) noexcept -> basic_angle&;
    auto operator-=(const basic_angle& rhs) noexcept -> basic_angle&;
    auto operator*=(real rhs) noexcept -> basic_angle&;
    auto operator/=(real rhs) noexcept -> basic_angle&;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    real m_angle; ///< The angle of this basic_angle
  };

  //============================================================================
  // non-member functions : class : basic_angle<AngleUnit>
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  template<typename AngleUnit>
  constexpr auto operator+(const basic_angle<AngleUnit>& lhs,
                           const basic_angle<AngleUnit>& rhs)
    noexcept -> basic_angle<AngleUnit>;
  template<typename AngleUnit>
  constexpr auto operator-(const basic_angle<AngleUnit>& lhs,
                           const basic_angle<AngleUnit>& rhs)
    noexcept -> basic_angle<AngleUnit>;
  template<typename AngleUnit>
  constexpr auto operator*(const basic_angle<AngleUnit>& lhs, real rhs)
    noexcept -> basic_angle<AngleUnit>;
  template<typename AngleUnit>
  constexpr auto operator*(real lhs, const basic_angle<AngleUnit>& rhs)
    noexcept -> basic_angle<AngleUnit>;
  template<typename AngleUnit>
  constexpr auto operator/(const basic_angle<AngleUnit>& lhs, real rhs)
    noexcept -> basic_angle<AngleUnit>;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  template<typename AngleUnit>
  constexpr auto operator==(const basic_angle<AngleUnit>& lhs,
                            const basic_angle<AngleUnit>& rhs) noexcept -> bool;
  template<typename AngleUnit>
  constexpr auto operator!=(const basic_angle<AngleUnit>& lhs,
                            const basic_angle<AngleUnit>& rhs) noexcept -> bool;
  template<typename AngleUnit>
  constexpr auto operator<(const basic_angle<AngleUnit>& lhs,
                           const basic_angle<AngleUnit>& rhs) noexcept -> bool;
  template<typename AngleUnit>
  constexpr auto operator>(const basic_angle<AngleUnit>& lhs,
                           const basic_angle<AngleUnit>& rhs) noexcept -> bool;
  template<typename AngleUnit>
  constexpr auto operator<=(const basic_angle<AngleUnit>& lhs,
                            const basic_angle<AngleUnit>& rhs) noexcept -> bool;
  template<typename AngleUnit>
  constexpr auto operator>=(const basic_angle<AngleUnit>& lhs,
                            const basic_angle<AngleUnit>& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two basic_angle relative to
  ///        \ref tolerance
  ///
  /// \param lhs the left basic_angle
  /// \param rhs the right basic_angle
  /// \return \c true if the two basic_angle contain almost equal values
  template<typename AngleUnit>
  constexpr auto almost_equal(const basic_angle<AngleUnit>& lhs,
                              const basic_angle<AngleUnit>& rhs,
                              real tolerance) noexcept -> bool;

  //---------------------------------------------------------------------------
  // Utilities : Math Functions
  //---------------------------------------------------------------------------

  /// \brief Rounds a given \p angle
  ///
  /// \param angle the angle to round
  /// \return the rounded angle
  template<typename AngleUnit>
  auto round(basic_angle<AngleUnit> angle) noexcept -> basic_angle<AngleUnit>;

  /// \brief Rounds a given \p angle up as if by calling
  ///        \code ceil( angle.value() ) \endcode
  ///
  /// \param angle the radian to round up
  /// \return the rounded angle
  template<typename AngleUnit>
  auto ceil(basic_angle<AngleUnit> angle) noexcept -> basic_angle<AngleUnit>;

  /// \brief Rounds a given \p angle up as if by calling
  ///        \code floor( angle.value() ) \endcode
  ///
  /// \param angle the angle to round down
  /// \return the rounded angle
  template<typename AngleUnit>
  auto floor(basic_angle<AngleUnit> angle) noexcept -> basic_angle<AngleUnit>;

  /// \brief Truncates the given \p angle as if by calling
  ///        \code trunc( angle.value() ) \endcode
  ///
  /// \param angle the angle to truncate
  /// \return the rounded angle
  template<typename AngleUnit>
  auto trunc(basic_angle<AngleUnit> angle) noexcept -> basic_angle<AngleUnit>;

  //------------------------------------------------------------------------

  /// \brief Retrieves the absolute value of the given \p angle
  ///        as if by calling \code abs( angle.value() ) \endcode
  ///
  /// \param angle the angle to retrieve the absolute value of
  /// \return the rounded angle
  template<typename AngleUnit>
  auto abs(basic_angle<AngleUnit> angle) noexcept -> basic_angle<AngleUnit>;

  //============================================================================
  // trait : is_angle
  //============================================================================

  /// \brief Trait to detect whether \p T is a \ref basic_angle type
  ///
  /// The result is aliased as \c ::value
  template<typename T>
  struct is_angle : std::false_type{};

  template<typename AngleUnit>
  struct is_angle<basic_angle<AngleUnit>> : std::true_type{};

  /// \brief Convenience variable template to retrieve the result of
  ///        \ref is_angle
  template<typename T>
  constexpr bool is_angle_v = is_angle<T>::value;

  //============================================================================
  // struct : basic_angle_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of gradian constants
  //////////////////////////////////////////////////////////////////////////////
  template<typename AngleUnit>
  struct basic_angle_constants
  {
    using angle_type = basic_angle<AngleUnit>;

    static inline constexpr auto revolution = angle_type{
      AngleUnit::revolution()
    };
    static inline constexpr auto half_revolution = revolution / real{2};
    static inline constexpr auto quarter_revolution = half_revolution / real{2};
  };

  //============================================================================
  // casts : class : basic_angle<AngleUnit>
  //============================================================================

  inline namespace casts {

    /// \brief Casts an angle of type \p From to type \p To
    ///
    /// \tparam To the angle type to cast to
    /// \param from the angle to cast
    /// \return the new angle
    template<typename To, typename From>
    inline constexpr auto to_angle(From from) -> To;

  } // inline namespace casts
} // namespace alloy::core


//==============================================================================
// class : basic_angle<AngleUnit>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr
alloy::core::basic_angle<AngleUnit>::basic_angle()
  noexcept
  : m_angle{0}
{

}

template<typename AngleUnit>
inline constexpr
alloy::core::basic_angle<AngleUnit>::basic_angle(real angle)
  noexcept
  : m_angle{angle}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr
auto alloy::core::basic_angle<AngleUnit>::value()
  const noexcept -> real
{
  return m_angle;
}

template<typename AngleUnit>
inline constexpr
auto alloy::core::basic_angle<AngleUnit>::revolutions()
  const noexcept -> real
{
  return m_angle / AngleUnit::revolution();
}

template<typename AngleUnit>
inline
auto alloy::core::basic_angle<AngleUnit>::constrained()
  const noexcept -> basic_angle<AngleUnit>
{
  const auto angle = std::fmod(m_angle, AngleUnit::revolution());

  if (angle < 0) {
    angle += AngleUnit::revolution();
  }

  return basic_angle{ real(angle) };
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr
auto alloy::core::basic_angle<AngleUnit>::operator+()
  const noexcept -> basic_angle<AngleUnit>
{
  return (*this);
}

template<typename AngleUnit>
inline constexpr
auto alloy::core::basic_angle<AngleUnit>::operator-()
  const noexcept
  -> basic_angle<AngleUnit>
{
  return basic_angle{-m_angle};
}

//------------------------------------------------------------------------------
// Compound Assignment
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline
auto alloy::core::basic_angle<AngleUnit>::operator+=(const basic_angle& rhs)
  noexcept -> basic_angle<AngleUnit>&
{
  m_angle += rhs.m_angle;
  return (*this);
}

template<typename AngleUnit>
inline
auto alloy::core::basic_angle<AngleUnit>::operator-=(const basic_angle& rhs)
  noexcept -> basic_angle<AngleUnit>&
{
  m_angle -= rhs.m_angle;
  return (*this);
}

template<typename AngleUnit>
inline
auto alloy::core::basic_angle<AngleUnit>::operator*=(real rhs)
  noexcept -> basic_angle<AngleUnit>&
{
  m_angle *= rhs;
  return (*this);
}

template<typename AngleUnit>
inline
auto alloy::core::basic_angle<AngleUnit>::operator/=(real rhs)
  noexcept -> basic_angle<AngleUnit>&
{
  m_angle /= rhs;
  return (*this);
}

//==============================================================================
// non-member functions : class : basic_angle<AngleUnit>
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr
auto alloy::core::operator+(const basic_angle<AngleUnit>& lhs,
                            const basic_angle<AngleUnit>& rhs)
  noexcept -> basic_angle<AngleUnit>
{
  return basic_angle<AngleUnit>{ lhs.value() + rhs.value() };
}

template<typename AngleUnit>
inline constexpr
auto alloy::core::operator-(const basic_angle<AngleUnit>& lhs,
                            const basic_angle<AngleUnit>& rhs)
  noexcept -> basic_angle<AngleUnit>
{
  return basic_angle<AngleUnit>{ lhs.value() - rhs.value() };
}

template<typename AngleUnit>
inline constexpr
auto alloy::core::operator*(const basic_angle<AngleUnit>& lhs, real rhs)
  noexcept -> basic_angle<AngleUnit>
{
  return basic_angle<AngleUnit>{ lhs.value() * rhs };
}

template<typename AngleUnit>
inline constexpr
auto alloy::core::operator*(real lhs, const basic_angle<AngleUnit>& rhs)
  noexcept -> basic_angle<AngleUnit>
{
  return basic_angle<AngleUnit>{ rhs.value() * lhs };
}

template<typename AngleUnit>
inline constexpr
auto alloy::core::operator/(const basic_angle<AngleUnit>& lhs, real rhs)
  noexcept -> basic_angle<AngleUnit>
{
  return basic_angle<AngleUnit>{ lhs.value() / rhs };
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

template<typename AngleUnit>
inline constexpr
auto alloy::core::operator==(const basic_angle<AngleUnit>& lhs,
                             const basic_angle<AngleUnit>& rhs)
  noexcept -> bool
{
  return lhs.value() == rhs.value();
}

template<typename AngleUnit>
inline constexpr
auto alloy::core::operator!=(const basic_angle<AngleUnit>& lhs,
                             const basic_angle<AngleUnit>& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

template<typename AngleUnit>
inline constexpr
auto alloy::core::operator<(const basic_angle<AngleUnit>& lhs,
                            const basic_angle<AngleUnit>& rhs)
  noexcept -> bool
{
  return lhs.value() < rhs.value();
}

template<typename AngleUnit>
inline constexpr
auto alloy::core::operator>(const basic_angle<AngleUnit>& lhs,
                            const basic_angle<AngleUnit>& rhs)
  noexcept -> bool
{
  return lhs.value() > rhs.value();
}

template<typename AngleUnit>
inline constexpr
auto alloy::core::operator<=(const basic_angle<AngleUnit>& lhs,
                             const basic_angle<AngleUnit>& rhs)
  noexcept -> bool
{
  return lhs.value() <= rhs.value();
}

template<typename AngleUnit>
inline constexpr
auto alloy::core::operator>=(const basic_angle<AngleUnit>& lhs,
                             const basic_angle<AngleUnit>& rhs)
  noexcept -> bool
{
  return lhs.value() >= rhs.value();
}

//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr
auto alloy::core::almost_equal(const basic_angle<AngleUnit>& lhs,
                               const basic_angle<AngleUnit>& rhs,
                               real tolerance)
  noexcept -> bool
{
  return almost_equal( lhs.value(), rhs.value(), tolerance );
}

//------------------------------------------------------------------------------
// Utilities : Math Functions
//------------------------------------------------------------------------------

template<typename AngleUnit>
auto alloy::core::round(basic_angle<AngleUnit> angle)
  noexcept -> basic_angle<AngleUnit>
{
  return basic_angle<AngleUnit>{ std::round(angle.value()) };
}

template<typename AngleUnit>
auto alloy::core::ceil(basic_angle<AngleUnit> angle)
  noexcept -> basic_angle<AngleUnit>
{
  return basic_angle<AngleUnit>{ std::ceil(angle.value()) };
}

template<typename AngleUnit>
auto alloy::core::floor(basic_angle<AngleUnit> angle)
  noexcept -> basic_angle<AngleUnit>
{
  return basic_angle<AngleUnit>{ std::floor(angle.value()) };
}

template<typename AngleUnit>
auto alloy::core::trunc(basic_angle<AngleUnit> angle)
  noexcept -> basic_angle<AngleUnit>
{
  return basic_angle<AngleUnit>{ std::trunc(angle.value()) };
}

//------------------------------------------------------------------------------

template<typename AngleUnit>
auto alloy::core::abs(basic_angle<AngleUnit> angle)
  noexcept -> basic_angle<AngleUnit>
{
  return basic_angle<AngleUnit>{ std::abs(angle.value()) };
}

//==============================================================================
// casts : class : basic_angle<AngleUnit>
//==============================================================================

namespace alloy::core::detail {
  // case: From != To
  template<typename AngleUnitTo, typename AngleUnitFrom>
  inline constexpr
  auto angle_cast(basic_angle<AngleUnitFrom> from)
    noexcept -> basic_angle<AngleUnitTo>
  {
    using From = AngleUnitFrom;
    using To   = AngleUnitTo;

    constexpr auto factor = To::revolution() / From::revolution();

    return basic_angle<AngleUnitTo>{ from.value() * factor };
  }

  // case: From == To
  template<typename AngleUnit>
  inline constexpr
  auto angle_cast(basic_angle<AngleUnit> from)
    noexcept -> const basic_angle<AngleUnit>&
  {
    return from;
  }
} // namespace alloy::core::detail

template<typename To, typename From>
inline constexpr
auto alloy::core::casts::to_angle(From from) -> To
{
  return detail::angle_cast<typename To::angle_unit>( from );
}

#endif /* ALLOY_CORE_MATH_ANGLE_BASIC_ANGLE_HPP */
