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
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/precision.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/math/math.hpp"

#include <cstdint> // std::uint32_t, std::uint64_t
#include <type_traits>

namespace alloy::core {

  //============================================================================
  // enum class : r, g, b, a
  //============================================================================

  enum class rgba32 : std::uint32_t{};
  enum class argb32 : std::uint32_t{};
  enum class abgr32 : std::uint32_t{};
  enum class rgba64 : std::uint64_t{};
  enum class argb64 : std::uint64_t{};
  enum class abgr64 : std::uint64_t{};

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

    /// \{
    /// \brief Converts the integral representation of a color into a
    ///        \ref color object
    ///
    /// \param c the color to convert
    /// \return a color instance
    static auto from_rgba32(rgba32 c) noexcept -> color;
    static auto from_argb32(argb32 c) noexcept -> color;
    static auto from_abgr32(abgr32 c) noexcept -> color;
    static auto from_rgba64(rgba64 c) noexcept -> color;
    static auto from_argb64(argb64 c) noexcept -> color;
    static auto from_abgr64(abgr64 c) noexcept -> color;
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
    constexpr color(const color& other) noexcept = default;

    /// \brief Move-constructs a color from an existing one
    ///
    /// \param other the other color to move
    constexpr color(color&& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a color from an existing one
    ///
    /// \param other the other color to copy
    /// \reference to \c (*this)
    color& operator=(const color& other) noexcept = default;

    /// \brief Move-assigns a color from an existing one
    ///
    /// \param other the other color to move
    /// \reference to \c (*this)
    color& operator=(color&& other) noexcept = default;

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
    auto to_rgba32() const noexcept -> rgba32;
    auto to_argb32() const noexcept -> argb32;
    auto to_abgr32() const noexcept -> abgr32;
    auto to_rgba64() const noexcept -> rgba64;
    auto to_argb64() const noexcept -> argb64;
    auto to_abgr64() const noexcept -> abgr64;
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

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::color::color()
  noexcept
  : color{real{0}, real{0}, real{0}, real{0}}
{

}

inline constexpr alloy::core::color::color(real r, real g, real b)
  noexcept
  : color{r, g, b, real{1}}
{

}

inline constexpr alloy::core::color::color(real r, real g, real b, real a)
  noexcept
  : m_data{r, g, b, a}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr auto alloy::core::color::r()
  noexcept
  -> real&
{
  return m_data[0];
}

inline constexpr auto alloy::core::color::r()
  const noexcept
  -> real
{
  return m_data[0];
}


inline constexpr auto alloy::core::color::g()
  noexcept
  -> real&
{
  return m_data[1];
}

inline constexpr auto alloy::core::color::g()
  const noexcept
  -> real
{
  return m_data[1];
}


inline constexpr auto alloy::core::color::b()
  noexcept
  -> real&
{
  return m_data[2];
}

inline constexpr auto alloy::core::color::b()
  const noexcept
  -> real
{
  return m_data[2];
}


inline constexpr auto alloy::core::color::a()
  noexcept
  -> real&
{
  return m_data[3];
}

inline constexpr auto alloy::core::color::a()
  const noexcept
  -> real
{
  return m_data[3];
}

//------------------------------------------------------------------------------

inline constexpr auto alloy::core::color::operator[](std::ptrdiff_t n)
  noexcept
  -> real&
{
  ALLOY_ASSERT(n >= 0 && n < 4);
  return m_data[n];
}

inline constexpr auto alloy::core::color::operator[](std::ptrdiff_t n)
  const noexcept
  -> real
{
  ALLOY_ASSERT(n >= 0 && n < 4);
  return m_data[n];
}
//------------------------------------------------------------------------------

inline constexpr auto alloy::core::color::data()
  noexcept
  -> real*
{
  return &m_data[0];
}

inline constexpr auto alloy::core::color::data()
  const noexcept
  -> const real*
{
  return &m_data[0];
}

//------------------------------------------------------------------------------

inline auto alloy::core::color::inverted()
  const noexcept
  -> color
{
  auto copy = (*this);
  copy.invert();
  return copy;
}

inline auto alloy::core::color::contrasted()
  const noexcept
  -> color
{
  auto copy = (*this);
  copy.contrast();
  return copy;
}

inline auto alloy::core::color::darkened(real percent)
  const noexcept
  -> color
{
  auto copy = (*this);
  copy.darken(percent);
  return copy;
}

inline auto alloy::core::color::brightened(real percent)
  const noexcept
  -> color
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

inline constexpr auto alloy::core::operator+(const color& lhs, const color& rhs)
  noexcept
  -> color
{
  auto result = color{};
  for (auto i = 0; i < 4; ++i ) {
    result[i] = saturate(lhs[i] + rhs[i]);
    ALLOY_ASSERT(result[i] >= real{0} && result[i] <= real{1});
  }
  return result;
}

inline constexpr auto alloy::core::operator-(const color& lhs, const color& rhs)
  noexcept
  -> color
{
  auto result = color{};
  for (auto i = 0; i < 4; ++i ) {
    result[i] = saturate(lhs[i] - rhs[i]);
    ALLOY_ASSERT(result[i] >= real{0} && result[i] <= real{1});
  }
  return result;
}

inline constexpr auto alloy::core::operator*(const color& lhs, real scalar)
  noexcept
  -> color
{
  auto result = color{};
  for (auto i = 0; i < 4; ++i ) {
    result[i] = saturate(lhs[i] * scalar);
    ALLOY_ASSERT(result[i] >= real{0} && result[i] <= real{1});
  }
  return result;
}

inline constexpr auto alloy::core::operator*(real scalar, const color& rhs)
  noexcept
  -> color
{
  return rhs * scalar;
}

inline constexpr auto alloy::core::operator/(const color& lhs, real scalar)
  noexcept
  -> color
{
  const auto reciprocal = real{1} / scalar;
  return lhs * reciprocal;
}

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool alloy::core::operator==(const color& lhs,
                                              const color& rhs)
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::operator!=(const color& lhs,
                                              const color& rhs)
  noexcept
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr auto alloy::core::almost_equal(const color& lhs,
                                                const color& rhs)
  noexcept
  -> bool
{
  return almost_equal(lhs, rhs, color::comparison_tolerance);
}

inline constexpr auto alloy::core::almost_equal(const color& lhs,
                                                const color& rhs,
                                                real tolerance)
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

inline constexpr auto alloy::core::blend(const color& c1, const color& c2)
  -> color
{
  const auto diff = real{1} - c2.a();

  auto result = color{};
  result.a() = c1.a() * diff + c2.a();
  if (almost_equal(result.a(),0)) {
    return color{0,0,0,0};
  }
  result.r() = (c1.r() * c1.a() * diff + c2.r() * c2.a()) / result.a();
  result.g() = (c1.g() * c1.a() * diff + c2.g() * c2.a()) / result.a();
  result.b() = (c1.b() * c1.a() * diff + c2.b() * c2.a()) / result.a();

  return result;
}

template<typename...Colors, typename>
inline constexpr auto alloy::core::blend(const color& c1,
                                         const color& c2,
                                         const Colors&...colors)
  -> color
{
  const auto r = blend(c1, c2);
  return blend(r, colors...);
}


#endif /* ALLOY_CORE_MODEL_COLOR_HPP */
