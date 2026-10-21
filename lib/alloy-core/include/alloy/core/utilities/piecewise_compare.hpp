// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file piecewise_compare.hpp
 *
 * \brief This header defines a comparator for piecewise-based comparisons
 *****************************************************************************/

#ifndef ALLOY_CORE_UTILITIES_PIECEWISE_COMPARE_HPP
#define ALLOY_CORE_UTILITIES_PIECEWISE_COMPARE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <type_traits> // std::false_type

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A comparator type that is specialized for piecewise comparisons
  ///
  /// This type is intended to be used for piecewise comparisons of the type's
  /// data-members, where a typical `operator<` would not be idiomatic.
  ///
  /// For example, 2 or 3 dimensional points can be radially sorted by
  /// comparing coordinate values independently -- but this does not make
  /// sense for a total-ordering operation to define it as the `operator<`
  /// default behavior.
  /////////////////////////////////////////////////////////////////////////////
  template<typename T>
  struct piecewise_compare
  {
    /// \brief Compares \p lhs with \p rhs piecewise
    ///
    /// \note The default implementation of this will static-assert
    ///
    /// \param lhs the left operand
    /// \param rhs the right operance
    /// \return `true` if \p lhs and \p rhs can compare piecewise
    constexpr auto operator()(const T& lhs, const T& rhs)
      const noexcept -> bool;
  };

} // namespace alloy::core

namespace alloy::core::detail {

  template<typename>
  struct piecewise_compare_not_implemented : std::false_type{};

} // namespace alloy::core::detail

template<typename T>
inline constexpr
auto alloy::core::piecewise_compare<T>::operator()(const T& lhs, const T& rhs)
  const noexcept -> bool
{
  static_assert(
    detail::piecewise_compare_not_implemented<T>::value,
    "piecewise_compare<T> must be specialized to be used."
  );
  return lhs < rhs;
}

#endif /* ALLOY_CORE_UTILITIES_PIECEWISE_COMPARE_HPP */
