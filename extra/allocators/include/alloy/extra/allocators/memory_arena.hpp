/*****************************************************************************
 * \file memory_arena.hpp
 *
 * \todo description
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
#ifndef ALLOY_EXTRA_ALLOCATORS_MEMORY_ARENA_HPP
#define ALLOY_EXTRA_ALLOCATORS_MEMORY_ARENA_HPP

#include <cstddef>     // std::size_t
#include <type_traits> // std::is_const_v
#include <memory>      // std::addressof

namespace alloy::extra {

  //============================================================================
  // class : memory_arena
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Wrapper around a block of memory, containing both the size
  ///        and the address of the memory block.
  //////////////////////////////////////////////////////////////////////////////
  class memory_arena
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using pointer = void*;
    using size_type = std::size_t;

    //--------------------------------------------------------------------------
    // Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Creates a memory block from a contiguous sequence container
    ///
    /// \param container the container to create a buffer from
    /// \return a memory block of the sequence container
    template <typename Container>
    static memory_arena from_container(Container& container) noexcept;

    /// \brief Creates a memory block from a raw array
    ///
    /// \param array the underlying array
    /// \return a memory block of the array
    template <typename T, std::size_t N>
    static memory_arena from_array(T(&array)[N]) noexcept;

    /// \brief Creates a memory block from a pointer and size
    ///
    /// \param data a pointer to the underlying data
    /// \param size the size of the array
    /// \return a memory block of the contiguous sequence
    template <typename T>
    static memory_arena from_data(T* data, size_type size) noexcept;

    /// \brief Creates a memory block by reading its object representation
    ///
    /// \param object the object to view
    /// \return a memory block from the object
    template <typename T>
    static memory_arena from_object(T& object) noexcept;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a memory block pointing to null with 0 size
    constexpr memory_arena() noexcept;

    /// \brief Constructs a memory_arena at the specified memory address,
    ///        \p data, with the specified size \p size
    ///
    /// \param data The memory address to start the block
    /// \param size The size of the memory block
    constexpr memory_arena(pointer data, size_type size) noexcept;

    /// \brief Copy-constructs a memory_arena by copying another memory_arena
    ///
    /// \param other the other memory_arena to copy
    constexpr memory_arena(const memory_arena& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a memory_arena from another memory_arena
    ///
    /// \param other the other memory_arena to copy
    /// \return reference to \c (*this)
    memory_arena& operator=(const memory_arena& other) noexcept = default;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \brief Retrieves the pointer to the data
    ///
    /// \return the pointer to the data
    constexpr void* data() const noexcept;

    /// \brief Retrieves the start address of this memory block
    ///
    /// This is the same as \c data()
    ///
    /// \return the pointer to the data block
    void* start_address() const noexcept;

    /// \brief Retrieves the end address of this memory block
    ///
    /// \note That the end address is 1 past the end of the memory block
    ///
    /// \return the pointer to the end of the data block
    void* end_address() const noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Retrieves the size of a data
    ///
    /// \return the size of the data
    constexpr std::size_t size() const noexcept;

    /// \brief Evaluates whether \p ptr exists within this memory_arena
    ///
    /// \param ptr the pointer to evaluate
    /// \return \c true whether \p ptr
    bool contains(const void* ptr) const noexcept;

    /// \brief Contextually convertible to \c bool
    constexpr explicit operator bool() const noexcept;

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    pointer m_data;   ///< Pointer to the start of the memory block
    size_type m_size; ///< The size of the memory block
  };

  //============================================================================
  // non-member functions : class : memory_arena
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr bool operator==(const memory_arena& lhs,
                            const memory_arena& rhs) noexcept;
  constexpr bool operator!=(const memory_arena& lhs,
                            const memory_arena& rhs) noexcept;

} // namespace alloy::extra

//==============================================================================
// inline definitions : class : memory_arena
//==============================================================================

//------------------------------------------------------------------------------
// Factories
//------------------------------------------------------------------------------

template<typename Container>
inline alloy::extra::memory_arena
  alloy::extra::memory_arena::from_container(Container& container)
  noexcept
{
  return from_data(container.data(), container.size());
}

template<typename T, std::size_t N>
inline alloy::extra::memory_arena
  alloy::extra::memory_arena::from_array(T(&array)[N])
  noexcept
{
  static_assert(!std::is_const_v<T>);

  return from_data(static_cast<T*>(array), N);
}

template<typename T>
inline alloy::extra::memory_arena
  alloy::extra::memory_arena::from_data(T* data,
                                        size_type size)
  noexcept
{
  static_assert(!std::is_const_v<T>);

  // Pointer is cast to 'std::byte*' before 'void*' so that later casts to
  // std::byte is well-defined (void* can only be cast back to the original
  // T* that produced the void*, anything else is UB -- even though std::byte*
  // can safely alias any type).
  return memory_arena{
    static_cast<void*>(reinterpret_cast<std::byte*>(data)),
    data != nullptr ? sizeof(T) * size : 0u
  };
}

template<typename T>
inline alloy::extra::memory_arena
  alloy::extra::memory_arena::from_object(T& object)
  noexcept
{
  static_assert(!std::is_const_v<T>);

  return from_data(
    std::addressof(object),
    1u
  );
}

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::extra::memory_arena::memory_arena()
  noexcept
  : memory_arena{nullptr,0u}
{

}

inline constexpr alloy::extra::memory_arena::memory_arena(pointer data,
                                                          size_type size)
  noexcept
  : m_data{data},
    m_size{size}
{

}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr alloy::extra::memory_arena::pointer
  alloy::extra::memory_arena::data()
  const noexcept
{
  return m_data;
}


inline void *alloy::extra::memory_arena::start_address()
  const noexcept
{
  return m_data;
}


inline void *alloy::extra::memory_arena::end_address()
  const noexcept
{
  auto* const p = static_cast<std::byte*>(m_data);

  return static_cast<void*>(p + m_size);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr std::size_t alloy::extra::memory_arena::size()
  const noexcept
{
  return m_size;
}


inline bool alloy::extra::memory_arena::contains(const void* ptr)
  const noexcept
{
  return (m_data < end_address() && m_data >= start_address());
}


inline constexpr alloy::extra::memory_arena::operator bool()
  const noexcept
{
  return m_data != nullptr;
}

//==============================================================================
// inline definitions : non-member functions : class : memory_arena
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

inline constexpr bool alloy::extra::operator==(const memory_arena& lhs,
                                               const memory_arena& rhs)
  noexcept
{
  return lhs.data() == rhs.data() && lhs.size() == rhs.size();
}


inline constexpr bool alloy::extra::operator!=(const memory_arena& lhs,
                                               const memory_arena& rhs)
  noexcept
{
  return !(lhs==rhs);
}

#endif /* ALLOY_EXTRA_ALLOCATORS_MEMORY_ARENA_HPP */