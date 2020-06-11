////////////////////////////////////////////////////////////////////////////////
/// \file clamped.hpp
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

#ifndef ALLOY_CORE_CLAMPED_HPP
#define ALLOY_CORE_CLAMPED_HPP

#include "alloy/core/assert.hpp"
#include "alloy/core/precision/real.hpp"

#include <type_traits> // std::is_floating_point
#include <functional>  // std::hash

namespace alloy::core {

  //===========================================================================
  // class : clamped
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A floating point value that is clamped between 0 and 1
  ///
  /// \tparam Float the underlying floating point value
  /////////////////////////////////////////////////////////////////////////////
  template <typename Float>
  class clamped
  {
    static_assert(std::is_floating_point<Float>::value);

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using element_type = Float;

    //-------------------------------------------------------------------------
    // Static Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Creates a clamped object with the given \p value with checking
    ///
    /// \pre \p value >= 0 and \p value <= 1
    ///
    /// \param value the value
    /// \return the clamped
    static constexpr clamped make(element_type value);

    /// \brief Creates a clamped object with the given \p value without
    ///        any checks
    ///
    /// \param value the value
    /// \return the clamped
    static constexpr clamped make_unchecked(element_type value) noexcept;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default-constructs this clamped object
    constexpr clamped() noexcept = default;

    /// \brief Copy-constructs this clamped from \p other
    ///
    /// \param other the other object to copy
    constexpr clamped(const clamped& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief
    constexpr clamped& operator=(const clamped& other) noexcept = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the underlying value
    ///
    /// \return the underlying value
    constexpr element_type value() const noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    element_type m_value;

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    /// \brief Constructs the clamped from the underlying \p value
    ///
    /// \param value the value
    constexpr explicit clamped(element_type value) noexcept;
  };

  //===========================================================================
  // non-member functions : class : clamped
  //===========================================================================

  //---------------------------------------------------------------------------
  // Comparison
  //---------------------------------------------------------------------------

  template <typename Float>
  constexpr bool operator==(const clamped<Float>& lhs,
                            const clamped<Float>& rhs) noexcept;
  template <typename Float>
  constexpr bool operator!=(const clamped<Float>& lhs,
                            const clamped<Float>& rhs) noexcept;
  template <typename Float>
  constexpr bool operator<(const clamped<Float>& lhs,
                           const clamped<Float>& rhs) noexcept;
  template <typename Float>
  constexpr bool operator>(const clamped<Float>& lhs,
                           const clamped<Float>& rhs) noexcept;
  template <typename Float>
  constexpr bool operator<=(const clamped<Float>& lhs,
                            const clamped<Float>& rhs) noexcept;
  template <typename Float>
  constexpr bool operator>=(const clamped<Float>& lhs,
                            const clamped<Float>& rhs) noexcept;

  //---------------------------------------------------------------------------

  /// \brief Determines relative equality between \p lhs and \p rhs relative
  ///        to \ref default_tolerance
  ///
  /// \param lhs the value on the left of the equation
  /// \param rhs the value on the right of the equation
  /// \return \c true if \p lhs is almost equal to \p rhs
  template <typename Float>
  constexpr bool almost_equal(const clamped<Float>& lhs,
                              const clamped<Float>& rhs) noexcept;

  /// \brief Determines relative equality between \p lhs and \p rhs relative
  ///        to the specified \p tolerance
  ///
  ///
  ///
  /// \param lhs the value on the left of the equation
  /// \param rhs the value on the right of the equation
  /// \param tolerance the tolerance to use for comparison
  /// \return \c true if \p lhs is almost equal to \p rhs
  template <typename Float>
  constexpr bool almost_equal(const clamped<Float>& lhs,
                              const clamped<Float>& rhs,
                              real tolerance) noexcept;

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Determines whether a given clamped value \p f is a nan
  ///
  /// \param f the value to check
  /// \return \c true if \p f is a \c nan
  template <typename Float>
  bool is_nan(clamped<Float> f) noexcept;

  //---------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is finite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c finite
  template <typename Float>
  bool is_finite(clamped<Float> f) noexcept;

  /// \brief Determines whether a given real value \p f is infinite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c infinite
  template <typename Float>
  bool is_infinite(clamped<Float> f) noexcept;

  //---------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is normal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c normal
  template <typename Float>
  bool is_normal(clamped<Float> f) noexcept;

  /// \brief Determines whether a given clamped value \p f is subnormal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c subnormal
  template <typename Float>
  bool is_subnormal(clamped<Float> f) noexcept;

} // namespace alloy::core

namespace std {
  template <typename Float>
  struct hash<::alloy::core::clamped<Float>>
  {
    constexpr std::size_t operator()(alloy::core::clamped<Float> f)
    {
      return std::hash<Float>{}(f.value());
    }
  };
} // namespace std

//=============================================================================
// class : clamped
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

template <typename Float>
inline constexpr alloy::core::clamped<Float>
  alloy::core::clamped<Float>::make(element_type value)
{
  ALLOY_ALWAYS_ASSERT(value >= Float{0} && value <= Float{1});

  return make_unchecked(value);
}

template <typename Float>
inline constexpr alloy::core::clamped<Float>
  alloy::core::clamped<Float>::make_unchecked(element_type value)
  noexcept
{
  return clamped{value};
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename Float>
inline constexpr typename alloy::core::clamped<Float>::element_type
  alloy::core::clamped<Float>::value()
  const noexcept
{
  return m_value;
}

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

template <typename Float>
inline constexpr alloy::core::clamped<Float>::clamped(element_type value)
  noexcept
  : m_value{value}
{

}


//=============================================================================
// non-member functions : class : clamped
//=============================================================================

//-----------------------------------------------------------------------------
// Comparison
//-----------------------------------------------------------------------------

template <typename Float>
inline constexpr bool alloy::core::operator==(const clamped<Float>& lhs,
                                              const clamped<Float>& rhs)
  noexcept
{
  return lhs.value() == rhs.value();
}

template <typename Float>
inline constexpr bool alloy::core::operator!=(const clamped<Float>& lhs,
                                              const clamped<Float>& rhs)
  noexcept
{
  return lhs.value() != rhs.value();
}

template <typename Float>
inline constexpr bool alloy::core::operator<(const clamped<Float>& lhs,
                                             const clamped<Float>& rhs)
  noexcept
{
  return lhs.value() < rhs.value();
}

template <typename Float>
inline constexpr bool alloy::core::operator>(const clamped<Float>& lhs,
                                             const clamped<Float>& rhs)
  noexcept
{
  return lhs.value() > rhs.value();
}

template <typename Float>
inline constexpr bool alloy::core::operator<=(const clamped<Float>& lhs,
                                              const clamped<Float>& rhs)
  noexcept
{
  return lhs.value() <= rhs.value();
}

template <typename Float>
inline constexpr bool alloy::core::operator>=(const clamped<Float>& lhs,
                                              const clamped<Float>& rhs)
  noexcept
{
  return lhs.value() >= rhs.value();
}

template <typename Float>
inline constexpr bool alloy::core::almost_equal(const clamped<Float>& lhs,
                                                const clamped<Float>& rhs)
  noexcept
{
  return almost_equal(lhs, rhs, static_cast<real>(1e8));
}

template <typename Float>
inline constexpr bool alloy::core::almost_equal(const clamped<Float>& lhs,
                                                const clamped<Float>& rhs,
                                                real tolerance)
  noexcept
{
  const auto tmp = (lhs.value() - rhs.value());

  return (((tmp < Float{0}) ? -tmp : tmp) <= tolerance);
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <typename Float>
inline bool alloy::core::is_nan(clamped<Float> f)
  noexcept
{
  return is_nan(f.value());
}

//---------------------------------------------------------------------------

template <typename Float>
inline bool alloy::core::is_finite(clamped<Float> f)
  noexcept
{
  return is_finite(f.value());
}

template <typename Float>
inline bool alloy::core::is_infinite(clamped<Float> f)
  noexcept
{
  return is_infinite(f.value());
}

//---------------------------------------------------------------------------

template <typename Float>
inline bool alloy::core::is_normal(clamped<Float> f)
  noexcept
{
  return is_normal(f.value());
}

template <typename Float>
inline bool alloy::core::is_subnormal(clamped<Float> f)
  noexcept
{
  return is_subnormal(f.value());
}

#endif /* ALLOY_CORE_CLAMPED_HPP */
