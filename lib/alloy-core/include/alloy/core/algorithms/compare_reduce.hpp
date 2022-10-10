/*****************************************************************************
 * \file
 *
 * \brief
 *****************************************************************************/

/*
 The MIT License (MIT)

 Copyright (c) 2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_ALGORITHM_COMPARE_REDUCE_HPP
#define ALLOY_CORE_ALGORITHM_COMPARE_REDUCE_HPP

#include "alloy/core/macros.hpp"
#include "alloy/core/traits/common_reference.hpp"
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
      const noexcept -> common_reference_t<Ts...>;

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
      const noexcept -> common_reference_t<T0, T1, TRest...>;

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
  const noexcept -> common_reference_t<Ts...>
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
  const noexcept -> common_reference_t<T0, T1, TRest...>
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

#endif /* ALLOY_CORE_ALGORITHM_COMPARE_REDUCE_HPP */
