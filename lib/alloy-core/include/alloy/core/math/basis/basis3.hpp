/*****************************************************************************
 * \file basis3.hpp
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
#ifndef ALLOY_CORE_MATH_BASIS_BASIS3_HPP
#define ALLOY_CORE_MATH_BASIS_BASIS3_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a basis3 vector given the up and right vectors
    ///
    /// \pre \p up and \p right are linearly independent
    ///
    /// \param up the 'up' vector
    /// \param right the 'right' vector
    /// \param forward the 'forward' vector
    constexpr basis3( vector3 up,
                      vector3 right,
                      vector3 forward) noexcept;

    /// \brief Constructs this basis3 by copying another instance
    ///
    /// \param other the other basis3 to copy
    constexpr basis3( const basis3& other ) noexcept = default;

    /// \brief Constructs this basis3 by moving another instance
    ///
    /// \param other the other basis3 to move
    constexpr basis3( basis3&& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of an existing basis3
    ///
    /// \param other the other basis3 to copy
    /// \return reference to \c (*this)
    basis3& operator=( const basis3& other ) noexcept = default;

    /// \brief Move-assigns the contents of an existing basis3
    ///
    /// \param other the other basis3 to move
    /// \return reference to \c (*this)
    basis3& operator=( basis3&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Retrievs the up vector
    ///
    /// \return reference to the up vector
    constexpr const vector3& up() const noexcept;

    /// \brief Retrieves the right vector
    ///
    /// \return reference to the right vector
    constexpr const vector3& right() const noexcept;

    /// \brief Retrieves the forward vector
    ///
    /// \return reference to the forward vector
    constexpr const vector3& forward() const noexcept;

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

  constexpr bool operator==( const basis3& lhs,
                             const basis3& rhs ) noexcept;
  constexpr bool operator!=( const basis3& lhs,
                             const basis3& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two basis3 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left basis3
  /// \param rhs the right basis3
  /// \return \c true if the two basis3 contain almost equal values
  constexpr bool almost_equal( const basis3& lhs,
                               const basis3& rhs ) noexcept;

  /// \brief Determines equality between two basis3 relative to \p tolerance
  ///
  /// \param lhs the left basis3
  /// \param rhs the right basis3
  /// \return \c true if the two basis3 contain almost equal values
  constexpr bool almost_equal( const basis3& lhs,
                               const basis3& rhs,
                               real tolerance ) noexcept;

} // namespace alloy::core

//==============================================================================
// class : basis3
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::basis3::basis3( vector3 up,
                                              vector3 right,
                                              vector3 forward )
  noexcept
  : m_up{up},
    m_right{right},
    m_forward{forward}
{
  ALLOY_ASSERT(are_linearly_independent(up,right,forward));
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::basis3::const_reference
  alloy::core::basis3::up()
  const noexcept
{
  ALLOY_ASSERT(are_linearly_independent(m_up,m_right,m_forward));
  return m_up;
}

inline constexpr alloy::core::basis3::const_reference
  alloy::core::basis3::right()
  const noexcept
{
  ALLOY_ASSERT(are_linearly_independent(m_up,m_right,m_forward));
  return m_right;
}

inline constexpr alloy::core::basis3::const_reference
  alloy::core::basis3::forward()
  const noexcept
{
  ALLOY_ASSERT(are_linearly_independent(m_up,m_right,m_forward));
  return m_forward;
}

//==============================================================================
// non-member functions : class : basis3
//==============================================================================

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool alloy::core::operator==( const basis3& lhs,
                                               const basis3& rhs )
  noexcept
{
  return lhs.up() == rhs.up() &&
         lhs.right() == rhs.right() &&
         lhs.forward() == rhs.forward();
}

inline constexpr bool alloy::core::operator!=( const basis3& lhs,
                                               const basis3& rhs )
  noexcept
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal( const basis3& lhs,
                                                 const basis3& rhs )
  noexcept
{
  return almost_equal(lhs.up(), rhs.up()) &&
         almost_equal(lhs.right(), rhs.right()) &&
         almost_equal(lhs.forward(), rhs.forward());
}

inline constexpr bool alloy::core::almost_equal( const basis3& lhs,
                                                 const basis3& rhs,
                                                 real tolerance )
  noexcept
{
  return almost_equal(lhs.up(), rhs.up(), tolerance) &&
         almost_equal(lhs.right(), rhs.right(), tolerance) &&
         almost_equal(lhs.forward(), rhs.forward(), tolerance);
}

#endif /* ALLOY_CORE_MATH_BASIS_BASIS3_HPP */