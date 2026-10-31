// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/******************************************************************************
 * \file raycast.hpp
 *
 * \brief This header defines raycasts, which find how far a ray travels
 *        before it reaches a geometric object
 ******************************************************************************/

#ifndef ALLOY_CORE_GEOMETRY_RAYCAST_HPP
#define ALLOY_CORE_GEOMETRY_RAYCAST_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/geometry/capsule.hpp"
#include "alloy/core/geometry/oriented_box.hpp"
#include "alloy/core/geometry/plane.hpp"
#include "alloy/core/geometry/ray.hpp"
#include "alloy/core/geometry/sphere.hpp"
#include "alloy/core/geometry/triangle.hpp"
#include "alloy/core/precision.hpp"

#include <concepts> // std::same_as
#include <optional> // std::optional

namespace alloy::core {

  //============================================================================
  // struct : raycast_traits
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A trait that casts rays against objects of the type \p Shape
  ///
  /// Specializations provide the static member function
  /// `raycast(const ray&, const Shape&) noexcept -> std::optional<real>`,
  /// which gets the distance that the ray travels before it first reaches the
  /// object, or `std::nullopt` if it never does. A ray that starts inside of
  /// or on the object reaches it at a distance of 0.
  ///
  /// The specializations in this header reach planes and triangles from
  /// either side, and accept planes that are not normalized. A ray that lies
  /// in the plane of a triangle does not reach it.
  ///
  /// \tparam Shape the type of the object
  //////////////////////////////////////////////////////////////////////////////
  template <typename Shape>
  struct raycast_traits {};

  template <>
  struct ALLOY_CORE_API raycast_traits<plane>
  {
    static auto raycast(const ray& r, const plane& p)
      noexcept -> std::optional<real>;
  };

  template <>
  struct ALLOY_CORE_API raycast_traits<triangle>
  {
    static auto raycast(const ray& r, const triangle& t)
      noexcept -> std::optional<real>;
  };

  template <>
  struct ALLOY_CORE_API raycast_traits<sphere>
  {
    static auto raycast(const ray& r, const sphere& s)
      noexcept -> std::optional<real>;
  };

  template <>
  struct ALLOY_CORE_API raycast_traits<capsule>
  {
    static auto raycast(const ray& r, const capsule& c)
      noexcept -> std::optional<real>;
  };

  template <>
  struct ALLOY_CORE_API raycast_traits<axis_aligned_box>
  {
    static auto raycast(const ray& r, const axis_aligned_box& b)
      noexcept -> std::optional<real>;
  };

  template <>
  struct ALLOY_CORE_API raycast_traits<oriented_box>
  {
    static auto raycast(const ray& r, const oriented_box& b)
      noexcept -> std::optional<real>;
  };

  //============================================================================
  // concept : raycastable
  //============================================================================

  /// \brief Determines whether rays can be cast against objects of the type
  ///        \p Shape
  ///
  /// \tparam Shape the type of the object
  template <typename Shape>
  concept raycastable = requires(const ray& r, const Shape& s) {
    {
      raycast_traits<Shape>::raycast(r, s)
    } -> std::same_as<std::optional<real>>;
  };

  //============================================================================
  // functions : raycast
  //============================================================================

  /// \brief Gets the distance that the ray \p r travels before it first
  ///        reaches \p shape
  ///
  /// \param r the ray to cast
  /// \param shape the object to cast the ray against
  /// \return the distance along \p r, or `std::nullopt` if \p r never reaches
  ///         \p shape
  template <typename Shape>
    requires raycastable<Shape>
  constexpr auto raycast(const ray& r, const Shape& shape)
    noexcept -> std::optional<real>;

} // namespace alloy::core

//==============================================================================
// functions : raycast
//==============================================================================

template <typename Shape>
  requires alloy::core::raycastable<Shape>
inline constexpr
auto alloy::core::raycast(const ray& r, const Shape& shape)
  noexcept -> std::optional<real>
{
  return raycast_traits<Shape>::raycast(r, shape);
}

#endif /* ALLOY_CORE_GEOMETRY_RAYCAST_HPP */
