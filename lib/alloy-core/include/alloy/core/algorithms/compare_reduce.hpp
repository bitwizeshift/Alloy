// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_CORE_ALGORITHMS_COMPARE_REDUCE_HPP
#define ALLOY_CORE_ALGORITHMS_COMPARE_REDUCE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/macros.hpp"
#include "alloy/core/intrinsics.hpp"

#include <type_traits>
#include <utility> // std::forward
#include <tuple>   // std::tuple

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Performs a reduction on a sequence of objects
  ///
  /// \tparam Compare the comparator
  /////////////////////////////////////////////////////////////////////////////
  template <typename Compare>
  class compare_reducer
    : std::tuple<Compare>
  {
    using base_type = std::tuple<Compare>;

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    /// \brief The underlying comparison
    using compare = Compare;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    compare_reducer() = default;

    explicit constexpr compare_reducer(const Compare& compare)
      noexcept(std::is_nothrow_copy_constructible<Compare>::value);
    explicit constexpr compare_reducer(Compare&& compare)
      noexcept(std::is_nothrow_move_constructible<Compare>::value);

    compare_reducer(const compare_reducer&) = default;
    compare_reducer(compare_reducer&&) = default;

    //-------------------------------------------------------------------------

    auto operator=(const compare_reducer&) -> compare_reducer& = default;
    auto operator=(compare_reducer&&) -> compare_reducer& = default;

    //-------------------------------------------------------------------------
    // Call Operator
    //-------------------------------------------------------------------------
  public:

    template <typename...Ts>
    constexpr auto operator()(Ts&&...values)
      const noexcept -> std::common_reference_t<Ts...>;

    //-------------------------------------------------------------------------
    // Private Implementation
    //-------------------------------------------------------------------------
  private:

    /// \brief Variadically computes the maximum value
    ///
    /// This function operates solely on references
    ///
    /// \param v0 The first value
    /// \param v1 The last value
    /// \param vn The nth value
    /// \return the maximum value
    template <typename T0, typename T1, typename...TRest>
    constexpr auto reduce(T0&& v0, T1&& v1, TRest&&...vn)
      const noexcept -> std::common_reference_t<T0, T1, TRest...>;

    /// \brief Performs the comparison between \p lhs and \p rhs
    ///
    /// \param lhs the left value
    /// \param rhs the right value
    /// \return the result of the comparison (`bool`)
    template <typename T, typename U>
    constexpr auto do_compare(const T& lhs, const U& rhs)
      const noexcept -> bool;
  };

} // namespace alloy::core

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename Compare>
ALLOY_FORCE_INLINE constexpr
alloy::core::compare_reducer<Compare>::compare_reducer(const Compare& compare)
  noexcept(std::is_nothrow_copy_constructible<Compare>::value)
  : base_type{compare}
{

}

template <typename Compare>
ALLOY_FORCE_INLINE constexpr
alloy::core::compare_reducer<Compare>::compare_reducer(Compare&& compare)
  noexcept(std::is_nothrow_move_constructible<Compare>::value)
  : base_type{std::move(compare)}
{

}

//-----------------------------------------------------------------------------
// Call Operator
//-----------------------------------------------------------------------------

template <typename Compare>
template <typename...Ts>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compare_reducer<Compare>::operator()(Ts&&...values)
  const noexcept -> std::common_reference_t<Ts...>
{
  // This function is force-inlined because `reduce` is where all the work is
  // done.
  return reduce(std::forward<Ts>(values)...);
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename Compare>
template <typename T0, typename T1, typename...TRest>
inline constexpr
auto alloy::core::compare_reducer<Compare>::reduce(T0&& v0, T1&& v1, TRest&&...vn)
  const noexcept -> std::common_reference_t<T0, T1, TRest...>
{
  if constexpr (sizeof...(TRest) == 0U) {
    return do_compare(v0,v1) ? std::forward<T0>(v0) : std::forward<T1>(v1);
  } else {
    return reduce(
      (do_compare(v0,v1) ? std::forward<T0>(v0) : std::forward<T1>(v1)),
      std::forward<TRest>(vn)...
    );
  }
}

template <typename Compare>
template <typename T, typename U>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compare_reducer<Compare>::do_compare(const T& lhs, const U& rhs)
  const noexcept -> bool
{
  return std::get<0>(static_cast<const base_type&>(*this))(lhs, rhs);
}

#endif /* ALLOY_CORE_ALGORITHMS_COMPARE_REDUCE_HPP */
