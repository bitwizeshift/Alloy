////////////////////////////////////////////////////////////////////////////////
/// \file attribute_buffer_view.hpp
///
/// \brief This header defines a utility for converting contiguous immutable
///        sequences of data into stronger higher-level semantic types
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_CORE_MODEL_ATTRIBUTE_BUFFER_VIEW_HPP
#define ALLOY_CORE_MODEL_ATTRIBUTE_BUFFER_VIEW_HPP

#include "alloy/core/config.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/containers/span.hpp"
#include "alloy/core/precision/real.hpp"
#include "alloy/core/memory/data_quantity.hpp"

#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/model/color.hpp"
#include "alloy/core/model/vertex.hpp"
#include "alloy/core/model/texture_coordinate.hpp"

#include <cstddef> // std::size_t
#include <utility> // std::index_sequence
#include <stdexcept> // std::out_of_range

namespace alloy::core {

  //===========================================================================
  // class : attribute_buffer_view
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A type that allows a contiguous sequence of real instances to be
  ///        interpreted immutably as a different 'T' instance.
  ///
  /// This type exists in order to allow for raw underlying graphics APIs like
  /// Vulkan, Metal, or OpenGL to see a proper contiguous sequence of floating
  /// point values as a higher-level semantic type, like a vector, color, etc.
  ///
  /// This also helps to abstract the otherwise confusing mathematics that
  /// often gets involved when passing these along to shader calls that might
  /// otherwise cause issues.
  ///
  /// \pre sizeof(T) / sizeof(real) must divide evenly
  ///
  /// \pre T must be constructible from (sizeof(T) / sizeof(real)) real
  ///      instances
  ///
  /// \tparam T the type to interpret the reals as
  /////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class attribute_buffer_view
  {
    static_assert(sizeof(T) % sizeof(real) == 0u);

    static inline constexpr auto elements_per_value = sizeof(T) / sizeof(real);

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using underlying_type = const real;
    using value_type = T;
    using reference  = value_type&;
    using pointer    = value_type*;
    using const_reference = const value_type&;
    using const_pointer   = const value_type*;

    using size_type = std::size_t;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default constructs this view without any underlying data
    constexpr attribute_buffer_view() noexcept;

    /// \brief Constructs a attribute_buffer_view from the underlying data and size
    ///
    /// \pre size is divisible by sizeof(T) / sizeof(real)
    ///
    /// \param data the underlying data
    /// \param size the number of reals
    constexpr attribute_buffer_view(const real* data, size_type size) noexcept;

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other array to copy
    constexpr attribute_buffer_view(const attribute_buffer_view& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other array to copy
    /// \return the reference to (*this)
    constexpr attribute_buffer_view& operator=(const attribute_buffer_view& other) noexcept = default;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets a copy of the element at the front element of this span
    ///
    /// \pre empty() is false
    /// \return reference to front element
    constexpr auto front() const noexcept -> value_type;

    /// \brief Gets a copy of the element at the back of this span
    ///
    /// \pre empty() is false
    /// \return reference to back element
    constexpr auto back() const noexcept -> value_type;

    /// \brief Gets a copy of the element at \p idx
    ///
    /// \pre \p idx is less than size()
    /// \param idx the index
    /// \return reference to the element at \p idx
    constexpr auto operator[](std::size_t idx) const noexcept -> value_type;

    /// \brief Gets a copy of the element at \p idx
    ///
    /// \pre \p idx is less than size()
    /// \param idx the index
    /// \return reference to the element at \p idx
    constexpr auto at(std::size_t idx) const -> value_type;

    /// \brief Gets a pointer to the start of the data
    ///
    /// \return pointer to the data
    constexpr auto data() const noexcept -> const real*;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of elements in this span
    ///
    /// \return the number of elements in this span
    constexpr auto size() const noexcept -> size_type;

    /// \brief Gets the size of my attribute buffer in bytes
    ///
    /// \return the number of bytes in this span
    constexpr auto size_bytes() const noexcept -> bytes;

    /// \brief Queries whether this span is empty
    ///
    /// \return true if this span is empty
    constexpr auto empty() const noexcept -> bool;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    span<const real> m_array;

    //-------------------------------------------------------------------------
    // Private Element Access
    //-------------------------------------------------------------------------
  private:

    template <std::size_t...Idx>
    constexpr auto do_get(std::size_t index, std::index_sequence<Idx...>) const noexcept
      -> value_type;

  template <typename U>
  friend constexpr auto operator==(const attribute_buffer_view<U>&,
                                   const attribute_buffer_view<U>&) noexcept -> bool;
  };

  //===========================================================================
  // non-member functions : class : attribute_buffer_view
  //===========================================================================

  //---------------------------------------------------------------------------
  // Equality
  //---------------------------------------------------------------------------

  template <typename T>
  constexpr auto operator==(const attribute_buffer_view<T>& lhs,
                            const attribute_buffer_view<T>& rhs) noexcept -> bool;
  template <typename T>
  constexpr auto operator!=(const attribute_buffer_view<T>& lhs,
                            const attribute_buffer_view<T>& rhs) noexcept -> bool;

  //===========================================================================
  // aliases : attribute_buffer_view
  //===========================================================================

  using vertex_buffer_view             = attribute_buffer_view<vertex>;
  using normal_buffer_view             = attribute_buffer_view<vector3>;
  using color_buffer_view              = attribute_buffer_view<color>;
  using texture_coordinate_buffer_view = attribute_buffer_view<texture_coordinate>;

  //---------------------------------------------------------------------------

//  extern template class attribute_buffer_view<vertex>;
//  extern template class attribute_buffer_view<vector3>;
//  extern template class attribute_buffer_view<color>;
//  extern template class attribute_buffer_view<texture_coordinate>;

} // namespace alloy::core

//=============================================================================
// class : attribute_buffer_view
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

template<typename T>
constexpr alloy::core::attribute_buffer_view<T>
  ::attribute_buffer_view()
  noexcept
  : m_array{}
{

}

template<typename T>
inline constexpr alloy::core::attribute_buffer_view<T>
  ::attribute_buffer_view(const real* data, size_type size)
  noexcept
  : m_array{data, size}
{
  ALLOY_ASSERT((size % elements_per_value) == 0u);
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

template <typename T>
inline constexpr auto
  alloy::core::attribute_buffer_view<T>::front()
  const noexcept
  -> value_type
{
  ALLOY_ASSERT(!empty());

  return (*this)[0];
}

template <typename T>
inline constexpr auto
  alloy::core::attribute_buffer_view<T>::back()
  const noexcept
  -> value_type
{
  ALLOY_ASSERT(!empty());

  return (*this)[size() - 1];
}

template <typename T>
inline constexpr auto
  alloy::core::attribute_buffer_view<T>::operator[](std::size_t idx)
  const noexcept
  -> value_type
{
  ALLOY_ASSERT(idx < size());

  return do_get(idx, std::make_index_sequence<sizeof(T) / sizeof(real)>{});
}

template <typename T>
inline constexpr auto
  alloy::core::attribute_buffer_view<T>::at(std::size_t idx)
  const
  -> value_type
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (idx >= size()) {
    throw std::out_of_range{"attribute_buffer_view::at"};
  }
#else
  ALLOY_ALWAYS_ASSERT(idx < size());
#endif

  return (*this)[idx];
}


template <typename T>
inline constexpr auto
  alloy::core::attribute_buffer_view<T>::data()
  const noexcept
  -> const real*
{
  return m_array.data();
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename T>
inline constexpr auto
  alloy::core::attribute_buffer_view<T>::size()
  const noexcept
  -> size_type
{
  return m_array.size() / elements_per_value;
}

template<typename T>
inline constexpr auto
  alloy::core::attribute_buffer_view<T>::size_bytes()
  const noexcept
  -> bytes
{
  return bytes{m_array.size_bytes()};
}

template <typename T>
inline constexpr auto
  alloy::core::attribute_buffer_view<T>::empty()
  const noexcept
  -> bool
{
  return m_array.empty();
}

//-----------------------------------------------------------------------------
// Private Element Access
//-----------------------------------------------------------------------------

template <typename T>
template <size_t... Idx>
inline constexpr auto
  alloy::core::attribute_buffer_view<T>::do_get(std::size_t index,
                                          std::index_sequence<Idx...>)
  const noexcept
  -> value_type
{
  const auto offset = elements_per_value * index;

  return T{m_array[offset + Idx]...};
}

template <typename T>
inline constexpr auto
  alloy::core::operator==(const attribute_buffer_view<T>& lhs,
                          const attribute_buffer_view<T>& rhs)
  noexcept
  -> bool
{
  return lhs.m_array == rhs.m_array;
}

template <typename T>
inline constexpr auto
  alloy::core::operator!=(const attribute_buffer_view<T>& lhs,
                          const attribute_buffer_view<T>& rhs)
  noexcept
  -> bool
{
  return !(lhs == rhs);
}

#endif /* ALLOY_CORE_MODEL_ATTRIBUTE_BUFFER_VIEW_HPP */
