// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file basis3.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_CORE_MATH_BASIS_BASIS3_HPP
#define ALLOY_CORE_MATH_BASIS_BASIS3_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/assert.hpp"    // ALLOY_ASSERT
#include "alloy/core/math/math.hpp" // core::almost_equal
#include "alloy/core/math/vector/vector3.hpp" // core::vector3

namespace alloy::core {

  //============================================================================
  // class : basis3
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Represents the basis of a 3d coordinate system
  ///
  /// Basis vectors do not have canonical names in mathematical terms. In this
  /// class, they have been named 'up', 'right', and 'forward' to differentiate
  /// the orthogonality of the vectors.
  //////////////////////////////////////////////////////////////////////////////
  class basis3
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using value_type      = vector3;
    using pointer         = value_type*;
    using reference       = value_type&;
    using const_pointer   = const value_type*;
    using const_reference = const value_type&;

    //--------------------------------------------------------------------------
    // Public Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Makes a basis by normalizing the up, right, and forward vectors
    ///
    /// \pre \p up is linearly independent from \p right and \p forward
    ///
    /// \param up the direction for up
    /// \param right the direction for right
    /// \param forward the direction for forward
    /// \return the newly constructed basis
    static auto make(
      const vector3& up,
      const vector3& right,
      const vector3& forward
    ) noexcept -> basis3;

    /// \brief Makes a basis without normalizing the up, right, and forward
    ///        vectors
    ///
    /// The vectors are assumed to be normalized and orthogonal
    ///
    /// \param up the direction for up
    /// \param right the direction for right
    /// \param forward the direction for forward
    /// \return the newly constructed basis
    static constexpr auto make_unchecked(
      const vector3& up,
      const vector3& right,
      const vector3& forward
    ) noexcept -> basis3;


    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this basis3 by copying another instance
    ///
    /// \param other the other basis3 to copy
    constexpr basis3(const basis3& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of an existing basis3
    ///
    /// \param other the other basis3 to copy
    /// \return reference to `(*this)`
    auto operator=(const basis3& other) noexcept -> basis3& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Retrievs the up vector
    ///
    /// \return reference to the up vector
    constexpr auto up() const noexcept -> const vector3&;

    /// \brief Retrieves the right vector
    ///
    /// \return reference to the right vector
    constexpr auto right() const noexcept -> const vector3&;

    /// \brief Retrieves the forward vector
    ///
    /// \return reference to the forward vector
    constexpr auto forward() const noexcept -> const vector3&;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    /// \brief Constructs a basis3 vector given the up and right vectors
    ///
    /// \pre \p up and \p right are linearly independent
    ///
    /// \param up the 'up' vector
    /// \param right the 'right' vector
    /// \param forward the 'forward' vector
    constexpr basis3(
      const vector3& up,
      const vector3& right,
      const vector3& forward
    ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    vector3 m_up;
    vector3 m_right;
    vector3 m_forward;
  };

  //============================================================================
  // non-member functions : class : basis3
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr auto operator==(const basis3& lhs, const basis3& rhs)
    noexcept -> bool;
  constexpr auto operator!=(const basis3& lhs, const basis3& rhs)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two basis3 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left basis3
  /// \param rhs the right basis3
  /// \return `true` if the two basis3 contain almost equal values
  constexpr auto almost_equal(const basis3& lhs, const basis3& rhs)
    noexcept -> bool;

  /// \brief Determines equality between two basis3 relative to \p tolerance
  ///
  /// \param lhs the left basis3
  /// \param rhs the right basis3
  /// \return `true` if the two basis3 contain almost equal values
  constexpr auto almost_equal(
    const basis3& lhs,
    const basis3& rhs,
    real tolerance
  ) noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : basis3
//==============================================================================


//------------------------------------------------------------------------------
// Public Static Factories
//------------------------------------------------------------------------------

inline
auto alloy::core::basis3::make(
  const vector3& up,
  const vector3& right,
  const vector3& forward
) noexcept -> basis3
{
  ALLOY_ASSERT(are_linearly_independent(up, right, forward));

  return make_unchecked(up.normalized(), right.normalized(), forward.normalized());
}

inline constexpr
auto alloy::core::basis3::make_unchecked(
  const vector3& up,
  const vector3& right,
  const vector3& forward
) noexcept -> basis3
{
  return basis3{up, right, forward};
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::basis3::up() const noexcept -> const_reference
{
  return m_up;
}

inline constexpr
auto alloy::core::basis3::right() const noexcept -> const_reference
{
  return m_right;
}

inline constexpr
auto alloy::core::basis3::forward() const noexcept -> const_reference
{
  return m_forward;
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::basis3::basis3(
  const vector3& up,
  const vector3& right,
  const vector3& forward
) noexcept
  : m_up{up},
    m_right{right},
    m_forward{forward}
{

}

//==============================================================================
// non-member functions : class : basis3
//==============================================================================

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const basis3& lhs, const basis3& rhs)
  noexcept -> bool
{
  return lhs.up() == rhs.up() &&
         lhs.right() == rhs.right() &&
         lhs.forward() == rhs.forward();
}

inline constexpr
auto alloy::core::operator!=(const basis3& lhs, const basis3& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal(const basis3& lhs,
                                                const basis3& rhs)
  noexcept
{
  return almost_equal(lhs, rhs, default_tolerance);
}

inline constexpr
auto alloy::core::almost_equal(
  const basis3& lhs,
  const basis3& rhs,
  real tolerance
) noexcept -> bool
{
  return almost_equal(lhs.up(), rhs.up(), tolerance) &&
         almost_equal(lhs.right(), rhs.right(), tolerance) &&
         almost_equal(lhs.forward(), rhs.forward(), tolerance);
}

#endif /* ALLOY_CORE_MATH_BASIS_BASIS3_HPP */

