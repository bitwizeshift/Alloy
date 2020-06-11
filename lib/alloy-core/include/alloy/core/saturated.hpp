////////////////////////////////////////////////////////////////////////////////
/// \file saturated.hpp
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

#ifndef ALLOY_CORE_SATURATED_HPP
#define ALLOY_CORE_SATURATED_HPP

#include "alloy/core/precision/real.hpp"

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
    static constexpr saturated make(element_type value) noexcept;

    /// \brief Creates a saturated object with the given \p value without
    ///        any checks
    ///
    /// \param value the value to saturate
    /// \return the saturated value
    static constexpr saturated make_unchecked(element_type value) noexcept;

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
    constexpr saturated& operator=(const saturated& other) noexcept = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the underlying value
    ///
    /// \return the underlying value
    constexpr element_type value() const noexcept;

    //-------------------------------------------------------------------------
    // Arithmetic Operators
    //-------------------------------------------------------------------------
  public:

    constexpr saturated operator-() const noexcept;
    constexpr saturated operator+() const noexcept;

    constexpr saturated& operator+=(const saturated& rhs) noexcept;
    constexpr saturated& operator+=(const element_type& rhs) noexcept;
    constexpr saturated& operator-=(const saturated& rhs) noexcept;
    constexpr saturated& operator-=(const element_type& rhs) noexcept;
    constexpr saturated& operator*=(const saturated& rhs) noexcept;
    constexpr saturated& operator*=(const element_type& rhs) noexcept;
    constexpr saturated& operator/=(const saturated& rhs) noexcept;
    constexpr saturated& operator/=(const element_type& rhs) noexcept;

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
    static constexpr element_type saturate(element_type value) noexcept;
  };
  
  //===========================================================================
  // non-member functions : class : saturated
  //===========================================================================

  //---------------------------------------------------------------------------
  // Comparison
  //---------------------------------------------------------------------------

  template <typename Float>
  constexpr bool operator==(const saturated<Float>& lhs,
                            const saturated<Float>& rhs) noexcept;
  template <typename Float>
  constexpr bool operator!=(const saturated<Float>& lhs,
                            const saturated<Float>& rhs) noexcept;
  template <typename Float>
  constexpr bool operator<(const saturated<Float>& lhs,
                           const saturated<Float>& rhs) noexcept;
  template <typename Float>
  constexpr bool operator>(const saturated<Float>& lhs,
                           const saturated<Float>& rhs) noexcept;
  template <typename Float>
  constexpr bool operator<=(const saturated<Float>& lhs,
                            const saturated<Float>& rhs) noexcept;
  template <typename Float>
  constexpr bool operator>=(const saturated<Float>& lhs,
                            const saturated<Float>& rhs) noexcept;

  //---------------------------------------------------------------------------

  /// \brief Determines relative equality between \p lhs and \p rhs relative
  ///        to \ref default_tolerance
  ///
  /// \param lhs the value on the left of the equation
  /// \param rhs the value on the right of the equation
  /// \return \c true if \p lhs is almost equal to \p rhs
  template <typename Float>
  constexpr bool almost_equal(const saturated<Float>& lhs,
                              const saturated<Float>& rhs) noexcept;

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
  constexpr bool almost_equal(const saturated<Float>& lhs,
                              const saturated<Float>& rhs,
                              real tolerance) noexcept;

  //---------------------------------------------------------------------------
  // Arithmetic Operators
  //---------------------------------------------------------------------------

  template <typename Float>
  constexpr saturated<Float>
    operator+(const saturated<Float>& lhs, const saturated<Float>& rhs) noexcept;
  template <typename Float, typename UFloat>
  constexpr saturated<std::common_type_t<Float,UFloat>>
    operator+(Float lhs, const saturated<UFloat>& rhs) noexcept;
  template <typename Float, typename UFloat>
  constexpr saturated<std::common_type_t<Float,UFloat>>
    operator+(const saturated<Float>& lhs, UFloat rhs) noexcept;

  template <typename Float>
  constexpr saturated<Float>
    operator-(const saturated<Float>& lhs, const saturated<Float>& rhs) noexcept;
  template <typename Float, typename UFloat>
  constexpr saturated<std::common_type_t<Float,UFloat>>
    operator-(Float lhs, const saturated<UFloat>& rhs) noexcept;
  template <typename Float, typename UFloat>
  constexpr saturated<std::common_type_t<Float,UFloat>>
    operator-(const saturated<Float>& lhs, UFloat rhs) noexcept;

  template <typename Float>
  constexpr saturated<Float>
    operator*(const saturated<Float>& lhs, const saturated<Float>& rhs) noexcept;
  template <typename Float, typename UFloat>
  constexpr saturated<std::common_type_t<Float,UFloat>>
    operator*(Float lhs, const saturated<UFloat>& rhs) noexcept;
  template <typename Float, typename UFloat>
  constexpr saturated<std::common_type_t<Float,UFloat>>
    operator*(const saturated<Float>& lhs, UFloat rhs) noexcept;

  template <typename Float>
  constexpr saturated<Float>
    operator/(const saturated<Float>& lhs, const saturated<Float>& rhs) noexcept;
  template <typename Float, typename UFloat>
  constexpr saturated<std::common_type_t<Float,UFloat>>
    operator/(Float lhs, const saturated<UFloat>& rhs) noexcept;
  template <typename Float, typename UFloat>
  constexpr saturated<std::common_type_t<Float,UFloat>>
    operator/(const saturated<Float>& lhs, UFloat rhs) noexcept;

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Determines whether a given saturated value \p f is a nan
  ///
  /// \param f the value to check
  /// \return \c true if \p f is a \c nan
  template <typename Float>
  bool is_nan(saturated<Float> f) noexcept;

  //---------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is finite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c finite
  template <typename Float>
  bool is_finite(saturated<Float> f) noexcept;

  /// \brief Determines whether a given real value \p f is infinite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c infinite
  template <typename Float>
  bool is_infinite(saturated<Float> f) noexcept;

  //---------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is normal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c normal
  template <typename Float>
  bool is_normal(saturated<Float> f) noexcept;

  /// \brief Determines whether a given saturated value \p f is subnormal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c subnormal
  template <typename Float>
  bool is_subnormal(saturated<Float> f) noexcept;

} // namespace alloy::core

namespace std {
  template <typename Float>
  struct hash<::alloy::core::saturated<Float>>
  {
    constexpr std::size_t operator()(alloy::core::saturated<Float> f)
    {
      return std::hash<Float>{}(f.value());
    }
  };
} // namespace std

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

template <typename Float>
inline constexpr alloy::core::saturated<Float>::saturated(element_type value)
  noexcept
  : m_value{value}
{

}

//-----------------------------------------------------------------------------
// Private Static Utilities
//-----------------------------------------------------------------------------

template <typename Float>
inline constexpr typename alloy::core::saturated<Float>::element_type
  alloy::core::saturated<Float>::saturate(element_type value)
  noexcept
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
inline constexpr alloy::core::saturated<Float>
  alloy::core::saturated<Float>::make(element_type value)
  noexcept
{
  return make_unchecked(saturate(value));
}

template <typename Float>
inline constexpr alloy::core::saturated<Float>
  alloy::core::saturated<Float>::make_unchecked(element_type value)
  noexcept
{
  return saturated{value};
}

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

template <typename Float>
template <typename UFloat, typename>
inline constexpr alloy::core::saturated<Float>::saturated(const saturated<UFloat>& other)
  noexcept
  : m_value{static_cast<Float>(other.value())}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename Float>
inline constexpr typename alloy::core::saturated<Float>::element_type
  alloy::core::saturated<Float>::value()
  const noexcept
{
  return m_value;
}

//-----------------------------------------------------------------------------
// Arithmetic Operators
//-----------------------------------------------------------------------------

template <typename Float>
inline constexpr alloy::core::saturated<Float>
  alloy::core::saturated<Float>::operator-()
  const noexcept
{
  return saturated{element_type{0}};
}

template <typename Float>
inline constexpr alloy::core::saturated<Float>
  alloy::core::saturated<Float>::operator+()
  const noexcept
{
  return (*this);
}


template <typename Float>
inline constexpr alloy::core::saturated<Float>&
  alloy::core::saturated<Float>::operator+=(const saturated& rhs)
  noexcept
{
  m_value = saturate(m_value + rhs.value());
  return (*this);
}

template <typename Float>
inline constexpr alloy::core::saturated<Float>&
  alloy::core::saturated<Float>::operator+=(const element_type& rhs)
  noexcept
{
  m_value = saturate(m_value + rhs);
  return (*this);
}

template <typename Float>
inline constexpr alloy::core::saturated<Float>&
  alloy::core::saturated<Float>::operator-=(const saturated& rhs)
  noexcept
{
  m_value = saturate(m_value - rhs.value());
  return (*this);
}

template <typename Float>
inline constexpr alloy::core::saturated<Float>&
  alloy::core::saturated<Float>::operator-=(const element_type& rhs)
  noexcept
{
  m_value = saturate(m_value - rhs);
  return (*this);
}

template <typename Float>
inline constexpr alloy::core::saturated<Float>&
  alloy::core::saturated<Float>::operator*=(const saturated& rhs)
  noexcept
{
  m_value = saturate(m_value * rhs.value());
  return (*this);
}

template <typename Float>
inline constexpr alloy::core::saturated<Float>&
  alloy::core::saturated<Float>::operator*=(const element_type& rhs)
  noexcept
{
  m_value = saturate(m_value * rhs);
  return (*this);
}

template <typename Float>
inline constexpr alloy::core::saturated<Float>&
  alloy::core::saturated<Float>::operator/=(const saturated& rhs)
  noexcept
{
  m_value = saturate(m_value / rhs.value());
  return (*this);
}

template <typename Float>
inline constexpr alloy::core::saturated<Float>&
  alloy::core::saturated<Float>::operator/=(const element_type& rhs)
  noexcept
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

template <typename Float>
inline constexpr bool alloy::core::operator==(const saturated<Float>& lhs,
                                              const saturated<Float>& rhs)
  noexcept
{
  return lhs.value() == rhs.value();
}

template <typename Float>
inline constexpr bool alloy::core::operator!=(const saturated<Float>& lhs,
                                              const saturated<Float>& rhs)
  noexcept
{
  return lhs.value() != rhs.value();
}

template <typename Float>
inline constexpr bool alloy::core::operator<(const saturated<Float>& lhs,
                                             const saturated<Float>& rhs)
  noexcept
{
  return lhs.value() < rhs.value();
}

template <typename Float>
inline constexpr bool alloy::core::operator>(const saturated<Float>& lhs,
                                             const saturated<Float>& rhs)
  noexcept
{
  return lhs.value() > rhs.value();
}

template <typename Float>
inline constexpr bool alloy::core::operator<=(const saturated<Float>& lhs,
                                              const saturated<Float>& rhs)
  noexcept
{
  return lhs.value() <= rhs.value();
}

template <typename Float>
inline constexpr bool alloy::core::operator>=(const saturated<Float>& lhs,
                                              const saturated<Float>& rhs)
  noexcept
{
  return lhs.value() >= rhs.value();
}

//-----------------------------------------------------------------------------

template <typename Float>
inline constexpr bool alloy::core::almost_equal(const saturated<Float>& lhs,
                                                const saturated<Float>& rhs)
  noexcept
{
  return almost_equal(lhs, rhs, static_cast<real>(1e8));
}

template <typename Float>
inline constexpr bool alloy::core::almost_equal(const saturated<Float>& lhs,
                                                const saturated<Float>& rhs,
                                                real tolerance)
  noexcept
{
  const auto tmp = (lhs.value() - rhs.value());

  return (((tmp < Float{0}) ? -tmp : tmp) <= tolerance);
}

//-----------------------------------------------------------------------------
// Arithmetic Operators
//-----------------------------------------------------------------------------

template <typename Float>
inline constexpr alloy::core::saturated<Float>
  alloy::core::operator+(const saturated<Float>& lhs, const saturated<Float>& rhs)
  noexcept
{
  return saturated<Float>::make(lhs.value() + rhs.value());
}

template <typename Float, typename UFloat>
inline constexpr alloy::core::saturated<std::common_type_t<Float,UFloat>>
  alloy::core::operator+(Float lhs, const saturated<UFloat>& rhs)
  noexcept
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs + rhs.value());
}

template <typename Float, typename UFloat>
inline constexpr alloy::core::saturated<std::common_type_t<Float,UFloat>>
  alloy::core::operator+(const saturated<Float>& lhs, UFloat rhs)
  noexcept
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs.value() + rhs);
}


template <typename Float>
inline constexpr alloy::core::saturated<Float>
  alloy::core::operator-(const saturated<Float>& lhs, const saturated<Float>& rhs)
  noexcept
{
  return saturated<Float>::make(lhs.value() - rhs.value());
}

template <typename Float, typename UFloat>
inline constexpr alloy::core::saturated<std::common_type_t<Float,UFloat>>
  alloy::core::operator-(Float lhs, const saturated<UFloat>& rhs)
  noexcept
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs - rhs.value());
}

template <typename Float, typename UFloat>
inline constexpr alloy::core::saturated<std::common_type_t<Float,UFloat>>
  alloy::core::operator-(const saturated<Float>& lhs, UFloat rhs)
  noexcept
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs.value() - rhs);
}


template <typename Float>
inline constexpr alloy::core::saturated<Float>
  alloy::core::operator*(const saturated<Float>& lhs, const saturated<Float>& rhs)
  noexcept
{
  return saturated<Float>::make(lhs.value() * rhs.value());
}

template <typename Float, typename UFloat>
inline constexpr alloy::core::saturated<std::common_type_t<Float,UFloat>>
  alloy::core::operator*(Float lhs, const saturated<UFloat>& rhs)
  noexcept
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs * rhs.value());
}

template <typename Float, typename UFloat>
inline constexpr alloy::core::saturated<std::common_type_t<Float,UFloat>>
  alloy::core::operator*(const saturated<Float>& lhs, UFloat rhs)
  noexcept
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs.value() * rhs);
}


template <typename Float>
inline constexpr alloy::core::saturated<Float>
  alloy::core::operator/(const saturated<Float>& lhs, const saturated<Float>& rhs)
  noexcept
{
  return saturated<Float>::make(lhs.value() / rhs.value());
}

template <typename Float, typename UFloat>
inline constexpr alloy::core::saturated<std::common_type_t<Float,UFloat>>
  alloy::core::operator/(Float lhs, const saturated<UFloat>& rhs)
  noexcept
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs / rhs.value());
}

template <typename Float, typename UFloat>
inline constexpr alloy::core::saturated<std::common_type_t<Float,UFloat>>
  alloy::core::operator/(const saturated<Float>& lhs, UFloat rhs)
  noexcept
{
  using saturated_type = saturated<std::common_type_t<Float,UFloat>>;

  return saturated_type::make(lhs.value() / rhs);
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <typename Float>
inline bool alloy::core::is_nan(saturated<Float> f)
  noexcept
{
  return is_nan(f.value());
}

//---------------------------------------------------------------------------

template <typename Float>
inline bool alloy::core::is_finite(saturated<Float> f)
  noexcept
{
  return is_finite(f.value());
}

template <typename Float>
inline bool alloy::core::is_infinite(saturated<Float> f)
  noexcept
{
  return is_infinite(f.value());
}

//---------------------------------------------------------------------------

template <typename Float>
inline bool alloy::core::is_normal(saturated<Float> f)
  noexcept
{
  return is_normal(f.value());
}

template <typename Float>
inline bool alloy::core::is_subnormal(saturated<Float> f)
  noexcept
{
  return is_subnormal(f.value());
}

#endif /* ALLOY_CORE_SATURATED_HPP */
