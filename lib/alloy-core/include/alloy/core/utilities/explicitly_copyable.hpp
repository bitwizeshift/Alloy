/*****************************************************************************
 * \file explicitly_copyable.hpp
 *
 * \brief This header defines a utility for removing implicit copies
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
#ifndef ALLOY_CORE_UTILITIES_EXPLICITLY_COPYABLE_HPP
#define ALLOY_CORE_UTILITIES_EXPLICITLY_COPYABLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/intrinsics.hpp"

#include <type_traits> // std::is_final, std::is_copy_constructible, etc

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A utility for making an underlying type only explicitly copyable
  ///
  /// This utility removes the implicit copy constructor/assignment, and
  /// instead replaces it with a \c copy() member function that must be
  /// explicitly called in order to produce a copy.
  ///
  /// This is intended for use with types that perform heap allocations such
  /// as containers, or for large objects that might be expensive to
  /// inadvertantly copy rather than move.
  ///
  /// The underlying type must not be final, and must be copy constructible
  ///
  /// \tparam T the underlying type
  /////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class explicitly_copyable : public T
  {
    static_assert(!std::is_final<T>::value);
    static_assert(std::is_copy_constructible<T>::value);

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    explicitly_copyable() = default;
    explicitly_copyable(explicitly_copyable&&) = default;
    explicitly_copyable(const T&) = delete;
    using T::T;

    auto operator=(explicitly_copyable&&) -> explicitly_copyable& = default;
    auto operator=(const T&) -> explicitly_copyable& = delete;
    using T::operator=;

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------
  public:

    /// \brief Creates a copy of the underlying type
    ///
    /// This is the only way to receive a copy of the type
    ///
    /// \return a copy of the underlying type
    auto copy()
      const noexcept(std::is_nothrow_copy_constructible<T>::value)
      -> explicitly_copyable;

    /// \brief Creates a copy of the underlying type using the specified
    ///        allocator
    ///
    /// \param alloc the allocator to use for the copy
    /// \return a copy of the underlying type using \p alloc
    template <typename Alloc = typename T::allocator_type,
              typename = std::enable_if_t<std::is_constructible<T,const T&, const Alloc&>::value>>
    auto copy_with(const Alloc& alloc)
     const noexcept(std::is_nothrow_constructible<T, const T&, const Alloc&>::value)
     -> explicitly_copyable;

    //-------------------------------------------------------------------------
    // Private Constructors / Assignment
    //-------------------------------------------------------------------------
  private:

    explicitly_copyable(const explicitly_copyable&) = default;

    explicitly_copyable& operator=(const explicitly_copyable&) = default;
  };

} // namespace alloy::core

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::explicitly_copyable<T>::copy()
  const noexcept(std::is_nothrow_copy_constructible<T>::value)
  -> explicitly_copyable
{
  return (*this);
}

template <typename T>
template <typename Alloc, typename>
ALLOY_FORCE_INLINE
auto alloy::core::explicitly_copyable<T>::copy_with(const Alloc& alloc)
  const noexcept(std::is_nothrow_constructible<T, const T&, const Alloc&>::value)
  -> explicitly_copyable
{
  return explicitly_copyable<T>{static_cast<const T&>(*this), alloc};
}

#endif /* ALLOY_CORE_UTILITIES_EXPLICITLY_COPYABLE_HPP */
