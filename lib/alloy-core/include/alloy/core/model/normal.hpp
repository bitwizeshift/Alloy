////////////////////////////////////////////////////////////////////////////////
/// \file normal.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_CORE_MODEL_NORMAL_HPP
#define ALLOY_CORE_MODEL_NORMAL_HPP

#include "alloy/core/math/math.hpp"
#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/math/vector/vector3.hpp"

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A semantic type that represents normal vectors in the system
  ///
  /// Normal vectors are 3-component vectors that uphold an invariant of always
  /// being in a normalized state (that is, that `|vec| == 1`).
  ///
  /// Normal vectors, unlike `vector3`, are immutable to ensure that operations
  /// do not break this invariant at any point.
  ///
  /// Unlike `vector3` objects, `normal`s must be constructed by the static
  /// `make` or `make_unchecked` factory functions which help to uphold the
  /// invariants. `make` can be used to ensure that a vector passed to the
  /// constructor is normalized, whereas `make_unchecked` can bypass this to
  /// assume that the vector is already normalized.
  ///
  /// Because this class is a small wrapper around `vector3`, most functions
  /// are force-inlined to ensure that this has negligible overhead to the
  /// `vector3` class itself.
  /////////////////////////////////////////////////////////////////////////////
  class normal
  {
    //-------------------------------------------------------------------------
    // Public Types
    //-------------------------------------------------------------------------
  public:

    using value_type      = vector3::value_type;
    using pointer         = vector3::pointer;
    using const_pointer   = vector3::const_pointer;
    using reference       = vector3::reference;
    using const_reference = vector3::const_reference;

    using size_type  = vector3::size_type;
    using index_type = vector3::index_type;

    //-------------------------------------------------------------------------
    // Public Static Constants
    //-------------------------------------------------------------------------
  public:

    /// \brief The comparison tolerance for a normal3
    inline static constexpr auto comparison_tolerance = default_tolerance;

    //-------------------------------------------------------------------------
    // Static Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Makes a normal vector given the \p x, \p y, and \p z components
    ///
    /// \param x the x-component
    /// \param y the y-component
    /// \param z the z-component
    /// \return the normalized vector
    static auto make(real x, real y, real z) noexcept -> normal;

    /// \brief Makes a normal vector given a non-normalized vector3 \p vec
    ///
    /// \param vec the vector3
    /// \return the normalized vector
    static auto make(const vector3& vec) noexcept -> normal;

    /// \brief Makes a normal vector from 3 components that are assumed to be
    ///        normalized to unit length
    ///
    /// \pre |vec{x,y,z}| == 1
    /// \param x the x-component
    /// \param y the y-component
    /// \param z the z-component
    /// \return the normalized vector
    static constexpr auto make_unchecked(real x, real y, real z) noexcept  -> normal;

    /// \brief Makes a normal vector from a \p vector3 that is assumed to be
    ///        normalized to unit length
    ///
    /// \pre vec.is_normalized()
    /// \param vec the vector3
    /// \return the normalized vector
    static constexpr auto make_unchecked(const vector3& vec) noexcept -> normal;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    // Not default-constructible
    normal() = delete;

    /// \brief Constructs a normal by copying the contents of \p other
    ///
    /// \param other the other normal to copy
    constexpr normal(const normal& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other normal to copy
    /// \return a reference to `(*this)`
    auto operator=(const normal& other) noexcept -> normal& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components in the vector3
    ///
    /// \return the number of components in the vector3
    constexpr auto size() const noexcept -> size_type;

    /// \brief Gets the x component of this vector
    ///
    /// \return reference to the x component
    constexpr auto x() const noexcept -> value_type;

    /// \brief Gets the y component of this vector
    ///
    /// \return reference to the y component
    constexpr auto y() const noexcept -> value_type;

    /// \brief Gets the z component of this normal
    ///
    /// \return reference to the z component
    constexpr auto z() const noexcept -> value_type;

    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr auto data() const noexcept -> const_pointer;

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the entry at the \p n position
    ///
    /// \throw std::out_of_range if \p n >= 3 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr auto at(index_type n) const -> value_type;

    /// \brief Gets the entry at the \p n position
    ///
    /// \note Undefined behaviour if \p n >= 3 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr auto operator[](index_type n) const noexcept -> value_type;

    //-------------------------------------------------------------------------
    // Quantifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Determines the dot-product of this and \p rhs
    ///
    /// \param rhs the other normal to perform the dot-product with
    /// \return the dot product of this and \p rhs
    constexpr auto dot(const normal& rhs) const noexcept -> real;

    /// \brief Calculates the cross-product of this and \p rhs
    ///
    /// \param rhs the other normal to perform the cross-product with
    /// \return the cross product of this and \p rhs
    auto cross(const normal& rhs) const noexcept -> normal;

    /// \brief Gets the square magnitude of this normal
    ///
    /// \return 1
    constexpr auto square_magnitude() const noexcept -> real;

    /// \brief Gets the magnitude of this normal
    ///
    /// \return 1
    auto magnitude() const noexcept -> real;

    /// \brief Gets the perpendicular of this normal
    ///
    /// \return the perpendicular normal to \c this
    auto perpendicular() const noexcept -> normal;

    /// \brief Gets the inverse of this normal
    ///
    /// \return the inverse of this normal
    constexpr auto inverse() const noexcept -> normal;

    /// \brief Determines the angle between this and \p other
    ///
    /// \param other the other normal to determine the angle between
    /// \return the angle between this and \p other
    auto angle_between(const normal& other) const noexcept -> radian;

    /// \brief Determines the angle from this to \p other
    ///
    /// \param other the other normal to get the angle to
    /// \return the angle from this to \p other
    auto angle_to(const normal& other) const noexcept -> radian;

    /// \brief Queries whether this normal is normalized
    ///
    /// \return true
    constexpr auto is_normalized() const noexcept -> bool;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    vector3 m_vector;

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    /// \brief Constructs a `normal` from a normalized `vector3`
    ///
    /// \param vec the vector to copy
    explicit constexpr normal(const vector3& vec) noexcept;
  };

  //============================================================================
  // non-member functions : class : normal
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr auto operator==(const normal& lhs, const normal& rhs) noexcept
    -> bool;
  constexpr auto operator!=(const normal& lhs, const normal& rhs) noexcept
    -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two normal relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left normal
  /// \param rhs the right normal
  /// \return \c true if the two normal contain almost equal values
  constexpr auto almost_equal(const normal& lhs, const normal& rhs) noexcept
    -> bool;

  /// \brief Determines equality between two normal relative to \p tolerance
  ///
  /// \param lhs the left normal
  /// \param rhs the right normal
  /// \return \c true if the two normal contain almost equal values
  constexpr auto almost_equal(const normal& lhs,
                              const normal& rhs,
                              real tolerance) noexcept
    -> bool;


  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  inline namespace casts {

    /// \brief Converts a normal \p n to a \ref vector3
    ///
    /// \param n the normal to convert
    /// \return the vector3
    constexpr auto to_vector(const normal& n) noexcept -> vector3;

  } // inline namespace casts
} // namespace alloy::core

//-----------------------------------------------------------------------------
// Static Factory Functions
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE auto
  alloy::core::normal::make(real x, real y, real z)
  noexcept
  -> normal
{
  return make(vector3{x,y,z});
}

ALLOY_FORCE_INLINE auto
  alloy::core::normal::make(const vector3& vec)
  noexcept
  -> normal
{
  ALLOY_ASSERT(vec.square_magnitude() > real{0});

  return normal{vec.normalized()};
}

ALLOY_FORCE_INLINE constexpr auto
  alloy::core::normal::make_unchecked(real x, real y, real z)
  noexcept
  -> normal
{
  return make_unchecked({x,y,z});
}

ALLOY_FORCE_INLINE constexpr auto
  alloy::core::normal::make_unchecked(const vector3& vec)
  noexcept
  -> normal
{
  ALLOY_ASSERT(vec.is_normalized());

  return normal{vec};
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr auto alloy::core::normal::size()
  const noexcept
  -> size_type
{
  return m_vector.size();
}

ALLOY_FORCE_INLINE constexpr auto alloy::core::normal::x()
  const noexcept
  -> value_type
{
  return m_vector.x();
}

ALLOY_FORCE_INLINE constexpr auto alloy::core::normal::y()
  const noexcept
  -> value_type
{
  return m_vector.y();
}

ALLOY_FORCE_INLINE constexpr auto alloy::core::normal::z()
  const noexcept
  -> value_type
{
  return m_vector.z();
}

ALLOY_FORCE_INLINE constexpr auto alloy::core::normal::data()
  const noexcept
  -> const_pointer
{
  return m_vector.data();
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr auto alloy::core::normal::at(index_type n)
  const
  -> value_type
{
  return m_vector.at(n);
}

ALLOY_FORCE_INLINE constexpr auto alloy::core::normal::operator[](index_type n)
  const noexcept
  -> value_type
{
  return m_vector[n];
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr auto alloy::core::normal::dot(const normal& rhs)
  const noexcept
  -> real
{
  return m_vector.dot(rhs.m_vector);
}

ALLOY_FORCE_INLINE auto alloy::core::normal::cross(const normal& rhs)
  const noexcept
  -> normal
{
  const auto result = m_vector.cross(rhs.m_vector);

  return normal{result.normalized()};
}

ALLOY_FORCE_INLINE constexpr auto alloy::core::normal::square_magnitude()
  const noexcept
  -> real
{
  return m_vector.square_magnitude();
}

ALLOY_FORCE_INLINE auto alloy::core::normal::magnitude()
  const noexcept
  -> real
{
  return m_vector.magnitude();
}

ALLOY_FORCE_INLINE auto alloy::core::normal::perpendicular()
  const noexcept
  -> normal
{
  return normal{m_vector.perpendicular()};
}

ALLOY_FORCE_INLINE constexpr auto alloy::core::normal::inverse()
  const noexcept
  -> normal
{
  return normal{m_vector.inverse()};
}

ALLOY_FORCE_INLINE auto alloy::core::normal::angle_between(const normal& other)
  const noexcept
  -> radian
{
  return m_vector.angle_between(other.m_vector);
}

ALLOY_FORCE_INLINE auto alloy::core::normal::angle_to(const normal& other)
  const noexcept
  -> radian
{
  return m_vector.angle_to(other.m_vector);
}

ALLOY_FORCE_INLINE constexpr auto alloy::core::normal::is_normalized()
  const noexcept
  -> bool
{
  return m_vector.is_normalized();
}


//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr alloy::core::normal::normal(const vector3& vec)
  noexcept
  : m_vector{vec}
{

}

//==============================================================================
// non-member functions : class : normal
//==============================================================================

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr auto alloy::core::operator==(const normal& lhs,
                                              const normal& rhs)
  noexcept
  -> bool
{
  for (auto i = 0; i < 3; ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr auto alloy::core::operator!=(const normal& lhs,
                                              const normal& rhs)
  noexcept
  -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr auto alloy::core::almost_equal(const normal& lhs,
                                                const normal& rhs)
  noexcept
  -> bool
{
  return almost_equal(lhs, rhs, normal::comparison_tolerance);
}

inline constexpr auto alloy::core::almost_equal(const normal& lhs,
                                                const normal& rhs,
                                                real tolerance)
  noexcept
  -> bool
{
  for (auto i = 0; i < 3; ++i) {
    if (!almost_equal(lhs[i], rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

inline constexpr auto alloy::core::casts::to_vector(const normal& n)
  noexcept
  -> vector3
{
  return {n.x(), n.y(), n.z()};
}

#endif /* ALLOY_CORE_MODEL_NORMAL_HPP */
