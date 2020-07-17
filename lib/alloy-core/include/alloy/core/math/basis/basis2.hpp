/*****************************************************************************
 * \file basis2.hpp
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
#ifndef ALLOY_CORE_MATH_BASIS_BASIS2_HPP
#define ALLOY_CORE_MATH_BASIS_BASIS2_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/assert.hpp"    // ALLOY_ASSERT
#include "alloy/core/math/math.hpp" // core::almost_equal
#include "alloy/core/math/vector/vector2.hpp" // core::vector2

namespace alloy::core {

  //============================================================================
  // class : basis2
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Represents the basis of a 2d coordinate system
  ///
  /// Basis vectors do not have canonical names in mathematical terms. In this
  /// class, they have been named 'up' and 'right' to differentiate the
  /// orthogonality of the vectors.
  //////////////////////////////////////////////////////////////////////////////
  class basis2
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using value_type      = vector2;
    using pointer         = value_type*;
    using reference       = value_type&;
    using const_pointer   = const value_type*;
    using const_reference = const value_type&;

    //--------------------------------------------------------------------------
    // Public Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Makes a basis by normalizing the up and right vectors
    ///
    /// \pre \p up is linearly independent from \p right
    ///
    /// \param up the direction for up
    /// \param right the direction for right
    /// \return the newly constructed basis
    static basis2 make(const vector2& up, const vector2& right) noexcept;

    /// \brief Makes a basis without normalizing the up and right vectors
    ///
    /// The vectors are assumed to be normalized and orthogonal
    ///
    /// \param up the direction for up
    /// \param right the direction for right
    /// \return the newly constructed basis
    static constexpr basis2 make_unchecked(const vector2& up, const vector2& right) noexcept;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this basis2 by copying another instance
    ///
    /// \param other the other basis2 to copy
    constexpr basis2(const basis2& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of an existing basis2
    ///
    /// \param other the other basis2 to copy
    /// \return reference to \c (*this)
    basis2& operator=(const basis2& other) noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Retrievs the up vector
    ///
    /// \return reference to the up vector
    constexpr const_reference up() const noexcept;

    /// \brief Retrieves the right vector
    ///
    /// \return reference to the right vector
    constexpr const_reference right() const noexcept;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    /// \brief Constructs a basis2 vector given the up and right vectors
    ///
    /// \pre \p up and \p right are linearly independent
    ///
    /// \param up the 'up' vector
    /// \param right the 'right' vector
    constexpr basis2(const vector2& up, const vector2& right) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    vector2 m_up;
    vector2 m_right;
  };

  //============================================================================
  // non-member functions : class : basis2
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr bool operator==(const basis2& lhs,
                            const basis2& rhs) noexcept;
  constexpr bool operator!=(const basis2& lhs,
                            const basis2& rhs) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two basis2 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left basis2
  /// \param rhs the right basis2
  /// \return \c true if the two basis2 contain almost equal values
  constexpr bool almost_equal(const basis2& lhs,
                              const basis2& rhs) noexcept;

  /// \brief Determines equality between two basis2 relative to \p tolerance
  ///
  /// \param lhs the left basis2
  /// \param rhs the right basis2
  /// \return \c true if the two basis2 contain almost equal values
  constexpr bool almost_equal(const basis2& lhs,
                              const basis2& rhs,
                              real tolerance) noexcept;

} // namespace alloy::core

//==============================================================================
// class : basis2
//==============================================================================

//------------------------------------------------------------------------------
// Public Static Factories
//------------------------------------------------------------------------------

inline alloy::core::basis2
  alloy::core::basis2::make(const vector2& up,
                            const vector2& right)
  noexcept
{
  ALLOY_ASSERT(are_linearly_independent(up, right));

  return make_unchecked(up.normalized(), right.normalized());
}

inline constexpr alloy::core::basis2
  alloy::core::basis2::make_unchecked(const vector2& up,
                                      const vector2& right)
  noexcept
{
  return basis2{up, right};
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::basis2::const_reference
  alloy::core::basis2::up()
  const noexcept
{
  return m_up;
}

inline constexpr alloy::core::basis2::const_reference
  alloy::core::basis2::right()
  const noexcept
{
  return m_right;
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::basis2::basis2(const vector2& up,
                                             const vector2& right)
  noexcept
  : m_up{up},
    m_right{right}
{

}

//==============================================================================
// non-member functions : class : basis2
//==============================================================================

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool alloy::core::operator==(const basis2& lhs,
                                              const basis2& rhs)
  noexcept
{
  return lhs.up() == rhs.up() &&
         lhs.right() == rhs.right();
}

inline constexpr bool alloy::core::operator!=(const basis2& lhs,
                                              const basis2& rhs)
  noexcept
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal(const basis2& lhs,
                                                const basis2& rhs)
  noexcept
{
  return almost_equal(lhs.up(), rhs.up()) &&
         almost_equal(lhs.right(), rhs.right());
}

inline constexpr bool alloy::core::almost_equal(const basis2& lhs,
                                                const basis2& rhs,
                                                real tolerance)
  noexcept
{
  return almost_equal(lhs.up(), rhs.up(), tolerance) &&
         almost_equal(lhs.right(), rhs.right(), tolerance);
}

#endif /* ALLOY_CORE_MATH_BASIS_BASIS2_HPP */
