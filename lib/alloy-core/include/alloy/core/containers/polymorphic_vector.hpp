/*****************************************************************************
 * \file polymorphic_vector.hpp
 * 
 * \brief This header defines the container `ptr_container` which allows for
 *        storing heterogenous types in a homogeneous way.
 *****************************************************************************/
 
/*
 The MIT License (MIT)
 Copyright (c) 2021 Matthew Rodusek. All rights reserved.
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the \Software\), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/
#ifndef ALLOY_CORE_CONTAINERS_POLYMORPHIC_VECTOR_HPP
#define ALLOY_CORE_CONTAINERS_POLYMORPHIC_VECTOR_HPP

#if defined(_MSC_VER)
# pragma once
#endif 

#include "alloy/core/containers/vector.hpp"
#include "alloy/core/memory/allocator.hpp"

#include <type_traits> // std::add_const, std::enable_if
#include <iterator>    // std::reverse_iterator
#include <cstddef>     // std::size_t, std::ptrdiff_t

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A functor for cloning `p` objects by calling a `clone` function
  ///
  /// This will allocate the clone using the specified allocator
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  struct default_allocator_clone
  {
    auto operator()(allocator alloc, const T& p) const noexcept -> T*;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A vector that stores polymorphic datatype
  ///
  /// This is roughly equivalent to a `vector<unique_ptr<Interface>>` where
  /// `unique_ptr` reuses the allocator from `vector`.
  ///
  /// Pointers to the objects themselves are stored within this container
  /// contiguously, but the objects themselves are allocated independently since
  /// the intent is to support derived types of arbitrary and differing sizes.
  ///
  /// `polymorphic_vector` does not require that the underlying types have a
  /// `virtual` destructor, as the correct type will be cast to during
  /// destruction/deallocation.
  ///
  /// \note
  /// Because of the way this class is implemented, if the underlying allocator
  /// distributes memory contiguously, and this vector never removes any
  /// elements, then the pointer data will be contiguous right alongside all
  /// allocated elements -- which can allow for efficient cache-locality.
  /// Consider using this in algorithms that don't require emptying any storage.
  ///
  /// \tparam Interface
  //////////////////////////////////////////////////////////////////////////////
  template <typename Interface>
  class polymorphic_vector
  {
    // Unfortunately we cannot test `is_abstract` since it returns `false` for
    // classes with non pure-virtual functions.
    static_assert(std::is_class_v<Interface>);

    template <typename T>
    class iterator_impl;

    friend iterator_impl<Interface>;
    friend iterator_impl<const Interface>;

    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using value_type      = Interface;
    using reference       = value_type&;
    using const_reference = std::add_const_t<value_type>&;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer         = value_type*;
    using const_pointer   = std::add_const_t<value_type>&;

    using iterator               = iterator_impl<value_type>;
    using const_iterator         = iterator_impl<const value_type>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using allocator_type = allocator;

    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an empty container
    ///
    /// \post `size() == 0`, `capacity() == 0`
    polymorphic_vector() noexcept;

    /// \brief Constructs a container that uses \p alloc as the underlying
    ///        allocator
    ///
    /// \post `size() == 0`, `capacity() == 0`
    /// \param alloc the allocator to use
    explicit polymorphic_vector(allocator alloc) noexcept;

    /// \brief Constructs a container contains \p n instances of \p copy
    ///
    /// \param n the number to copy
    /// \param copy the value to copy into the container
    template <typename Derived,
              typename = std::enable_if_t<std::is_base_of_v<Interface,Derived>>>
    polymorphic_vector(size_type n, const Derived& copy);

    /// \brief Moves the contents of \p other
    ///
    /// \param other the other container to copy
    polymorphic_vector(polymorphic_vector&& other) noexcept;

    //--------------------------------------------------------------------------

    ~polymorphic_vector();

    //--------------------------------------------------------------------------

    auto operator=(polymorphic_vector&& other) noexcept -> polymorphic_vector&;

    /// \brief Gets the underlying allocator
    ///
    /// \return the underlying allocator
    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator;

    /// \brief Produces a deep copy of this polymorphic_vector by cloning each
    ///        object with \p cloner and storing all data in \p alloc
    ///
    /// \param alloc the allocator to copy with
    /// \param cloner the policy for cloning objects
    /// \return the cloned vector
    template <typename Cloner = default_allocator_clone<Interface>>
    [[nodiscard]]
    auto copy_with(allocator alloc, const Cloner& cloner = Cloner{}) const -> polymorphic_vector;

    /// \brief Produces a deep copy of this polymorphic_vector by cloning each
    ///        object with \p cloner
    ///
    /// \param cloner the policy for cloning objects
    /// \return the cloned vector
    template <typename Cloner = default_allocator_clone<Interface>>
    [[nodiscard]]
    auto copy(const Cloner& cloner = Cloner{}) const -> polymorphic_vector;

    //--------------------------------------------------------------------------
    // Capacity
    //--------------------------------------------------------------------------
  public:

    /// \brief Reserves space for \p n elements
    ///
    /// \note
    /// Unlike conventional `vector::reserve`, this does not reserve space for
    /// the derived objects themselves -- since the exact size of the derived
    /// objects is not yet known.
    ///
    /// \param n the number of elements to reserve
    auto reserve(size_type n) -> void;

    /// \brief Gets the number of elements stored in this vector
    ///
    /// \return the number of elements
    [[nodiscard]]
    auto size() const noexcept -> size_type;

    /// \brief Gets the max number of elements that can be stored in this vector
    ///
    /// \return the max number of elements
    [[nodiscard]]
    auto max_size() const noexcept -> size_type;

    /// \brief Gets the capacity of the underlying vector
    ///
    /// \return the capacity
    [[nodiscard]]
    auto capacity() const noexcept -> size_type;

    /// \brief Queries if the underlying vector is empty
    ///
    /// \return `true` if empty
    [[nodiscard]]
    auto empty() const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Clears all elements out of this vector
    ///
    /// \post `size() == 0u`, `capacity()` is unchanged
    auto clear() noexcept -> void;

    /// \brief Emplace an object of type \p Derived into this vector
    ///
    /// \tparam Derived the type to construct
    /// \param args the arguments to forward to `Derived`
    /// \return a reference to the `Derived` object
    template <typename Derived, typename...Args,
              typename = std::enable_if_t<(
                std::is_base_of_v<Interface,Derived> &&
                std::is_constructible_v<Derived,Args...>
              )>>
    auto emplace_back(Args&&...args) -> Derived&;

    /// \brief Emplaces an object at the specified position \p pos
    ///
    /// \tparam Derived the type to construct
    /// \param pos the position to construct the element at
    /// \param args the arguments to forward to `Derived`
    /// \return an iterator after the constructed element
    template <typename Derived, typename...Args,
              typename = std::enable_if_t<(
                std::is_base_of_v<Interface,Derived> &&
                std::is_constructible_v<Derived,Args...>
              )>>
    auto emplace(const_iterator pos, Args&&...args) -> const_iterator;

    /// \brief Removes the last element in this vector
    ///
    /// \pre `size()` is greater than 0
    /// \post `size()` is decreased by 1
    auto pop_back() -> void;

    /// \brief Swaps the contents of `this` with \p other
    ///
    /// \param other the other vector to swap with
    auto swap(polymorphic_vector& other) noexcept -> void;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets an element to the nth element in this vector
    ///
    /// \throw std::out_of_range if \p n exceeds `size()`
    /// \param n the index
    /// \return a reference to the nth element
    [[nodiscard]]
    auto at(size_type n) -> reference;
    [[nodiscard]]
    auto at(size_type n) const -> const_reference;
    /// \}

    /// \{
    /// \brief Gets an element to the nth element in this vector
    ///
    /// \pre \p n is less than `size()`
    /// \param n the index
    /// \return a reference to the nth element
    [[nodiscard]]
    auto operator[](size_type n) noexcept -> reference;
    [[nodiscard]]
    auto operator[](size_type n) const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets a reference to the front element in this container
    ///
    /// \pre `size()` is greater than `0`
    /// \return a reference to the first element
    [[nodiscard]]
    auto front() noexcept -> reference;
    [[nodiscard]]
    auto front() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets a reference to the last element in this container
    ///
    /// \pre `size()` is greater than `0`
    /// \return a reference to the last element
    [[nodiscard]]
    auto back() noexcept -> reference;
    [[nodiscard]]
    auto back() const noexcept -> const_reference;
    /// \}

    //--------------------------------------------------------------------------
    // Iterators
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets an iterator to the start of the container
    ///
    /// \return the iterator
    [[nodiscard]]
    auto begin() noexcept -> iterator;
    [[nodiscard]]
    auto begin() const noexcept -> const_iterator;
    [[nodiscard]]
    auto cbegin() const noexcept -> const_iterator;
    /// \}

    /// \{
    /// \brief Gets an iterator to the end of the container
    ///
    /// \return the iterator
    [[nodiscard]]
    auto end() noexcept -> iterator;
    [[nodiscard]]
    auto end() const noexcept -> const_iterator;
    [[nodiscard]]
    auto cend() const noexcept -> const_iterator;
    /// \}

    /// \{
    /// \brief Gets a reverse iterator to the start of the container
    ///
    /// \return the iterator
    [[nodiscard]]
    auto rbegin() noexcept -> reverse_iterator;
    [[nodiscard]]
    auto rbegin() const noexcept -> const_reverse_iterator;
    [[nodiscard]]
    auto crbegin() const noexcept -> const_reverse_iterator;
    /// \}

    /// \{
    /// \brief Gets an iterator to the end of the container
    ///
    /// \return the iterator
    [[nodiscard]]
    auto rend() noexcept -> reverse_iterator;
    [[nodiscard]]
    auto rend() const noexcept -> const_reverse_iterator;
    [[nodiscard]]
    auto crend() const noexcept -> const_reverse_iterator;
    /// \}

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    using destroy_function = void(*)(allocator, Interface*);

    struct storage {
      Interface* pointer;
      destroy_function destroy;
    };

    vector<storage> m_entries;
  };

  template <typename Interface>
  template <typename T>
  class polymorphic_vector<Interface>::iterator_impl
  {
    using underlying_iterator = std::conditional_t<
      std::is_const_v<T>,
      typename vector<storage>::const_iterator,
      typename vector<storage>::iterator
    >;

    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using value_type = std::reference_wrapper<T>;
    using reference  = T&;
    using pointer    = T*;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    using iterator_category = std::random_access_iterator_tag;
    using iterator_concept  = std::random_access_iterator_tag;

    //--------------------------------------------------------------------------
    // Constructor / Destructor / Assignment
    //--------------------------------------------------------------------------

    iterator_impl() = default;
    explicit iterator_impl(underlying_iterator self) noexcept;
    iterator_impl(const iterator_impl& other) = default;

    //--------------------------------------------------------------------------

    auto operator=(const iterator_impl& other) -> iterator_impl& = default;

    //--------------------------------------------------------------------------
    // Iteration
    //--------------------------------------------------------------------------
  public:

    auto operator++() noexcept -> iterator_impl&;
    auto operator++(int) noexcept -> iterator_impl;
    auto operator--() noexcept -> iterator_impl&;
    auto operator--(int) noexcept -> iterator_impl;

    auto operator+=(difference_type n) noexcept -> iterator_impl&;
    auto operator-=(difference_type n) noexcept -> iterator_impl&;

    friend auto operator+(iterator_impl i, difference_type n) -> iterator_impl { return i.m_iterator + n; }
    friend auto operator+(difference_type n, iterator_impl i) -> iterator_impl { return n + i.m_iterator; }
    friend auto operator-(iterator_impl i, difference_type n) -> iterator_impl { return i.m_iterator - n; }
    friend auto operator-(iterator_impl l, iterator_impl r) -> difference_type { return l.m_iterator - r.m_iterator; }

    //--------------------------------------------------------------------------
    // Dereference
    //--------------------------------------------------------------------------
  public:

    auto operator[](difference_type n) const noexcept -> reference;
    auto operator->() const noexcept -> pointer;
    auto operator*() const noexcept -> reference;

    //--------------------------------------------------------------------------
    // Comparison
    //--------------------------------------------------------------------------
  public:

    auto operator==(const iterator_impl& other) const noexcept -> bool;
    auto operator!=(const iterator_impl& other) const noexcept -> bool;
    auto operator<(const iterator_impl& other) const noexcept -> bool;
    auto operator>(const iterator_impl& other) const noexcept -> bool;
    auto operator<=(const iterator_impl& other) const noexcept -> bool;
    auto operator>=(const iterator_impl& other) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    underlying_iterator m_iterator;

    friend polymorphic_vector<Interface>;
  };

  //============================================================================
  // non-member functions : class : polymorphic_vector
  //============================================================================

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  template <typename T>
  auto swap(polymorphic_vector<T>& lhs, polymorphic_vector<T>& rhs) noexcept -> void;

  //----------------------------------------------------------------------------
  // Comparison
  //----------------------------------------------------------------------------

  template <typename T>
  auto operator==(const polymorphic_vector<T>& lhs,
                  const polymorphic_vector<T>& rhs) noexcept -> bool;
  template <typename T>
  auto operator!=(const polymorphic_vector<T>& lhs,
                  const polymorphic_vector<T>& rhs) noexcept -> bool;
  template <typename T>
  auto operator<(const polymorphic_vector<T>& lhs,
                 const polymorphic_vector<T>& rhs) noexcept -> bool;
  template <typename T>
  auto operator>(const polymorphic_vector<T>& lhs,
                 const polymorphic_vector<T>& rhs) noexcept -> bool;
  template <typename T>
  auto operator<=(const polymorphic_vector<T>& lhs,
                  const polymorphic_vector<T>& rhs) noexcept -> bool;
  template <typename T>
  auto operator>=(const polymorphic_vector<T>& lhs,
                  const polymorphic_vector<T>& rhs) noexcept -> bool;

} // namespace alloy::core

template <typename T>
auto alloy::core::default_allocator_clone<T>::operator()(allocator alloc, const T& p)
  const noexcept -> T*
{
  return p.clone_with(alloc);
}

//==============================================================================
// class : polymorphic_vector::iterator_impl
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

template <typename Interface>
template <typename T>
inline
alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::iterator_impl(underlying_iterator self)
  noexcept
  : m_iterator{self}
{

}

//------------------------------------------------------------------------------
// Iteration
//------------------------------------------------------------------------------

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator++()
  noexcept -> iterator_impl&
{
  ++m_iterator;
  return (*this);
}

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator++(int)
  noexcept -> iterator_impl
{
  auto copy = (*this);
  ++(*this);
  return copy;
}

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator--()
  noexcept -> iterator_impl&
{
  --m_iterator;
  return (*this);
}

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator--(int)
  noexcept -> iterator_impl
{
  auto copy = (*this);
  --(*this);
  return copy;
}

//------------------------------------------------------------------------------

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator+=(difference_type n)
  noexcept -> iterator_impl&
{
  m_iterator += n;
  return (*this);
}

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator-=(difference_type n)
  noexcept -> iterator_impl&
{
  m_iterator -= n;
  return (*this);
}

//------------------------------------------------------------------------------
// Dereference
//------------------------------------------------------------------------------

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator[](difference_type n)
  const noexcept -> reference
{
  return m_iterator[n];
}

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator->()
  const noexcept -> pointer
{
  return (*m_iterator).pointer;
}

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator*()
  const noexcept -> reference
{
  return (*(*m_iterator).pointer);
}

//------------------------------------------------------------------------------
// Comparison
//------------------------------------------------------------------------------

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator==(const iterator_impl& other)
  const noexcept -> bool
{
  return m_iterator == other.m_iterator;
}

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator!=(const iterator_impl& other)
  const noexcept -> bool
{
  return m_iterator != other.m_iterator;
}

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator<(const iterator_impl& other)
  const noexcept -> bool
{
  return m_iterator < other.m_iterator;
}

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator>(const iterator_impl& other)
const noexcept -> bool
{
  return m_iterator > other.m_iterator;
}

template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator<=(const iterator_impl& other)
  const noexcept -> bool
{
  return m_iterator <= other.m_iterator;
}
template <typename Interface>
template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::iterator_impl<T>::operator>=(const iterator_impl& other)
  const noexcept -> bool
{
  return m_iterator >= other.m_iterator;
}

//==============================================================================
// class : polymorphic_vector
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

template <typename Interface>
inline
alloy::core::polymorphic_vector<Interface>::polymorphic_vector()
  noexcept
  : m_entries{}
{

}

template <typename Interface>
inline
alloy::core::polymorphic_vector<Interface>::polymorphic_vector(allocator alloc)
  noexcept
  : m_entries{stl_allocator_adapter<storage>{alloc}}
{

}

template <typename Interface>
template <typename Derived, typename>
inline
alloy::core::polymorphic_vector<Interface>::polymorphic_vector(
  size_type n, const Derived& copy
) : m_entries{}
{
  reserve(n);

  for (auto i = 0u; i < n; ++i) {
    emplace_back<Derived>(copy);
  }
}


template <typename Interface>
inline
alloy::core::polymorphic_vector<Interface>::polymorphic_vector(polymorphic_vector&& other)
noexcept
  : m_entries{std::move(other).m_entries}
{
  // Clear, to prevent polymorphic vector from attempting to iterate.
  other.m_entries.clear();
}

//------------------------------------------------------------------------------

template <typename Interface>
inline
alloy::core::polymorphic_vector<Interface>::~polymorphic_vector<Interface>()
{
  clear();
}

//------------------------------------------------------------------------------

template <typename Interface>
inline
auto alloy::core::polymorphic_vector<Interface>::operator=(polymorphic_vector&& other)
  noexcept -> polymorphic_vector&
{
  swap(other);
  other.clear();

  return (*this);
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::get_allocator()
  const noexcept -> class allocator
{
  return m_entries.get_allocator().underlying();
}

template <typename Interface>
template <typename Cloner>
auto alloy::core::polymorphic_vector<Interface>::copy(const Cloner& cloner)
const -> polymorphic_vector
{
  return copy_with(get_allocator(), cloner);
}

template <typename Interface>
template <typename Cloner>
auto alloy::core::polymorphic_vector<Interface>::copy_with(
  allocator alloc, const Cloner& cloner
) const -> polymorphic_vector
{
  auto copy = polymorphic_vector{alloc};

  auto& entries = copy.m_entries;
  entries.reserve(size());

  for (const auto& entry : m_entries) {
    const auto& value   = (*entry.pointer);
    const auto& destroy = (*entry.destroy);

    auto* p = cloner(alloc, value);
    entries.push_back(storage{p, destroy});
  }

  return copy;
}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::reserve(size_type n)
  -> void
{
  m_entries.reserve(n);
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::size()
  const noexcept -> size_type
{
  return m_entries.size();
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::max_size()
const noexcept -> size_type
{
  return m_entries.max_size();
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::capacity()
  const noexcept -> size_type
{
  return m_entries.capacity();
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::empty()
  const noexcept -> bool
{
  return m_entries.empty();
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <typename Interface>
inline
auto alloy::core::polymorphic_vector<Interface>::clear()
  noexcept -> void
{
  auto alloc = get_allocator();
  for (auto& entry : m_entries) {
    (*entry.destroy)(alloc, entry.pointer);
  }
  m_entries.clear();
}

template <typename Interface>
template <typename Derived, typename... Args, typename>
inline
auto alloy::core::polymorphic_vector<Interface>::emplace_back(Args&&...args)
  -> Derived&
{
  const auto p = allocator().make<Derived>(std::forward<Args>(args)...);
  const auto destroy = static_cast<destroy_function>([](allocator alloc, Interface* p){
    alloc.dispose<Derived>(static_cast<Derived*>(p));
  });

  m_entries.push_back(storage{p, destroy});
  return *p;
}

template <typename Interface>
template <typename Derived, typename... Args, typename>
inline
auto alloy::core::polymorphic_vector<Interface>::emplace(const_iterator pos, Args&&...args)
  -> const_iterator
{
  const auto p = allocator().make<Derived>(std::forward<Args>(args)...);
  const auto destroy = static_cast<destroy_function>([](allocator alloc, Interface* p){
    alloc.dispose<Derived>(static_cast<Derived*>(p));
  });

  const auto result = m_entries.insert(pos.m_iterator, storage{p,destroy});
  return const_iterator{result};
}

template <typename Interface>
inline
auto alloy::core::polymorphic_vector<Interface>::pop_back()
  -> void
{
  auto& last = m_entries.back();
  (*last.destroy)(get_allocator(), last.pointer);

  m_entries.pop_back();
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::swap(polymorphic_vector& other)
  noexcept -> void
{
  m_entries.swap(other.m_entries);
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::at(size_type n)
  -> reference
{
  return (*m_entries.at(n).pointer);
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::at(size_type n)
  const -> const_reference
{
  return (*m_entries.at(n).pointer);
}

//------------------------------------------------------------------------------

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::operator[](size_type n)
  noexcept -> reference
{
  return (*m_entries[n].pointer);
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::operator[](size_type n)
  const noexcept -> const_reference
{
  return (*m_entries[n].pointer);
}

//------------------------------------------------------------------------------

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::front()
  noexcept -> reference
{
  return (*m_entries.front().pointer);
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::front()
  const noexcept -> const_reference
{
  return (*m_entries.front().pointer);
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::back()
  noexcept -> reference
{
  return (*m_entries.back().pointer);
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::back()
  const noexcept -> const_reference
{
  return (*m_entries.back().pointer);
}

//------------------------------------------------------------------------------
// Iterators
//------------------------------------------------------------------------------

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::begin()
  noexcept -> iterator
{
  return iterator();
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::begin()
  const noexcept -> const_iterator
{
  return cbegin();
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::cbegin()
  const noexcept -> const_iterator
{
  return const_iterator();
}

//------------------------------------------------------------------------------

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::end()
  noexcept -> iterator
{
  return iterator();
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::end()
  const noexcept -> const_iterator
{
  return cend();
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::cend()
  const noexcept -> const_iterator
{
  return const_iterator();
}

//------------------------------------------------------------------------------

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::rbegin()
  noexcept -> reverse_iterator
{
  return std::make_reverse_iterator(end());
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::rbegin()
  const noexcept -> const_reverse_iterator
{
  return crbegin();
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::crbegin()
  const noexcept -> const_reverse_iterator
{
  return std::make_reverse_iterator(cend());
}

//------------------------------------------------------------------------------

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::rend()
  noexcept -> reverse_iterator
{
  return std::make_reverse_iterator(begin());
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::rend()
  const noexcept -> const_reverse_iterator
{
  return crend();
}

template <typename Interface>
ALLOY_FORCE_INLINE
auto alloy::core::polymorphic_vector<Interface>::crend()
  const noexcept -> const_reverse_iterator
{
  return std::make_reverse_iterator(cbegin());
}

//==============================================================================
// non-member functions : class : polymorphic_vector
//==============================================================================

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::swap(polymorphic_vector<T>& lhs, polymorphic_vector<T>& rhs)
  noexcept -> void
{
  lhs.swap(rhs);
}

//------------------------------------------------------------------------------
// Comparison
//------------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::operator==(
  const polymorphic_vector<T>& lhs,
  const polymorphic_vector<T>& rhs
) noexcept -> bool
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::operator!=(
  const polymorphic_vector<T>& lhs,
  const polymorphic_vector<T>& rhs
) noexcept -> bool
{
  return !(lhs == rhs);
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::operator<(
  const polymorphic_vector<T>& lhs,
  const polymorphic_vector<T>& rhs
) noexcept -> bool
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::operator>(
  const polymorphic_vector<T>& lhs,
  const polymorphic_vector<T>& rhs
) noexcept -> bool
{
  return (rhs < lhs);
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::operator<=(
  const polymorphic_vector<T>& lhs,
  const polymorphic_vector<T>& rhs
) noexcept -> bool
{
  return !(rhs < lhs);
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::operator>=(
  const polymorphic_vector<T>& lhs,
  const polymorphic_vector<T>& rhs
) noexcept -> bool
{
  return !(lhs < rhs);
}

#endif /* ALLOY_CORE_CONTAINERS_POLYMORPHIC_VECTOR_HPP */
