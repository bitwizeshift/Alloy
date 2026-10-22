// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file intersection.hpp
 *
 * \brief This header defines the tests for intersections between geometric
 *        objects, and the result of an intersection calculation
 *****************************************************************************/

#ifndef ALLOY_CORE_GEOMETRY_INTERSECTION_HPP
#define ALLOY_CORE_GEOMETRY_INTERSECTION_HPP

#ifdef _MSC_VER
# pragma once
#endif

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

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"    // ALLOY_ASSERT
#include "alloy/core/math/math.hpp" // default_tolerance
#include "alloy/core/precision.hpp" // real

#include <concepts>
#include <type_traits> // std::conjunction, std::negation, std::is_reference
#include <utility>     // std::declval
#include <variant>

namespace alloy::core {

  //============================================================================
  // class : intersection
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The result of an intersection calculation
  ///
  /// The result of an intersection computation may at most one of N geometric
  /// types denoted by Types.... In the case that there is no intersection, then
  /// has_intersection will return false.
  ///
  /// \tparam Types the geometric types that the intersection may be
  //////////////////////////////////////////////////////////////////////////////
  template <typename...Types>
  class intersection
  {
    static_assert(
      sizeof...(Types) >= 1,
      "Intersection must contain at least one geometric type."
    );

    static_assert(
      (!std::is_reference_v<Types> && ...),
      "Intersection types of references are ill-formed."
    );

    static_assert(
      (!std::is_void_v<Types> && ...),
      "Intersection types of void types are ill-formed"
    );

    static_assert(
      (!std::is_pointer_v<Types> && ...),
      "Intersection types of pointer types are ill-formed"
    );

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an intersection result that holds no real intersection
    constexpr intersection() noexcept;

    /// \brief Constructs an intersection with the given type
    ///
    /// \param type the type of intersection
    template <typename Type>
      requires std::constructible_from<std::variant<Types...>, Type>
    constexpr intersection(Type&& type) noexcept;

    /// \brief Constructs a intersection by moving an existing instance
    ///
    /// \param other the other intersection to move
    intersection(intersection&& other) = default;

    /// \brief Constructs a intersection by copying an existing instance
    ///
    /// \param other the other intersection to copy
    intersection(const intersection& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Move-assigns the contents of an existing intersection
    ///
    /// \param other the other intersection to move
    /// \return reference to (*this)
    auto operator=(intersection&& other) -> intersection& = default;

    /// \brief Copy-assigns the contents of an existing intersection
    ///
    /// \param other the other intersection to copy
    /// \return reference to (*this)
    auto operator=(const intersection& other) -> intersection& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Checks whether there is an intersection
    ///
    /// \return true if there is an intersection
    [[nodiscard]]
    constexpr auto has_intersection() const noexcept -> bool;

    /// \brief Checks if this intersection contains the geometric type Type
    ///
    /// \tparam Type the geometric type
    /// \return true if this intersection contains the specified type
    template<typename Type>
    [[nodiscard]]
    constexpr auto contains() const noexcept -> bool;

    /// \brief Attempts to get the underlying intersection type as Type
    ///
    /// \tparam Type the type to try to get from the intersection
    /// \return nullptr on failure, a pointer to Type on success
    template<typename Type>
    constexpr auto try_as() const noexcept -> const Type*;

    /// \brief Gets the underlying intersection type as Type
    ///
    /// \pre Type is the correct underlying type
    /// \tparam Type the type to try to get from the intersection
    /// \return a reference to Type on success
    template<typename Type>
    constexpr auto as() const -> const Type&;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    using variant_type = std::variant<std::monostate, Types...>;

    variant_type m_intersection_types;

    template<typename... UTypes>
    friend constexpr auto operator==(
      const intersection<UTypes...>&,
      const intersection<UTypes...>&
    ) noexcept -> bool;

    template<typename... UTypes>
    friend constexpr auto almost_equal(
      const intersection<UTypes...>&,
      const intersection<UTypes...>&,
      real
    ) noexcept -> bool;
  };

  //============================================================================
  // non-member functions : class : intersection
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  template<typename... Types>
  constexpr auto operator==(
    const intersection<Types...>& lhs,
    const intersection<Types...>& rhs
  ) noexcept -> bool;
  template<typename... Types>
  constexpr auto operator!=(
    const intersection<Types...>& lhs,
    const intersection<Types...>& rhs
  ) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two intersection objects relative to
  ///        \p tolerance
  ///
  /// \param lhs the left intersection
  /// \param rhs the right intersection
  /// \param tolerance the tolerance value to compare against
  /// \return `true` if the two intersections contain almost equal values
  template<typename... Types>
  constexpr auto almost_equal(
    const intersection<Types...>& lhs,
    const intersection<Types...>& rhs,
    real tolerance
  ) noexcept -> bool;

  //============================================================================
  // struct : intersection_traits
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A trait that determines whether objects of the types \p L and
  ///        \p R intersect
  ///
  /// Specializations provide the static member function
  /// `intersects(const L&, const R&) noexcept -> bool`, which determines
  /// whether the two objects share at least one point. Objects that only touch
  /// at their boundaries intersect.
  ///
  /// Specializations for pairs whose intersection is itself geometry may also
  /// provide the static member function `intersection(const L&, const R&)`,
  /// which gets that geometry as a `core::intersection` of the types that it
  /// may be.
  ///
  /// Only one of `<L, R>` and `<R, L>` is specialized for each pair of types,
  /// since `intersects` accepts its arguments in either order.
  ///
  /// The specializations in this header accept planes that are not
  /// normalized.
  ///
  /// \tparam L the type of the left object
  /// \tparam R the type of the right object
  //////////////////////////////////////////////////////////////////////////////
  template <typename L, typename R>
  struct intersection_traits {};

  template <>
  struct ALLOY_CORE_API intersection_traits<plane, plane>
  {
    static auto intersects(const plane& a, const plane& b) noexcept -> bool;
    static auto intersection(const plane& a, const plane& b)
      noexcept -> core::intersection<line, plane>;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<plane, line>
  {
    static auto intersects(const plane& a, const line& b) noexcept -> bool;
    static auto intersection(const plane& a, const line& b)
      noexcept -> core::intersection<point3, line>;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<plane, ray>
  {
    static auto intersects(const plane& a, const ray& b) noexcept -> bool;
    static auto intersection(const plane& a, const ray& b)
      noexcept -> core::intersection<point3, ray>;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<plane, line_segment>
  {
    static auto intersects(const plane& a, const line_segment& b)
      noexcept -> bool;
    static auto intersection(const plane& a, const line_segment& b)
      noexcept -> core::intersection<point3, line_segment>;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<plane, triangle>
  {
    static auto intersects(const plane& a, const triangle& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<plane, sphere>
  {
    static auto intersects(const plane& a, const sphere& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<plane, capsule>
  {
    static auto intersects(const plane& a, const capsule& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<plane, axis_aligned_box>
  {
    static auto intersects(const plane& a, const axis_aligned_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<plane, oriented_box>
  {
    static auto intersects(const plane& a, const oriented_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<line, triangle>
  {
    static auto intersects(const line& a, const triangle& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<line, sphere>
  {
    static auto intersects(const line& a, const sphere& b) noexcept -> bool;
    static auto intersection(const line& a, const sphere& b)
      noexcept -> core::intersection<point3, line_segment>;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<line, capsule>
  {
    static auto intersects(const line& a, const capsule& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<line, axis_aligned_box>
  {
    static auto intersects(const line& a, const axis_aligned_box& b)
      noexcept -> bool;
    static auto intersection(const line& a, const axis_aligned_box& b)
      noexcept -> core::intersection<point3, line_segment>;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<line, oriented_box>
  {
    static auto intersects(const line& a, const oriented_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<ray, triangle>
  {
    static auto intersects(const ray& a, const triangle& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<ray, sphere>
  {
    static auto intersects(const ray& a, const sphere& b) noexcept -> bool;
    static auto intersection(const ray& a, const sphere& b)
      noexcept -> core::intersection<point3, line_segment>;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<ray, capsule>
  {
    static auto intersects(const ray& a, const capsule& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<ray, axis_aligned_box>
  {
    static auto intersects(const ray& a, const axis_aligned_box& b)
      noexcept -> bool;
    static auto intersection(const ray& a, const axis_aligned_box& b)
      noexcept -> core::intersection<point3, line_segment>;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<ray, oriented_box>
  {
    static auto intersects(const ray& a, const oriented_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<line_segment, triangle>
  {
    static auto intersects(const line_segment& a, const triangle& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<line_segment, sphere>
  {
    static auto intersects(const line_segment& a, const sphere& b)
      noexcept -> bool;
    static auto intersection(const line_segment& a, const sphere& b)
      noexcept -> core::intersection<point3, line_segment>;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<line_segment, capsule>
  {
    static auto intersects(const line_segment& a, const capsule& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<line_segment, axis_aligned_box>
  {
    static auto intersects(const line_segment& a, const axis_aligned_box& b)
      noexcept -> bool;
    static auto intersection(const line_segment& a, const axis_aligned_box& b)
      noexcept -> core::intersection<point3, line_segment>;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<line_segment, oriented_box>
  {
    static auto intersects(const line_segment& a, const oriented_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<triangle, triangle>
  {
    static auto intersects(const triangle& a, const triangle& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<triangle, sphere>
  {
    static auto intersects(const triangle& a, const sphere& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<triangle, capsule>
  {
    static auto intersects(const triangle& a, const capsule& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<triangle, axis_aligned_box>
  {
    static auto intersects(const triangle& a, const axis_aligned_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<triangle, oriented_box>
  {
    static auto intersects(const triangle& a, const oriented_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<sphere, sphere>
  {
    static auto intersects(const sphere& a, const sphere& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<sphere, capsule>
  {
    static auto intersects(const sphere& a, const capsule& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<sphere, axis_aligned_box>
  {
    static auto intersects(const sphere& a, const axis_aligned_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<sphere, oriented_box>
  {
    static auto intersects(const sphere& a, const oriented_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<capsule, capsule>
  {
    static auto intersects(const capsule& a, const capsule& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<capsule, axis_aligned_box>
  {
    static auto intersects(const capsule& a, const axis_aligned_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<capsule, oriented_box>
  {
    static auto intersects(const capsule& a, const oriented_box& b)
      noexcept -> bool;
  };

  template <>
  struct intersection_traits<axis_aligned_box, axis_aligned_box>
  {
    static auto intersects(const axis_aligned_box& a, const axis_aligned_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<axis_aligned_box, oriented_box>
  {
    static auto intersects(const axis_aligned_box& a, const oriented_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<oriented_box, oriented_box>
  {
    static auto intersects(const oriented_box& a, const oriented_box& b)
      noexcept -> bool;
  };

  /// \brief Determines whether an object may be inside of a frustum
  ///
  /// This is the conventional test for culling: it rejects the object only
  /// when the object lies entirely outside of one of the planes of the
  /// frustum. An object near a corner of the frustum may lie outside of the
  /// frustum, but outside of no single plane, so this may report an
  /// intersection where there is none. It never fails to report one that
  /// exists.
  template <>
  struct ALLOY_CORE_API intersection_traits<frustum, sphere>
  {
    static auto intersects(const frustum& a, const sphere& b) noexcept -> bool;
  };

  /// \copydoc intersection_traits<frustum, sphere>
  template <>
  struct ALLOY_CORE_API intersection_traits<frustum, axis_aligned_box>
  {
    static auto intersects(const frustum& a, const axis_aligned_box& b)
      noexcept -> bool;
  };

  /// \copydoc intersection_traits<frustum, sphere>
  template <>
  struct ALLOY_CORE_API intersection_traits<frustum, oriented_box>
  {
    static auto intersects(const frustum& a, const oriented_box& b)
      noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<circle, circle>
  {
    static auto intersects(const circle& a, const circle& b) noexcept -> bool;
  };

  template <>
  struct ALLOY_CORE_API intersection_traits<circle, rectangle>
  {
    static auto intersects(const circle& a, const rectangle& b)
      noexcept -> bool;
  };

  template <>
  struct intersection_traits<rectangle, rectangle>
  {
    static auto intersects(const rectangle& a, const rectangle& b)
      noexcept -> bool;
  };

  //============================================================================
  // concept : intersectable
  //============================================================================

  namespace detail {

    template <typename L, typename R>
    concept has_intersection_traits = requires(const L& l, const R& r) {
      { intersection_traits<L, R>::intersects(l, r) } -> std::same_as<bool>;
    };

  } // namespace detail

  /// \brief Determines whether objects of the types \p L and \p R can be
  ///        tested for an intersection, in either order
  ///
  /// \tparam L the type of the left object
  /// \tparam R the type of the right object
  template <typename L, typename R>
  concept intersectable = detail::has_intersection_traits<L, R> ||
                          detail::has_intersection_traits<R, L>;

  //============================================================================
  // functions : intersects
  //============================================================================

  /// \brief Determines whether the objects \p lhs and \p rhs share at least
  ///        one point
  ///
  /// This evaluates the `intersection_traits` specialized for the types of
  /// the arguments, in whichever order it is specialized.
  ///
  /// \param lhs the first object
  /// \param rhs the second object
  /// \return `true` if \p lhs and \p rhs intersect
  template <typename L, typename R>
    requires intersectable<L, R>
  constexpr auto intersects(const L& lhs, const R& rhs) noexcept -> bool;

  //============================================================================
  // concept : intersection_computable
  //============================================================================

  namespace detail {

    template <typename L, typename R>
    concept has_intersection_member = requires(const L& l, const R& r) {
      intersection_traits<L, R>::intersection(l, r);
    };

    template <typename L, typename R>
    struct intersection_of_result
    {
      using type = decltype(
        intersection_traits<R, L>::intersection(
          std::declval<const R&>(),
          std::declval<const L&>()
        )
      );
    };

    template <typename L, typename R>
      requires has_intersection_member<L, R>
    struct intersection_of_result<L, R>
    {
      using type = decltype(
        intersection_traits<L, R>::intersection(
          std::declval<const L&>(),
          std::declval<const R&>()
        )
      );
    };

  } // namespace detail

  /// \brief Determines whether the geometry of the intersection of objects
  ///        of the types \p L and \p R can be computed, in either order
  ///
  /// \tparam L the type of the left object
  /// \tparam R the type of the right object
  template <typename L, typename R>
  concept intersection_computable = detail::has_intersection_member<L, R> ||
                                    detail::has_intersection_member<R, L>;

  /// \brief The type of the intersection of objects of the types \p L and
  ///        \p R
  ///
  /// \tparam L the type of the left object
  /// \tparam R the type of the right object
  template <typename L, typename R>
    requires intersection_computable<L, R>
  using intersection_of_t = typename detail::intersection_of_result<L, R>::type;

  //============================================================================
  // functions : intersection_of
  //============================================================================

  /// \brief Gets the geometry where the objects \p lhs and \p rhs intersect
  ///
  /// This evaluates the `intersection_traits` specialized for the types of
  /// the arguments, in whichever order it is specialized.
  ///
  /// \param lhs the first object
  /// \param rhs the second object
  /// \return the intersection, which has no value if \p lhs and \p rhs do
  ///         not intersect
  template <typename L, typename R>
    requires intersection_computable<L, R>
  constexpr auto intersection_of(const L& lhs, const R& rhs)
    noexcept -> intersection_of_t<L, R>;

} // namespace alloy::core

//==============================================================================
// class : intersection
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename...Types>
inline constexpr
alloy::core::intersection<Types...>::intersection() noexcept
  : m_intersection_types{}
{

}

template <typename...Types>
template <typename Type>
  requires std::constructible_from<std::variant<Types...>, Type>
inline constexpr
alloy::core::intersection<Types...>::intersection(Type&& type) noexcept
  : m_intersection_types{std::forward<Type>(type)}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template<typename... Types>
inline constexpr
auto alloy::core::intersection<Types...>::has_intersection()
  const noexcept -> bool
{
  return !std::holds_alternative<std::monostate>(m_intersection_types);
}

template<typename... Types>
template<typename Type>
inline constexpr
auto alloy::core::intersection<Types...>::contains() const noexcept -> bool
{
  return std::holds_alternative<Type>(m_intersection_types);
}

template<typename... Types>
template<typename Type>
inline constexpr
auto alloy::core::intersection<Types...>::try_as() const noexcept -> const Type*
{
  return std::get_if<Type>(&m_intersection_types);
}

template<typename... Types>
template<typename Type>
inline constexpr
auto alloy::core::intersection<Types...>::as() const -> const Type&
{
  const auto* p = try_as<Type>();

  ALLOY_ALWAYS_ASSERT(p != nullptr);

  return *p;
}

//==============================================================================
// non-member functions : class : intersection
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

template<typename... Types>
inline constexpr
auto alloy::core::operator==(
  const intersection<Types...>& lhs,
  const intersection<Types...>& rhs
) noexcept -> bool
{
  return lhs.m_intersection_types == rhs.m_intersection_types;
}

template<typename... Types>
inline constexpr
auto alloy::core::operator!=(
  const intersection<Types...>& lhs,
  const intersection<Types...>& rhs
) noexcept -> bool
{
  return !(lhs == rhs);
}

//------------------------------------------------------------------------------

template<typename... Types>
inline constexpr
auto alloy::core::almost_equal(
  const intersection<Types...>& lhs,
  const intersection<Types...>& rhs,
  real tolerance
) noexcept -> bool
{
  if (lhs.m_intersection_types.index() != rhs.m_intersection_types.index()) {
    return false;
  }
  const auto compare = [tolerance]<typename T, typename U>(
    const T& l,
    const U& r
  ) -> bool {
    if constexpr (!std::same_as<T, U>) {
      return false;
    } else if constexpr (std::same_as<T, std::monostate>) {
      return true;
    } else {
      return almost_equal(l, r, tolerance);
    }
  };
  return std::visit(
    compare,
    lhs.m_intersection_types,
    rhs.m_intersection_types
  );
}

//==============================================================================
// struct : intersection_traits
//==============================================================================

inline
auto alloy::core::intersection_traits<
  alloy::core::axis_aligned_box,
  alloy::core::axis_aligned_box
>::intersects(const axis_aligned_box& a, const axis_aligned_box& b)
  noexcept -> bool
{
  return a.intersects(b);
}

inline
auto alloy::core::intersection_traits<
  alloy::core::rectangle,
  alloy::core::rectangle
>::intersects(const rectangle& a, const rectangle& b) noexcept -> bool
{
  return a.intersects(b);
}

//==============================================================================
// functions : intersects
//==============================================================================

template <typename L, typename R>
  requires alloy::core::intersectable<L, R>
inline constexpr
auto alloy::core::intersects(const L& lhs, const R& rhs) noexcept -> bool
{
  if constexpr (detail::has_intersection_traits<L, R>) {
    return intersection_traits<L, R>::intersects(lhs, rhs);
  } else {
    return intersection_traits<R, L>::intersects(rhs, lhs);
  }
}

//==============================================================================
// functions : intersection_of
//==============================================================================

template <typename L, typename R>
  requires alloy::core::intersection_computable<L, R>
inline constexpr
auto alloy::core::intersection_of(const L& lhs, const R& rhs)
  noexcept -> intersection_of_t<L, R>
{
  if constexpr (detail::has_intersection_member<L, R>) {
    return intersection_traits<L, R>::intersection(lhs, rhs);
  } else {
    return intersection_traits<R, L>::intersection(rhs, lhs);
  }
}

#endif /* ALLOY_CORE_GEOMETRY_INTERSECTION_HPP */
