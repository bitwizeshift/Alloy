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
#ifndef ALLOY_GRAPHICS_COLOR_HPP
#define ALLOY_GRAPHICS_COLOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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
  class color
  {
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
    static color from_rgba32(rgba32 c) noexcept;
    static color from_argb32(argb32 c) noexcept;
    static color from_abgr32(abgr32 c) noexcept;
    static color from_rgba64(rgba64 c) noexcept;
    static color from_argb64(argb64 c) noexcept;
    static color from_abgr64(abgr64 c) noexcept;
    /// \}

    /// \brief Converts a {hue,saturation,brightness} triple into a color
    ///
    /// \note the alpha component is always '1.0' for this function
    /// \return the color denoted by te triple
    static color from_hsb(hsb c) noexcept;

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
    constexpr color( real r, real g, real b ) noexcept;

    /// \brief Constructs a color from the given color components
    ///
    /// \param r the red color component
    /// \param g the green color component
    /// \param b the blue color component
    /// \param a the alpha component
    constexpr color( real r, real g, real b, real a ) noexcept;

    /// \brief Copy-constructs a color from an existing one
    ///
    /// \param other the other color to copy
    constexpr color( const color& other ) noexcept = default;

    /// \brief Move-constructs a color from an existing one
    ///
    /// \param other the other color to move
    constexpr color( color&& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a color from an existing one
    ///
    /// \param other the other color to copy
    /// \reference to \c (*this)
    color& operator=( const color& other ) noexcept = default;

    /// \brief Move-assigns a color from an existing one
    ///
    /// \param other the other color to move
    /// \reference to \c (*this)
    color& operator=( color&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Inverts this color
    void invert() noexcept;

    /// \brief Contrasts this color
    void contrast() noexcept;

    /// \brief Darkens this color by \p percent
    ///
    /// A value of \c 1 darkens this to be black
    ///
    /// \param percent the percentage to darken this by
    void darken( real percent ) noexcept;

    /// \brief Brightens this color by \p percent
    ///
    /// A value of \c 1 brightens this to be white
    ///
    /// \param percent the percentage to brighten this by
    void brighten( real percent ) noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the red component of this color
    ///
    /// \return the red component
    constexpr real& r() noexcept;
    constexpr real r() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the green component of this color
    ///
    /// \return the green component
    constexpr real& g() noexcept;
    constexpr real g() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the blue component of this color
    ///
    /// \return the blue component
    constexpr real& b() noexcept;
    constexpr real b() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the alpha component of this color
    ///
    /// \return the alpha component
    constexpr real& a() noexcept;
    constexpr real a() const noexcept;
    /// \}


    /// \{
    /// \brief Gets the color at the given index \p n
    ///
    /// \param n the index
    /// \return the color at index \p n
    constexpr real& operator[]( std::ptrdiff_t n ) noexcept;
    constexpr real operator[]( std::ptrdiff_t n ) const noexcept;
    /// \}


    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the underlying data
    constexpr real* data() noexcept;
    constexpr const real* data() const noexcept;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Converts this color to an integral representation
    ///
    /// \return the integral representation of the color
    rgba32 to_rgba32() const noexcept;
    argb32 to_argb32() const noexcept;
    abgr32 to_abgr32() const noexcept;
    rgba64 to_rgba64() const noexcept;
    argb64 to_argb64() const noexcept;
    abgr64 to_abgr64() const noexcept;
    /// \}

    /// \brief Converts this color to a {hue,saturation,brightness} triple
    ///
    /// \return the triple
    hsb to_hsb() const noexcept;

    //--------------------------------------------------------------------------

    /// \brief Returns a copy of this color inverted
    ///
    /// \return the inverted color
    color inverted() const noexcept;

    /// \brief Returns a copy of this color contrasted
    ///
    /// \return the contrasted color
    color contrasted() const noexcept;

    /// \brief Returns a copy of this color darkened by \p percent
    ///
    /// \param percent the percentage to darken this by
    /// \return the color
    color darkened( real percent ) const noexcept;

    /// \brief Returns a copy of this color brightened by \p percent
    ///
    /// \param percent the percentage to brighten this by
    /// \return the color
    color brightened( real percent ) const noexcept;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    color& operator+=( const color& other ) noexcept;
    color& operator-=( const color& other ) noexcept;
    color& operator*=( real scalar ) noexcept;
    color& operator/=( real scalar ) noexcept;

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

  constexpr bool operator==( const color& lhs, const color& rhs ) noexcept;
  constexpr bool operator!=( const color& lhs, const color& rhs ) noexcept;

  constexpr bool almost_equal( const color& lhs, const color& rhs ) noexcept;
  constexpr bool almost_equal( const color& lhs, const color& rhs,
                               real tolerance ) noexcept;

  //----------------------------------------------------------------------------
  // Arithmetic
  //----------------------------------------------------------------------------

  constexpr color operator+( const color& lhs, const color& rhs ) noexcept;
  constexpr color operator-( const color& lhs, const color& rhs ) noexcept;
  constexpr color operator*( const color& lhs, real scalar ) noexcept;
  constexpr color operator*( real scalar, const color& rhs ) noexcept;
  constexpr color operator/( const color& lhs, real scalar ) noexcept;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  constexpr color blend( const color& c1, const color& c2 );

  template<typename...Colors,
           typename=std::enable_if_t<(sizeof...(Colors) != 0)>>
  constexpr color blend( const color& c1,
                         const color& c2,
                         const Colors&...colors );

} // namespace alloy::core

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::color::color()
  noexcept
  : color{real{0}, real{0}, real{0}, real{0}}
{

}

inline constexpr alloy::core::color::color( real r, real g, real b )
  noexcept
  : color{r, g, b, real{1}}
{

}

inline constexpr alloy::core::color::color( real r, real g, real b, real a )
  noexcept
  : m_data{r, g, b, a}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real& alloy::core::color::r()
  noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::real alloy::core::color::r()
  const noexcept
{
  return m_data[0];
}


inline constexpr alloy::core::real& alloy::core::color::g()
  noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::real alloy::core::color::g()
  const noexcept
{
  return m_data[1];
}


inline constexpr alloy::core::real& alloy::core::color::b()
  noexcept
{
  return m_data[2];
}

inline constexpr alloy::core::real alloy::core::color::b()
  const noexcept
{
  return m_data[2];
}


inline constexpr alloy::core::real& alloy::core::color::a()
  noexcept
{
  return m_data[3];
}

inline constexpr alloy::core::real alloy::core::color::a()
  const noexcept
{
  return m_data[3];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::real&
  alloy::core::color::operator[]( std::ptrdiff_t n )
  noexcept
{
  ALLOY_ASSERT( n >= 0 && n < 4 );
  return m_data[n];
}

inline constexpr alloy::core::real
  alloy::core::color::operator[]( std::ptrdiff_t n )
  const noexcept
{
  ALLOY_ASSERT( n >= 0 && n < 4 );
  return m_data[n];
}
//------------------------------------------------------------------------------

inline constexpr alloy::core::real* alloy::core::color::data()
  noexcept
{
  return &m_data[0];
}

inline constexpr const alloy::core::real* alloy::core::color::data()
  const noexcept
{
  return &m_data[0];
}

//------------------------------------------------------------------------------

inline alloy::core::color alloy::core::color::inverted()
  const noexcept
{
  auto copy = (*this);
  copy.invert();
  return copy;
}

inline alloy::core::color alloy::core::color::contrasted()
  const noexcept
{
  auto copy = (*this);
  copy.contrast();
  return copy;
}

inline alloy::core::color alloy::core::color::darkened( real percent )
  const noexcept
{
  auto copy = (*this);
  copy.darken(percent);
  return copy;
}

inline alloy::core::color alloy::core::color::brightened( real percent )
  const noexcept
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

inline constexpr alloy::core::color
  alloy::core::operator+( const color& lhs, const color& rhs )
  noexcept
{
  auto result = color{};
  for (auto i = 0; i < 4; ++i ) {
    result[i] = saturate(lhs[i] + rhs[i]);
    ALLOY_ASSERT( result[i] >= real{0} && result[i] <= real{1} );
  }
  return result;
}

inline constexpr alloy::core::color
  alloy::core::operator-( const color& lhs, const color& rhs )
  noexcept
{
  auto result = color{};
  for (auto i = 0; i < 4; ++i ) {
    result[i] = saturate(lhs[i] - rhs[i]);
    ALLOY_ASSERT( result[i] >= real{0} && result[i] <= real{1} );
  }
  return result;
}

inline constexpr alloy::core::color
  alloy::core::operator*( const color& lhs, real scalar )
  noexcept
{
  auto result = color{};
  for (auto i = 0; i < 4; ++i ) {
    result[i] = saturate(lhs[i] * scalar);
    ALLOY_ASSERT( result[i] >= real{0} && result[i] <= real{1} );
  }
  return result;
}

inline constexpr alloy::core::color
  alloy::core::operator*( real scalar, const color& rhs )
  noexcept
{
  return rhs * scalar;
}

inline constexpr alloy::core::color
  alloy::core::operator/( const color& lhs, real scalar )
  noexcept
{
  const auto reciprocal = real{1} / scalar;
  return lhs * reciprocal;
}

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

inline constexpr bool alloy::core::operator==( const color& lhs,
                                               const color& rhs )
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::operator!=( const color& lhs,
                                               const color& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal( const color& lhs,
                                                 const color& rhs )
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (!almost_equal(lhs[i], rhs[i])) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::almost_equal( const color& lhs,
                                                 const color& rhs,
                                                 real tolerance )
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (!almost_equal(lhs[i], rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::color
  alloy::core::blend( const color& c1, const color& c2 )
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
inline constexpr alloy::core::color
  alloy::core::blend( const color& c1,
                      const color& c2,
                      const Colors&...colors )
{
  const auto r = blend( c1, c2 );
  return blend( r, colors... );
}


#endif /* ALLOY_GRAPHICS_COLOR_HPP */