////////////////////////////////////////////////////////////////////////////////
/// \file spherical.hpp
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

#ifndef ALLOY_CORE_GEOMETRY_POINT_SPHERICAL_HPP
#define ALLOY_CORE_GEOMETRY_POINT_SPHERICAL_HPP

#include "alloy/core/math/trigonometry.hpp"
#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/precision.hpp"

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A point using spherical coordinates
  ///
  /// A spherical-coordinate points are identified by the distance from the
  /// center of a sphere ("rho"), and two angular components representing its
  /// position on the sphere ("phi" and "theta").
  //////////////////////////////////////////////////////////////////////////////
  class spherical
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-initializes this spherical point
    spherical() = default;

    /// \brief Constructs this point with the specified \p rho, \p phi, and
    ///        \p theta
    ///
    /// \param rho the distance from the origin
    /// \param theta the first angular component
    /// \param theta the second angular component
    constexpr spherical(real rho, radian phi, radian theta) noexcept;

    /// \brief Copies the contents from \p other
    ///
    /// \param other the other spherical point to copy
    spherical(const spherical& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other point to copy
    /// \return reference to `(*this)`
    auto operator=(const spherical& other) -> spherical& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the distance component of this spherical
    ///
    /// \return reference to the distance component
    constexpr auto rho() noexcept -> real&;
    constexpr auto rho() const noexcept -> real;
    /// \}

    /// \{
    /// \brief Gets the phi component of this spherical
    ///
    /// \return reference to the phi component
    constexpr auto phi() noexcept -> radian&;
    constexpr auto phi() const noexcept -> radian;
    /// \}

    /// \{
    /// \brief Gets the theta component of this spherical
    ///
    /// \return reference to the theta component
    constexpr auto theta() noexcept -> radian&;
    constexpr auto theta() const noexcept -> radian;
    /// \}

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    real m_rho;
    radian m_phi;
    radian m_theta;
  };

  //============================================================================
  // non-member functions : class : spherical
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr auto operator==(const spherical& lhs, const spherical& rhs) noexcept -> bool;
  constexpr auto operator!=(const spherical& lhs, const spherical& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two spherical relative to \p tolerance
  ///
  /// \param lhs the left spherical
  /// \param rhs the right spherical
  /// \return \c true if the two spherical contain almost equal values
  constexpr auto almost_equal(const spherical& lhs,
                              const spherical& rhs,
                              real tolerance) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Converts a cartesian `point3` into a `spherical` point
  ///
  /// \param p the cartesian point
  /// \return the spherical point representation
  auto to_spherical(const point3& p) noexcept -> spherical;

  /// \brief Converts a `spherical` point into a cartesian `point3`
  ///
  /// \param p the cartesian point
  /// \return the spherical point representation
  auto to_point(const spherical& p) noexcept -> point3;


  //============================================================================
  // struct : piecewise_compare<point>
  //============================================================================

  template<>
  struct piecewise_compare<spherical>
  {
    constexpr auto operator()(const spherical& lhs, const spherical& rhs)
      noexcept -> bool;
  };

} // namespace alloy::core

//==============================================================================
// definitions : class : spherical
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::spherical::spherical(real rho, radian phi, radian theta)
  noexcept
  : m_rho{rho},
    m_phi{phi},
    m_theta{theta}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::spherical::rho()
  noexcept -> real&
{
  return m_rho;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::spherical::rho()
  const noexcept -> real
{
  return m_rho;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::spherical::phi()
  noexcept -> radian&
{
  return m_phi;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::spherical::phi()
  const noexcept -> radian
{
  return m_phi;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::spherical::theta()
  noexcept -> radian&
{
  return m_theta;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::spherical::theta()
  const noexcept -> radian
{
  return m_theta;
}

//==============================================================================
// definitions : non-member functions : class : spherical
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const spherical& lhs, const spherical& rhs) noexcept -> bool
{
  return (
    lhs.rho() == rhs.rho() &&
    lhs.phi() == rhs.phi() &&
    lhs.theta() == rhs.theta()
  );
}

inline constexpr
auto alloy::core::operator!=(const spherical& lhs, const spherical& rhs) noexcept -> bool
{
  return !(lhs == rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(const spherical& lhs,
                               const spherical& rhs,
                               real tolerance)
  noexcept -> bool
{
  return (
    almost_equal(lhs.rho(), rhs.rho(), tolerance) &&
    almost_equal(lhs.phi(), rhs.phi(), tolerance) &&
    almost_equal(lhs.theta(), rhs.theta(), tolerance)
  );
}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

inline
auto alloy::core::to_spherical(const point3& p)
  noexcept -> spherical
{
  const auto phi_distance = vector2{p.x(), p.y()}.magnitude();
  const auto distance = to_vector(p).magnitude();
  const auto phi = trigonometry::arctan2(phi_distance, p.z());
  const auto theta = trigonometry::arctan2(p.y(), p.x());

  return spherical{distance, phi, theta};
}

inline
auto alloy::core::to_point(const spherical& p)
  noexcept -> point3
{
  const auto sin_phi = trigonometry::sin(p.phi());
  const auto cos_phi = trigonometry::cos(p.phi());
  const auto cos_theta = trigonometry::cos(p.theta());
  const auto sin_theta = trigonometry::cos(p.theta());

  const auto x = p.rho() * sin_phi * cos_theta;
  const auto y = p.rho() * sin_phi * sin_theta;
  const auto z = p.rho() * cos_phi;

  return point3{x, y, z};
}


//==============================================================================
// struct : piecewise_compare<spherical>
//==============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::piecewise_compare<alloy::core::spherical>
  ::operator()(const spherical& lhs, const spherical& rhs)
  noexcept -> bool
{
  return (lhs.rho() == rhs.rho()) ?
           (lhs.phi() == rhs.phi()) ?
             (lhs.theta() < rhs.theta()) :
           (lhs.phi() < rhs.phi()) :
         (lhs.rho() < rhs.rho());
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_GEOMETRY_POINT_SPHERICAL_HPP */

