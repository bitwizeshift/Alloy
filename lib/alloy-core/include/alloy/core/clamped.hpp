////////////////////////////////////////////////////////////////////////////////
/// \file clamped.hpp
///
/// \brief This header contains a utility for clamping values between 0 and 1
///        without any saturating behavior
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
#include "alloy/core/saturated.hpp"
#include "alloy/core/precision/real.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/utilities/result.hpp"

#include <type_traits> // std::is_floating_point
#include <functional>  // std::hash
#include <cstdint>     // std::uint8_t

namespace alloy::core {

  //===========================================================================
  // class : clamped
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A floating point value that is clamped between 0 and 1
  ///
  /// The `clamped` class template is a very simple, stripped down type that
  /// exists to differentiate APIs that require floating point values clamped
  /// in the range [0, 1].
  ///
  /// For proper saturating behavior, use the `saturated` class template. This
  /// API exists only to represent clamped values where it would be a logic
  /// bug to exceed the 1.0 or precede 0.0 (for example, open-gl-like APIs that
  /// use proper clamping).
  ///
  /// ### Examples
  ///
  /// ```cpp
  /// // Make performs boundary checking for precondition testing
  /// const auto x = alloy::core::clamped<float>::make(y);
  ///
  /// // Throws if 'x' exceeds 1.0 or precedes 0.0
  /// some_function_requiring_clamping(x.value());
  /// ```
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

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Error cases for the `clamped` type
    ///////////////////////////////////////////////////////////////////////////
    enum class error : std::uint8_t
    {
      none,      ///< No error
      overflow,  ///< Constructed value exceeds 1.0
      underflow, ///< Constructed value precedes 0.0
    };

    //-------------------------------------------------------------------------
    // Static Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Creates a clamped object with the given \p value with checking
    ///
    /// Unlike `saturated::make`, this `make` function returns an error if the
    /// input value either exceeds 1.0 or precedes 0.0. This has been done
    /// since generally it is a *logic bug* to pass values outside of this range
    /// to any APIs requiring clamped values.
    ///
    /// ### Examples
    ///
    /// Basic Use:
    ///
    /// ```cpp
    /// using clamped       = alloy::core::clamped<float>;
    /// using clamped_error = clamped::error;
    ///
    /// ALLOY_ASSERT(clamped::make(2.0f) == alloy::core::fail(clampf_error::overflow));
    /// ALLOY_ASSERT(clamped::make(-1.0f) == alloy::core::fail(clampf_error::underflow));
    /// ALLOY_ASSERT(clamped::make(0.5f) == 0.5f);
    /// ```
    ///
    /// \pre `value >= 0` and `value <= 1`
    /// \param value the value
    /// \return the clamped value, if \p value is within [0,1]; an error
    ///         otherwise. If \p value exceeds 1.0, this returns
    ///         `error::overflow`. If \p value precedes 0.0, this returns
    ///         `error::underflow`.
    static constexpr auto make(element_type value)
      noexcept -> result<clamped, error>;

    /// \brief Creates a clamped object with the given \p value without
    ///        any checks.
    ///
    /// This assumes that \p value is between the clamped range [0.0, 1.0]
    ///
    /// ### Examples
    ///
    /// Basic Use:
    ///
    /// ```cpp
    /// const auto x = alloy::core::clamped<float>::make_unchecked(0.5f);
    /// ```
    ///
    /// \param value the value
    /// \return the clamped object
    static constexpr auto make_unchecked(element_type value) noexcept -> clamped;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default-constructs this clamped object
    ///
    /// ### Examples
    ///
    /// Basic Use:
    ///
    /// ```cpp
    /// // Creates 'x' with '0.0f' as underlying value
    /// const auto x = alloy::core::clamped<float>{};
    /// ```
    constexpr clamped() noexcept = default;

    /// \brief Copy-constructs this clamped from \p other
    ///
    /// ### Examples
    ///
    /// Basic Use:
    ///
    /// ```cpp
    /// const auto other = alloy::core::clamped<float>::make_unchecked(0.5f);
    ///
    /// const auto copy = other;
    /// ```
    ///
    /// \param other the other object to copy
    constexpr clamped(const clamped& other) noexcept = default;

    /// \brief Convert-constructs this clamped from a clamped of a different
    ///        underlying float
    ///
    /// ### Examples
    ///
    /// Basic Use:
    ///
    /// ```cpp
    /// const auto other = alloy::core::saturated<float>::make_unchecked(0.5f);
    ///
    /// const auto copy = alloy::core::clamped<float>{other};
    /// ```
    ///
    /// \param other the other object to copy
    template <typename UFloat,
              typename = std::enable_if_t<std::is_convertible<UFloat, Float>::value>>
    constexpr explicit clamped(const saturated<UFloat>& other) noexcept;

    /// \brief Convert-constructs this clamped from a clamped of a different
    ///        underlying float
    ///
    /// ### Examples
    ///
    /// Basic Use:
    ///
    /// ```cpp
    /// const auto other = alloy::core::clamped<double>::make_unchecked(0.5);
    ///
    /// const auto copy = alloy::core::clamped<float>{other};
    /// ```
    ///
    /// \param other the other object to copy
    template <typename UFloat,
              typename = std::enable_if_t<std::is_convertible<UFloat, Float>::value>>
    constexpr explicit clamped(const clamped<UFloat>& other) noexcept;

    //-------------------------------------------------------------------------

    /// \brief Copies the value of \p other
    ///
    /// ### Examples
    ///
    /// Basic Use:
    ///
    /// ```cpp
    /// auto foo = alloy::core::clamped<float>::make_unchecked(0.5f);
    ///
    /// // re-assign
    /// foo = alloy::core::clamped<float>::make_unchecked(0.1f);
    /// ```
    ///
    /// \param other the other clamped to copy
    /// \return reference to (*this)
    auto operator=(const clamped& other) noexcept -> clamped& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the underlying value
    ///
    /// \return the underlying value
    constexpr auto value() const noexcept -> element_type;

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

  template <typename Float, typename UFloat>
  constexpr auto operator==(clamped<Float> lhs, clamped<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() == std::declval<const T&>())>
  constexpr auto operator==(clamped<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() == std::declval<const Float&>())>
  constexpr auto operator==(const T& lhs, clamped<Float> rhs)
    noexcept -> bool;

  template <typename Float, typename UFloat>
  constexpr auto operator!=(clamped<Float> lhs, clamped<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() != std::declval<const T&>())>
  constexpr auto operator!=(clamped<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() != std::declval<const Float&>())>
  constexpr auto operator!=(const T& lhs, clamped<Float> rhs)
    noexcept -> bool;

  template <typename Float, typename UFloat>
  constexpr auto operator<(clamped<Float> lhs, clamped<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() < std::declval<const T&>())>
  constexpr auto operator<(clamped<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() < std::declval<const Float&>())>
  constexpr auto operator<(const T& lhs, clamped<Float> rhs)
    noexcept -> bool;

  template <typename Float, typename UFloat>
  constexpr auto operator>(clamped<Float> lhs, clamped<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() > std::declval<const T&>())>
  constexpr auto operator>(clamped<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() > std::declval<const Float&>())>
  constexpr auto operator>(const T& lhs, clamped<Float> rhs)
    noexcept -> bool;

  template <typename Float, typename UFloat>
  constexpr auto operator<=(clamped<Float> lhs, clamped<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() <= std::declval<const T&>())>
  constexpr auto operator<=(clamped<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() <= std::declval<const Float&>())>
  constexpr auto operator<=(const T& lhs, clamped<Float> rhs)
    noexcept -> bool;

  template <typename Float, typename UFloat>
  constexpr auto operator>=(clamped<Float> lhs, clamped<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() >= std::declval<const T&>())>
  constexpr auto operator>=(clamped<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() == std::declval<const Float&>())>
  constexpr auto operator>=(const T& lhs, clamped<Float> rhs)
    noexcept -> bool;

  //---------------------------------------------------------------------------

  /// \brief Determines relative equality between \p lhs and \p rhs relative
  ///        to \ref default_tolerance
  ///
  /// \param lhs the value on the left of the equation
  /// \param rhs the value on the right of the equation
  /// \return \c true if \p lhs is almost equal to \p rhs
  template <typename Float>
  constexpr auto almost_equal(clamped<Float> lhs, clamped<Float> rhs)
    noexcept -> bool;

  /// \brief Determines relative equality between \p lhs and \p rhs relative
  ///        to the specified \p tolerance
  ///
  /// A tolerance of '1' or greater will always return 'true'
  ///
  /// \param lhs the value on the left of the equation
  /// \param rhs the value on the right of the equation
  /// \param tolerance the tolerance to use for comparison
  /// \return \c true if \p lhs is almost equal to \p rhs
  template <typename Float>
  constexpr auto almost_equal(clamped<Float> lhs,
                              clamped<Float> rhs,
                              real tolerance) noexcept -> bool;

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Determines whether a given clamped value \p f is a nan
  ///
  /// \param f the value to check
  /// \return \c true if \p f is a \c nan
  template <typename Float>
  auto is_nan(clamped<Float> f) noexcept -> bool;

  //---------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is finite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c finite
  template <typename Float>
  auto is_finite(clamped<Float> f) noexcept -> bool;

  /// \brief Determines whether a given real value \p f is infinite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c infinite
  template <typename Float>
  auto is_infinite(clamped<Float> f) noexcept -> bool;

  //---------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is normal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c normal
  template <typename Float>
  auto is_normal(clamped<Float> f) noexcept -> bool;

  /// \brief Determines whether a given clamped value \p f is subnormal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c subnormal
  template <typename Float>
  auto is_subnormal(clamped<Float> f) noexcept -> bool;

} // namespace alloy::core

namespace std {
  template <typename Float>
  struct hash<::alloy::core::clamped<Float>>
  {
    constexpr auto operator()(alloy::core::clamped<Float> f) -> std::size_t
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
inline constexpr
auto alloy::core::clamped<Float>::make(element_type value)
  noexcept -> result<clamped, error>
{
  if (value < Float{0}) {
    return fail(error::underflow);
  }
  if (value > Float{1}) {
    return fail(error::overflow);
  }
  return make_unchecked(value);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::clamped<Float>::make_unchecked(element_type value)
  noexcept -> clamped<Float>
{
  return clamped{value};
}

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

template <typename Float>
template <typename UFloat, typename>
ALLOY_FORCE_INLINE constexpr
alloy::core::clamped<Float>::clamped(const saturated<UFloat>& other)
  noexcept
  : m_value{static_cast<Float>(other.value())}
{

}

template <typename Float>
template <typename UFloat, typename>
ALLOY_FORCE_INLINE constexpr
alloy::core::clamped<Float>::clamped(const clamped<UFloat>& other)
  noexcept
  : m_value{static_cast<Float>(other.value())}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::clamped<Float>::value()
  const noexcept -> element_type
{
  return m_value;
}

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE constexpr
alloy::core::clamped<Float>::clamped(element_type value)
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

template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator==(clamped<Float> lhs, clamped<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() == rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator==(clamped<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() == rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator==(const T& lhs, clamped<Float> rhs)
  noexcept -> bool
{
  return lhs == rhs.value();
}


template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator!=(clamped<Float> lhs, clamped<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() != rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator!=(clamped<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() != rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator!=(const T& lhs, clamped<Float> rhs)
  noexcept -> bool
{
  return lhs != rhs.value();
}


template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<(clamped<Float> lhs, clamped<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() < rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<(clamped<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() < rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<(const T& lhs, clamped<Float> rhs)
  noexcept -> bool
{
  return lhs < rhs.value();
}


template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>(clamped<Float> lhs, clamped<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() > rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>(clamped<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() > rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>(const T& lhs, clamped<Float> rhs)
  noexcept -> bool
{
  return lhs > rhs.value();
}


template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<=(clamped<Float> lhs, clamped<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() <= rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<=(clamped<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() <= rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<=(const T& lhs,
                             clamped<Float> rhs)
  noexcept -> bool
{
  return lhs <= rhs.value();
}


template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>=(clamped<Float> lhs, clamped<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() >= rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>=(clamped<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() >= rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>=(const T& lhs, clamped<Float> rhs)
  noexcept -> bool
{
  return lhs >= rhs.value();
}


template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::almost_equal(clamped<Float> lhs, clamped<Float> rhs)
  noexcept -> bool
{
  return almost_equal(lhs, rhs, static_cast<real>(1e8));
}

template <typename Float>
inline constexpr
auto alloy::core::almost_equal(clamped<Float> lhs,
                               clamped<Float> rhs,
                               real tolerance)
  noexcept -> bool
{
  const auto tmp = (lhs.value() - rhs.value());

  return (((tmp < Float{0}) ? -tmp : tmp) <= tolerance);
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE
auto alloy::core::is_nan(clamped<Float> f)
  noexcept -> bool
{
  return is_nan(f.value());
}

//---------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE
auto alloy::core::is_finite(clamped<Float> f)
  noexcept -> bool
{
  return is_finite(f.value());
}

template <typename Float>
ALLOY_FORCE_INLINE
auto alloy::core::is_infinite(clamped<Float> f)
  noexcept -> bool
{
  return is_infinite(f.value());
}

//---------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE
auto alloy::core::is_normal(clamped<Float> f)
  noexcept -> bool
{
  return is_normal(f.value());
}

template <typename Float>
ALLOY_FORCE_INLINE
auto alloy::core::is_subnormal(clamped<Float> f)
  noexcept -> bool
{
  return is_subnormal(f.value());
}

#endif /* ALLOY_CORE_CLAMPED_HPP */
