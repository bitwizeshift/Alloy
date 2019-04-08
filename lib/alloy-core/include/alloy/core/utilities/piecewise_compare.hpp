/*****************************************************************************
 * \file piecewise_compare.hpp
 *
 * \brief This header defines a comparator for piecewise-based comparisons
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_UTILITIES_PIECEWISE_COMPARE_HPP
#define ALLOY_CORE_UTILITIES_PIECEWISE_COMPARE_HPP

#include <type_traits> // std::false_type

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A comparator type that is specialized for piecewise comparisons
  ///
  /// This type is intended to be used for piecewise comparisons of the type's
  /// data-members, where a typical \c operator< would not be idiomatic.
  ///
  /// For example, 2 or 3 dimensional points can be radially sorted by
  /// comparing coordinate values independently -- but this does not make
  /// sense for a total-ordering operation to define it as the \c operator<
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
    /// \return \c true if \p lhs and \p rhs can compare piecewise
    constexpr bool operator()( const T& lhs, const T& rhs ) noexcept;
  };

} // namespace alloy::core

namespace alloy::core::detail {

  template<typename>
  struct piecewise_compare_not_implemented : std::false_type{};

} // namespace alloy::core::detail

template<typename T>
inline constexpr bool
  alloy::core::piecewise_compare<T>::operator()( const T& lhs, const T& rhs )
  noexcept
{
  static_assert( detail::piecewise_compare_not_implemented<T>::value,
                 "piecewise_compare<T> must be specialized to be used." );
  return lhs < rhs;
}

#endif /* ALLOY_CORE_UTILITIES_PIECEWISE_COMPARE_HPP */