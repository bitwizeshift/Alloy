////////////////////////////////////////////////////////////////////////////////
/// \file frustum.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_CORE_GEOMETRY_FRUSTUM_HPP
#define ALLOY_CORE_GEOMETRY_FRUSTUM_HPP

#include "alloy/core/api.hpp"
#include "alloy/core/geometry/plane.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/containers/span.hpp"

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
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API frustum
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    static auto make(const plane& left, const plane& right,
                     const plane& top, const plane& bottom,
                     const plane& near, const plane& far) -> frustum;

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
    static constexpr auto make_unchecked(const plane& left, const plane& right,
                                         const plane& top, const plane& bottom,
                                         const plane& near, const plane& far) noexcept -> frustum;

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
    constexpr auto left() const noexcept -> const plane&;
    constexpr auto right() const noexcept -> const plane&;
    constexpr auto top() const noexcept -> const plane&;
    constexpr auto bottom() const noexcept -> const plane&;
    constexpr auto near() const noexcept -> const plane&;
    constexpr auto far() const noexcept -> const plane&;
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
    constexpr auto planes() const noexcept -> span<const plane, 6u>;

    //--------------------------------------------------------------------------
    // Contains
    //--------------------------------------------------------------------------
  public:

    /// \brief Checks whether a given point \p p is contained within this
    ///        sphere
    ///
    /// \param p the point to check
    /// \return \c true if the point is contained in this sphere
    auto contains(const point3& p) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    plane m_planes[6];

    constexpr frustum(const plane& left, const plane& right,
                      const plane& top, const plane& bottom,
                      const plane& near, const plane& far) noexcept;
  };

} // namespace alloy::core

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::frustum::make_unchecked(const plane& left,
                                          const plane& right,
                                          const plane& top,
                                          const plane& bottom,
                                          const plane& near,
                                          const plane& far)
  noexcept -> frustum
{
  return frustum{left, right, top, bottom, near, far};
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::frustum::left()
  const noexcept -> const plane&
{
  return m_planes[0];
}

inline constexpr
auto alloy::core::frustum::right()
  const noexcept -> const alloy::core::plane&
{
  return m_planes[1];
}

inline constexpr
auto alloy::core::frustum::top()
  const noexcept -> const plane&
{
  return m_planes[2];
}

inline constexpr
auto alloy::core::frustum::bottom()
  const noexcept -> const plane&
{
  return m_planes[3];
}

inline constexpr
auto alloy::core::frustum::near()
  const noexcept -> const plane&
{
  return m_planes[4];
}

inline constexpr
auto alloy::core::frustum::far()
  const noexcept -> const plane&
{
  return m_planes[5];
}

inline constexpr
auto alloy::core::frustum::planes()
  const noexcept -> span<const plane, 6u>
{
  return span<const plane, 6u>{m_planes};
}

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::frustum::frustum(const plane& left, const plane& right,
                              const plane& top, const plane& bottom,
                              const plane& near, const plane& far)
  noexcept
  : m_planes{left, right, top, bottom, near, far}
{

}

#endif /* ALLOY_CORE_GEOMETRY_FRUSTUM_HPP */
