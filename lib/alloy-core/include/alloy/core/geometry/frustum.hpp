// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file frustum.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_GEOMETRY_FRUSTUM_HPP
#define ALLOY_CORE_GEOMETRY_FRUSTUM_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/geometry/plane.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"

#include <span>

#if defined(near)
# undef near
#endif
#if defined(far)
# undef far
#endif

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A 3-dimensional box constructed from 6 intersecting planes.
  ///
  /// Frustums are conventionally used to represent the viewing area of a scene
  /// camera, where the planes represent the clipping boundaries.
  ///
  /// Frustums in alloy are largely buckets of state that act as generic
  /// geometric objects.
  ///
  /// The normal of every plane faces into the frustum, so a point is inside
  /// of the frustum when it is on the side that every normal faces.
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API frustum
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a frustum from the clip-space transformation
    ///        \p view_projection
    ///
    /// \p view_projection maps points to clip space by multiplying them as
    /// column vectors, with a clip-space depth between `-w` and `w`, as the
    /// matrices of `projection` do. The planes of the frustum are normalized.
    ///
    /// \param view_projection the transformation into clip space
    /// \return the frustum of the points that are not clipped
    static auto from_matrix(const matrix4& view_projection) noexcept -> frustum;

    /// \brief Constructs a viewing frustum from 6 planes
    ///
    /// \pre the normals of each pair of opposite planes face toward each
    ///      other
    ///
    /// \param left the left plane of the frustum
    /// \param right the right plane of the frustum
    /// \param top the top plane of the frustum
    /// \param bottom the bottom plane of the frustum
    /// \param near the near plane of the frustum
    /// \param far the far plane of the frustum
    /// \return the frustum
    static auto make(
      const plane& left,
      const plane& right,
      const plane& top,
      const plane& bottom,
      const plane& near,
      const plane& far
    ) noexcept -> frustum;

    /// \brief Constructs a viewing frustum from 6 planes without doing any
    ///        validation
    ///
    /// \param left the left plane of the frustum
    /// \param right the right plane of the frustum
    /// \param top the top plane of the frustum
    /// \param bottom the bottom plane of the frustum
    /// \param near the near plane of the frustum
    /// \param far the far plane of the frustum
    /// \return the frustum
    static constexpr auto make_unchecked(
      const plane& left,
      const plane& right,
      const plane& top,
      const plane& bottom,
      const plane& near,
      const plane& far
    ) noexcept -> frustum;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    frustum() = default;

    frustum(const frustum& other) = default;

    auto operator=(const frustum& other) -> frustum& = default;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the specified plane of this frustum
    ///
    /// \return the left plane
    [[nodiscard]] constexpr auto left() const noexcept -> const plane&;
    [[nodiscard]] constexpr auto right() const noexcept -> const plane&;
    [[nodiscard]] constexpr auto top() const noexcept -> const plane&;
    [[nodiscard]] constexpr auto bottom() const noexcept -> const plane&;
    [[nodiscard]] constexpr auto near() const noexcept -> const plane&;
    [[nodiscard]] constexpr auto far() const noexcept -> const plane&;
    /// \}

    /// \brief Returns the underlying planes in this frustum
    ///
    /// The planes are ordered:
    ///
    /// 0. left
    /// 1. right
    /// 2. top
    /// 3. bottom
    /// 4. near
    /// 5. far
    ///
    /// \return the 6 underlying planes
    [[nodiscard]]
    constexpr auto planes() const noexcept -> std::span<const plane, 6u>;

    //--------------------------------------------------------------------------
    // Contains
    //--------------------------------------------------------------------------
  public:

    /// \brief Checks whether a given point \p p is contained within this
    ///        frustum
    ///
    /// \param p the point to check
    /// \return `true` if the point is contained in this frustum
    [[nodiscard]]
    auto contains(const point3& p) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    plane m_planes[6];

    constexpr frustum(
      const plane& left,
      const plane& right,
      const plane& top,
      const plane& bottom,
      const plane& near,
      const plane& far
    ) noexcept;
  };

} // namespace alloy::core

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::frustum::make_unchecked(
  const plane& left,
  const plane& right,
  const plane& top,
  const plane& bottom,
  const plane& near,
  const plane& far
) noexcept -> frustum
{
  return frustum{left, right, top, bottom, near, far};
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::frustum::left() const noexcept -> const plane&
{
  return m_planes[0];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::frustum::right() const noexcept -> const plane&
{
  return m_planes[1];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::frustum::top() const noexcept -> const plane&
{
  return m_planes[2];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::frustum::bottom() const noexcept -> const plane&
{
  return m_planes[3];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::frustum::near() const noexcept -> const plane&
{
  return m_planes[4];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::frustum::far() const noexcept -> const plane&
{
  return m_planes[5];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::frustum::planes() const noexcept -> std::span<const plane, 6u>
{
  return std::span<const plane, 6u>{m_planes};
}

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::frustum::frustum(
  const plane& left,
  const plane& right,
  const plane& top,
  const plane& bottom,
  const plane& near,
  const plane& far
) noexcept
  : m_planes{left, right, top, bottom, near, far}
{

}

#endif /* ALLOY_CORE_GEOMETRY_FRUSTUM_HPP */
