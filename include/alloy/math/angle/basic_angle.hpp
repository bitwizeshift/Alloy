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
#ifndef ALLOY_MATH_ANGLE_BASIC_ANGLE_HPP
#define ALLOY_MATH_ANGLE_BASIC_ANGLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"

#include <cmath>       // std::fmod
#include <type_traits> // std::true_type / std::false_type

namespace alloy::math {

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
  ///                   revolution as a \ref core::real
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
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Default constructs a basic_angle.
    ///
    /// The angle defaults to 0 basic_angles.
    constexpr basic_angle() noexcept;

    /// \brief Constructs a basic_angle from the given floating point value
    ///
    /// \param value the value of the basic_angle angle
    constexpr explicit basic_angle( core::real value ) noexcept;

    /// \brief Copy-constructs a basic_angle from another basic_angle
    ///
    /// \param other the other basic_angle to copy
    constexpr basic_angle( const basic_angle& other ) noexcept = default;

    /// \brief Move-constructs a basic_angle from another basic_angle
    ///
    /// \param other the other basic_angle to move
    constexpr basic_angle( basic_angle&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Copy-assigns a basic_angle from another basic_angle
    ///
    /// \param other the other basic_angle to copy
    basic_angle& operator=( const basic_angle& other ) noexcept = default;

    /// \brief Move-assigns a basic_angle from another basic_angle
    ///
    /// \param other the other basic_angle to move
    basic_angle& operator=( basic_angle&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the value decimal value of the angle
    ///
    /// \return the value of the angle
    constexpr core::real value() const noexcept;

    /// \brief Counts the number of revolutions for the current
    ///        \ref basic_angle
    ///
    /// \return the number of revolutions
    constexpr core::real revolutions() const noexcept;

    /// \brief Constrains the \ref basic_angle between \c 0 and
    ///        \c AngleUnit::revolution()
    ///
    /// \return the constrained \ref basic_angle
    basic_angle constrained() const noexcept;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    constexpr basic_angle operator+() const noexcept;
    constexpr basic_angle operator-() const noexcept;

    //--------------------------------------------------------------------------
    // Compound Assignment
    //--------------------------------------------------------------------------
  public:

    basic_angle& operator+=( const basic_angle& rhs ) noexcept;
    basic_angle& operator-=( const basic_angle& rhs ) noexcept;
    basic_angle& operator*=( core::real rhs ) noexcept;
    basic_angle& operator/=( core::real rhs ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::real m_angle; ///< The angle of this basic_angle
  };

  //============================================================================
  // non-member functions : class : basic_angle<AngleUnit>
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  template<typename AngleUnit>
  constexpr basic_angle<AngleUnit>
    operator+( const basic_angle<AngleUnit>& lhs,
               const basic_angle<AngleUnit>& rhs ) noexcept;
  template<typename AngleUnit>
  constexpr basic_angle<AngleUnit>
    operator-( const basic_angle<AngleUnit>& lhs,
               const basic_angle<AngleUnit>& rhs ) noexcept;
  template<typename AngleUnit>
  constexpr basic_angle<AngleUnit>
    operator*( const basic_angle<AngleUnit>& lhs, core::real rhs ) noexcept;
  template<typename AngleUnit>
  constexpr basic_angle<AngleUnit>
    operator*( core::real rhs, const basic_angle<AngleUnit>& lhs ) noexcept;
  template<typename AngleUnit>
  constexpr basic_angle<AngleUnit>
    operator/( const basic_angle<AngleUnit>& lhs, core::real rhs ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  template<typename AngleUnit>
  constexpr bool operator==( const basic_angle<AngleUnit>& lhs,
                             const basic_angle<AngleUnit>& rhs ) noexcept;
  template<typename AngleUnit>
  constexpr bool operator!=( const basic_angle<AngleUnit>& lhs,
                             const basic_angle<AngleUnit>& rhs ) noexcept;
  template<typename AngleUnit>
  constexpr bool operator<( const basic_angle<AngleUnit>& lhs,
                            const basic_angle<AngleUnit>& rhs ) noexcept;
  template<typename AngleUnit>
  constexpr bool operator>( const basic_angle<AngleUnit>& lhs,
                            const basic_angle<AngleUnit>& rhs ) noexcept;
  template<typename AngleUnit>
  constexpr bool operator<=( const basic_angle<AngleUnit>& lhs,
                             const basic_angle<AngleUnit>& rhs ) noexcept;
  template<typename AngleUnit>
  constexpr bool operator>=( const basic_angle<AngleUnit>& lhs,
                             const basic_angle<AngleUnit>& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two basic_angle relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left basic_angle
  /// \param rhs the right basic_angle
  /// \return \c true if the two basic_angle contain almost equal values
  template<typename AngleUnit>
  constexpr bool almost_equal( const basic_angle<AngleUnit>& lhs,
                               const basic_angle<AngleUnit>& rhs ) noexcept;

  /// \brief Determines equality between two basic_angle relative to
  ///        \ref tolerance
  ///
  /// \param lhs the left basic_angle
  /// \param rhs the right basic_angle
  /// \return \c true if the two basic_angle contain almost equal values
  template<typename AngleUnit>
  constexpr bool almost_equal( const basic_angle<AngleUnit>& lhs,
                               const basic_angle<AngleUnit>& rhs,
                               core::real tolerance ) noexcept;

  //---------------------------------------------------------------------------
  // Utilities : Math Functions
  //---------------------------------------------------------------------------

  /// \brief Rounds a given \p angle
  ///
  /// \param angle the angle to round
  /// \return the rounded angle
  template<typename AngleUnit>
  basic_angle<AngleUnit> round( basic_angle<AngleUnit> angle ) noexcept;

  /// \brief Rounds a given \p angle up as if by calling
  ///        \code ceil( angle.value() ) \endcode
  ///
  /// \param angle the radian to round up
  /// \return the rounded angle
  template<typename AngleUnit>
  basic_angle<AngleUnit> ceil( basic_angle<AngleUnit> angle ) noexcept;

  /// \brief Rounds a given \p angle up as if by calling
  ///        \code floor( angle.value() ) \endcode
  ///
  /// \param angle the angle to round down
  /// \return the rounded angle
  template<typename AngleUnit>
  basic_angle<AngleUnit> floor( basic_angle<AngleUnit> angle ) noexcept;

  /// \brief Truncates the given \p angle as if by calling
  ///        \code trunc( angle.value() ) \endcode
  ///
  /// \param angle the angle to truncate
  /// \return the rounded angle
  template<typename AngleUnit>
  basic_angle<AngleUnit> trunc( basic_angle<AngleUnit> angle ) noexcept;

  //------------------------------------------------------------------------

  /// \brief Retrieves the absolute value of the given \p angle
  ///        as if by calling \code abs( angle.value() ) \endcode
  ///
  /// \param angle the angle to retrieve the absolute value of
  /// \return the rounded angle
  template<typename AngleUnit>
  basic_angle<AngleUnit> abs( basic_angle<AngleUnit> angle ) noexcept;

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
  // struct : gradian_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of gradian constants
  //////////////////////////////////////////////////////////////////////////////
  template<typename AngleUnit>
  struct basic_angle_constants
  {
    using angle_type = basic_angle<AngleUnit>;

    static inline constexpr auto revolution = angle_type{ AngleUnit::revolution() };
    static inline constexpr auto half_revolution = revolution / core::real{2};
    static inline constexpr auto quarter_revolution = half_revolution / core::real{2};
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
    inline constexpr To angle_cast( From from );

  } // inline namespace casts
} // namespace alloy::math

#include "detail/basic_angle.inl"

#endif /* ALLOY_BASIC_ANGLE_HPP */