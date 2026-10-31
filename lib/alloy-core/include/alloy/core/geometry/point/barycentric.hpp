// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file barycentric.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_GEOMETRY_POINT_BARYCENTRIC_HPP
#define ALLOY_CORE_GEOMETRY_POINT_BARYCENTRIC_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/vector/vector3.hpp"

#include <cstddef> // std::size_t

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Represents a point in barycentric coordinate system
  //////////////////////////////////////////////////////////////////////////////
  class barycentric
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Computes the barycentric coordinate given a world point \p p and
    ///        a triangle made up from points \p t1, \p t2, and \p t3
    ///
    /// \param p the world point
    /// \param t1 the first point in the triangle
    /// \param t2 the second point in the triangle
    /// \param t3 the third point in the triangle
    /// \return the barycentric coordinate
    static auto from_cartesian(
      const point3& p,
      const point3& t1,
      const point3& t2,
      const point3& t3
    ) noexcept -> barycentric;

    /// \brief Computes the barycentric coordinate given a world point \p p and
    ///        a triangle made up from points \p t1, \p t2, and \p t3
    ///
    /// \param p the world point
    /// \param t1 the first point in the triangle
    /// \param t2 the second point in the triangle
    /// \param t3 the third point in the triangle
    /// \return the barycentric coordinate
    static auto from_cartesian(
      const point2& p,
      const point2& t1,
      const point2& t2,
      const point2& t3
    ) noexcept -> barycentric;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------

    /// \brief Default constructs this barycentric point with all components 0
    constexpr barycentric() noexcept;

    /// \brief Constructs a barycentric point from the 3 components
    ///
    /// \param u the u component (alpha)
    /// \param v the v component (beta0
    /// \param w the w component (gamma)
    constexpr barycentric(float u, float v, float w) noexcept;

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other barycentric to copy
    barycentric(const barycentric& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other barycentric to copy
    /// \return reference to `(*this)`
    auto operator=(const barycentric& other) noexcept -> barycentric& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the u component
    ///
    /// \return the u component
    constexpr auto u() noexcept -> float&;
    constexpr auto u() const noexcept -> float;
    /// \}

    /// \{
    /// \brief Gets the v component
    ///
    /// \return the v component
    constexpr auto v() noexcept -> float&;
    constexpr auto v() const noexcept -> float;
    /// \}

    /// \{
    /// \brief Gets the w component
    ///
    /// \return the w component
    constexpr auto w() noexcept -> float&;
    constexpr auto w() const noexcept -> float;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    [[nodiscard]]
    constexpr auto data() const noexcept -> const float*;

    /// \brief Gets the number of components in this barycentric coordinate
    ///
    /// \return 3u
    [[nodiscard]]
    constexpr auto size() const noexcept -> std::size_t;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    float m_data[3];
  };

} // namespace alloy::core

inline constexpr
alloy::core::barycentric::barycentric() noexcept
  : m_data{0, 0, 0}
{

}

inline constexpr
alloy::core::barycentric::barycentric(float u, float v, float w) noexcept
  : m_data{u, v, w}
{

}

inline
auto alloy::core::barycentric::from_cartesian(
  const point3& p,
  const point3& t0,
  const point3& t1,
  const point3& t2
) noexcept -> barycentric
{
    const auto v0 = (t1 - t0);
    const auto v1 = (t2 - t0);
    const auto v2 = (p  - t0);

    const auto d00 = v0.dot(v0);
    const auto d01 = v0.dot(v1);
    const auto d11 = v1.dot(v1);
    const auto d20 = v2.dot(v0);
    const auto d21 = v2.dot(v1);

    const auto reciprocal = 1.0f / (d00 * d11 - d01 * d01);
    const auto v = (d11 * d20 - d01 * d21) * reciprocal;
    const auto w = (d00 * d21 - d01 * d20) * reciprocal;
    const auto u = 1.0f - v - w;

    return barycentric{u, v, w};
}


inline
auto alloy::core::barycentric::from_cartesian(
  const point2& p,
  const point2& t0,
  const point2& t1,
  const point2& t2
) noexcept -> barycentric
{
  const auto v0 = (t1 - t0);
  const auto v1 = (t2 - t0);
  const auto v2 = (p  - t0);

  const auto reciprocal = 1.0f / ((v0.x() * v1.y()) - (v1.x() * v0.y()));
  const auto v = (v2.x() * v1.y() - v1.x() * v2.y()) * reciprocal;
  const auto w = (v0.x() * v2.y() - v2.x() * v0.y()) * reciprocal;
  const auto u = 1.0f - v - w;

  return barycentric{u, v, w};
}

inline constexpr
auto alloy::core::barycentric::u() noexcept -> float&
{
  return m_data[0];
}

inline constexpr
auto alloy::core::barycentric::u() const noexcept -> float
{
  return m_data[0];
}

inline constexpr
auto alloy::core::barycentric::v() noexcept -> float&
{
  return m_data[1];
}

inline constexpr
auto alloy::core::barycentric::v() const noexcept -> float
{
  return m_data[1];
}

inline constexpr
auto alloy::core::barycentric::w() noexcept -> float&
{
  return m_data[2];
}

inline constexpr
auto alloy::core::barycentric::w() const noexcept -> float
{
  return m_data[2];
}

inline constexpr
auto alloy::core::barycentric::data() const noexcept -> const float*
{
  return m_data;
}

inline constexpr
auto alloy::core::barycentric::size() const noexcept -> std::size_t
{
  return 3u;
}

#endif /* ALLOY_CORE_GEOMETRY_POINT_BARYCENTRIC_HPP */

