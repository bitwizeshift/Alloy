////////////////////////////////////////////////////////////////////////////////
/// \file projection.hpp
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

#ifndef ALLOY_CORE_MODEL_PROJECTION_HPP
#define ALLOY_CORE_MODEL_PROJECTION_HPP

#include "alloy/core/api.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/precision/real.hpp"
#include "alloy/core/math/angle/degree.hpp"
#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/utilities/not_null.hpp"

#include <variant>

// Some headers like <windows.h>, if included before this header, will cause
// failures due to them `#define`-ing `near` and `far`. Remove these relics of
// history so that they don't cause build failures.
#if defined(near)
# undef near
#endif
#if defined(far)
# undef far
#endif

namespace alloy::core {

  //===========================================================================
  // struct : clipping_space
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A representation of the clipping-space
  ///
  /// This is comprised of three pieces:
  /// * The horizontal distance across,
  /// * The vertical distance across, and
  /// * The depth across
  ///
  /// clip_space is immutable to ensure that all preconditions are always
  /// upheld after construction
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API clip_space
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    struct horizontal{
      real left;
      real right;
    };

    struct vertical{
      real bottom;
      real top;
    };

    struct depth{
      real near;
      real far;
    };

    //-------------------------------------------------------------------------
    // Static Member Functions
    //-------------------------------------------------------------------------
  public:

    /// \brief Makes the clip_space by checking that all inputs do not violate
    ///        preconditions
    ///
    /// \pre `h.left < h.right`, `v.bottom < v.top`, `d.near < d.far`
    ///
    /// On precondition failure, this function asserts -- even in production
    ///
    /// \param h the horizontal space
    /// \param v the vertical space
    /// \param d the depth space
    /// \return the clip_space
    static constexpr auto make(const horizontal& h,
                               const vertical& v,
                               const depth& d)
      noexcept -> clip_space;

    /// \brief Makes the clip_space by checking that all inputs do not violate
    ///        preconditions
    ///
    /// \pre `left < right`, `bottom < top`, `near < far`
    ///
    /// On precondition failure, this function asserts -- even in production
    ///
    /// \param left the leftmost visible area horizontal space
    /// \param right the rightmost visible area horizontal space
    /// \param bottom the bottom-most visible area in the vertical space
    /// \param top the top-most visible area in the vertical space
    /// \param near the nearest visible area ib the depth space
    /// \param far the furthest visible area in the depth space
    /// \return the clip_space
    static constexpr auto make(real left, real right,
                               real bottom, real top,
                               real near, real far)
      noexcept -> clip_space;

    /// \brief Makes a clip_space with symmetric viewing area
    ///
    /// The symmetric area will be made from:
    /// * [-horizontal, horizontal],
    /// * [-vertical, vertical],
    /// * [-distance, distance]
    ///
    /// \pre `horizontal > 0`, `vertical > 0`, `distance > 0`
    ///
    /// On precondition failure, this function asserts -- even in production
    ///
    /// \param horizontal the horizontal space to make symmetric
    /// \param vertical the vertical space to make symmetric
    /// \param distance the distance
    /// \return the clip_space
    static constexpr auto make_symmetric(real horizontal,
                                         real vertical,
                                         real distance)
      noexcept -> clip_space;

    //-------------------------------------------------------------------------

    /// \brief Makes the clip_space by checking that all inputs do not violate
    ///        preconditions
    ///
    /// \pre `h.left < h.right`, `v.bottom < v.top`, `d.near < d.far`
    ///
    /// Unlike `make`, this function does not check for valid preconditions
    ///
    /// \param h the horizontal space
    /// \param v the vertical space
    /// \param d the depth space
    /// \return the clip_space
    static constexpr auto make_unchecked(const horizontal& h,
                                         const vertical& v,
                                         const depth& d)
      noexcept -> clip_space;

    /// \brief Makes the clip_space by checking that all inputs do not violate
    ///        preconditions
    ///
    /// \pre `left < right`, `bottom < top`, `near < far`
    ///
    /// Unlike `make`, this function does not check for valid preconditions
    ///
    /// \param left the leftmost visible area horizontal space
    /// \param right the rightmost visible area horizontal space
    /// \param bottom the bottom-most visible area in the vertical space
    /// \param top the top-most visible area in the vertical space
    /// \param near the nearest visible area ib the depth space
    /// \param far the furthest visible area in the depth space
    /// \return the clip_space
    static constexpr auto make_unchecked(real left, real right,
                                         real bottom, real top,
                                         real near, real far)
      noexcept -> clip_space;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    // Deleted default constructor
    clip_space() = delete;

    /// \brief Copies the clip_space from \p other
    ///
    /// \param other the other clip_space to copy
    constexpr clip_space(const clip_space& other) noexcept = default;

    /// \brief Copies the clip_space from \p other
    ///
    /// \param other the other clip_space to copy
    /// \return reference to `(*this)`
    constexpr auto operator=(const clip_space& other) noexcept -> clip_space& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief gets a reference to the horizontal clipping range
    ///
    /// \return reference to the horizontal
    constexpr auto get_horizontal() const noexcept -> const horizontal&;

    /// \brief gets a reference to the vertical clipping range
    ///
    /// \return reference to the vertical
    constexpr auto get_vertical() const noexcept -> const vertical&;

    /// \brief gets a reference to the depth clipping range
    ///
    /// \return reference to the depth
    constexpr auto get_depth() const noexcept -> const depth&;

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    /// \brief Constructs a clip space with the specified horizontal, vertical,
    ///        and depth clip spaces
    ///
    /// \pre h.left < h.right
    /// \pre v.bottom < v.top
    /// \pre d.near < d.far
    ///
    /// \param h the horizontal
    /// \param v the vertical
    /// \param d the depth
    constexpr clip_space(const horizontal& h,
                         const vertical& v,
                         const depth& d) noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    horizontal m_horizontal;
    vertical m_vertical;
    depth m_depth;
  };

  //===========================================================================
  // class : projection
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A class for handling projection transformations
  ///
  /// This allows for the creation of both orthographic and perspective
  /// transformation matrices.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API projection
  {
    //--------------------------------------------------------------------------
    // Static Factories : Identity
    //--------------------------------------------------------------------------
  public:

    /// \brief Creates an identity projection
    ///
    /// An identity projection does not actually contain any projection data,
    /// which results in a flat view of the (-1,1) space.
    ///
    /// \return the identity projection
    [[nodiscard]]
    static auto identity() noexcept -> projection;

    //--------------------------------------------------------------------------
    // Static Factories : Perspective
    //--------------------------------------------------------------------------
  public:

    /// \brief Projects a perspective with the given information
    ///
    /// \pre fov > 0
    /// \pre aspect_ratio > 0
    /// \pre depth.near < depth.far
    ///
    /// \param fov the field-of-view in degrees
    /// \param aspect_ratio the aspect ratio
    /// \param depth the depth of the clipping space
    /// \return A perspective projection
    [[nodiscard]]
    static auto perspective(degree fov, real aspect_ratio, clip_space::depth depth)
      noexcept -> projection;

    /// \brief Projects a perspective with the given information
    ///
    /// \pre fov > 0
    /// \pre aspect_ratio > 0
    /// \pre depth.near < depth.far
    ///
    /// \param fov the field-of-view in radians
    /// \param aspect_ratio the aspect ratio
    /// \param depth the depth of the clipping space
    /// \return A perspective projection
    [[nodiscard]]
    static auto perspective(radian fov, real aspect_ratio, clip_space::depth depth)
      noexcept -> projection;

    //--------------------------------------------------------------------------
    // Static Factories: Orthographic
    //--------------------------------------------------------------------------
  public:

    /// \brief Projects an orthographic view from the given clip space
    ///
    /// \param left the right part of the view
    /// \param right the left part of the view
    /// \param bottom the bottom part of the view
    /// \param top the top part of the view
    /// \param near the near part of the view
    /// \param far the far part of the view
    /// \return An orthographic projection
    [[nodiscard]]
    static auto orthographic(real left, real right,
                             real bottom, real top,
                             real near, real far)
      noexcept -> projection;

    /// \brief Projects an orthographic view from the given clip \p space
    ///
    /// \param space the clip space
    /// \return An orthographic projection
    [[nodiscard]]
    static auto orthographic(const clip_space& space) noexcept -> projection;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other transform to copy
    projection(const projection& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other transform to copy
    /// \return reference to (*this)
    auto operator=(const projection& other) noexcept -> projection& = default;

    //--------------------------------------------------------------------------
    // Conversion
    //--------------------------------------------------------------------------
  public:

    /// \brief Extracts the transformation matrix from this projection
    ///
    /// \param out the matrix to output
    auto extract_matrix(not_null<matrix4*> out) const noexcept -> void;

    /// \brief Converts this projection to a matrix4
    ///
    /// \return the matrix4
    [[nodiscard]]
    auto to_matrix4() const noexcept -> matrix4;

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    struct identity_data {};
    struct perspective_data
    {
      radian fov;
      real aspect_ratio;
      clip_space::depth depth;
    };
    using orthographic_data = clip_space;

    using storage_type = std::variant<
      identity_data,     // Default state: performs identity transformation
      perspective_data,  // performs perspective transformation
      orthographic_data  // performs orthographic transformation
    >;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    storage_type m_storage; ///< Either perspective or ortho data

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    /// \brief Constructs a projection with nothing but an identity matrix
    ///
    /// \note This is marked private so that users have to use the named static
    ///       factory of `projection::identity()` to explicitly request the
    ///       identity projection.
    projection() noexcept = default;

    /// \brief Constructs a projection with perspective data
    ///
    /// \param data the perspective data
    constexpr explicit projection(const perspective_data& data) noexcept;

    /// \brief Constructs a projection with orthographic data
    ///
    /// \param data the orthographic data
    constexpr explicit projection(const orthographic_data& data) noexcept;

  };

} // namespace alloy::core

//=============================================================================
// class : clip_space
//=============================================================================

//-----------------------------------------------------------------------------
// Public Static Factories
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::clip_space::make(const horizontal& h,
                                   const vertical& v,
                                   const depth& d)
  noexcept -> clip_space
{
  ALLOY_ALWAYS_ASSERT(h.left < h.right);
  ALLOY_ALWAYS_ASSERT(v.bottom < v.top);
  ALLOY_ALWAYS_ASSERT(d.near < d.far);

  return make_unchecked(h, v, d);
}

inline constexpr
auto alloy::core::clip_space::make(real left,
                                   real right,
                                   real bottom,
                                   real top,
                                   real near,
                                   real far)
  noexcept -> clip_space
{
  ALLOY_ALWAYS_ASSERT(left < right);
  ALLOY_ALWAYS_ASSERT(bottom < top);
  ALLOY_ALWAYS_ASSERT(near < far);

  return make_unchecked(left, right, bottom, top, near, far);
}

inline constexpr
auto alloy::core::clip_space::make_symmetric(real horizontal,
                                             real vertical,
                                             real distance)
  noexcept -> clip_space
{
  ALLOY_ALWAYS_ASSERT(horizontal > 0);
  ALLOY_ALWAYS_ASSERT(vertical > 0);
  ALLOY_ALWAYS_ASSERT(distance > 0);

  return make_unchecked(
    -horizontal, horizontal,
    -vertical, vertical,
    -distance, distance
  );
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::clip_space::make_unchecked(const horizontal& h,
                                             const vertical& v,
                                             const depth& d)
  noexcept -> clip_space
{
  return clip_space{h,v,d};
}

inline constexpr
auto alloy::core::clip_space::make_unchecked(real left,
                                             real right,
                                             real bottom,
                                             real top,
                                             real near,
                                             real far)
  noexcept -> alloy::core::clip_space
{
  return make_unchecked(
    horizontal{left, right},
    vertical{bottom, top},
    depth{near, far}
  );
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::clip_space::get_horizontal()
  const noexcept -> const horizontal&
{
  return m_horizontal;
}

inline constexpr
auto alloy::core::clip_space::get_vertical()
  const noexcept -> const vertical&
{
  return m_vertical;
}

inline constexpr
auto alloy::core::clip_space::get_depth()
  const noexcept -> const depth&
{
  return m_depth;
}

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

inline constexpr
alloy::core::clip_space::clip_space(const horizontal& h,
                                    const vertical& v,
                                    const depth& d)
  noexcept
  : m_horizontal{h},
    m_vertical{v},
    m_depth{d}
{

}

//==============================================================================
// class : projection
//==============================================================================

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::projection::projection(const orthographic_data& data)
  noexcept
  : m_storage{data}
{

}

inline constexpr
alloy::core::projection::projection(const perspective_data& data)
  noexcept
  : m_storage{data}
{

}

#endif /* ALLOY_CORE_MODEL_PROJECTION_HPP */
