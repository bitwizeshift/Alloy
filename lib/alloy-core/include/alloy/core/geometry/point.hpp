/******************************************************************************
 * \file point.hpp
 *
 * \brief This header contains an implementation of a 3d point location
 ******************************************************************************/
#ifndef ALLOY_CORE_GEOMETRY_POINT_POINT_HPP
#define ALLOY_CORE_GEOMETRY_POINT_POINT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/assert.hpp"    // ALLOY_ASSERT
#include "alloy/core/utilities/piecewise_compare.hpp" // core::piecewise_compare
#include "alloy/core/math/vector/vector3.hpp" // core::vector3
#include "alloy/core/math/math.hpp"           // core::almost_equal

#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <type_traits> // std::true_type, std::false_type

namespace alloy::core {

  //============================================================================
  // class : point
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief This class represents a coordinate in 3d space
  ///
  //////////////////////////////////////////////////////////////////////////////
  class point
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
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a point at the origin
    constexpr point() noexcept;

    /// \brief Constructs a point at coordinates \c {x,y,z}
    ///
    /// \param x the x-coordinate
    /// \param y the y-coordinate
    /// \param z the z-coordinate
    constexpr point( real x, real y, real z ) noexcept;

    /// \brief Move-constructs a point from another point
    ///
    /// \param other the other point to move
    constexpr point( point&& other ) noexcept = default;

    /// \brief Copy-constructs a point from another point
    ///
    /// \param other the other point to copy
    constexpr point( const point& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Move-assigns a point from another point
    ///
    /// \param other the other point to move
    /// \return reference to \c (*this)
    point& operator=( point&& other ) noexcept = default;

    /// \brief Copy-assigns a point from another point
    ///
    /// \param other the other point to copy
    /// \return reference to \c (*this)
    point& operator=( const point& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    point& operator+=( const vector3& rhs ) noexcept;
    point& operator-=( const vector3& rhs ) noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the x component of this point
    ///
    /// \return reference to the x component
    constexpr reference x() noexcept;
    constexpr const_reference x() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the y component of this point
    ///
    /// \return reference to the y component
    constexpr reference y() noexcept;
    constexpr const_reference y() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the z component of this point
    ///
    /// \return reference to the z component
    constexpr reference z() noexcept;
    constexpr const_reference z() const noexcept;
    /// \}

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr pointer data() noexcept;
    constexpr const_pointer data() const noexcept;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Gets an element to the \p nth element
    ///
    /// \param n the index to get
    /// \return the nth element
    constexpr reference operator[]( index_type n ) noexcept;
    constexpr const_reference operator[]( index_type n ) const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    real m_data[3]; ///< The storage data
  };

  //============================================================================
  // non-member functions : class : point
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  constexpr vector3 operator-( const point& lhs, const point& rhs ) noexcept;
  constexpr point operator+( const point& lhs, const vector3& rhs ) noexcept;
  constexpr point operator-( const point& lhs, const vector3& rhs ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr bool operator==( const point& lhs, const point& rhs ) noexcept;
  constexpr bool operator!=( const point& lhs, const point& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two point relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left point
  /// \param rhs the right point
  /// \return \c true if the two point contain almost equal values
  constexpr bool almost_equal( const point& lhs, const point& rhs ) noexcept;

  /// \brief Determines equality between two point relative to \p tolerance
  ///
  /// \param lhs the left point
  /// \param rhs the right point
  /// \return \c true if the two point contain almost equal values
  constexpr bool almost_equal( const point& lhs,
                               const point& rhs,
                               real tolerance ) noexcept;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Converts a point \p p to a \ref vector3
  ///
  /// \param p the point to convert
  /// \return the vector3
  constexpr vector3 to_vector( const point& p ) noexcept;

  //============================================================================
  // struct : piecewise_compare<point>
  //============================================================================

  template<>
  struct piecewise_compare<point>
  {
    constexpr bool operator()( const point& lhs,
                               const point& rhs ) noexcept;
  };

} // namespace alloy::core

//==============================================================================
// class : point
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::point::point()
  noexcept
  : m_data{0,0,0}
{

}

inline constexpr alloy::core::point::point( real x, real y, real z )
  noexcept
  : m_data{x,y,z}
{

}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline alloy::core::point& alloy::core::point::operator+=( const vector3& rhs )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] + rhs[i];
  }
  return (*this);
}

inline alloy::core::point& alloy::core::point::operator-=( const vector3& rhs )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] - rhs[i];
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::point::reference alloy::core::point::x()
  noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::point::const_reference alloy::core::point::x()
  const noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::point::reference alloy::core::point::y()
  noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::point::const_reference alloy::core::point::y()
  const noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::point::reference alloy::core::point::z()
  noexcept
{
  return m_data[2];
}

inline constexpr alloy::core::point::const_reference alloy::core::point::z()
  const noexcept
{
  return m_data[2];
}

inline constexpr alloy::core::point::pointer alloy::core::point::data()
  noexcept
{
  return &m_data[0];
}

inline constexpr alloy::core::point::const_pointer alloy::core::point::data()
  const noexcept
{
  return &m_data[0];
}

inline constexpr alloy::core::point::reference
  alloy::core::point::operator[]( index_type n )
  noexcept
{
  ALLOY_ASSERT( n >= 0 && n < 3 );
  return m_data[n];
}

inline constexpr alloy::core::point::const_reference
  alloy::core::point::operator[]( index_type n )
  const noexcept
{
  ALLOY_ASSERT( n >= 0 && n < 3 );
  return m_data[n];
}

//==============================================================================
// non-member functions : class : point
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3
  alloy::core::operator-( const point& lhs, const point& rhs )
  noexcept
{
  auto result = vector3{};
  for (auto i = 0; i < 3; ++i ) {
    result[i] = lhs[i] - rhs[i];
  }
  return result;
}

inline constexpr alloy::core::point
  alloy::core::operator+( const point& lhs, const vector3& rhs )
  noexcept
{
  auto result = point{};
  for (auto i = 0; i < 3; ++i ) {
    result[i] = lhs[i] + rhs[i];
  }
  return result;
}

inline constexpr alloy::core::point
  alloy::core::operator-( const point& lhs, const vector3& rhs )
  noexcept
{
  auto result = point{};
  for (auto i = 0; i < 3; ++i ) {
    result[i] = lhs[i] - rhs[i];
  }
  return result;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

inline constexpr bool alloy::core::operator==( const point& lhs,
                                               const point& rhs )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::operator!=( const point& lhs,
                                               const point& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal( const point& lhs,
                                                 const point& rhs )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    if (!almost_equal(lhs[i], rhs[i])) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::almost_equal( const point& lhs,
                                                 const point& rhs,
                                                 real tolerance )
  noexcept
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

inline constexpr alloy::core::vector3 alloy::core::to_vector( const point& p )
  noexcept
{
  return {p.x(), p.y(), p.z()};
}

//==============================================================================
// struct : piecewise_compare<point>
//==============================================================================

inline constexpr bool
  alloy::core::piecewise_compare<alloy::core::point>
  ::operator()( const point& lhs, const point& rhs )
  noexcept
{
  return (lhs.x() == rhs.x()) ?
           (lhs.y() == rhs.y()) ?
             (lhs.z() < rhs.z()) :
           (lhs.y() < rhs.y()) :
         (lhs.x() < rhs.x());
}

#endif /* ALLOY_CORE_GEOMETRY_POINT_POINT_HPP */
