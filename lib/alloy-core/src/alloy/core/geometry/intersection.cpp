// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/geometry/intersection.hpp"

#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/geometry/capsule.hpp"
#include "alloy/core/geometry/circle.hpp"
#include "alloy/core/geometry/frustum.hpp"
#include "alloy/core/geometry/line.hpp"
#include "alloy/core/geometry/line_segment.hpp"
#include "alloy/core/geometry/oriented_box.hpp"
#include "alloy/core/geometry/plane.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/geometry/ray.hpp"
#include "alloy/core/geometry/rectangle.hpp"
#include "alloy/core/geometry/sphere.hpp"
#include "alloy/core/geometry/triangle.hpp"

#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/precision/real.hpp"

#include <algorithm> // std::min, std::max, std::swap
#include <array>     // std::array
#include <limits>    // std::numeric_limits
#include <optional>  // std::optional
#include <span>      // std::span

namespace {

  using alloy::core::axis_aligned_box;
  using alloy::core::capsule;
  using alloy::core::default_tolerance;
  using alloy::core::line_segment;
  using alloy::core::oriented_box;
  using alloy::core::plane;
  using alloy::core::point3;
  using alloy::core::real;
  using alloy::core::sphere;
  using alloy::core::triangle;
  using alloy::core::vector3;

  constexpr auto infinity = std::numeric_limits<real>::infinity();

  /// The range of distances along a line-like object that lie in a shape
  struct interval
  {
    real near;
    real far;
  };

  /// Clips the range [t_min, t_max] of the line 'origin + t * direction' to
  /// the range that lies within the box [box_min, box_max]
  auto clip_to_slabs(
    const point3& origin,
    const vector3& direction,
    interval range,
    const point3& box_min,
    const point3& box_max
  ) noexcept -> std::optional<interval>
  {
    for (auto i = 0; i < 3; ++i) {
      if (alloy::core::abs(direction[i]) <= default_tolerance) {
        // Parallel to this slab: either always inside of it, or never.
        if (origin[i] < box_min[i] || origin[i] > box_max[i]) {
          return std::nullopt;
        }
        continue;
      }
      const auto reciprocal = real{1} / direction[i];
      auto t0 = (box_min[i] - origin[i]) * reciprocal;
      auto t1 = (box_max[i] - origin[i]) * reciprocal;
      if (t0 > t1) {
        std::swap(t0, t1);
      }
      range.near = std::max(range.near, t0);
      range.far = std::min(range.far, t1);
      if (range.near > range.far) {
        return std::nullopt;
      }
    }
    return range;
  }

  /// Gets the distance from the plane 'p' to the point 'q' in units of the
  /// world, regardless of whether 'p' is normalized
  auto true_distance(const plane& p, const point3& q) noexcept -> real
  {
    return p.normalized().distance_to(q);
  }

  /// Gets the radius of the box 'b' projected onto the unit normal 'n'
  auto projected_radius(const axis_aligned_box& b, const vector3& n)
    noexcept -> real
  {
    const auto extent = (b.top_right_point() - b.bottom_left_point()) / 2;

    return extent.x() * alloy::core::abs(n.x()) +
           extent.y() * alloy::core::abs(n.y()) +
           extent.z() * alloy::core::abs(n.z());
  }

  auto center_of(const axis_aligned_box& b) noexcept -> point3
  {
    const auto extent = (b.top_right_point() - b.bottom_left_point()) / 2;

    return b.bottom_left_point() + extent;
  }

  auto intersects_point(const sphere& s, const point3& p) noexcept -> bool
  {
    const auto offset = (p - s.center());

    return offset.square_magnitude() <= (s.radius() * s.radius());
  }

  /// The extremes of a set of points projected onto an axis
  struct projection
  {
    real min;
    real max;
  };

  auto project(std::span<const point3> points, const vector3& axis)
    noexcept -> projection
  {
    auto result = projection{infinity, -infinity};
    for (const auto& p : points) {
      const auto distance = alloy::core::to_vector(p).dot(axis);
      result.min = std::min(result.min, distance);
      result.max = std::max(result.max, distance);
    }
    return result;
  }

  /// Determines whether the convex hulls of the points 'a' and of the points
  /// 'b' are separated along any of the 'axes', by the separating axis
  /// theorem. Degenerate axes, from crossing parallel edges, are skipped.
  auto is_separated(
    std::span<const vector3> axes,
    std::span<const point3> a,
    std::span<const point3> b
  ) noexcept -> bool
  {
    for (const auto& axis : axes) {
      if (axis.square_magnitude() <= (default_tolerance * default_tolerance)) {
        continue;
      }
      const auto pa = project(a, axis);
      const auto pb = project(b, axis);
      if (pa.max < pb.min || pb.max < pa.min) {
        return true;
      }
    }
    return false;
  }

  auto vertices_of(const triangle& t) noexcept -> std::array<point3, 3>
  {
    return {t.a(), t.b(), t.c()};
  }

  auto edges_of(const triangle& t) noexcept -> std::array<vector3, 3>
  {
    return {t.b() - t.a(), t.c() - t.b(), t.a() - t.c()};
  }

  /// The normal of 't', without normalizing it, so that it is zero for a
  /// degenerate triangle rather than undefined
  auto raw_normal_of(const triangle& t) noexcept -> vector3
  {
    return (t.b() - t.a()).cross(t.c() - t.a());
  }

  /// Minimizes the convex function 'fn' over the interval [0, 1] with a
  /// ternary search
  template <typename Fn>
  auto minimize_over_unit_interval(const Fn& fn) noexcept -> real
  {
    auto low = real{0};
    auto high = real{1};
    for (auto i = 0; i < 64; ++i) {
      const auto third = (high - low) / 3;
      if (fn(low + third) < fn(high - third)) {
        high = high - third;
      } else {
        low = low + third;
      }
    }
    return fn((low + high) / 2);
  }

  /// Gets the squared distance between the segment 's' and the convex
  /// 'shape'. The distance from a point on 's' to a convex shape is a convex
  /// function along 's', so its minimum is found by a ternary search.
  template <typename Shape>
  auto square_distance(const line_segment& s, const Shape& shape)
    noexcept -> real
  {
    const auto direction = s.direction();

    return minimize_over_unit_interval([&](real t) -> real {
      const auto p = s.start_point() + (direction * t);
      return (p - shape.nearest_point(p)).square_magnitude();
    });
  }

  /// Gets the part of the line 'origin + t * direction' with 't' in both
  /// 'limits' and 'range', as a segment
  auto to_segment(
    const point3& origin,
    const vector3& direction,
    interval limits,
    interval range
  ) noexcept -> std::optional<line_segment>
  {
    const auto near = std::max(limits.near, range.near);
    const auto far = std::min(limits.far, range.far);
    if (near > far) {
      return std::nullopt;
    }
    return line_segment{
      origin + (direction * near),
      origin + (direction * far)
    };
  }

  /// Gets the range of 't' along 'origin + t * direction' that the points
  /// project onto, which bounds the 't' of every point in their convex hull
  auto range_of(
    const point3& origin,
    const vector3& direction,
    std::span<const point3> points
  ) noexcept -> interval
  {
    auto result = interval{infinity, -infinity};
    for (const auto& p : points) {
      const auto t = (p - origin).dot(direction);
      result.near = std::min(result.near, t);
      result.far = std::max(result.far, t);
    }
    return result;
  }

  auto range_of(
    const point3& origin,
    const vector3& direction,
    const capsule& c
  ) noexcept -> interval
  {
    const auto& s = c.segment();
    const auto ends = std::array<point3, 2>{s.start_point(), s.end_point()};
    const auto range = range_of(origin, direction, ends);

    return interval{range.near - c.radius(), range.far + c.radius()};
  }

  /// Gets the radius of the box 'b' projected onto the unit normal 'n'
  auto projected_radius(const oriented_box& b, const vector3& n)
    noexcept -> real
  {
    const auto axes = b.axes();
    const auto& extents = b.half_extents();

    return extents.x() * alloy::core::abs(n.dot(axes[0])) +
           extents.y() * alloy::core::abs(n.dot(axes[1])) +
           extents.z() * alloy::core::abs(n.dot(axes[2]));
  }

  /// Clips the range of the line 'origin + t * direction' to the box 'b', by
  /// clipping it in the space of the box
  auto clip_to_box(
    const point3& origin,
    const vector3& direction,
    interval range,
    const oriented_box& b
  ) noexcept -> std::optional<interval>
  {
    const auto axes = b.axes();
    const auto offset = (origin - b.center());
    const auto local_origin = point3{
      offset.dot(axes[0]),
      offset.dot(axes[1]),
      offset.dot(axes[2])
    };
    const auto local_direction = vector3{
      direction.dot(axes[0]),
      direction.dot(axes[1]),
      direction.dot(axes[2])
    };
    const auto& extents = b.half_extents();

    return clip_to_slabs(
      local_origin,
      local_direction,
      range,
      point3{-extents.x(), -extents.y(), -extents.z()},
      point3{extents.x(), extents.y(), extents.z()}
    );
  }

  /// Determines whether the boxes 'a' and 'b' are separated, by the 15 axes
  /// of the separating axis theorem for boxes
  auto is_separated(const oriented_box& a, const oriented_box& b)
    noexcept -> bool
  {
    const auto aa = a.axes();
    const auto ba = b.axes();
    auto axes = std::array<vector3, 15>{
      aa[0], aa[1], aa[2],
      ba[0], ba[1], ba[2]
    };
    auto it = axes.begin() + 6;
    for (const auto& u : aa) {
      for (const auto& v : ba) {
        *it++ = u.cross(v);
      }
    }
    return is_separated(axes, a.corners(), b.corners());
  }

  /// Determines whether the triangle 't' and the box 'b' are separated, by
  /// the 13 axes of the separating axis theorem for a triangle and a box
  auto is_separated(const triangle& t, const oriented_box& b) noexcept -> bool
  {
    const auto box_axes = b.axes();
    auto axes = std::array<vector3, 13>{
      box_axes[0], box_axes[1], box_axes[2],
      raw_normal_of(t)
    };
    auto it = axes.begin() + 4;
    for (const auto& e : edges_of(t)) {
      for (const auto& u : box_axes) {
        *it++ = e.cross(u);
      }
    }
    return is_separated(axes, vertices_of(t), b.corners());
  }

  /// Determines whether the segment 's' touches the triangle 't'
  auto touches(const line_segment& s, const triangle& t) noexcept -> bool
  {
    const auto surface = t.supporting_plane();
    const auto d0 = surface.distance_to(s.start_point());
    const auto d1 = surface.distance_to(s.end_point());

    if ((d0 * d1) > real{0}) {
      return false;
    }
    if (alloy::core::abs(d0 - d1) <= default_tolerance) {
      // The segment lies on the plane of the triangle.
      return square_distance(s, t) <= (default_tolerance * default_tolerance);
    }
    const auto crossing = s.start_point() + (s.direction() * (d0 / (d0 - d1)));

    return t.contains(crossing);
  }

} // namespace

//------------------------------------------------------------------------------
// plane
//------------------------------------------------------------------------------

auto alloy::core::intersection_traits<
  alloy::core::plane,
  alloy::core::plane
>::intersects(const plane& a, const plane& b) noexcept -> bool
{
  const auto na = a.normalized();
  const auto nb = b.normalized();
  const auto cross = na.normal().cross(nb.normal());

  if (cross.square_magnitude() > (default_tolerance * default_tolerance)) {
    return true;
  }
  // Parallel planes only intersect when they are the same plane, which is
  // when the same offset is reached along the same normal.
  const auto same_facing = na.normal().dot(nb.normal()) > real{0};
  const auto offset = same_facing ? (na.d() - nb.d()) : (na.d() + nb.d());

  return abs(offset) <= default_tolerance;
}

auto alloy::core::intersection_traits<
  alloy::core::plane,
  alloy::core::line
>::intersects(const plane& a, const line& b) noexcept -> bool
{
  const auto n = a.normalized();
  if (abs(n.normal().dot(b.direction())) > default_tolerance) {
    return true;
  }
  return abs(n.distance_to(b.origin())) <= default_tolerance;
}

auto alloy::core::intersection_traits<
  alloy::core::plane,
  alloy::core::ray
>::intersects(const plane& a, const ray& b) noexcept -> bool
{
  const auto n = a.normalized();
  const auto distance = n.distance_to(b.origin());
  if (abs(distance) <= default_tolerance) {
    return true;
  }
  // The ray moves toward the plane when it travels against the side of the
  // plane that its origin is on.
  return (n.normal().dot(b.direction()) * distance) < real{0};
}

auto alloy::core::intersection_traits<
  alloy::core::plane,
  alloy::core::line_segment
>::intersects(const plane& a, const line_segment& b) noexcept -> bool
{
  const auto d0 = a.distance_to(b.start_point());
  const auto d1 = a.distance_to(b.end_point());

  return (d0 * d1) <= real{0};
}

auto alloy::core::intersection_traits<
  alloy::core::plane,
  alloy::core::triangle
>::intersects(const plane& a, const triangle& b) noexcept -> bool
{
  const auto d0 = a.distance_to(b.a());
  const auto d1 = a.distance_to(b.b());
  const auto d2 = a.distance_to(b.c());

  return std::min({d0, d1, d2}) <= real{0} && std::max({d0, d1, d2}) >= real{0};
}

auto alloy::core::intersection_traits<
  alloy::core::plane,
  alloy::core::sphere
>::intersects(const plane& a, const sphere& b) noexcept -> bool
{
  return abs(true_distance(a, b.center())) <= b.radius();
}

auto alloy::core::intersection_traits<
  alloy::core::plane,
  alloy::core::capsule
>::intersects(const plane& a, const capsule& b) noexcept -> bool
{
  const auto n = a.normalized();
  const auto d0 = n.distance_to(b.segment().start_point());
  const auto d1 = n.distance_to(b.segment().end_point());

  if ((d0 * d1) <= real{0}) {
    return true;
  }
  return std::min(abs(d0), abs(d1)) <= b.radius();
}

auto alloy::core::intersection_traits<
  alloy::core::plane,
  alloy::core::axis_aligned_box
>::intersects(const plane& a, const axis_aligned_box& b) noexcept -> bool
{
  const auto n = a.normalized();

  return abs(n.distance_to(center_of(b))) <= projected_radius(b, n.normal());
}

auto alloy::core::intersection_traits<
  alloy::core::plane,
  alloy::core::oriented_box
>::intersects(const plane& a, const oriented_box& b) noexcept -> bool
{
  const auto n = a.normalized();

  return abs(n.distance_to(b.center())) <= projected_radius(b, n.normal());
}

//------------------------------------------------------------------------------
// line
//------------------------------------------------------------------------------

auto alloy::core::intersection_traits<
  alloy::core::line,
  alloy::core::triangle
>::intersects(const line& a, const triangle& b) noexcept -> bool
{
  const auto vertices = vertices_of(b);
  const auto segment = to_segment(
    a.origin(),
    a.direction(),
    interval{-infinity, infinity},
    range_of(a.origin(), a.direction(), vertices)
  );

  return segment.has_value() && core::intersects(*segment, b);
}

auto alloy::core::intersection_traits<
  alloy::core::line,
  alloy::core::sphere
>::intersects(const line& a, const sphere& b) noexcept -> bool
{
  const auto t = (b.center() - a.origin()).dot(a.direction());

  return intersects_point(b, a.point_at_distance(t));
}

auto alloy::core::intersection_traits<
  alloy::core::line,
  alloy::core::capsule
>::intersects(const line& a, const capsule& b) noexcept -> bool
{
  const auto segment = to_segment(
    a.origin(),
    a.direction(),
    interval{-infinity, infinity},
    range_of(a.origin(), a.direction(), b)
  );

  return segment.has_value() && core::intersects(*segment, b);
}

auto alloy::core::intersection_traits<
  alloy::core::line,
  alloy::core::axis_aligned_box
>::intersects(const line& a, const axis_aligned_box& b) noexcept -> bool
{
  return clip_to_slabs(
    a.origin(),
    a.direction(),
    interval{-infinity, infinity},
    b.bottom_left_point(),
    b.top_right_point()
  ).has_value();
}

auto alloy::core::intersection_traits<
  alloy::core::line,
  alloy::core::oriented_box
>::intersects(const line& a, const oriented_box& b) noexcept -> bool
{
  return clip_to_box(
    a.origin(),
    a.direction(),
    interval{-infinity, infinity},
    b
  ).has_value();
}

//------------------------------------------------------------------------------
// ray
//------------------------------------------------------------------------------

auto alloy::core::intersection_traits<
  alloy::core::ray,
  alloy::core::triangle
>::intersects(const ray& a, const triangle& b) noexcept -> bool
{
  const auto vertices = vertices_of(b);
  const auto segment = to_segment(
    a.origin(),
    a.direction(),
    interval{real{0}, infinity},
    range_of(a.origin(), a.direction(), vertices)
  );

  return segment.has_value() && core::intersects(*segment, b);
}

auto alloy::core::intersection_traits<
  alloy::core::ray,
  alloy::core::sphere
>::intersects(const ray& a, const sphere& b) noexcept -> bool
{
  const auto t = (b.center() - a.origin()).dot(a.direction());

  return intersects_point(b, a.point_at_distance(std::max(t, real{0})));
}

auto alloy::core::intersection_traits<
  alloy::core::ray,
  alloy::core::capsule
>::intersects(const ray& a, const capsule& b) noexcept -> bool
{
  const auto segment = to_segment(
    a.origin(),
    a.direction(),
    interval{real{0}, infinity},
    range_of(a.origin(), a.direction(), b)
  );

  return segment.has_value() && core::intersects(*segment, b);
}

auto alloy::core::intersection_traits<
  alloy::core::ray,
  alloy::core::axis_aligned_box
>::intersects(const ray& a, const axis_aligned_box& b) noexcept -> bool
{
  return clip_to_slabs(
    a.origin(),
    a.direction(),
    interval{real{0}, infinity},
    b.bottom_left_point(),
    b.top_right_point()
  ).has_value();
}

auto alloy::core::intersection_traits<
  alloy::core::ray,
  alloy::core::oriented_box
>::intersects(const ray& a, const oriented_box& b) noexcept -> bool
{
  return clip_to_box(
    a.origin(),
    a.direction(),
    interval{real{0}, infinity},
    b
  ).has_value();
}

//------------------------------------------------------------------------------
// line_segment
//------------------------------------------------------------------------------

auto alloy::core::intersection_traits<
  alloy::core::line_segment,
  alloy::core::triangle
>::intersects(const line_segment& a, const triangle& b) noexcept -> bool
{
  return touches(a, b);
}

auto alloy::core::intersection_traits<
  alloy::core::line_segment,
  alloy::core::sphere
>::intersects(const line_segment& a, const sphere& b) noexcept -> bool
{
  return intersects_point(b, a.nearest_point(b.center()));
}

auto alloy::core::intersection_traits<
  alloy::core::line_segment,
  alloy::core::capsule
>::intersects(const line_segment& a, const capsule& b) noexcept -> bool
{
  return square_distance(b.segment(), a) <= (b.radius() * b.radius());
}

auto alloy::core::intersection_traits<
  alloy::core::line_segment,
  alloy::core::axis_aligned_box
>::intersects(const line_segment& a, const axis_aligned_box& b) noexcept -> bool
{
  return clip_to_slabs(
    a.start_point(),
    a.direction(),
    interval{real{0}, real{1}},
    b.bottom_left_point(),
    b.top_right_point()
  ).has_value();
}

auto alloy::core::intersection_traits<
  alloy::core::line_segment,
  alloy::core::oriented_box
>::intersects(const line_segment& a, const oriented_box& b) noexcept -> bool
{
  return clip_to_box(
    a.start_point(),
    a.direction(),
    interval{real{0}, real{1}},
    b
  ).has_value();
}

//------------------------------------------------------------------------------
// triangle
//------------------------------------------------------------------------------

auto alloy::core::intersection_traits<
  alloy::core::triangle,
  alloy::core::triangle
>::intersects(const triangle& a, const triangle& b) noexcept -> bool
{
  const auto ea = edges_of(a);
  const auto eb = edges_of(b);
  const auto na = raw_normal_of(a);
  const auto nb = raw_normal_of(b);

  // The normals and the crossed edges separate triangles that are not
  // coplanar. The edges crossed with the normals separate coplanar ones.
  auto axes = std::array<vector3, 17>{na, nb};
  auto it = axes.begin() + 2;
  for (const auto& u : ea) {
    for (const auto& v : eb) {
      *it++ = u.cross(v);
    }
  }
  for (const auto& u : ea) {
    *it++ = na.cross(u);
  }
  for (const auto& v : eb) {
    *it++ = nb.cross(v);
  }
  return !is_separated(axes, vertices_of(a), vertices_of(b));
}

auto alloy::core::intersection_traits<
  alloy::core::triangle,
  alloy::core::sphere
>::intersects(const triangle& a, const sphere& b) noexcept -> bool
{
  return intersects_point(b, a.nearest_point(b.center()));
}

auto alloy::core::intersection_traits<
  alloy::core::triangle,
  alloy::core::capsule
>::intersects(const triangle& a, const capsule& b) noexcept -> bool
{
  if (touches(b.segment(), a)) {
    return true;
  }
  return square_distance(b.segment(), a) <= (b.radius() * b.radius());
}

auto alloy::core::intersection_traits<
  alloy::core::triangle,
  alloy::core::axis_aligned_box
>::intersects(const triangle& a, const axis_aligned_box& b) noexcept -> bool
{
  return !is_separated(a, oriented_box::from_axis_aligned_box(b));
}

auto alloy::core::intersection_traits<
  alloy::core::triangle,
  alloy::core::oriented_box
>::intersects(const triangle& a, const oriented_box& b) noexcept -> bool
{
  return !is_separated(a, b);
}

//------------------------------------------------------------------------------
// sphere
//------------------------------------------------------------------------------

auto alloy::core::intersection_traits<
  alloy::core::sphere,
  alloy::core::sphere
>::intersects(const sphere& a, const sphere& b) noexcept -> bool
{
  const auto offset = (b.center() - a.center());
  const auto radii = a.radius() + b.radius();

  return offset.square_magnitude() <= (radii * radii);
}

auto alloy::core::intersection_traits<
  alloy::core::sphere,
  alloy::core::capsule
>::intersects(const sphere& a, const capsule& b) noexcept -> bool
{
  const auto offset = (a.center() - b.segment().nearest_point(a.center()));
  const auto radii = a.radius() + b.radius();

  return offset.square_magnitude() <= (radii * radii);
}

auto alloy::core::intersection_traits<
  alloy::core::sphere,
  alloy::core::axis_aligned_box
>::intersects(const sphere& a, const axis_aligned_box& b) noexcept -> bool
{
  return b.square_distance_to(a.center()) <= (a.radius() * a.radius());
}

auto alloy::core::intersection_traits<
  alloy::core::sphere,
  alloy::core::oriented_box
>::intersects(const sphere& a, const oriented_box& b) noexcept -> bool
{
  return intersects_point(a, b.nearest_point(a.center()));
}

//------------------------------------------------------------------------------
// capsule
//------------------------------------------------------------------------------

auto alloy::core::intersection_traits<
  alloy::core::capsule,
  alloy::core::capsule
>::intersects(const capsule& a, const capsule& b) noexcept -> bool
{
  const auto radii = a.radius() + b.radius();

  return square_distance(a.segment(), b.segment()) <= (radii * radii);
}

auto alloy::core::intersection_traits<
  alloy::core::capsule,
  alloy::core::axis_aligned_box
>::intersects(const capsule& a, const axis_aligned_box& b) noexcept -> bool
{
  return square_distance(a.segment(), b) <= (a.radius() * a.radius());
}

auto alloy::core::intersection_traits<
  alloy::core::capsule,
  alloy::core::oriented_box
>::intersects(const capsule& a, const oriented_box& b) noexcept -> bool
{
  return square_distance(a.segment(), b) <= (a.radius() * a.radius());
}

//------------------------------------------------------------------------------
// axis_aligned_box
//------------------------------------------------------------------------------

auto alloy::core::intersection_traits<
  alloy::core::axis_aligned_box,
  alloy::core::oriented_box
>::intersects(const axis_aligned_box& a, const oriented_box& b) noexcept -> bool
{
  return !is_separated(oriented_box::from_axis_aligned_box(a), b);
}

//------------------------------------------------------------------------------
// oriented_box
//------------------------------------------------------------------------------

auto alloy::core::intersection_traits<
  alloy::core::oriented_box,
  alloy::core::oriented_box
>::intersects(const oriented_box& a, const oriented_box& b) noexcept -> bool
{
  return !is_separated(a, b);
}

//------------------------------------------------------------------------------
// frustum
//------------------------------------------------------------------------------

auto alloy::core::intersection_traits<
  alloy::core::frustum,
  alloy::core::sphere
>::intersects(const frustum& a, const sphere& b) noexcept -> bool
{
  for (const auto& p : a.planes()) {
    if (true_distance(p, b.center()) < -b.radius()) {
      return false;
    }
  }
  return true;
}

auto alloy::core::intersection_traits<
  alloy::core::frustum,
  alloy::core::axis_aligned_box
>::intersects(const frustum& a, const axis_aligned_box& b) noexcept -> bool
{
  const auto center = center_of(b);
  for (const auto& p : a.planes()) {
    const auto n = p.normalized();
    if (n.distance_to(center) < -projected_radius(b, n.normal())) {
      return false;
    }
  }
  return true;
}

auto alloy::core::intersection_traits<
  alloy::core::frustum,
  alloy::core::oriented_box
>::intersects(const frustum& a, const oriented_box& b) noexcept -> bool
{
  for (const auto& p : a.planes()) {
    const auto n = p.normalized();
    if (n.distance_to(b.center()) < -projected_radius(b, n.normal())) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// circle
//------------------------------------------------------------------------------

auto alloy::core::intersection_traits<
  alloy::core::circle,
  alloy::core::circle
>::intersects(const circle& a, const circle& b) noexcept -> bool
{
  const auto offset = (b.center() - a.center());
  const auto radii = a.radius() + b.radius();

  return offset.square_magnitude() <= (radii * radii);
}

auto alloy::core::intersection_traits<
  alloy::core::circle,
  alloy::core::rectangle
>::intersects(const circle& a, const rectangle& b) noexcept -> bool
{
  return b.square_distance_to(a.center()) <= (a.radius() * a.radius());
}
