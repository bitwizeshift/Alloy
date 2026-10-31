// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/geometry/raycast.hpp"

#include "slab_clipping.hpp"
#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/geometry/capsule.hpp"
#include "alloy/core/geometry/oriented_box.hpp"
#include "alloy/core/geometry/plane.hpp"
#include "alloy/core/geometry/ray.hpp"
#include "alloy/core/geometry/sphere.hpp"
#include "alloy/core/geometry/triangle.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector3.hpp"

#include <algorithm> // std::min, std::max
#include <limits>    // std::numeric_limits
#include <optional>  // std::optional

namespace {

  constexpr auto infinity = std::numeric_limits<float>::infinity();

  /// Gets the lesser of two optional distances, where a missing distance is
  /// never reached
  auto nearest_of(std::optional<float> a, std::optional<float> b)
    noexcept -> std::optional<float>
  {
    if (!a.has_value()) {
      return b;
    }
    if (!b.has_value()) {
      return a;
    }
    return std::min(*a, *b);
  }

} // namespace

//==============================================================================
// struct : raycast_traits
//==============================================================================

auto alloy::core::raycast_traits<alloy::core::plane>::raycast(
  const ray& r,
  const plane& p
) noexcept -> std::optional<float>
{
  const auto n = p.normalized();
  const auto distance = n.distance_to(r.origin());
  if (abs(distance) <= default_tolerance) {
    return 0.0f;
  }
  const auto approach = n.normal().dot(r.direction());
  if (abs(approach) <= default_tolerance) {
    return std::nullopt;
  }
  const auto t = -distance / approach;
  if (t < 0.0f) {
    return std::nullopt;
  }
  return t;
}

auto alloy::core::raycast_traits<alloy::core::triangle>::raycast(
  const ray& r,
  const triangle& t
) noexcept -> std::optional<float>
{
  // The Möller-Trumbore algorithm, which solves for the distance along the
  // ray and the barycentric coordinates of the hit at once.
  const auto e1 = (t.b() - t.a());
  const auto e2 = (t.c() - t.a());
  const auto p = r.direction().cross(e2);
  const auto determinant = e1.dot(p);
  if (abs(determinant) <= default_tolerance) {
    return std::nullopt;
  }
  const auto reciprocal = 1.0f / determinant;

  const auto s = (r.origin() - t.a());
  const auto u = s.dot(p) * reciprocal;
  if (u < 0.0f || u > 1.0f) {
    return std::nullopt;
  }
  const auto q = s.cross(e1);
  const auto v = r.direction().dot(q) * reciprocal;
  if (v < 0.0f || (u + v) > 1.0f) {
    return std::nullopt;
  }
  const auto distance = e2.dot(q) * reciprocal;
  if (distance < 0.0f) {
    return std::nullopt;
  }
  return distance;
}

auto alloy::core::raycast_traits<alloy::core::sphere>::raycast(
  const ray& r,
  const sphere& s
) noexcept -> std::optional<float>
{
  // Solves '|offset + t * direction|^2 = radius^2' for the lesser 't'.
  const auto offset = (r.origin() - s.center());
  const auto b = offset.dot(r.direction());
  const auto c = offset.square_magnitude() - (s.radius() * s.radius());
  if (c <= 0.0f) {
    return 0.0f;
  }
  if (b > 0.0f) {
    return std::nullopt;
  }
  const auto discriminant = (b * b) - c;
  if (discriminant < 0.0f) {
    return std::nullopt;
  }
  return std::max(-b - sqrt(discriminant), 0.0f);
}

auto alloy::core::raycast_traits<alloy::core::capsule>::raycast(
  const ray& r,
  const capsule& c
) noexcept -> std::optional<float>
{
  const auto& segment = c.segment();
  const auto radius = c.radius();
  const auto offset = (r.origin() - segment.nearest_point(r.origin()));
  if (offset.square_magnitude() <= (radius * radius)) {
    return 0.0f;
  }

  // The capsule is the union of the spheres at its ends, and the cylinder
  // between them.
  auto result = nearest_of(
    raycast_traits<sphere>::raycast(r, sphere{segment.start_point(), radius}),
    raycast_traits<sphere>::raycast(r, sphere{segment.end_point(), radius})
  );

  const auto axis = segment.direction();
  const auto length = axis.magnitude();
  if (length <= default_tolerance) {
    return result;
  }
  const auto unit_axis = axis / length;

  // Solves '|m + t * d|^2 = radius^2' for the parts 'm' of the offset to the
  // ray and 'd' of the direction of the ray that are perpendicular to the
  // axis.
  const auto start_offset = (r.origin() - segment.start_point());
  const auto m = start_offset - (unit_axis * start_offset.dot(unit_axis));
  const auto d = r.direction() - (unit_axis * r.direction().dot(unit_axis));
  const auto a = d.square_magnitude();
  if (a <= (default_tolerance * default_tolerance)) {
    return result;
  }
  const auto b = m.dot(d);
  const auto excess = m.square_magnitude() - (radius * radius);
  const auto discriminant = (b * b) - (a * excess);
  if (discriminant < 0.0f) {
    return result;
  }
  const auto t = (-b - sqrt(discriminant)) / a;
  const auto along_axis = (start_offset + (r.direction() * t)).dot(unit_axis);
  if (t >= 0.0f && along_axis >= 0.0f && along_axis <= length) {
    result = nearest_of(result, t);
  }
  return result;
}

auto alloy::core::raycast_traits<alloy::core::axis_aligned_box>::raycast(
  const ray& r,
  const axis_aligned_box& b
) noexcept -> std::optional<float>
{
  const auto range = detail::clip_to_slabs(
    r.origin(),
    r.direction(),
    detail::interval{0.0f, infinity},
    b.bottom_left_point(),
    b.top_right_point()
  );
  if (!range.has_value()) {
    return std::nullopt;
  }
  return range->near;
}

auto alloy::core::raycast_traits<alloy::core::oriented_box>::raycast(
  const ray& r,
  const oriented_box& b
) noexcept -> std::optional<float>
{
  const auto range = detail::clip_to_box(
    r.origin(),
    r.direction(),
    detail::interval{0.0f, infinity},
    b
  );
  if (!range.has_value()) {
    return std::nullopt;
  }
  return range->near;
}
