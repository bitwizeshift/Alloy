////////////////////////////////////////////////////////////////////////////////
/// \file texture_coordinate.hpp
///
/// \brief This header provides the definition for the semantic type used for
///        texture coordinates
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

#ifndef ALLOY_CORE_MODEL_TEXTURE_COORDINATE_HPP
#define ALLOY_CORE_MODEL_TEXTURE_COORDINATE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"    // ALLOY_ASSERT
#include "alloy/core/utilities/piecewise_compare.hpp" // core::piecewise_compare
#include "alloy/core/math/vector/vector2.hpp" // vector2
#include "alloy/core/math/math.hpp"           // almost_equal

#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <type_traits> // std::true_type, std::false_type

namespace alloy::core {

  //============================================================================
  // class : texture_coordinate
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief This class represents a texture coordinate for texture mapping
  ///
  /// Texture coordinates are denoted by a "u" and "v" value, which represent
  /// the (usually normalized) (x,y) coordinate on a texture used to map onto
  /// a 3d model.
  ///
  /// The functionality in this class is intentionally limited, since this is
  /// meant to act as a thin wrapper around 2 floating point "real" values that
  /// are used in the underlying graphics engine.
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API texture_coordinate
  {
    //--------------------------------------------------------------------------
    // Public Types
    //--------------------------------------------------------------------------
  public:

    using value_type      = real;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;

    using size_type  = std::size_t;
    using index_type = std::ptrdiff_t;

    //--------------------------------------------------------------------------
    // Public Static Member
    //--------------------------------------------------------------------------
  public:

    inline static constexpr auto comparison_tolerance = default_tolerance;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a texture_coordinate at the origin
    constexpr texture_coordinate() noexcept;

    /// \brief Constructs a texture_coordinate from {u, v}
    ///
    /// \param u the x-coordinate on the texture
    /// \param v the y-coordinate on the texture
    constexpr texture_coordinate(real u, real v) noexcept;

    /// \brief Copy-constructs a texture_coordinate from another texture_coordinate
    ///
    /// \param other the other texture_coordinate to copy
    constexpr texture_coordinate(const texture_coordinate& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a texture_coordinate from another texture_coordinate
    ///
    /// \param other the other texture_coordinate to copy
    /// \return reference to \c (*this)
    constexpr texture_coordinate& operator=(const texture_coordinate& other) noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the x component of this texture_coordinate
    ///
    /// \return reference to the x component
    constexpr auto u() noexcept -> reference;
    constexpr auto u() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the y component of this texture_coordinate
    ///
    /// \return reference to the y component
    constexpr auto v() noexcept -> reference;
    constexpr auto v() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr auto data() noexcept -> pointer;
    constexpr auto data() const noexcept -> const_pointer;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Gets an element to the \p nth element
    ///
    /// \param n the index to get
    /// \return the nth element
    constexpr auto operator[](index_type n) noexcept -> reference;
    constexpr auto operator[](index_type n) const noexcept -> const_reference;
    /// \}

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    real m_data[2]; ///< The storage data
  };

  //============================================================================
  // non-member functions : class : texture_coordinate
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr auto operator==(const texture_coordinate& lhs, const texture_coordinate& rhs) noexcept
    -> bool;
  constexpr auto operator!=(const texture_coordinate& lhs, const texture_coordinate& rhs) noexcept
    -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two texture_coordinate relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left texture_coordinate
  /// \param rhs the right texture_coordinate
  /// \return \c true if the two texture_coordinate contain almost equal values
  constexpr auto almost_equal(const texture_coordinate& lhs, const texture_coordinate& rhs) noexcept
    -> bool;

  /// \brief Determines equality between two texture_coordinate relative to \p tolerance
  ///
  /// \param lhs the left texture_coordinate
  /// \param rhs the right texture_coordinate
  /// \return \c true if the two texture_coordinate contain almost equal values
  constexpr auto almost_equal(const texture_coordinate& lhs,
                              const texture_coordinate& rhs,
                              real tolerance) noexcept
    -> bool;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Converts a texture_coordinate \p v to a \ref vector2
  ///
  /// \param v the texture_coordinate to convert
  /// \return the vector2
  constexpr auto to_vector(const texture_coordinate& v) noexcept -> vector2;

  //============================================================================
  // struct : piecewise_compare<texture_coordinate>
  //============================================================================

  template<>
  struct piecewise_compare<texture_coordinate>
  {
    constexpr auto operator()(const texture_coordinate& lhs,
                              const texture_coordinate& rhs) noexcept -> bool;
  };

} // namespace alloy::core

//==============================================================================
// class : texture_coordinate
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::texture_coordinate::texture_coordinate()
  noexcept
  : m_data{0,0}
{

}

inline constexpr alloy::core::texture_coordinate::texture_coordinate(real u, real v)
  noexcept
  : m_data{u,v}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr auto alloy::core::texture_coordinate::u()
  noexcept
  -> reference
{
  return m_data[0];
}

inline constexpr auto alloy::core::texture_coordinate::u()
  const noexcept
  -> const_reference
{
  return m_data[0];
}

inline constexpr auto alloy::core::texture_coordinate::v()
  noexcept
  -> reference
{
  return m_data[1];
}

inline constexpr auto alloy::core::texture_coordinate::v()
  const noexcept
  -> const_reference
{
  return m_data[1];
}

inline constexpr auto alloy::core::texture_coordinate::data()
  noexcept
  -> pointer
{
  return &m_data[0];
}

inline constexpr auto alloy::core::texture_coordinate::data()
  const noexcept
  -> const_pointer
{
  return &m_data[0];
}

inline constexpr auto alloy::core::texture_coordinate::operator[](index_type n)
  noexcept
  -> reference
{
  ALLOY_ASSERT(n >= 0 && n < 2);
  return m_data[n];
}

inline constexpr auto alloy::core::texture_coordinate::operator[](index_type n)
  const noexcept
  -> const_reference
{
  ALLOY_ASSERT(n >= 0 && n < 2);
  return m_data[n];
}

//==============================================================================
// non-member functions : class : texture_coordinate
//==============================================================================

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr auto alloy::core::operator==(const texture_coordinate& lhs,
                                              const texture_coordinate& rhs)
  noexcept
  -> bool
{
  for (auto i = 0; i < 2; ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr auto alloy::core::operator!=(const texture_coordinate& lhs,
                                              const texture_coordinate& rhs)
  noexcept
  -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr auto alloy::core::almost_equal(const texture_coordinate& lhs,
                                                const texture_coordinate& rhs)
  noexcept
  -> bool
{
  return almost_equal(lhs, rhs, texture_coordinate::comparison_tolerance);
}

inline constexpr auto alloy::core::almost_equal(const texture_coordinate& lhs,
                                                const texture_coordinate& rhs,
                                                real tolerance)
  noexcept
  -> bool
{
  for (auto i = 0; i < 2; ++i) {
    if (!almost_equal(lhs[i], rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

inline constexpr auto alloy::core::to_vector(const texture_coordinate& p)
  noexcept
  -> vector2
{
  return {p.u(), p.v()};
}

//==============================================================================
// struct : piecewise_compare<texture_coordinate>
//==============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr auto
  alloy::core::piecewise_compare<alloy::core::texture_coordinate>
  ::operator()(const texture_coordinate& lhs, const texture_coordinate& rhs)
  noexcept
  -> bool
{
  return (lhs.u() == rhs.u()) ?
           (lhs.v() < rhs.v()) :
         (lhs.u() < rhs.u());
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_MODEL_TEXTURE_COORDINATE_HPP */
