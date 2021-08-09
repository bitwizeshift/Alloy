/*****************************************************************************
 * \file color.hpp
 *
 * \brief
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
#ifndef ALLOY_CORE_MODEL_COLOR_HPP
#define ALLOY_CORE_MODEL_COLOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"
#include "alloy/core/types.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/precision.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/int_utilities.hpp"

#include <type_traits>

namespace alloy::core::detail {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An internal type for providing a "reference"-like type for uint8
  ///        representations of colors.
  ///
  /// This type exists to allow for a fluid API where users may write:
  ///
  /// ```cpp
  /// auto c = color{ ... };
  /// c.r8() = 0xff;
  /// ```
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class color_reference
  {
    //--------------------------------------------------------------------------
    // Constructor / Assignment
    //--------------------------------------------------------------------------
  public:

    // Deleted default-constructor
    color_reference() = delete;

    /// \brief Constructs this color from a non-null pointer to the color
    ///
    /// \param color the color
    constexpr explicit color_reference(not_null<real*> color) noexcept;
    color_reference(const color_reference&) = delete;
    color_reference(color_reference&&) = delete;

    //--------------------------------------------------------------------------

    auto operator=(color_reference&&) -> color_reference& = delete;

    /// \brief Copies the contents of \p other
    ///
    /// \param v the value to assign
    /// \return reference to `(*this)`
    constexpr auto operator=(const color_reference& other) noexcept -> color_reference&;

    /// \brief Converts the `T` value \p v into the floating point value to
    ///        assign it to the color
    ///
    /// \param v the value to assign
    /// \return reference to `(*this)`
    constexpr auto operator=(T v) noexcept -> color_reference&;

    //--------------------------------------------------------------------------
    // Conversion
    //--------------------------------------------------------------------------
  public:

    /// \brief Convertible to the underlying uint8_t value
    constexpr operator T() const noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    /// The color component
    not_null<real*> m_color;
  };

  template <typename T>
  constexpr auto operator==(const color_reference<T>& lhs, const color_reference<T>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator!=(const color_reference<T>& lhs, const color_reference<T>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator<(const color_reference<T>& lhs, const color_reference<T>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator>(const color_reference<T>& lhs, const color_reference<T>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator<=(const color_reference<T>& lhs, const color_reference<T>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator>=(const color_reference<T>& lhs, const color_reference<T>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator==(const color_reference<T>& lhs, T rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator!=(const color_reference<T>& lhs, T rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator<(const color_reference<T>& lhs,  T rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator>(const color_reference<T>& lhs,  T rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator<=(const color_reference<T>& lhs, T rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator>=(const color_reference<T>& lhs, T rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator==(T lhs, const color_reference<T>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator!=(T lhs, const color_reference<T>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator<(T lhs, const color_reference<T>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator>(T lhs, const color_reference<T>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator<=(T lhs, const color_reference<T>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator>=(T lhs, const color_reference<T>& rhs) noexcept -> bool;

} // namespace alloy::core::detail

namespace alloy::core {

  //============================================================================
  // enum class : r, g, b, a
  //============================================================================

  enum class rgba8 : u32{};
  enum class argb8 : u32{};
  enum class abgr8 : u32{};
  enum class rgba16 : u64{};
  enum class argb16 : u64{};
  enum class abgr16 : u64{};

  //============================================================================
  // class : color
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A triple containing hue, saturation, and brightness settings
  //////////////////////////////////////////////////////////////////////////////
  struct hsb
  {
    real hue;
    real saturation;
    real brightness;
  };

  //============================================================================
  // class : color
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Representation of a color using floating-point values
  ///
  /// All colors values are in the range [0,1]
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API color
  {
    //--------------------------------------------------------------------------
    // Public Static Member
    //--------------------------------------------------------------------------
  public:

    inline static constexpr auto comparison_tolerance = default_tolerance;

    //--------------------------------------------------------------------------
    // Static Member Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a color from 8-bit integral rgba components
    ///
    /// \param r the red component
    /// \param g the green component
    /// \param b the blue component
    /// \return the constructed color value
    static constexpr auto from_rgb8(u8 r, u8 g, u8 b)
      noexcept -> color;

    /// \brief Constructs a color from 8-bit integral rgba components
    ///
    /// \param r the red component
    /// \param g the green component
    /// \param b the blue component
    /// \param a the alpha component (default: 255)
    /// \return the constructed color value
    static constexpr auto from_rgba8(u8 r, u8 g, u8 b, u8 a)
      noexcept -> color;

    //--------------------------------------------------------------------------

    /// \brief Constructs a color from 16-bit integral rgba components
    ///
    /// \param r the red component
    /// \param g the green component
    /// \param b the blue component
    /// \param a the alpha component (default: 65535)
    /// \return the constructed color value
    static constexpr auto from_rgb16(u16 r, u16 g, u16 b)
      noexcept -> color;

    /// \brief Constructs a color from 16-bit integral rgba components
    ///
    /// \param r the red component
    /// \param g the green component
    /// \param b the blue component
    /// \param a the alpha component
    /// \return the constructed color value
    static constexpr auto from_rgba16(u16 r, u16 g, u16 b, u16 a)
      noexcept -> color;

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Converts the integral representation of a color into a
    ///        \ref color object
    ///
    /// \param c the color to convert
    /// \return a color instance
    static constexpr auto from_rgba8(rgba8 c) noexcept -> color;
    static constexpr auto from_argb8(argb8 c) noexcept -> color;
    static constexpr auto from_abgr8(abgr8 c) noexcept -> color;
    static constexpr auto from_rgba16(rgba16 c) noexcept -> color;
    static constexpr auto from_argb16(argb16 c) noexcept -> color;
    static constexpr auto from_abgr16(abgr16 c) noexcept -> color;
    /// \}

    /// \brief Converts a {hue,saturation,brightness} triple into a color
    ///
    /// \note the alpha component is always '1.0' for this function
    /// \return the color denoted by te triple
    static auto from_hsb(hsb c) noexcept -> color;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs the color with 0 values
    constexpr color() noexcept;

    /// \brief Constructs a color from the given color components
    ///
    /// \param r the red color component
    /// \param g the green color component
    /// \param b the blue color component
    constexpr color(real r, real g, real b) noexcept;

    /// \brief Constructs a color from the given color components
    ///
    /// \param r the red color component
    /// \param g the green color component
    /// \param b the blue color component
    /// \param a the alpha component
    constexpr color(real r, real g, real b, real a) noexcept;

    /// \brief Copy-constructs a color from an existing one
    ///
    /// \param other the other color to copy
    color(const color& other) = default;

    /// \brief Move-constructs a color from an existing one
    ///
    /// \param other the other color to move
    color(color&& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a color from an existing one
    ///
    /// \param other the other color to copy
    /// \reference to \c (*this)
    auto operator=(const color& other) -> color& = default;

    /// \brief Move-assigns a color from an existing one
    ///
    /// \param other the other color to move
    /// \reference to \c (*this)
    auto operator=(color&& other) -> color& = default;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Inverts this color
    auto invert() noexcept -> void;

    /// \brief Contrasts this color
    auto contrast() noexcept -> void;

    /// \brief Darkens this color by \p percent
    ///
    /// A value of \c 1 darkens this to be black
    ///
    /// \param percent the percentage to darken this by
    auto darken(real percent) noexcept -> void;

    /// \brief Brightens this color by \p percent
    ///
    /// A value of \c 1 brightens this to be white
    ///
    /// \param percent the percentage to brighten this by
    auto brighten(real percent) noexcept -> void;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the red component of this color
    ///
    /// \return the red component
    constexpr auto r() noexcept -> real&;
    constexpr auto r() const noexcept -> real;
    /// \}

    /// \{
    /// \brief Gets the green component of this color
    ///
    /// \return the green component
    constexpr auto g() noexcept -> real&;
    constexpr auto g() const noexcept -> real;
    /// \}

    /// \{
    /// \brief Gets the blue component of this color
    ///
    /// \return the blue component
    constexpr auto b() noexcept -> real&;
    constexpr auto b() const noexcept -> real;
    /// \}

    /// \{
    /// \brief Gets the alpha component of this color
    ///
    /// \return the alpha component
    constexpr auto a() noexcept -> real&;
    constexpr auto a() const noexcept -> real;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Gets the red component of this color as a uint8_t
    ///
    /// \return the red component
    constexpr auto r8() noexcept -> detail::color_reference<u8>;
    constexpr auto r8() const noexcept -> u8;
    /// \}

    /// \{
    /// \brief Gets the green component of this color as a uint8_t
    ///
    /// \return the green component
    constexpr auto g8() noexcept -> detail::color_reference<u8>;
    constexpr auto g8() const noexcept -> u8;
    /// \}

    /// \{
    /// \brief Gets the blue component of this color as a uint8_t
    ///
    /// \return the blue component
    constexpr auto b8() noexcept -> detail::color_reference<u8>;
    constexpr auto b8() const noexcept -> u8;
    /// \}

    /// \{
    /// \brief Gets the alpha component of this color as a uint8_t
    ///
    /// \return the alpha component
    constexpr auto a8() noexcept -> detail::color_reference<u8>;
    constexpr auto a8() const noexcept -> u8;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Gets the red component of this color as a uint16_t
    ///
    /// \return the red component
    constexpr auto r16() noexcept -> detail::color_reference<u16>;
    constexpr auto r16() const noexcept -> u16;
    /// \}

    /// \{
    /// \brief Gets the green component of this color as a uint16_t
    ///
    /// \return the green component
    constexpr auto g16() noexcept -> detail::color_reference<u16>;
    constexpr auto g16() const noexcept -> u16;
    /// \}

    /// \{
    /// \brief Gets the blue component of this color as a uint16_t
    ///
    /// \return the blue component
    constexpr auto b16() noexcept -> detail::color_reference<u16>;
    constexpr auto b16() const noexcept -> u16;
    /// \}

    /// \{
    /// \brief Gets the alpha component of this color as a uint16_t
    ///
    /// \return the alpha component
    constexpr auto a16() noexcept -> detail::color_reference<u16>;
    constexpr auto a16() const noexcept -> u16;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Gets the color at the given index \p n
    ///
    /// \param n the index
    /// \return the color at index \p n
    constexpr auto operator[](std::ptrdiff_t n) noexcept -> real&;
    constexpr auto operator[](std::ptrdiff_t n) const noexcept -> real;
    /// \}

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the underlying data
    constexpr auto data() noexcept -> real*;
    constexpr auto data() const noexcept -> const real*;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Converts this color to an integral representation
    ///
    /// \return the integral representation of the color
    constexpr auto to_rgba8() const noexcept -> rgba8;
    constexpr auto to_argb8() const noexcept -> argb8;
    constexpr auto to_abgr8() const noexcept -> abgr8;
    constexpr auto to_rgba16() const noexcept -> rgba16;
    constexpr auto to_argb16() const noexcept -> argb16;
    constexpr auto to_abgr16() const noexcept -> abgr16;
    /// \}

    /// \brief Converts this color to a {hue,saturation,brightness} triple
    ///
    /// \return the triple
    auto to_hsb() const noexcept -> hsb;

    //--------------------------------------------------------------------------

    /// \brief Returns a copy of this color inverted
    ///
    /// \return the inverted color
    auto inverted() const noexcept -> color;

    /// \brief Returns a copy of this color contrasted
    ///
    /// \return the contrasted color
    auto contrasted() const noexcept -> color;

    /// \brief Returns a copy of this color darkened by \p percent
    ///
    /// \param percent the percentage to darken this by
    /// \return the color
    auto darkened(real percent) const noexcept -> color;

    /// \brief Returns a copy of this color brightened by \p percent
    ///
    /// \param percent the percentage to brighten this by
    /// \return the color
    auto brightened(real percent) const noexcept -> color;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    real m_data[4];
  };

  //============================================================================
  // non-member functions : class : color
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr auto operator==(const color& lhs, const color& rhs) noexcept -> bool;
  constexpr auto operator!=(const color& lhs, const color& rhs) noexcept -> bool;

  constexpr auto almost_equal(const color& lhs, const color& rhs) noexcept -> bool;
  constexpr auto almost_equal(const color& lhs, const color& rhs,
                              real tolerance) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Arithmetic
  //----------------------------------------------------------------------------

  constexpr auto operator+(const color& lhs, const color& rhs) noexcept -> color;
  constexpr auto operator-(const color& lhs, const color& rhs) noexcept -> color;
  constexpr auto operator*(const color& lhs, real scalar) noexcept -> color;
  constexpr auto operator*(real scalar, const color& rhs) noexcept -> color;
  constexpr auto operator/(const color& lhs, real scalar) noexcept -> color;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  constexpr auto blend(const color& c1, const color& c2) -> color;

  template<typename...Colors,
           typename=std::enable_if_t<(sizeof...(Colors) != 0)>>
  constexpr auto blend(const color& c1,
                       const color& c2,
                       const Colors&...colors) -> color;

} // namespace alloy::core

//==============================================================================
// class : color_reference<T>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE constexpr
alloy::core::detail::color_reference<T>::color_reference(not_null<real*> color)
  noexcept
  : m_color{color}
{

}

//------------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::detail::color_reference<T>::operator=(const color_reference<T>& other)
  noexcept -> color_reference<T>&
{
  // Don't shallow-copy on assignment; copy the underlying color
  (*m_color) = (*other.m_color);
  return (*this);
}

template <typename T>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::detail::color_reference<T>::operator=(T v)
  noexcept -> color_reference<T>&
{
  (*m_color) = (static_cast<real>(v) / static_cast<real>(std::numeric_limits<T>::max()));
  return (*this);
}

//------------------------------------------------------------------------------
// Conversion
//------------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE constexpr
alloy::core::detail::color_reference<T>::operator T()
  const noexcept
{
  return static_cast<T>((*m_color) * std::numeric_limits<T>::max());
}

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::detail::operator==(const color_reference<T>& lhs,
                                     const color_reference<T>& rhs)
  noexcept -> bool
{
  return static_cast<T>(lhs) == static_cast<T>(rhs);
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator!=(const color_reference<T>& lhs,
                                     const color_reference<T>& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator<(const color_reference<T>& lhs,
                                    const color_reference<T>& rhs)
  noexcept -> bool
{
  return static_cast<T>(lhs) < static_cast<T>(rhs);
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator>(const color_reference<T>& lhs,
                                    const color_reference<T>& rhs)
  noexcept -> bool
{
  return (rhs < lhs);
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator<=(const color_reference<T>& lhs,
                                     const color_reference<T>& rhs)
  noexcept -> bool
{
  return !(rhs < lhs);
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator>=(const color_reference<T>& lhs,
                                     const color_reference<T>& rhs)
  noexcept -> bool
{
  return !(lhs < rhs);
}

//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::detail::operator==(const color_reference<T>& lhs, T rhs)
  noexcept -> bool
{
  return static_cast<T>(lhs) == rhs;
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator!=(const color_reference<T>& lhs, T rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator<(const color_reference<T>& lhs, T rhs)
  noexcept -> bool
{
  return static_cast<T>(lhs) < rhs;
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator>(const color_reference<T>& lhs, T rhs)
  noexcept -> bool
{
  return static_cast<T>(lhs) > rhs;
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator<=(const color_reference<T>& lhs, T rhs)
  noexcept -> bool
{
  return static_cast<T>(lhs) <= rhs;
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator>=(const color_reference<T>& lhs, T rhs)
  noexcept -> bool
{
  return static_cast<T>(lhs) >= rhs;
}

//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::detail::operator==(T lhs, const color_reference<T>& rhs)
  noexcept -> bool
{
  return lhs == static_cast<T>(rhs);
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator!=(T lhs, const color_reference<T>& rhs)
  noexcept -> bool
{
  return lhs != static_cast<T>(rhs);
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator<(T lhs, const color_reference<T>& rhs)
  noexcept -> bool
{
  return lhs < static_cast<T>(rhs);
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator>(T lhs, const color_reference<T>& rhs)
  noexcept -> bool
{
  return lhs > static_cast<T>(rhs);
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator<=(T lhs, const color_reference<T>& rhs)
  noexcept -> bool
{
  return lhs <= static_cast<T>(rhs);
}

template <typename T>
inline constexpr
auto alloy::core::detail::operator>=(T lhs, const color_reference<T>& rhs)
  noexcept -> bool
{
  return lhs >= static_cast<T>(rhs);
}

//==============================================================================
// class : color
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::color::from_rgb8(u8 r, u8 g, u8 b)
  noexcept -> color
{
  return from_rgba8(r,g,b,255u);
}

inline constexpr
auto alloy::core::color::from_rgba8(u8 r, u8 g, u8 b, u8 a)
  noexcept -> color
{
  constexpr auto inverse_radix = real{1} / real{255};

  const auto rf = static_cast<real>(r) * inverse_radix;
  const auto gf = static_cast<real>(g) * inverse_radix;
  const auto bf = static_cast<real>(b) * inverse_radix;
  const auto af = static_cast<real>(a) * inverse_radix;

  return color{rf, gf, bf, af};
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::color::from_rgb16(u16 r, u16 g, u16 b)
  noexcept -> color
{
  return from_rgba16(r,g,b,65535u);
}

inline constexpr
auto alloy::core::color::from_rgba16(u16 r, u16 g, u16 b, u16 a)
  noexcept -> color
{
  constexpr auto inverse_radix = real{1} / real{65535};

  const auto rf = static_cast<real>(r) * inverse_radix;
  const auto gf = static_cast<real>(g) * inverse_radix;
  const auto bf = static_cast<real>(b) * inverse_radix;
  const auto af = static_cast<real>(a) * inverse_radix;

  return color{rf, gf, bf, af};
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::color::from_rgba8(rgba8 c)
  noexcept -> color
{
  const auto [r,g,b,a] = int_utilities::split<u8>(c);

  return from_rgba8(r,g,b,a);
}

inline constexpr
auto alloy::core::color::from_argb8(argb8 c)
  noexcept -> color
{
  const auto [a,r,g,b] = int_utilities::split<u8>(c);

  return from_rgba8(r,g,b,a);
}

inline constexpr
auto alloy::core::color::from_abgr8(abgr8 c)
  noexcept -> color
{
  const auto [a,b,g,r] = int_utilities::split<u8>(c);

  return from_rgba8(r,g,b,a);
}

inline constexpr
auto alloy::core::color::from_rgba16(rgba16 c)
  noexcept -> color
{
  const auto [r,g,b,a] = int_utilities::split<u16>(c);

  return from_rgba16(r,g,b,a);
}

inline constexpr
auto alloy::core::color::from_argb16(argb16 c)
  noexcept -> color
{
  const auto [a,r,g,b] = int_utilities::split<u16>(c);

  return from_rgba16(r,g,b,a);
}

inline constexpr
auto alloy::core::color::from_abgr16(abgr16 c)
  noexcept -> color
{
  const auto [a,b,g,r] = int_utilities::split<u16>(c);

  return from_rgba16(r,g,b,a);
}

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::color::color()
  noexcept
  : color{real{0}, real{0}, real{0}, real{0}}
{

}

inline constexpr
alloy::core::color::color(real r, real g, real b)
  noexcept
  : color{r, g, b, real{1}}
{

}

inline constexpr
alloy::core::color::color(real r, real g, real b, real a)
  noexcept
  : m_data{r, g, b, a}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::r()
  noexcept -> real&
{
  return m_data[0];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::r()
  const noexcept -> real
{
  return m_data[0];
}


ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::g()
  noexcept -> real&
{
  return m_data[1];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::g()
  const noexcept -> real
{
  return m_data[1];
}


ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::b()
  noexcept -> real&
{
  return m_data[2];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::b()
  const noexcept -> real
{
  return m_data[2];
}


ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::a()
  noexcept -> real&
{
  return m_data[3];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::a()
  const noexcept -> real
{
  return m_data[3];
}

//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::r8()
  noexcept -> detail::color_reference<u8>
{
  return detail::color_reference<u8>{&r()};
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::r8()
  const noexcept -> u8
{
  return static_cast<u8>(r() * 255.0f);
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::g8()
  noexcept -> detail::color_reference<u8>
{
  return detail::color_reference<u8>{&g()};
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::g8()
  const noexcept -> u8
{
  return static_cast<u8>(g() * 255.0f);
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::b8()
  noexcept -> detail::color_reference<u8>
{
  return detail::color_reference<u8>{&b()};
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::b8()
  const noexcept -> u8
{
  return static_cast<u8>(b() * 255.0f);
}
ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::a8()
  noexcept -> detail::color_reference<u8>
{
  return detail::color_reference<u8>{&a()};
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::a8()
  const noexcept -> u8
{
  return static_cast<u8>(a() * 255.0f);
}

//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::r16()
  noexcept -> detail::color_reference<u16>
{
  return detail::color_reference<u16>{&r()};
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::r16()
  const noexcept -> u16
{
  return static_cast<u16>(r() * 65535.0f);
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::g16()
  noexcept -> detail::color_reference<u16>
{
  return detail::color_reference<u16>{&g()};
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::g16()
  const noexcept -> u16
{
  return static_cast<u16>(g() * 65535.0f);
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::b16()
  noexcept -> detail::color_reference<u16>
{
  return detail::color_reference<u16>{&b()};
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::b16()
  const noexcept -> u16
{
  return static_cast<u16>(b() * 65535.0f);
}
ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::a16()
  noexcept -> detail::color_reference<u16>
{
  return detail::color_reference<u16>{&a()};
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::a16()
  const noexcept -> u16
{
  return static_cast<u16>(a() * 65535.0f);
}

//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::operator[](std::ptrdiff_t n)
  noexcept
  -> real&
{
  ALLOY_ASSERT(n >= 0 && n < 4);
  return m_data[n];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::operator[](std::ptrdiff_t n)
  const noexcept
  -> real
{
  ALLOY_ASSERT(n >= 0 && n < 4);
  return m_data[n];
}
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::data()
  noexcept
  -> real*
{
  return &m_data[0];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::data()
  const noexcept
  -> const real*
{
  return &m_data[0];
}

//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::to_rgba8()
  const noexcept -> rgba8
{
  return int_utilities::make<rgba8>(r8(), g8(), b8(), a8());
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::to_argb8()
  const noexcept-> argb8
{
  return int_utilities::make<argb8>(a8(), r8(), g8(), b8());
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::to_abgr8()
  const noexcept -> abgr8
{
  return int_utilities::make<abgr8>(a8(), b8(), g8(), r8());
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::to_rgba16()
  const noexcept -> rgba16
{
  return int_utilities::make<rgba16>(r16(), g16(), b16(), a16());
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::to_argb16()
  const noexcept -> argb16
{
  return int_utilities::make<argb16>(a16(), r16(), g16(), b16());
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::color::to_abgr16()
  const noexcept -> abgr16
{
  return int_utilities::make<abgr16>(a16(), b16(), g16(), r16());
}

//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::color::inverted()
  const noexcept -> color
{
  auto copy = (*this);
  copy.invert();
  return copy;
}

ALLOY_FORCE_INLINE
auto alloy::core::color::contrasted()
  const noexcept -> color
{
  auto copy = (*this);
  copy.contrast();
  return copy;
}

ALLOY_FORCE_INLINE
auto alloy::core::color::darkened(real percent)
  const noexcept -> color
{
  auto copy = (*this);
  copy.darken(percent);
  return copy;
}

ALLOY_FORCE_INLINE
auto alloy::core::color::brightened(real percent)
  const noexcept -> color
{
  auto copy = (*this);
  copy.brighten(percent);
  return copy;
}

//==============================================================================
// non-member functions : class : color
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::operator+(const color& lhs, const color& rhs)
  noexcept -> color
{
  auto result = color{};
  for (auto i = 0; i < 4; ++i ) {
    result[i] = saturate(lhs[i] + rhs[i]);
    ALLOY_ASSERT(result[i] >= real{0} && result[i] <= real{1});
  }
  return result;
}

inline constexpr
auto alloy::core::operator-(const color& lhs, const color& rhs)
  noexcept -> color
{
  auto result = color{};
  for (auto i = 0; i < 4; ++i ) {
    result[i] = saturate(lhs[i] - rhs[i]);
    ALLOY_ASSERT(result[i] >= real{0} && result[i] <= real{1});
  }
  return result;
}

inline constexpr
auto alloy::core::operator*(const color& lhs, real scalar)
  noexcept -> color
{
  auto result = color{};
  for (auto i = 0; i < 4; ++i ) {
    result[i] = saturate(lhs[i] * scalar);
    ALLOY_ASSERT(result[i] >= real{0} && result[i] <= real{1});
  }
  return result;
}

inline constexpr
auto alloy::core::operator*(real scalar, const color& rhs)
  noexcept -> color
{
  return rhs * scalar;
}

inline constexpr
auto alloy::core::operator/(const color& lhs, real scalar)
  noexcept -> color
{
  const auto reciprocal = real{1} / scalar;
  return lhs * reciprocal;
}

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const color& lhs, const color& rhs)
  noexcept -> bool
{
  for (auto i=0; i<4; ++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr
auto alloy::core::operator!=(const color& lhs, const color& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(const color& lhs, const color& rhs)
  noexcept
  -> bool
{
  return almost_equal(lhs, rhs, color::comparison_tolerance);
}

inline constexpr
auto alloy::core::almost_equal(const color& lhs, const color& rhs, real tolerance)
  noexcept
  -> bool
{
  for (auto i=0; i<4; ++i) {
    if (!almost_equal(lhs[i], rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::blend(const color& c1, const color& c2)
  -> color
{
  const auto diff = real{1} - c2.a();

  auto result = color{};
  const auto alpha_delta = c1.a() * diff;
  result.a() = alpha_delta + c2.a();
  if (almost_equal(result.a(),0)) {
    return color{0,0,0,0};
  }
  result.r() = ((c1.r() * alpha_delta) + (c2.r() * c2.a())) / result.a();
  result.g() = ((c1.g() * alpha_delta) + (c2.g() * c2.a())) / result.a();
  result.b() = ((c1.b() * alpha_delta) + (c2.b() * c2.a())) / result.a();

  return result;
}

template<typename...Colors, typename>
inline constexpr
auto alloy::core::blend(const color& c1, const color& c2, const Colors&...colors)
  -> color
{
  const auto r = blend(c1, c2);
  return blend(r, colors...);
}


#endif /* ALLOY_CORE_MODEL_COLOR_HPP */
