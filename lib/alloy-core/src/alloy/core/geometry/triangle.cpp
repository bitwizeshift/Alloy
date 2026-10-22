// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/geometry/triangle.hpp"

#include "alloy/core/geometry/plane.hpp"
#include "alloy/core/geometry/point/barycentric.hpp"
#include "alloy/core/geometry/point/point3.hpp"

#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/precision/real.hpp"

//==============================================================================
// class : triangle
//==============================================================================

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::core::triangle::normal() const noexcept -> vector3
{
  return (m_b - m_a).cross(m_c - m_a).normalized();
}

auto alloy::core::triangle::supporting_plane() const noexcept -> plane
{
  return plane::from_point_and_normal(m_a, normal());
}

auto alloy::core::triangle::area() const noexcept -> real
{
  return (m_b - m_a).cross(m_c - m_a).magnitude() * real{0.5};
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::triangle::barycentric_of(const point3& p)
  const noexcept -> barycentric
{
  return barycentric::from_cartesian(p, m_a, m_b, m_c);
}

auto alloy::core::triangle::nearest_point(const point3& p)
  const noexcept -> point3
{
  // This tests which of the 7 Voronoi regions of the triangle (3 vertices,
  // 3 edges, and the face) that 'p' projects into, as described in
  // "Real-Time Collision Detection" by Christer Ericson, section 5.1.5.
  const auto ab = (m_b - m_a);
  const auto ac = (m_c - m_a);

  const auto ap = (p - m_a);
  const auto d1 = ab.dot(ap);
  const auto d2 = ac.dot(ap);
  if (d1 <= real{0} && d2 <= real{0}) {
    return m_a;
  }

  const auto bp = (p - m_b);
  const auto d3 = ab.dot(bp);
  const auto d4 = ac.dot(bp);
  if (d3 >= real{0} && d4 <= d3) {
    return m_b;
  }

  const auto vc = (d1 * d4) - (d3 * d2);
  if (vc <= real{0} && d1 >= real{0} && d3 <= real{0}) {
    return m_a + ((d1 / (d1 - d3)) * ab);
  }

  const auto cp = (p - m_c);
  const auto d5 = ab.dot(cp);
  const auto d6 = ac.dot(cp);
  if (d6 >= real{0} && d5 <= d6) {
    return m_c;
  }

  const auto vb = (d5 * d2) - (d1 * d6);
  if (vb <= real{0} && d2 >= real{0} && d6 <= real{0}) {
    return m_a + ((d2 / (d2 - d6)) * ac);
  }

  const auto va = (d3 * d6) - (d5 * d4);
  if (va <= real{0} && (d4 - d3) >= real{0} && (d5 - d6) >= real{0}) {
    const auto w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
    return m_b + (w * (m_c - m_b));
  }

  const auto reciprocal = real{1} / (va + vb + vc);
  const auto v = vb * reciprocal;
  const auto w = vc * reciprocal;

  return m_a + (v * ab) + (w * ac);
}

auto alloy::core::triangle::contains(const point3& p) const noexcept -> bool
{
  return contains(p, default_tolerance);
}

auto alloy::core::triangle::contains(const point3& p, real tolerance)
  const noexcept -> bool
{
  const auto offset = (p - nearest_point(p));

  return offset.square_magnitude() <= (tolerance * tolerance);
}
