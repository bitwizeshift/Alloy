/*****************************************************************************
 * \file intersection.hpp
 *
 * \brief This header defines the result of an intersection calculation
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2020-2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_GEOMETRY_INTERSECTION_HPP
#define ALLOY_CORE_GEOMETRY_INTERSECTION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/assert.hpp"             // ALLOY_ASSERT
#include "alloy/core/precision.hpp"          // real
#include "alloy/core/math/math.hpp"          // default_tolerance
#include "alloy/core/utilities/result.hpp"   // result

#include <type_traits> // std::conjunction, std::negation, std::is_reference
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
    template <typename Type,
              typename = std::enable_if_t<std::is_constructible<std::variant<Types...>,Type>::value>>
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

} // namespace alloy::core

//==============================================================================
// class : intersection
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename...Types>
inline constexpr alloy::core::intersection<Types...>::intersection()
  noexcept
  : m_intersection_types{}
{

}

template <typename...Types>
template <typename Type, typename>
inline constexpr alloy::core::intersection<Types...>::intersection(Type&& type)
  noexcept
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
auto alloy::core::intersection<Types...>::contains()
  const noexcept -> bool
{
  return std::holds_alternative<Type>(m_intersection_types);
}

template<typename... Types>
template<typename Type>
inline constexpr
auto alloy::core::intersection<Types...>::try_as()
  const noexcept -> const Type*
{
  return std::get_if<Type>(&m_intersection_types);
}

template<typename... Types>
template<typename Type>
inline constexpr
auto alloy::core::intersection<Types...>::as()
  const -> const Type&
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

namespace alloy::core::detail {

  /// A visitor that compares using 'almost_equal' for any types that may
  /// compare with 'almost_equal'. For anything that is not comparable, it
  /// returns false.
  struct intersection_almost_equal_visitor
  {
    real tolerance;

    template<typename T>
    inline constexpr
    auto operator()(const std::monostate&, const std::monostate&) noexcept -> bool
    {
      return true; // two monostates are always equal
    }

    template<typename T>
    inline constexpr
    auto operator()(const T&, const std::monostate&) noexcept -> bool
    {
      return false;
    }

    template<typename T>
    inline constexpr
    auto operator()(const std::monostate&, const T&) noexcept -> bool
    {
      return false;
    }

    template<typename T>
    inline constexpr
    auto operator()(const T& lhs, const T& rhs) noexcept -> bool
    {
      return almost_equal(lhs, rhs, tolerance);
    }

    template<typename T, typename U>
    inline constexpr
    auto operator()(const T&, const U&) noexcept -> bool
    {
      return false;
    }
  };

} // namespace alloy::core::detail

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
  return std::visit(
    detail::intersection_almost_equal_visitor{tolerance},
    lhs.m_intersection_types,
    rhs.m_intersection_types
  );
}

#endif /* ALLOY_CORE_GEOMETRY_INTERSECTION_HPP */
