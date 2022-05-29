////////////////////////////////////////////////////////////////////////////////
/// \file barycentric.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021-2022 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_CORE_GEOMETRY_POINT_BARYCENTRIC_HPP
#define ALLOY_CORE_GEOMETRY_POINT_BARYCENTRIC_HPP

#include "alloy/core/precision.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/geometry/point/point2.hpp"

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
    static auto from_cartesion(const point3& p,
                               const point3& t1,
                               const point3& t2,
                               const point3& t3) noexcept -> barycentric;

    /// \brief Computes the barycentric coordinate given a world point \p p and
    ///        a triangle made up from points \p t1, \p t2, and \p t3
    ///
    /// \param p the world point
    /// \param t1 the first point in the triangle
    /// \param t2 the second point in the triangle
    /// \param t3 the third point in the triangle
    /// \return the barycentric coordinate
    static auto from_cartesion(const point2& p,
                               const point2& t1,
                               const point2& t2,
                               const point2& t3) noexcept -> barycentric;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------

    /// \brief Default constructs this barycentric point without any value
    barycentric() = default;

    /// \brief Constructs a barycentric point from the 3 components
    ///
    /// \param u the u component (alpha)
    /// \param v the v component (beta0
    /// \param w the w component (gamma)
    constexpr barycentric(real u, real v, real w) noexcept;

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
    constexpr auto u() noexcept -> real&;
    constexpr auto u() const noexcept -> real;
    /// \}

    /// \{
    /// \brief Gets the v component
    ///
    /// \return the v component
    constexpr auto v() noexcept -> real&;
    constexpr auto v() const noexcept -> real;
    /// \}

    /// \{
    /// \brief Gets the w component
    ///
    /// \return the w component
    constexpr auto w() noexcept -> real&;
    constexpr auto w() const noexcept -> real;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    [[nodiscard]]
    constexpr auto data() const noexcept -> const real*;

    /// \brief Gets the number of components in this barycentric coordinate
    ///
    /// \return 3u
    [[nodiscard]]
    constexpr auto size() const noexcept -> std::size_t;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  public:

    real m_data[3];
  };

} // namespace alloy::core

inline constexpr
alloy::core::barycentric::barycentric(real u, real v, real w)
  noexcept
  : m_data{u, v, w}
{

}

inline
auto alloy::core::barycentric::from_cartesion(const point3& p,
                                              const point3& t0,
                                              const point3& t1,
                                              const point3& t2)
  noexcept -> barycentric
{
    const auto v0 = (t1 - t0);
    const auto v1 = (t2 - t0);
    const auto v2 = (p  - t0);

    const auto d00 = v0.dot(v0);
    const auto d01 = v0.dot(v1);
    const auto d11 = v1.dot(v1);
    const auto d20 = v2.dot(v0);
    const auto d21 = v2.dot(v1);

    const auto reciprocal = real{1} / (d00 * d11 - d01 * d01);
    const auto v = (d11 * d20 - d01 * d21) * reciprocal;
    const auto w = (d00 * d21 - d01 * d20) * reciprocal;
    const auto u = real{1} - v - w;

    return barycentric{u, v, w};
}


inline
auto alloy::core::barycentric::from_cartesion(const point2& p,
                                              const point2& t0,
                                              const point2& t1,
                                              const point2& t2)
  noexcept -> barycentric
{
  const auto v0 = (t1 - t0);
  const auto v1 = (t2 - t0);
  const auto v2 = (p  - t0);

  const auto reciprocal = real{1} / ((v0.x() * v1.y()) - (v1.x() * v0.y()));
  const auto v = (v2.x() * v1.y() - v1.x() * v2.y()) * reciprocal;
  const auto w = (v0.x() * v2.y() - v2.x() * v0.y()) * reciprocal;
  const auto u = real{1} - v - w;

  return barycentric{u, v, w};
}

inline constexpr
auto alloy::core::barycentric::u()
  noexcept -> real&
{
  return m_data[0];
}

inline constexpr
auto alloy::core::barycentric::u()
  const noexcept -> real
{
  return m_data[0];
}

inline constexpr
auto alloy::core::barycentric::v()
  noexcept -> real&
{
  return m_data[1];
}

inline constexpr
auto alloy::core::barycentric::v()
  const noexcept -> real
{
  return m_data[1];
}

inline constexpr
auto alloy::core::barycentric::w()
  noexcept -> real&
{
  return m_data[2];
}

inline constexpr
auto alloy::core::barycentric::w()
  const noexcept -> real
{
  return m_data[2];
}

inline constexpr
auto alloy::core::barycentric::data()
  const noexcept -> const real*
{
  return m_data;
}

inline constexpr
auto alloy::core::barycentric::size()
  const noexcept -> std::size_t
{
  return 3u;
}

#endif /* ALLOY_CORE_GEOMETRY_POINT_BARYCENTRIC_HPP */

