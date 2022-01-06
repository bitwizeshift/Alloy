/*****************************************************************************
 * \file set_view.hpp
 *
 * \brief This header contains the definition of a non-owning view of set
 *        objects.
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
#ifndef ALLOY_CORE_CONTAINERS_SET_VIEW_HPP
#define ALLOY_CORE_CONTAINERS_SET_VIEW_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/delegate.hpp"

#include <cstddef>     // std::size_t
#include <type_traits> // std::decay_t
#include <memory>      // std::addressof

namespace alloy::core {

  namespace detail {

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A home-brew constructed Vtable for set-like objects
    ///
    /// This is used to produce a "virtual-like" behaviour from different sets,
    /// even though they are not part of the same hierarchy. This view only
    /// acts on the immutable portions of the API.
    ///
    /// \tparam T the underlying `value_type` or `key_type` of the set
    ////////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct set_view_vtable final
    {
      using for_each_callback = delegate<void(const T&)>;

      //------------------------------------------------------------------------
      // Function Types
      //------------------------------------------------------------------------

      // Note: These two functions currently make up the definition of `empty`
      //       and `contains` as well, both of which could be written more
      //       optimally as separate hooks -- at the expense of additional
      //       storage space for these functions.
      //
      //       Since most containers require constant-time lookup on `size()`,
      //       and multisets are seldomly used, these two functions should pose
      //       negligible (likely zero) cost.
      using count_function_type = auto (*)(const void*,const T&) -> std::size_t;
      using size_function_type = auto (*)(const void*) -> std::size_t;
      using for_each_function_type = auto(*)(const void*, for_each_callback) -> void;

      //------------------------------------------------------------------------
      // Functions
      //------------------------------------------------------------------------

      count_function_type count_function;
      size_function_type size_function;
      for_each_function_type for_each_function;

      //------------------------------------------------------------------------
      // Stubs
      //------------------------------------------------------------------------

      /// \brief An implementation of the `count` function in terms of `Set`
      ///
      /// \param p the pointer to a Set instance
      /// \param value the value
      /// \return `static_cast<const Set*>(p)->count(value)`
      template <typename Set>
      static auto set_count(const void* p, const T& value) -> std::size_t;

      /// \brief An implementation of the `count` function in terms of no
      ///        object. This returns 0
      ///
      /// \note This is the default value for default-construction
      ///
      /// \param p ignored
      /// \param value ignored
      /// \return 0
      static constexpr auto null_count(const void* p, const T& value) -> std::size_t;

      //------------------------------------------------------------------------

      /// \brief An implementation of the `size` function in terms of `Set`
      ///
      /// \param p the pointer to a `Set` instance
      /// \return the size of the set; `static_cast<const Set*>(p)->size()`
      template <typename Set>
      static auto set_size(const void* p) -> std::size_t;

      /// \brief An implementation of the `size` function in terms of no object.
      ///        This returns 0
      ///
      /// \note This is the default value for default-construction
      ///
      /// \param p ignored
      /// \return 0
      static constexpr auto null_size(const void* p) -> std::size_t;

      //------------------------------------------------------------------------

      /// \brief An implementation of iteration in terms of `Map`
      ///
      /// \param p the pointer to a `Map` instance
      /// \param cb the callback to invoke
      template <typename Set>
      static constexpr auto set_for_each(const void* p, for_each_callback cb) -> void;

      /// \brief An implementation of iteration in terms of no object
      ///
      /// \note This is the default value for default-construction
      ///
      /// \param p ignored
      /// \param cb ignored
      static constexpr auto null_for_each(const void* p, for_each_callback cb) -> void;
    };

  } // namespace detail

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An immutable view of a set-like container
  ///
  /// This class is similar to the `string_view`-like classes, except it
  /// performs non-owning type-erasuring using template machinery to generate a
  /// "polymorphic"-like behavior.
  ///
  /// Since this is a non-owning view, no mutations are possible to the
  /// underlying reference; though queries can be performed on existence and
  /// size.
  ///
  /// \note
  /// `set_view` is capable of viewing associated containers like `std::map`
  /// in addition to `std::set`/`std::unordered_set`-like classes. In which
  /// cases, it only views the *keys* and not the associated values.
  ///
  /// \tparam T the element type of the set
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class set_view
  {
    //--------------------------------------------------------------------------
    // Public Members
    //--------------------------------------------------------------------------
  public:

    using key_type = T;
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this `set_view` to an empty/non-existent set.
    ///
    /// \post `this->empty()` is `true`
    /// \post `this->contains(anything)` is `false`
    constexpr set_view() noexcept;

    /// \brief Constructs this `set_view` to view \p set
    ///
    /// \param set the set to view.
    ///
    /// \post `this->size() == set.size()`
    /// \post `this->count(v) == set.count(v)`
    template <typename Set>
    constexpr set_view(const Set& set) noexcept;

    /// \brief Constructs this view to view the set viewed by \p other
    ///
    /// \param other the other set to view
    set_view(const set_view& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Rebinds this view to view the set viewed by \p other
    ///
    /// \param other the other set to view
    /// \return reference to `(*this)`
    auto operator=(const set_view& other) -> set_view& = default;

    //--------------------------------------------------------------------------
    // Capacity
    //--------------------------------------------------------------------------
  public:

    /// \brief Queries the number of elements stored within this set
    ///
    /// \return the number of elements in this set
    constexpr auto size() const noexcept -> size_type;

    /// \brief Queries if this set contains any elements
    ///
    /// \return `true` if this set is empty
    constexpr auto empty() const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Lookup
    //--------------------------------------------------------------------------
  public:

    /// \brief Counts the number of occurrences of the specified \p value
    ///
    /// \param value the value to count
    /// \return the number of occurrences
    constexpr auto count(const T& value) const noexcept -> std::size_t;

    /// \brief Checks whether the set contains the specified \p value
    ///
    /// \param value the value to check for
    /// \return `true` if \p value is contained in this set
    constexpr auto contains(const T& value) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Iteration
    //--------------------------------------------------------------------------
  public:

    /// \brief Iterates over all entries in this set, invoking \p cb on each
    ///        entry
    ///
    /// \note There is no way to early-terminate iteration from this function.
    ///
    /// \param cb the callback to invoke on each entry
    template <typename Callback,
              typename = std::enable_if_t<std::is_invocable_v<Callback,const T&>>>
    constexpr auto for_each(Callback&& cb) const -> void;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    using vtable_type = detail::set_view_vtable<T>;

    //------------------------------------------------------------------------
    // Static Constants
    //------------------------------------------------------------------------

    /// \brief A constexpr constant of the null vtable
    static inline constexpr auto null_value = vtable_type{
      &vtable_type::null_count,
      &vtable_type::null_size,
      &vtable_type::null_for_each,
    };

    /// \brief A constexpr constant of a set vtable
    ///
    /// \note
    /// This is not defined as 'auto' because, for some reason, this badly
    /// screws with clang's deduction -- resulting it deducing the type as
    /// "const auto" -- which is apparently not convertible to any type, even
    /// "const set_view_vtable" (which this type actually is).
    template <typename Set>
    static inline constexpr vtable_type value = vtable_type{
      &vtable_type::template set_count<std::decay_t<Set>>,
      &vtable_type::template set_size<std::decay_t<Set>>,
      &vtable_type::template set_for_each<std::decay_t<Set>>,
    };

    const void* m_instance;
    not_null<const vtable_type*> m_vtable;
  };

  template <typename Set>
  set_view(const Set&) -> set_view<typename Set::key_type>;

} // namespace alloy::core

//==============================================================================
// class : set_view_vtable
//==============================================================================

template <typename T>
template <typename Set>
inline
auto alloy::core::detail::set_view_vtable<T>::set_count(
  const void* p,
  const T& value
) -> std::size_t
{
  return static_cast<const Set*>(p)->count(value);
}

template <typename T>
inline constexpr
auto alloy::core::detail::set_view_vtable<T>::null_count(
  const void* p,
  const T& value
) -> std::size_t
{
  compiler::unused(p, value);
  return 0u;
}

//------------------------------------------------------------------------------

template <typename T>
template <typename Set>
inline
auto alloy::core::detail::set_view_vtable<T>::set_size(const void* p)
  -> std::size_t
{
  return static_cast<const Set*>(p)->size();
}

template <typename T>
inline constexpr
auto alloy::core::detail::set_view_vtable<T>::null_size(const void* p)
  -> std::size_t
{
  compiler::unused(p);
  return 0u;
}

//------------------------------------------------------------------------------

template <typename T>
template <typename Set>
inline constexpr
auto alloy::core::detail::set_view_vtable<T>::set_for_each(
  const void* p,
  for_each_callback cb
) -> void
{
  // Sets have the same `key_type` and `value_type`
  if constexpr (std::is_same_v<typename Set::key_type, typename Set::value_type>) {
    for (const auto& value : *static_cast<const Set*>(p)) {
      cb(value);
    }
  } else { // Maps only provide the key as the first element
    for (const auto& pair : *static_cast<const Set*>(p)) {
      cb(pair.first);
    }
  }
}

template <typename T>
inline constexpr
auto alloy::core::detail::set_view_vtable<T>::null_for_each(
  const void* p,
  for_each_callback cb
) -> void
{
  compiler::unused(p, cb); // do nothing
}

//==============================================================================
// class : set_view
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE constexpr
alloy::core::set_view<T>::set_view()
  noexcept
  : m_instance{nullptr},
    m_vtable{assume_not_null(&null_value)}
{

}

template <typename T>
template <typename Set>
ALLOY_FORCE_INLINE constexpr
alloy::core::set_view<T>::set_view(const Set& set)
  noexcept
  : m_instance{std::addressof(set)},
    m_vtable{assume_not_null(&value<Set>)}
{

}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::set_view<T>::size()
  const noexcept -> size_type
{
  return (*m_vtable->size_function)(m_instance);
}

template <typename T>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::set_view<T>::empty()
  const noexcept -> bool
{
  return size() == 0u;
}

//------------------------------------------------------------------------------
// Lookup
//------------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::set_view<T>::count(const T& value)
  const noexcept -> std::size_t
{
  return (*m_vtable->count_function)(m_instance, value);
}

template <typename T>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::set_view<T>::contains(const T& value)
  const noexcept -> bool
{
  // This is pessimized for multisets, since multisets would be counting each
  // instance rather than a singular one. However, multisets are seldomly
  // used, and so this should be mostly negligible
  return count(value) > 0u;
}

//------------------------------------------------------------------------------
// Iteration
//------------------------------------------------------------------------------

template <typename T>
template <typename Callback, typename>
inline constexpr
auto alloy::core::set_view<T>::for_each(Callback&& cb)
  const -> void
{
  using callback_type = typename vtable_type::for_each_callback;

  // Use a local-capture delegate, since the scope does not exceed this function
  const auto callback = callback_type::make(&cb);
  (*m_vtable->for_each_function)(m_instance, callback);
}

#endif /* ALLOY_CORE_CONTAINERS_SET_VIEW_HPP */
