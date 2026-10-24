// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file projection.hpp
///
/// \brief This header defines projections, which transform view space
///        into clip space
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MODEL_PROJECTION_HPP
#define ALLOY_CORE_MODEL_PROJECTION_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/math/angle/degree.hpp"
#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/precision/real.hpp"
#include "alloy/core/utilities/not_null.hpp"

#include <cstdint> // std::uint8_t
#include <variant> // std::variant

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
  // class : view_volume
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief The bounds of the volume that a projection makes visible
  ///
  /// The bounds are measured in view space, where the viewer looks down the
  /// negative z-axis. The depth bounds are distances in front of the viewer,
  /// so a depth of `{1, 10}` spans from `z = -1` to `z = -10`. A projection
  /// maps this volume onto the clip volume.
  ///
  /// The volume is comprised of three pieces:
  /// * The horizontal bounds, from left to right,
  /// * The vertical bounds, from bottom to top, and
  /// * The depth bounds, from near to far
  ///
  /// view_volume is immutable to ensure that all preconditions are always
  /// upheld after construction
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API view_volume
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    struct horizontal_bounds {
      real left;
      real right;
    };

    struct vertical_bounds {
      real bottom;
      real top;
    };

    struct depth_bounds {
      real near;
      real far;
    };

    //-------------------------------------------------------------------------
    // Static Member Functions
    //-------------------------------------------------------------------------
  public:

    /// \brief Makes the view_volume by checking that all inputs do not violate
    ///        preconditions
    ///
    /// \pre `h.left < h.right`, `v.bottom < v.top`, `d.near < d.far`
    ///
    /// On precondition failure, this function asserts -- even in production
    ///
    /// \param h the horizontal space
    /// \param v the vertical space
    /// \param d the depth space
    /// \return the view_volume
    static constexpr auto make(
      const horizontal_bounds& h,
      const vertical_bounds& v,
      const depth_bounds& d
    ) noexcept -> view_volume;

    /// \brief Makes the view_volume by checking that all inputs do not violate
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
    /// \param near the nearest visible area in the depth space
    /// \param far the furthest visible area in the depth space
    /// \return the view_volume
    static constexpr auto make(
      real left,
      real right,
      real bottom,
      real top,
      real near,
      real far
    ) noexcept -> view_volume;

    /// \brief Makes a view_volume with symmetric viewing area
    ///
    /// The symmetric area will be made from:
    /// * [-half_width, half_width],
    /// * [-half_height, half_height],
    /// * [-distance, distance]
    ///
    /// \pre `half_width > 0`, `half_height > 0`, `distance > 0`
    ///
    /// On precondition failure, this function asserts -- even in production
    ///
    /// \param half_width the distance from the center to the left and right
    /// \param half_height the distance from the center to the bottom and top
    /// \param distance the distance
    /// \return the view_volume
    static constexpr auto make_symmetric(
      real half_width,
      real half_height,
      real distance
    ) noexcept -> view_volume;

    //-------------------------------------------------------------------------

    /// \brief Makes the view_volume without checking preconditions
    ///
    /// \pre `h.left < h.right`, `v.bottom < v.top`, `d.near < d.far`
    ///
    /// Unlike `make`, this function does not check for valid preconditions
    ///
    /// \param h the horizontal space
    /// \param v the vertical space
    /// \param d the depth space
    /// \return the view_volume
    static constexpr auto make_unchecked(
      const horizontal_bounds& h,
      const vertical_bounds& v,
      const depth_bounds& d
    ) noexcept -> view_volume;

    /// \brief Makes the view_volume without checking preconditions
    ///
    /// \pre `left < right`, `bottom < top`, `near < far`
    ///
    /// Unlike `make`, this function does not check for valid preconditions
    ///
    /// \param left the leftmost visible area horizontal space
    /// \param right the rightmost visible area horizontal space
    /// \param bottom the bottom-most visible area in the vertical space
    /// \param top the top-most visible area in the vertical space
    /// \param near the nearest visible area in the depth space
    /// \param far the furthest visible area in the depth space
    /// \return the view_volume
    static constexpr auto make_unchecked(
      real left,
      real right,
      real bottom,
      real top,
      real near,
      real far
    ) noexcept -> view_volume;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    // Deleted default constructor
    view_volume() = delete;

    /// \brief Copies the view_volume from \p other
    ///
    /// \param other the other view_volume to copy
    constexpr view_volume(const view_volume& other) noexcept = default;

    /// \brief Copies the view_volume from \p other
    ///
    /// \param other the other view_volume to copy
    /// \return reference to `(*this)`
    constexpr auto operator=(const view_volume& other)
      noexcept -> view_volume& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the horizontal bounds of this volume
    ///
    /// \return reference to the horizontal bounds
    constexpr auto horizontal() const noexcept -> const horizontal_bounds&;

    /// \brief Gets the vertical bounds of this volume
    ///
    /// \return reference to the vertical bounds
    constexpr auto vertical() const noexcept -> const vertical_bounds&;

    /// \brief Gets the depth bounds of this volume
    ///
    /// \return reference to the depth bounds
    constexpr auto depth() const noexcept -> const depth_bounds&;

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
    constexpr view_volume(const horizontal_bounds& h, const vertical_bounds& v, const depth_bounds& d)
      noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    horizontal_bounds m_horizontal;
    vertical_bounds m_vertical;
    depth_bounds m_depth;
  };

  //===========================================================================
  // enum class : clip_depth
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief The range of depths that a projection maps the view volume into
  ///
  /// Graphics APIs disagree on the depth range of their clip volume, so a
  /// projection has to be built for the API that it is used with.
  /////////////////////////////////////////////////////////////////////////////
  enum class clip_depth : std::uint8_t {
    negative_one_to_one, ///< Depths in [-1, 1], as used by OpenGL
    zero_to_one,         ///< Depths in [0, 1], as used by Vulkan, D3D, Metal
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
    /// \pre `0 < fov < 180`
    /// \pre aspect_ratio > 0
    /// \pre `0 < depth.near < depth.far`
    ///
    /// \param fov the vertical field-of-view in degrees
    /// \param aspect_ratio the aspect ratio
    /// \param depth the depth bounds of the view volume
    /// \param clip the depth range to map the view volume into
    /// \return A perspective projection
    [[nodiscard]]
    static auto perspective(
      degree fov,
      real aspect_ratio,
      view_volume::depth_bounds depth,
      clip_depth clip = clip_depth::negative_one_to_one
    ) noexcept -> projection;

    /// \brief Projects a perspective with the given information
    ///
    /// \pre `0 < fov < pi`
    /// \pre aspect_ratio > 0
    /// \pre `0 < depth.near < depth.far`
    ///
    /// \param fov the vertical field-of-view in radians
    /// \param aspect_ratio the aspect ratio
    /// \param depth the depth bounds of the view volume
    /// \param clip the depth range to map the view volume into
    /// \return A perspective projection
    [[nodiscard]]
    static auto perspective(
      radian fov,
      real aspect_ratio,
      view_volume::depth_bounds depth,
      clip_depth clip = clip_depth::negative_one_to_one
    ) noexcept -> projection;

    //--------------------------------------------------------------------------
    // Static Factories: Orthographic
    //--------------------------------------------------------------------------
  public:

    /// \brief Projects an orthographic view of the given bounds
    ///
    /// \pre `left < right`, `bottom < top`, `near < far`
    ///
    /// \param left the left part of the view
    /// \param right the right part of the view
    /// \param bottom the bottom part of the view
    /// \param top the top part of the view
    /// \param near the near part of the view
    /// \param far the far part of the view
    /// \param clip the depth range to map the view volume into
    /// \return An orthographic projection
    [[nodiscard]]
    static auto orthographic(
      real left,
      real right,
      real bottom,
      real top,
      real near,
      real far,
      clip_depth clip = clip_depth::negative_one_to_one
    ) noexcept -> projection;

    /// \brief Projects an orthographic view of the view \p volume
    ///
    /// \param volume the view volume
    /// \param clip the depth range to map the view volume into
    /// \return An orthographic projection
    [[nodiscard]]
    static auto orthographic(
      const view_volume& volume,
      clip_depth clip = clip_depth::negative_one_to_one
    ) noexcept -> projection;

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
      view_volume::depth_bounds depth;
      clip_depth clip;
    };
    struct orthographic_data
    {
      view_volume volume;
      clip_depth clip;
    };

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

    //--------------------------------------------------------------------------
    // Private Static Functions
    //--------------------------------------------------------------------------
  private:

    /// \{
    /// \brief Extracts the matrix of the projection described by \p data
    ///
    /// There is one overload for each alternative of the storage, so that
    /// visiting the storage fails to compile if an alternative is unhandled.
    ///
    /// \param data the projection data
    /// \param out the matrix to output
    static auto extract_matrix_of(
      const identity_data& data,
      not_null<matrix4*> out
    ) noexcept -> void;
    static auto extract_matrix_of(
      const perspective_data& data,
      not_null<matrix4*> out
    ) noexcept -> void;
    static auto extract_matrix_of(
      const orthographic_data& data,
      not_null<matrix4*> out
    ) noexcept -> void;
    /// \}

  };

} // namespace alloy::core

//=============================================================================
// class : view_volume
//=============================================================================

//-----------------------------------------------------------------------------
// Public Static Factories
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::view_volume::make(
  const horizontal_bounds& h,
  const vertical_bounds& v,
  const depth_bounds& d
) noexcept -> view_volume
{
  ALLOY_ALWAYS_ASSERT(h.left < h.right);
  ALLOY_ALWAYS_ASSERT(v.bottom < v.top);
  ALLOY_ALWAYS_ASSERT(d.near < d.far);

  return make_unchecked(h, v, d);
}

inline constexpr
auto alloy::core::view_volume::make(
  real left,
  real right,
  real bottom,
  real top,
  real near,
  real far
) noexcept -> view_volume
{
  ALLOY_ALWAYS_ASSERT(left < right);
  ALLOY_ALWAYS_ASSERT(bottom < top);
  ALLOY_ALWAYS_ASSERT(near < far);

  return make_unchecked(left, right, bottom, top, near, far);
}

inline constexpr
auto alloy::core::view_volume::make_symmetric(
  real half_width,
  real half_height,
  real distance
) noexcept -> view_volume
{
  ALLOY_ALWAYS_ASSERT(half_width > 0);
  ALLOY_ALWAYS_ASSERT(half_height > 0);
  ALLOY_ALWAYS_ASSERT(distance > 0);

  return make_unchecked(
    -half_width, half_width,
    -half_height, half_height,
    -distance, distance
  );
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::view_volume::make_unchecked(
  const horizontal_bounds& h,
  const vertical_bounds& v,
  const depth_bounds& d
) noexcept -> view_volume
{
  return view_volume{h,v,d};
}

inline constexpr
auto alloy::core::view_volume::make_unchecked(
  real left,
  real right,
  real bottom,
  real top,
  real near,
  real far
) noexcept -> alloy::core::view_volume
{
  return make_unchecked(
    horizontal_bounds{left, right},
    vertical_bounds{bottom, top},
    depth_bounds{near, far}
  );
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::view_volume::horizontal()
  const noexcept -> const horizontal_bounds&
{
  return m_horizontal;
}

inline constexpr
auto alloy::core::view_volume::vertical()
  const noexcept -> const vertical_bounds&
{
  return m_vertical;
}

inline constexpr
auto alloy::core::view_volume::depth() const noexcept -> const depth_bounds&
{
  return m_depth;
}

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

inline constexpr
alloy::core::view_volume::view_volume(
  const horizontal_bounds& h,
  const vertical_bounds& v,
  const depth_bounds& d
) noexcept
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
alloy::core::projection::projection(const orthographic_data& data) noexcept
  : m_storage{data}
{

}

inline constexpr
alloy::core::projection::projection(const perspective_data& data) noexcept
  : m_storage{data}
{

}

#endif /* ALLOY_CORE_MODEL_PROJECTION_HPP */
