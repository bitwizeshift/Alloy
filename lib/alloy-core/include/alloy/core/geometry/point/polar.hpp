////////////////////////////////////////////////////////////////////////////////
/// \file polar.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021-2022 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_CORE_GEOMETRY_POINT_POLAR_HPP
#define ALLOY_CORE_GEOMETRY_POINT_POLAR_HPP

#include "alloy/core/math/trigonometry.hpp"
#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/precision.hpp"

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A point using polar coordinates
  ///
  /// A polar-coordinate point is identified by the distance from the center of
  /// a "circle", and the angle from the x-axis.
  //////////////////////////////////////////////////////////////////////////////
  class polar
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-initializes this polar point
    polar() = default;

    /// \brief Constructs this point with the specified \p distance and \p theta
    ///        angle
    ///
    /// \param distance the distance from the origin
    /// \param theta the angle
    constexpr polar(real distance, radian theta) noexcept;

    /// \brief Copies the contents from \p other
    ///
    /// \param other the other polar point to copy
    polar(const polar& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other point to copy
    /// \return reference to `(*this)`
    auto operator=(const polar& other) -> polar& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the distance component of this polar
    ///
    /// \return reference to the distance component
    constexpr auto r() noexcept -> real&;
    constexpr auto r() const noexcept -> real;
    /// \}

    /// \{
    /// \brief Gets the theta component of this polar
    ///
    /// \return reference to the theta component
    constexpr auto theta() noexcept -> radian&;
    constexpr auto theta() const noexcept -> radian;
    /// \}

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    real m_distance;
    radian m_theta;
  };

  //============================================================================
  // non-member functions : class : polar
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr auto operator==(const polar& lhs, const polar& rhs) noexcept -> bool;
  constexpr auto operator!=(const polar& lhs, const polar& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two polar relative to \p tolerance
  ///
  /// \param lhs the left polar
  /// \param rhs the right polar
  /// \return `true` if the two polar contain almost equal values
  constexpr auto almost_equal(const polar& lhs,
                              const polar& rhs,
                              real tolerance) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Converts a cartesian `point2` into a `polar` point
  ///
  /// \param p the cartesian point
  /// \return the polar point representation
  auto to_polar(const point2& p) noexcept -> polar;

  /// \brief Converts a `polar` point into a cartesian `point2`
  ///
  /// \param p the cartesian point
  /// \return the polar point representation
  auto to_point(const polar& p) noexcept -> point2;


  //============================================================================
  // struct : piecewise_compare<point>
  //============================================================================

  template<>
  struct piecewise_compare<polar>
  {
    constexpr auto operator()(const polar& lhs, const polar& rhs)
      noexcept -> bool;
  };

} // namespace alloy::core

//==============================================================================
// definitions : class : polar
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::polar::polar(real distance, radian theta)
  noexcept
  : m_distance{distance},
    m_theta{theta}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::polar::r()
  noexcept -> real&
{
  return m_distance;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::polar::r()
  const noexcept -> real
{
  return m_distance;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::polar::theta()
  noexcept -> radian&
{
  return m_theta;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::polar::theta()
  const noexcept -> radian
{
  return m_theta;
}

//==============================================================================
// definitions : non-member functions : class : polar
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const polar& lhs, const polar& rhs)
  noexcept -> bool
{
  return lhs.r() == rhs.r() && lhs.theta() == rhs.theta();
}

inline constexpr
auto alloy::core::operator!=(const polar& lhs, const polar& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(const polar& lhs, const polar& rhs, real tolerance)
  noexcept -> bool
{
  return (
    almost_equal(lhs.theta(), rhs.theta(), tolerance) &&
    almost_equal(lhs.r(), rhs.r(), tolerance)
  );
}


//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

inline
auto alloy::core::to_polar(const point2& p)
  noexcept -> polar
{
  const auto distance = to_vector(p).magnitude();
  const auto theta = trigonometry::arctan2(p.x(), p.y());

  return polar{distance, theta};
}

inline
auto alloy::core::to_point(const polar& p)
  noexcept -> point2
{
  const auto x = p.r() * trigonometry::cos(p.theta());
  const auto y = p.r() * trigonometry::sin(p.theta());

  return point2{x, y};
}


//==============================================================================
// struct : piecewise_compare<polar>
//==============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::piecewise_compare<alloy::core::polar>
  ::operator()(const polar& lhs, const polar& rhs)
  noexcept -> bool
{
  return (lhs.r() == rhs.r()) ?
           (lhs.theta() < rhs.theta()) :
         (lhs.r() < rhs.r());
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_GEOMETRY_POINT_POLAR_HPP */
