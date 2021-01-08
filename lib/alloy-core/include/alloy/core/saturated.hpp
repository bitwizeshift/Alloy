////////////////////////////////////////////////////////////////////////////////
/// \file saturated.hpp
///
/// \brief This header contains a utility for clamping values between 0 and 1
///        with automatic saturation
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

#ifndef ALLOY_CORE_SATURATED_HPP
#define ALLOY_CORE_SATURATED_HPP

#include "alloy/core/precision/real.hpp" // real
#include "alloy/core/intrinsics.hpp"     // ALLOY_FORCE_INLINE
#include "alloy/core/assert.hpp"

#include <type_traits> // std::is_floating_point
#include <functional>  // std::hash

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A value that will always saturate between values of [0...1]
  ///
  /// Unlike the "saturated" type, "saturated" is defined to have operations
  /// similar to floating point types, but with saturating behavior. All
  /// operations are guaranteed to saturate values to either '0' or '1'.
  ///
  /// \tparam Float the underlying floating point type
  /////////////////////////////////////////////////////////////////////////////
  template <typename Float>
  class saturated
  {
    static_assert(std::is_floating_point<Float>::value);
    static_assert(std::is_trivially_copyable<Float>::value);

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using element_type = Float;

    //-------------------------------------------------------------------------
    // Static Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Creates a saturated object with the given \p value with checking
    ///
    /// If \p value is below 0, it saturates to 0. If \p value is above 1, it
    /// saturates to 1.
    ///
    /// \param value the value to saturate
    /// \return the saturated value
    static constexpr auto make(element_type value) noexcept -> saturated;

    /// \brief Creates a saturated object with the given \p value without
    ///        any checks
    ///
    /// \param value the value to saturate
    /// \return the saturated value
    static constexpr auto make_unchecked(element_type value) noexcept -> saturated;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default-constructs this saturated object
    constexpr saturated() noexcept = default;

    /// \brief Copy-constructs this saturated from \p other
    ///
    /// \param other the other object to copy
    constexpr saturated(const saturated& other) noexcept = default;

    /// \brief Convert-constructs this saturated from a saturated of a different
    ///        underlying float
    ///
    /// \param other the other object to copy
    template <typename UFloat,
              typename = std::enable_if_t<std::is_convertible<UFloat, Float>::value>>
    constexpr explicit saturated(const saturated<UFloat>& other) noexcept;

    //-------------------------------------------------------------------------

    /// \brief Copies the value of \p other
    ///
    /// \param other the other saturated to copy
    /// \return reference to (*this)
    auto operator=(const saturated& other) noexcept -> saturated& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the underlying value
    ///
    /// \return the underlying value
    constexpr auto value() const noexcept -> element_type;

    //-------------------------------------------------------------------------
    // Arithmetic Operators
    //-------------------------------------------------------------------------
  public:
    constexpr auto operator-() const noexcept -> saturated;
    constexpr auto operator+() const noexcept -> saturated;

    constexpr auto operator+=(saturated rhs) noexcept -> saturated&;
    constexpr auto operator+=(element_type rhs) noexcept -> saturated&;
    constexpr auto operator-=(saturated rhs) noexcept -> saturated&;
    constexpr auto operator-=(element_type rhs) noexcept -> saturated&;
    constexpr auto operator*=(saturated rhs) noexcept -> saturated&;
    constexpr auto operator*=(element_type rhs) noexcept -> saturated&;
    constexpr auto operator/=(saturated rhs) noexcept -> saturated&;
    constexpr auto operator/=(element_type rhs) noexcept -> saturated&;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    element_type m_value;

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    /// \brief Constructs the saturated from the underlying \p value
    ///
    /// \param value the value
    constexpr explicit saturated(element_type value) noexcept;

    //-------------------------------------------------------------------------
    // Private Static Utilities
    //-------------------------------------------------------------------------
  private:

    /// \brief Saturates the \p value to either 0 or 1
    ///
    /// \param value the value to saturate
    /// \return the floating point value
    static constexpr auto saturate(element_type value) noexcept -> element_type;
  };
  
  //===========================================================================
  // non-member functions : class : saturated
  //===========================================================================

  //---------------------------------------------------------------------------
  // Comparison
  //---------------------------------------------------------------------------


  template <typename Float, typename UFloat>
  constexpr auto operator==(saturated<Float> lhs, saturated<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() == std::declval<const T&>())>
  constexpr auto operator==(saturated<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() == std::declval<const Float&>())>
  constexpr auto operator==(const T& lhs, saturated<Float> rhs)
    noexcept -> bool;

  template <typename Float, typename UFloat>
  constexpr auto operator!=(saturated<Float> lhs, saturated<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() != std::declval<const T&>())>
  constexpr auto operator!=(saturated<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() != std::declval<const Float&>())>
  constexpr auto operator!=(const T& lhs, saturated<Float> rhs)
    noexcept -> bool;

  template <typename Float, typename UFloat>
  constexpr auto operator<(saturated<Float> lhs, saturated<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() < std::declval<const T&>())>
  constexpr auto operator<(saturated<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() < std::declval<const Float&>())>
  constexpr auto operator<(const T& lhs, saturated<Float> rhs)
    noexcept -> bool;

  template <typename Float, typename UFloat>
  constexpr auto operator>(saturated<Float> lhs, saturated<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() > std::declval<const T&>())>
  constexpr auto operator>(saturated<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() > std::declval<const Float&>())>
  constexpr auto operator>(const T& lhs, saturated<Float> rhs)
    noexcept -> bool;

  template <typename Float, typename UFloat>
  constexpr auto operator<=(saturated<Float> lhs, saturated<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() <= std::declval<const T&>())>
  constexpr auto operator<=(saturated<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() <= std::declval<const Float&>())>
  constexpr auto operator<=(const T& lhs, saturated<Float> rhs)
    noexcept -> bool;

  template <typename Float, typename UFloat>
  constexpr auto operator>=(saturated<Float> lhs, saturated<UFloat> rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const Float&>() >= std::declval<const T&>())>
  constexpr auto operator>=(saturated<Float> lhs, const T& rhs)
    noexcept -> bool;
  template <typename Float, typename T,
            typename=decltype(std::declval<const T&>() == std::declval<const Float&>())>
  constexpr auto operator>=(const T& lhs, saturated<Float> rhs)
    noexcept -> bool;

  //---------------------------------------------------------------------------

  /// \brief Determines relative equality between \p lhs and \p rhs relative
  ///        to \ref default_tolerance
  ///
  /// \param lhs the value on the left of the equation
  /// \param rhs the value on the right of the equation
  /// \return \c true if \p lhs is almost equal to \p rhs
  template <typename Float>
  constexpr auto almost_equal(saturated<Float> lhs, saturated<Float> rhs)
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
  constexpr auto almost_equal(saturated<Float> lhs,
                              saturated<Float> rhs,
                              real tolerance) noexcept -> bool;

  //---------------------------------------------------------------------------
  // Arithmetic Operators
  //---------------------------------------------------------------------------

  template <typename Float>
  constexpr auto operator+(saturated<Float> lhs, saturated<Float> rhs)
    noexcept -> saturated<Float>;
  template <typename Float, typename UFloat>
  constexpr auto operator+(Float lhs, saturated<UFloat> rhs)
    noexcept -> saturated<std::common_type_t<Float, UFloat>>;
  template <typename Float, typename UFloat>
  constexpr auto operator+(saturated<Float> lhs, UFloat rhs)
    noexcept -> saturated<std::common_type_t<Float, UFloat>>;

  template <typename Float>
  constexpr auto operator-(saturated<Float> lhs, saturated<Float> rhs)
    noexcept -> saturated<Float>;
  template <typename Float, typename UFloat>
  constexpr auto operator-(Float lhs, saturated<UFloat> rhs)
    noexcept -> saturated<std::common_type_t<Float, UFloat>>;
  template <typename Float, typename UFloat>
  constexpr auto operator-(saturated<Float> lhs, UFloat rhs)
    noexcept -> saturated<std::common_type_t<Float, UFloat>>;

  template <typename Float>
  constexpr auto operator*(saturated<Float> lhs, saturated<Float> rhs)
    noexcept -> saturated<Float>;
  template <typename Float, typename UFloat>
  constexpr auto operator*(Float lhs, saturated<UFloat> rhs)
    noexcept -> saturated<std::common_type_t<Float, UFloat>>;
  template <typename Float, typename UFloat>
  constexpr auto operator*(saturated<Float> lhs, UFloat rhs)
    noexcept -> saturated<std::common_type_t<Float, UFloat>>;

  template <typename Float>
  constexpr auto operator/(saturated<Float> lhs, saturated<Float> rhs)
    noexcept -> saturated<Float>;
  template <typename Float, typename UFloat>
  constexpr auto operator/(Float lhs, saturated<UFloat> rhs)
    noexcept -> saturated<std::common_type_t<Float, UFloat>>;
  template <typename Float, typename UFloat>
  constexpr auto operator/(saturated<Float> lhs, UFloat rhs)
    noexcept -> saturated<std::common_type_t<Float, UFloat>>;

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Determines whether a given saturated value \p f is a nan
  ///
  /// \param f the value to check
  /// \return \c true if \p f is a \c nan
  template <typename Float>
  auto is_nan(saturated<Float> f) noexcept -> bool;

  //---------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is finite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c finite
  template <typename Float>
  auto is_finite(saturated<Float> f) noexcept -> bool;

  /// \brief Determines whether a given real value \p f is infinite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c infinite
  template <typename Float>
  auto is_infinite(saturated<Float> f) noexcept -> bool;

  //---------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is normal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c normal
  template <typename Float>
  auto is_normal(saturated<Float> f) noexcept -> bool;

  /// \brief Determines whether a given saturated value \p f is subnormal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c subnormal
  template <typename Float>
  auto is_subnormal(saturated<Float> f) noexcept -> bool;

} // namespace alloy::core

namespace std {
  template <typename Float>
  struct hash<::alloy::core::saturated<Float>>
  {
    constexpr auto operator()(alloy::core::saturated<Float> f) -> std::size_t
    {
      return std::hash<Float>{}(f.value());
    }
  };
} // namespace std

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE constexpr
alloy::core::saturated<Float>::saturated(element_type value)
  noexcept
  : m_value{value}
{

}

//-----------------------------------------------------------------------------
// Private Static Utilities
//-----------------------------------------------------------------------------

template <typename Float>
inline constexpr
auto alloy::core::saturated<Float>::saturate(element_type value)
  noexcept -> element_type
{
  constexpr auto min = element_type{0};
  constexpr auto max = element_type{1};

  if (value > max) {
    return max;
  }
  if (value < min) {
    return min;
  }
  return value;
}

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::make(element_type value)
  noexcept -> saturated
{
  return make_unchecked(saturate(value));
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::make_unchecked(element_type value)
  noexcept -> saturated
{
  ALLOY_ASSERT(value >= element_type{0} && value <= element_type{1});

  return saturated{value};
}

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

template <typename Float>
template <typename UFloat, typename>
ALLOY_FORCE_INLINE constexpr
alloy::core::saturated<Float>::saturated(const saturated<UFloat>& other)
  noexcept
  : m_value{static_cast<Float>(other.value())}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::value()
  const noexcept -> element_type
{
  return m_value;
}

//-----------------------------------------------------------------------------
// Arithmetic Operators
//-----------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::operator-()
  const noexcept -> saturated
{
  return saturated{element_type{0}};
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::operator+()
  const noexcept -> saturated
{
  return (*this);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::operator+=(saturated rhs)
  noexcept -> saturated&
{
  m_value = saturate(m_value + rhs.value());
  return (*this);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::operator+=(element_type rhs)
  noexcept -> saturated&
{
  m_value = saturate(m_value + rhs);
  return (*this);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::operator-=(saturated rhs)
  noexcept -> saturated&
{
  m_value = saturate(m_value - rhs.value());
  return (*this);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::operator-=(element_type rhs)
  noexcept -> saturated&
{
  m_value = saturate(m_value - rhs);
  return (*this);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::operator*=(saturated rhs)
  noexcept -> saturated&
{
  // Multiplication of two numbers between [0.0, 1.0] will always land between
  // [0.0, 1.0].
  m_value = m_value * rhs.value();
  return (*this);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::operator*=(element_type rhs)
  noexcept -> saturated&
{
  m_value = saturate(m_value * rhs);
  return (*this);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::operator/=(saturated rhs)
  noexcept -> saturated&
{
  m_value = saturate(m_value / rhs.value());
  return (*this);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::saturated<Float>::operator/=(element_type rhs)
  noexcept -> saturated&
{
  m_value = saturate(m_value / rhs);
  return (*this);
}

//=============================================================================
// non-member functions : class : saturated
//=============================================================================

//-----------------------------------------------------------------------------
// Comparison
//-----------------------------------------------------------------------------

template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator==(saturated<Float> lhs, saturated<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() == rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator==(saturated<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() == rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator==(const T& lhs, saturated<Float> rhs)
  noexcept -> bool
{
  return lhs == rhs.value();
}


template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator!=(saturated<Float> lhs, saturated<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() != rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator!=(saturated<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() != rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator!=(const T& lhs, saturated<Float> rhs)
  noexcept -> bool
{
  return lhs != rhs.value();
}


template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<(saturated<Float> lhs, saturated<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() < rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<(saturated<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() < rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<(const T& lhs, saturated<Float> rhs)
  noexcept -> bool
{
  return lhs < rhs.value();
}


template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>(saturated<Float> lhs, saturated<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() > rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>(saturated<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() > rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>(const T& lhs, saturated<Float> rhs)
  noexcept -> bool
{
  return lhs > rhs.value();
}


template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<=(saturated<Float> lhs, saturated<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() <= rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<=(saturated<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() <= rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator<=(const T& lhs,
                             saturated<Float> rhs)
  noexcept -> bool
{
  return lhs <= rhs.value();
}


template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>=(saturated<Float> lhs, saturated<UFloat> rhs)
  noexcept -> bool
{
  return lhs.value() >= rhs.value();
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>=(saturated<Float> lhs, const T& rhs)
  noexcept -> bool
{
  return lhs.value() >= rhs;
}

template <typename Float, typename T, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator>=(const T& lhs, saturated<Float> rhs)
  noexcept -> bool
{
  return lhs >= rhs.value();
}

//-----------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::almost_equal(saturated<Float> lhs, saturated<Float> rhs)
  noexcept -> bool
{
  return almost_equal(lhs, rhs, static_cast<real>(1e8));
}

template <typename Float>
inline constexpr
auto alloy::core::almost_equal(saturated<Float> lhs,
                               saturated<Float> rhs,
                               real tolerance)
  noexcept -> bool
{
  const auto tmp = (lhs.value() - rhs.value());

  return (((tmp < Float{0}) ? -tmp : tmp) <= tolerance);
}

//-----------------------------------------------------------------------------
// Arithmetic Operators
//-----------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator+(saturated<Float> lhs, saturated<Float> rhs)
  noexcept -> saturated<Float>
{
  return saturated<Float>::make(lhs.value() + rhs.value());
}

template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator+(Float lhs, saturated<UFloat> rhs)
  noexcept -> saturated<std::common_type_t<Float, UFloat>>
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs + rhs.value());
}

template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator+(saturated<Float> lhs, UFloat rhs)
  noexcept -> saturated<std::common_type_t<Float, UFloat>>
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs.value() + rhs);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator-(saturated<Float> lhs, saturated<Float> rhs)
  noexcept -> alloy::core::saturated<Float>
{
  return saturated<Float>::make(lhs.value() - rhs.value());
}

template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator-(Float lhs, saturated<UFloat> rhs)
  noexcept -> saturated<std::common_type_t<Float, UFloat>>
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs - rhs.value());
}

template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator-(saturated<Float> lhs, UFloat rhs)
  noexcept -> saturated<std::common_type_t<Float, UFloat>>
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs.value() - rhs);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator*(saturated<Float> lhs, saturated<Float> rhs)
  noexcept -> saturated<Float>
{
  return saturated<Float>::make_unchecked(lhs.value() * rhs.value());
}

template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator*(Float lhs, saturated<UFloat> rhs)
  noexcept -> saturated<std::common_type_t<Float, UFloat>>
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs * rhs.value());
}

template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator*(saturated<Float> lhs, UFloat rhs)
  noexcept -> saturated<std::common_type_t<Float, UFloat>>
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs.value() * rhs);
}

template <typename Float>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator/(saturated<Float> lhs, saturated<Float> rhs)
  noexcept -> saturated<Float>
{
  return saturated<Float>::make(lhs.value() / rhs.value());
}

template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator/(Float lhs, saturated<UFloat> rhs)
  noexcept -> saturated<std::common_type_t<Float, UFloat>>
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs / rhs.value());
}

template <typename Float, typename UFloat>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::operator/(saturated<Float> lhs, UFloat rhs)
  noexcept -> saturated<std::common_type_t<Float, UFloat>>
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs.value() / rhs);
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE
auto alloy::core::is_nan(saturated<Float> f)
  noexcept -> bool
{
  return is_nan(f.value());
}

//---------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE
auto alloy::core::is_finite(saturated<Float> f)
  noexcept -> bool
{
  return is_finite(f.value());
}

template <typename Float>
ALLOY_FORCE_INLINE
auto alloy::core::is_infinite(saturated<Float> f)
  noexcept -> bool
{
  return is_infinite(f.value());
}

//---------------------------------------------------------------------------

template <typename Float>
ALLOY_FORCE_INLINE
auto alloy::core::is_normal(saturated<Float> f)
  noexcept -> bool
{
  return is_normal(f.value());
}

template <typename Float>
ALLOY_FORCE_INLINE
auto alloy::core::is_subnormal(saturated<Float> f)
  noexcept -> bool
{
  return is_subnormal(f.value());
}

#endif /* ALLOY_CORE_SATURATED_HPP */
