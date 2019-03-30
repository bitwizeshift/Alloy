/******************************************************************************
 * \file euler_angles.hpp
 *
 * \brief This header contains the definition of an euler angle class
 ******************************************************************************/

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
#ifndef ALLOY_CORE_MATH_EULER_ANGLES_HPP
#define ALLOY_CORE_MATH_EULER_ANGLES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/assert.hpp"            // ALLOY_ASSERT
#include "alloy/core/precision.hpp"         // core::real
#include "alloy/core/math/math.hpp"         // core::almost_equals
#include "alloy/core/math/angle/radian.hpp" // core::radian

#include <cstddef>   // std::size_t, std::ptrdiff_t
#include <stdexcept> // std::out_of_range

namespace alloy::core {

  //============================================================================
  // class : euler_angles
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A representation of Euler Angles
  ///
  /// The angles are chosen to be represented in (yaw,pitch,roll) form.
  //////////////////////////////////////////////////////////////////////////////
  class euler_angles
  {
    //--------------------------------------------------------------------------
    // Public member Types
    //--------------------------------------------------------------------------
  public:

    using value_type      = radian;            ///< The underlying value type
    using pointer         = value_type*;       ///< The pointer type
    using reference       = value_type&;       ///< The reference type
    using const_pointer   = const value_type*; ///< The const reference type
    using const_reference = const value_type&; ///< The const pointer type

    using size_type  = std::size_t;           ///< The type used for sizes
    using index_type = std::ptrdiff_t;        ///< The type used for indices

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this euler_angles with 0 angles
    constexpr euler_angles() noexcept;

    /// \brief Constructs this euler_angles with the given \p yaw, \p pitch,
    ///        and \p roll
    ///
    /// \param yaw the yaw angle
    /// \param pitch the pitch angle
    /// \param roll the roll angle
    constexpr euler_angles( radian yaw, radian pitch, radian roll ) noexcept;

    /// \brief Constructs this euler_angles by copying another instance
    ///
    /// \param other the other euler_angles to copy
    constexpr euler_angles( const euler_angles& other ) noexcept = default;

    /// \brief Constructs this euler_angles by moving another instance
    ///
    /// \param other the other euler_angles to move
    constexpr euler_angles( euler_angles&& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of an existing euler_angles
    ///
    /// \param other the other euler_angles to copy
    /// \return reference to \c (*this)
    euler_angles& operator=( const euler_angles& other ) noexcept = default;

    /// \brief Move-assigns the contents of an existing euler_angles
    ///
    /// \param other the other euler_angles to move
    /// \return reference to \c (*this)
    euler_angles& operator=( euler_angles&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components in the vector2
    ///
    /// \return the number of components in the vector2
    constexpr size_type size() const noexcept;

    /// \{
    /// \brief Gets the yaw angle
    ///
    /// \return the yaw angle
    constexpr reference yaw() noexcept;
    constexpr const_reference yaw() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the pitch angle
    ///
    /// \return the pitch angle
    constexpr reference pitch() noexcept;
    constexpr const_reference pitch() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the roll angle
    ///
    /// \return the roll angle
    constexpr reference roll() noexcept;
    constexpr const_reference roll() const noexcept;
    /// \}

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return pointer to the underlying radians
    constexpr pointer data() noexcept;
    constexpr const_pointer data() const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// Retrieves the element of the quaternion at \p n
    ///
    /// \throw std::out_of_range if indx is out of range
    ///
    /// \param n the element index
    /// \return a reference to the entry
    reference at( index_type n );
    const_reference at( index_type n ) const;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// Retrieves the element of the quaternion at \p n
    ///
    /// \param n the element index
    /// \return a reference to the entry
    constexpr reference operator[] ( index_type n ) noexcept;
    constexpr const_reference operator[] ( index_type n ) const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Determines the dot-product of \c this and \p rhs
    ///
    /// \param rhs the other euler_angles to perform the dot-product with
    /// \return the dot product of \c this and \p rhs
    constexpr real dot( const euler_angles& rhs ) const noexcept;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    constexpr const euler_angles& operator+() const noexcept;
    constexpr euler_angles operator-() const noexcept;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    euler_angles& operator+=( const euler_angles& rhs ) noexcept;
    euler_angles& operator-=( const euler_angles& rhs ) noexcept;
    euler_angles& operator*=( real scalar ) noexcept;
    euler_angles& operator/=( real scalar ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    radian m_data[3];
  };

  //============================================================================
  // non-member functions : class : euler_angles
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  constexpr euler_angles operator+( const euler_angles& lhs,
                                    const euler_angles& rhs ) noexcept;
  constexpr euler_angles operator-( const euler_angles& lhs,
                                    const euler_angles& rhs ) noexcept;
  constexpr euler_angles operator*( const euler_angles& lhs,
                                    real scalar ) noexcept;
  constexpr euler_angles operator*( real scalar,
                                    const euler_angles& rhs ) noexcept;
  constexpr euler_angles operator/( const euler_angles& lhs,
                                    real scalar ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr bool operator==( const euler_angles& lhs,
                             const euler_angles& rhs ) noexcept;
  constexpr bool operator!=( const euler_angles& lhs,
                             const euler_angles& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two euler_angles relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left euler_angles
  /// \param rhs the right euler_angles
  /// \return \c true if the two euler_angles contain almost equal values
  constexpr bool almost_equal( const euler_angles& lhs,
                               const euler_angles& rhs ) noexcept;

  /// \brief Determines equality between two euler_angles relative to
  ///        \p tolerance
  ///
  /// \param lhs the left euler_angles
  /// \param rhs the right euler_angles
  /// \return \c true if the two euler_angles contain almost equal values
  constexpr bool almost_equal( const euler_angles& lhs,
                               const euler_angles& rhs,
                               real tolerance ) noexcept;

  //----------------------------------------------------------------------------
  // Quantifiers
  //----------------------------------------------------------------------------

  /// \brief Performs the dot product between \p lhs and \p rhs
  ///
  /// \param lhs the left euler_angles
  /// \param rhs the right euler_angles
  /// \return the result of the dot product
  constexpr real dot( const euler_angles& lhs,
                      const euler_angles& rhs ) noexcept;

} // namespace alloy::core

//==============================================================================
// class : euler_angles
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::euler_angles::euler_angles()
  noexcept
  : m_data{
      radian{real{0}},
      radian{real{0}},
      radian{real{0}}
    }
{

}

inline constexpr alloy::core::euler_angles::euler_angles( radian yaw,
                                                          radian pitch,
                                                          radian roll)
  noexcept
  : m_data{yaw, pitch, roll}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::euler_angles::size_type
  alloy::core::euler_angles::size()
  const noexcept
{
  return 3;
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::euler_angles::reference
  alloy::core::euler_angles::yaw()
  noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::euler_angles::const_reference
  alloy::core::euler_angles::yaw()
  const noexcept
{
  return m_data[0];
}


inline constexpr alloy::core::euler_angles::reference
  alloy::core::euler_angles::pitch()
  noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::euler_angles::const_reference
  alloy::core::euler_angles::pitch()
  const noexcept
{
  return m_data[1];
}


inline constexpr alloy::core::euler_angles::reference
  alloy::core::euler_angles::roll()
  noexcept
{
  return m_data[2];
}

inline constexpr alloy::core::euler_angles::const_reference
  alloy::core::euler_angles::roll()
  const noexcept
{
  return m_data[2];
}


inline constexpr alloy::core::euler_angles::pointer
  alloy::core::euler_angles::data()
  noexcept
{
  return &m_data[0];
}

inline constexpr alloy::core::euler_angles::const_pointer
  alloy::core::euler_angles::data()
  const noexcept
{
  return &m_data[0];
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline alloy::core::euler_angles::reference
  alloy::core::euler_angles::at(index_type n)
{
  if (n < 0 || n >= 3) {
    throw std::out_of_range{"euler_angles::at"};
  }
  return m_data[n];
}

inline alloy::core::euler_angles::const_reference
alloy::core::euler_angles::at(index_type n)
  const
{
  if (n < 0 || n >= 3) {
    throw std::out_of_range{"euler_angles::at"};
  }
  return m_data[n];
}


inline constexpr alloy::core::euler_angles::reference
  alloy::core::euler_angles::operator[](index_type n)
  noexcept
{
  return m_data[n];
}

inline constexpr alloy::core::euler_angles::const_reference
  alloy::core::euler_angles::operator[](index_type n)
  const noexcept
{
  return m_data[n];
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::euler_angles::dot(const euler_angles& rhs)
  const noexcept
{
  auto result = real{0};
  for (auto i = 0; i < 3; ++i) {
    result = m_data[i].value() * rhs.m_data[i].value();
  }
  return result;
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

inline constexpr const alloy::core::euler_angles&
  alloy::core::euler_angles::operator+()
  const noexcept
{
  return (*this);
}

inline constexpr alloy::core::euler_angles
  alloy::core::euler_angles::operator-()
  const noexcept
{
  return euler_angles{ -yaw(), -pitch(), -roll() };
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline alloy::core::euler_angles&
  alloy::core::euler_angles::operator+=( const euler_angles& rhs )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] += rhs.m_data[i];
  }
  return (*this);
}

inline alloy::core::euler_angles&
  alloy::core::euler_angles::operator-=( const euler_angles& rhs )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] -= rhs.m_data[i];
  }
  return (*this);
}

inline alloy::core::euler_angles&
  alloy::core::euler_angles::operator*=(real scalar)
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] *= scalar;
  }
  return (*this);
}

inline alloy::core::euler_angles&
  alloy::core::euler_angles::operator/=(real scalar)
  noexcept
{
  const auto reciprocal = real{1} / scalar;

  for (auto i = 0; i < 3; ++i) {
    m_data[i] *= reciprocal;
  }
  return (*this);
}

//==============================================================================
// non-member functions : class : euler_angles
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::euler_angles
  alloy::core::operator+( const euler_angles& lhs,
                          const euler_angles& rhs )
  noexcept
{
  return euler_angles{
    lhs.yaw() + rhs.yaw(),
    lhs.pitch() + rhs.pitch(),
    lhs.roll() + rhs.roll()
  };
}

inline constexpr alloy::core::euler_angles
  alloy::core::operator-( const euler_angles& lhs,
                          const euler_angles& rhs )
  noexcept
{
  return euler_angles{
    lhs.yaw() - rhs.yaw(),
    lhs.pitch() - rhs.pitch(),
    lhs.roll() - rhs.roll()
  };
}

inline constexpr alloy::core::euler_angles
  alloy::core::operator*( const euler_angles& lhs, real scalar )
  noexcept
{
  return euler_angles{
    lhs.yaw() * scalar,
    lhs.pitch() * scalar,
    lhs.roll() * scalar
  };
}

inline constexpr alloy::core::euler_angles
  alloy::core::operator*( real scalar, const euler_angles& rhs )
  noexcept
{
  return rhs * scalar;
}

inline constexpr alloy::core::euler_angles
  alloy::core::operator/( const euler_angles& lhs, real scalar )
  noexcept
{
  const auto reciprocal = real{1} / scalar;

  return euler_angles{
    lhs.yaw() * reciprocal,
    lhs.pitch() * reciprocal,
    lhs.roll() * reciprocal
  };
}

//----------------------------------------------------------------------------
// Comparisons
//----------------------------------------------------------------------------

inline constexpr bool alloy::core::operator==( const euler_angles& lhs,
                                               const euler_angles& rhs )
  noexcept
{
  for (auto i =0; i < 3; ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::operator!=( const euler_angles& lhs,
                                               const euler_angles& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal( const euler_angles& lhs,
                                                 const euler_angles& rhs )
  noexcept
{
  for (auto i =0; i < 3; ++i) {
    if (!almost_equal(lhs,rhs)) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::almost_equal( const euler_angles& lhs,
                                                 const euler_angles& rhs,
                                                 real tolerance )
  noexcept
{
  for (auto i =0; i < 3; ++i) {
    if (!almost_equal(lhs,rhs,tolerance)) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::dot( const euler_angles& lhs, const euler_angles& rhs )
  noexcept
{
  return lhs.dot(rhs);
}

#endif /* ALLOY_CORE_MATH_EULER_ANGLES_HPP */