/*****************************************************************************
 * \file map_view.hpp
 *
 * \brief This header contains the definition of a non-owning view of map
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
#include "alloy/core/utilities/result.hpp"
#include "alloy/core/utilities/delegate.hpp"

#include <cstddef>     // std::size_t
#include <type_traits> // std::decay_t
#include <memory>      // std::addressof

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Errors that can be triggered from lookup
  //////////////////////////////////////////////////////////////////////////////
  enum class map_view_lookup_error {
    none=0,
    key_not_found=1,
  };

  namespace detail {

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A home-brew constructed Vtable for map-like objects
    ///
    /// This is used to produce a "virtual-like" behaviour from different maps,
    /// even though they are not part of the same hierarchy. This view only
    /// acts on the immutable portions of the API.
    ///
    /// \tparam T the underlying `value_type` or `key_type` of the map
    ////////////////////////////////////////////////////////////////////////////
    template <typename K, typename V>
    struct map_view_vtable final
    {
      using for_each_callback = delegate<void(const K&, const V&)>;

      //------------------------------------------------------------------------
      // Function Types
      //------------------------------------------------------------------------

      // Note: These two functions currently make up the definition of `empty`
      //       and `contains` as well, both of which could be written more
      //       optimally as separate hooks -- at the expense of additional
      //       storage space for these functions.
      //
      //       Since most containers require constant-time lookup on `size()`,
      //       and multimaps are seldomly used, these two functions should pose
      //       negligible (likely zero) cost.
      using count_function_type = auto (*)(const void*,const K&) -> std::size_t;
      using size_function_type = auto (*)(const void*) -> std::size_t;
      using at_function_type = auto (*)(const void*, const K&) -> result<const V&,map_view_lookup_error>;
      using for_each_function_type = auto(*)(const void*, for_each_callback) -> void;

      //------------------------------------------------------------------------
      // Functions
      //------------------------------------------------------------------------

      count_function_type count_function;
      size_function_type size_function;
      at_function_type at_function;
      for_each_function_type for_each_function;

      //------------------------------------------------------------------------
      // Stubs
      //------------------------------------------------------------------------

      /// \brief An implementation of the `count` function in terms of `Map`
      ///
      /// \param p the pointer to a Map instance
      /// \param key the value
      /// \return `static_cast<const Map*>(p)->count(value)`
      template <typename Map>
      static constexpr auto map_count(const void* p, const K& key) -> std::size_t;

      /// \brief An implementation of the `count` function in terms of no
      ///        object. This returns 0
      ///
      /// \note This is the default value for default-construction
      ///
      /// \param p ignored
      /// \param key ignored
      /// \return 0
      static constexpr auto null_count(const void* p, const K& key) -> std::size_t;

      //------------------------------------------------------------------------

      /// \brief An implementation of the `size` function in terms of `Map`
      ///
      /// \param p the pointer to a `Map` instance
      /// \return the size of the map; `static_cast<const Map*>(p)->size()`
      template <typename Map>
      static constexpr auto map_size(const void* p) -> std::size_t;

      /// \brief An implementation of the `size` function in terms of no object.
      ///        This returns 0
      ///
      /// \note This is the default value for default-construction
      ///
      /// \param p ignored
      /// \return 0
      static constexpr auto null_size(const void* p) -> std::size_t;

      //------------------------------------------------------------------------

      /// \brief An implementation of the `at(...)` function in terms of `Map`
      ///
      /// \param p the pointer to a `Map` instance
      /// \param key the key to the map entry
      /// \return the map value, if found; an error otherwise
      template <typename Map>
      static constexpr auto map_at(const void* p, const K& key) -> result<const V&,map_view_lookup_error>;

      /// \brief An implementation of the `at(...)` function in terms of no
      ///        object.
      ///
      /// \note This is the default value for default-construction
      ///
      /// \param p ignored
      /// \param key ignored
      /// \return an error
      static constexpr auto null_at(const void* p, const K& key) -> result<const V&,map_view_lookup_error>;

      //------------------------------------------------------------------------

      /// \brief An implementation of iteration in terms of `Map`
      ///
      /// \param p the pointer to a `Map` instance
      /// \param cb the callback to invoke
      template <typename Map>
      static constexpr auto map_for_each(const void* p, for_each_callback cb) -> void;

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
  /// \brief An immutable view of a map-like container
  ///
  /// This class is similar to the `string_view`-like classes, except it
  /// performs non-owning type-erasuring using template machinery to generate a
  /// "polymorphic"-like behavior.
  ///
  /// Since this is a non-owning view, no mutations are possible to the
  /// underlying reference; though queries can be performed on existence and
  /// size.
  //////////////////////////////////////////////////////////////////////////////
  template <typename K, typename V>
  class map_view
  {
    //--------------------------------------------------------------------------
    // Public Members
    //--------------------------------------------------------------------------
  public:

    using key_type = K;
    using mapped_type = V;
    using value_type = std::pair<const K, V>;
    using reference = value_type&;
    using const_reference = const value_type&;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this `map_view` to an empty/non-existent map.
    ///
    /// \post `this->empty()` is `true`
    /// \post `this->contains(anything)` is `false`
    constexpr map_view() noexcept;

    /// \brief Constructs this `map_view` to view \p map
    ///
    /// \param map the map to view.
    ///
    /// \post `this->size() == map.size()`
    /// \post `this->count(v) == map.count(v)`
    template <typename Map>
    constexpr map_view(const Map& map) noexcept;

    /// \brief Constructs this view to view the map viewed by \p other
    ///
    /// \param other the other map to view
    map_view(const map_view& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Rebinds this view to view the map viewed by \p other
    ///
    /// \param other the other map to view
    /// \return reference to `(*this)`
    auto operator=(const map_view& other) -> map_view& = default;

    //--------------------------------------------------------------------------
    // Capacity
    //--------------------------------------------------------------------------
  public:

    /// \brief Queries the number of elements stored within this map
    ///
    /// \return the number of elements in this map
    constexpr auto size() const noexcept -> size_type;

    /// \brief Queries if this map contains any elements
    ///
    /// \return `true` if this map is empty
    constexpr auto empty() const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Lookup
    //--------------------------------------------------------------------------
  public:

    /// \brief Counts the number of occurrences of the specified \p key
    ///
    /// \param key the value to count
    /// \return the number of occurrences
    constexpr auto count(const key_type& key) const noexcept -> std::size_t;

    /// \brief Checks whether the map contains the specified \p key
    ///
    /// \param key the value to check for
    /// \return `true` if \p key is contained in this map
    constexpr auto contains(const key_type& key) const noexcept -> bool;

    /// \brief Retrieves the value for the given \p key, if one exists
    ///
    /// \param key the key to retrieve
    /// \return the value at the given key, if it exists, otherwise an error
    constexpr auto at(const key_type& key) const noexcept -> result<const V&,map_view_lookup_error>;

    //--------------------------------------------------------------------------
    // Iteration
    //--------------------------------------------------------------------------
  public:

    /// \brief Iterates over all entries in this map, invoking \p cb on each
    ///        entry
    ///
    /// \note There is no way to early-terminate iteration from this function.
    ///
    /// \param cb the callback to invoke on each entry
    template <typename Callback,
              typename = std::enable_if_t<std::is_invocable_v<Callback,const K&, const V&>>>
    constexpr auto for_each(Callback&& cb) const -> void;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    using vtable_type = detail::map_view_vtable<K,V>;

    /// \brief A constexpr constant of the null vtable
    static inline constexpr auto null_value = vtable_type {
      &vtable_type::null_count,
      &vtable_type::null_size,
      &vtable_type::null_at,
      &vtable_type::null_for_each,
    };

    /// \brief A constexpr constant of a map vtable
    ///
    /// \note
    /// This is not defined as 'auto' because, for some reason, this badly
    /// screws with clang's deduction -- resulting it deducing the type as
    /// "const auto" -- which is apparently not convertible to any type, even
    /// "const map_view_vtable" (which this type actually is).
    template <typename Map>
    static inline constexpr vtable_type value = vtable_type {
      &vtable_type::template map_count<std::decay_t<Map>>,
      &vtable_type::template map_size<std::decay_t<Map>>,
      &vtable_type::template map_at<std::decay_t<Map>>,
      &vtable_type::template map_for_each<std::decay_t<Map>>,
    };

    const void* m_instance;
    not_null<const vtable_type*> m_vtable;
  };

  template <typename Map>
  map_view(const Map&) -> map_view<typename Map::key_type, typename Map::mapped_type>;

} // namespace alloy::core

//==============================================================================
// class : map_view_vtable
//==============================================================================

template <typename K, typename V>
template <typename Map>
inline constexpr
auto alloy::core::detail::map_view_vtable<K,V>::map_count(
  const void* p,
  const K& key
) -> std::size_t
{
  return static_cast<const Map*>(p)->count(key);
}

template <typename K, typename V>
inline constexpr
auto alloy::core::detail::map_view_vtable<K,V>::null_count(
  const void* p,
  const K& key
) -> std::size_t
{
  compiler::unused(p, key);
  return 0u;
}

//------------------------------------------------------------------------------

template <typename K, typename V>
template <typename Map>
inline constexpr
auto alloy::core::detail::map_view_vtable<K,V>::map_size(const void* p)
  -> std::size_t
{
  return static_cast<const Map*>(p)->size();
}

template <typename K, typename V>
inline constexpr
auto alloy::core::detail::map_view_vtable<K,V>::null_size(const void* p)
  -> std::size_t
{
  compiler::unused(p);
  return 0u;
}

//------------------------------------------------------------------------------

template <typename K, typename V>
template <typename Map>
inline constexpr
auto alloy::core::detail::map_view_vtable<K,V>::map_at(const void* p, const K& key)
  -> result<const V&,map_view_lookup_error>
{
  const auto it = static_cast<const Map*>(p)->find(key);
  if (it != static_cast<const Map*>(p)->end()) {
    return it->second;
  }
  return fail(map_view_lookup_error::key_not_found);
}

template <typename K, typename V>
inline constexpr
auto alloy::core::detail::map_view_vtable<K,V>::null_at(const void* p, const K& key)
  -> result<const V&,map_view_lookup_error>
{
  compiler::unused(p, key);
  return fail(map_view_lookup_error::key_not_found);
}

//------------------------------------------------------------------------------

template <typename K, typename V>
template <typename Map>
inline constexpr
auto alloy::core::detail::map_view_vtable<K,V>::map_for_each(
  const void* p,
  for_each_callback cb
) -> void
{
  for (const auto& pair : *static_cast<const Map*>(p)) {
    cb(pair.first, pair.second);
  }
}

template <typename K, typename V>
inline constexpr
auto alloy::core::detail::map_view_vtable<K,V>::null_for_each(
  const void* p,
  for_each_callback cb
) -> void
{
  compiler::unused(p, cb); // do nothing
}


//==============================================================================
// class : map_view
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename K, typename V>
ALLOY_FORCE_INLINE constexpr
alloy::core::map_view<K,V>::map_view()
  noexcept
  : m_instance{nullptr},
    m_vtable{assume_not_null(&null_value)}
{

}


template <typename K, typename V>
template <typename Map>
ALLOY_FORCE_INLINE constexpr
alloy::core::map_view<K,V>::map_view(const Map& map)
  noexcept
  : m_instance{std::addressof(map)},
    m_vtable{assume_not_null(&value<Map>)}
{

}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

template <typename K, typename V>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::map_view<K,V>::size()
  const noexcept -> size_type
{
  return (*m_vtable->size_function)(m_instance);
}

template <typename K, typename V>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::map_view<K,V>::empty()
  const noexcept -> bool
{
  return size() == 0u;
}

//------------------------------------------------------------------------------
// Lookup
//------------------------------------------------------------------------------

template <typename K, typename V>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::map_view<K,V>::count(const K& key)
  const noexcept -> std::size_t
{
  return (*m_vtable->count_function)(m_instance, key);
}

template <typename K, typename V>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::map_view<K,V>::contains(const K& key)
  const noexcept -> bool
{
  return count(key) > 0u;
}

template <typename K, typename V>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::map_view<K,V>::at(const key_type& key)
  const noexcept -> result<const V&,map_view_lookup_error>
{
  return (*m_vtable->at_function)(m_instance, key);
}

//------------------------------------------------------------------------------
// Iteration
//------------------------------------------------------------------------------

template <typename K, typename V>
template <typename Callback, typename>
inline constexpr
auto alloy::core::map_view<K,V>::for_each(Callback&& cb)
  const -> void
{
  using callback_type = typename vtable_type::for_each_callback;

  // Use a local-capture delegate, since the scope does not exceed this function
  const auto callback = callback_type::make(&cb);
  (*m_vtable->for_each_function)(m_instance, callback);
}

#endif /* ALLOY_CORE_CONTAINERS_SET_VIEW_HPP */
