////////////////////////////////////////////////////////////////////////////////
/// \file span.hpp
///
/// \brief This header provides definitions from the C++ header <span>
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
#ifndef ALLOY_CORE_CONTAINER_SPAN_HPP
#define ALLOY_CORE_CONTAINER_SPAN_HPP

#if __cplusplus >= 202003L && __has_include(<span>)

#include <span>

namespace alloy::core {
  using std::span;

  using std::dynamic_extent;
} // namespace alloy:core

#else

// This header has been taken from the BackportCpp library project:
// https://github.com/bitwizeshift/backportcpp

#include <cstddef>     // byte
#include <memory>      // to_address
#include <type_traits> // conjunction, remove_cvref_t, etc

#include <cstddef>  // std::size_t
#include <array>    // std::array
#include <iterator> // std::iterator_traits

namespace alloy::core {

  //============================================================================
  // constants : dynamic_extent
  //============================================================================

  inline constexpr auto dynamic_extent = static_cast<std::size_t>(-1);

  namespace detail {

    template <typename T>
    inline constexpr
    T* to_address(T* p)
      noexcept
    {
      static_assert(
        !std::is_function<T>::value,
        "T* must not be a function pointer"
      );

      return p;
    }

    template <typename T>
    inline constexpr auto to_address(const T& p)
      noexcept
    {
      return to_address(p.operator->());
    }

    template <std::size_t Extent>
    class extent_storage
    {
    public:
      constexpr extent_storage() noexcept = default;

      template <std::size_t UExtent>
      constexpr explicit extent_storage(extent_storage<UExtent>)
      {
        static_assert(
          UExtent == Extent,
          "Extent must be the same size as UExtent"
        );
      }

      constexpr explicit extent_storage(std::size_t)
      {

      }

      constexpr std::size_t size() const noexcept
      {
        return Extent;
      }
    };

    template <>
    class extent_storage<dynamic_extent>
    {
    public:
      template <std::size_t Extent>
      constexpr explicit extent_storage(extent_storage<Extent> ext)
        : m_size{ext.size()}
      {

      }

      constexpr explicit extent_storage(std::size_t size)
        : m_size{size}
      {

      }

      constexpr std::size_t size() const noexcept
      {
        return m_size;
      }

    private:

      std::size_t m_size;
    };

    template <typename T, std::size_t Extent>
    class span_storage_type : public extent_storage<Extent>
    {
      //------------------------------------------------------------------------
      // Constructors
      //------------------------------------------------------------------------
    public:

      template <typename ExtentType>
      constexpr span_storage_type(T* data, ExtentType ext)
        : extent_storage<Extent>(ext),
          m_data{data}
      {

      }

      //------------------------------------------------------------------------
      // Observers
      //------------------------------------------------------------------------
    public:

      using extent_storage<Extent>::size;

      constexpr T* data() const noexcept { return m_data; }

      //------------------------------------------------------------------------
      // Private Members
      //------------------------------------------------------------------------
    private:

        T* m_data;
    };

    template <std::size_t From, std::size_t To>
    struct is_allowed_extent_conversion
      : std::bool_constant<(From == To) || (To == dynamic_extent)>{};

    template <std::size_t Extent, std::size_t Offset, std::size_t Count>
    struct compute_subspan_size
      : std::integral_constant<std::size_t,
          (Count != dynamic_extent)
          ? Count
          : ((Extent != dynamic_extent) ? (Extent - Offset) : Extent)
        >{};

    template <typename It>
    using iter_reference = typename std::iterator_traits<It>::reference;

    template <typename T>
    using remove_cvref_t = std::remove_const_t<std::remove_reference_t<T>>;

    template <typename It, typename T>
    using is_iter_convertible = std::conjunction<
      std::is_same<remove_cvref_t<iter_reference<It>>,std::remove_cv_t<T>>,
      std::is_convertible<iter_reference<It>,std::remove_cv_t<T>>
    >;

  } // namespace detail

  //============================================================================
  // class : span
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief This class is a non-owning view of a contiguous piece of memory
  ///
  /// This type describes an object that can refer to a contiguous sequence of
  /// objects with the first element of the sequence at position zero. A span
  /// can either have a static extent, in which case the number of elements in
  /// the sequence is known and encoded in the type, or a dynamic extent,
  /// in which case the size is known at runtime.
  ///
  /// \tparam T the underlying type of the span
  /// \tparam Extent the extent (size) of the span
  //////////////////////////////////////////////////////////////////////////////
  template <typename T, std::size_t Extent = dynamic_extent>
  class span
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using element_type    = T;
    using value_type      = std::remove_cv_t<T>;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    using pointer         = element_type*;
    using const_pointer   = const element_type*;
    using reference       = element_type&;
    using const_reference = const element_type&;

    using iterator         = T*;
    using reverse_iterator = std::reverse_iterator<iterator>;

    //--------------------------------------------------------------------------
    // Public Member Constants
    //--------------------------------------------------------------------------
  public:

    inline static constexpr std::size_t extent = Extent;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a span
    ///
    /// This constructor only participates in overload resolution if the span
    /// either is size 0, or has a dynamic extent
    template <std::size_t UExtent = Extent,
              typename = std::enable_if_t<detail::is_allowed_extent_conversion<0,UExtent>::value>>
    constexpr span() noexcept;

    /// \{
    /// \brief Constructs a span from an iterator \p it and the \p count
    ///
    /// This constructor only participates in overload resolution if the
    /// following criteria are met:
    /// * Extent is dynamic_extent
    /// * to_address(it) is convertible to T*
    ///
    /// This constructor is explicit if `Extent` != `dynamic_extent`
    ///
    /// \param it the iterator
    /// \param count the number of entries in the sequence
    template <typename It,
              std::enable_if_t<(Extent == alloy::core::dynamic_extent) &&
                               alloy::core::detail::is_iter_convertible<It, T>::value,int> = 0>
    constexpr span(It it, size_type count) noexcept;
    template <typename It,
              std::enable_if_t<(Extent != alloy::core::dynamic_extent) &&
                               alloy::core::detail::is_iter_convertible<It, T>::value,int> = 0>
    constexpr explicit span(It it, size_type count) noexcept;
    /// \}

    /// \brief Constructs a span from an iterator range
    ///
    /// This constructor only participates in overload resolution if the
    /// following criteria are met:
    /// * Extent is dynamic_extent
    /// * to_address(it) is convertible to T*
    ///
    /// \param it the iterator
    /// \param end the end iterator
    template <typename It,
              std::enable_if_t<(Extent == alloy::core::dynamic_extent) &&
                                alloy::core::detail::is_iter_convertible<It, T>::value, int> = 0>
    constexpr span(It it, It end) noexcept;
    template <typename It,
              std::enable_if_t<(Extent != alloy::core::dynamic_extent) &&
                                alloy::core::detail::is_iter_convertible<It, T>::value, int> = 0>
    constexpr explicit span(It it, It end) noexcept;

    /// \brief Constructs a span from an array reference
    ///
    /// This constructor only participates in overload resolution if any of the
    /// following criteria are met:
    /// * Extent is dynamic_extent
    /// * N is the same as Extent
    ///
    /// \param arr the array reference
    template <std::size_t N,
              typename = std::enable_if_t<detail::is_allowed_extent_conversion<N,Extent>::value>>
    // cppcheck-suppress noExplicitConstructor
    constexpr span(element_type (&arr)[N]) noexcept;

    /// \{
    /// \brief Constructs a span from a reference to a std::array
    ///
    /// This constructor only participates in overload resolution if the
    /// following criteria are met:
    /// * Extent is dynamic_extent or N is the same as Extent
    /// * U is at most a cv-qualification difference from T
    ///
    /// \param arr the array reference
    template <typename U, std::size_t N,
              typename = std::enable_if_t<detail::is_allowed_extent_conversion<N,Extent>::value>>
    // cppcheck-suppress noExplicitConstructor
    constexpr span(std::array<U, N>& arr) noexcept;
    template <typename U, std::size_t N,
              typename = std::enable_if_t<detail::is_allowed_extent_conversion<N,Extent>::value>>
    // cppcheck-suppress noExplicitConstructor
    constexpr span(const std::array<U, N>& arr) noexcept;
    /// \}

    // range-constructor omitted since ranges are not part of backport yet

    /// \brief Constructs a span from a different span
    ///
    /// This constructor only participates in overload resolution if the
    /// following criteria are met:
    /// * Extent is dynamic_extent or N is the same as Extent
    /// * U is at most a cv-qualification difference from T
    ///
    /// \param s the span
    template <typename U, std::size_t N,
              typename = std::enable_if_t<detail::is_allowed_extent_conversion<N,Extent>::value>>
    // cppcheck-suppress noExplicitConstructor
    constexpr span(const span<U, N>& s) noexcept;

    /// \brief Constructs a span by copying \p other
    ///
    /// \param other the other span to copy
    constexpr span(const span& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Assigns a span from \p other
    ///
    /// \param other the other span to copy
    /// \return reference to this
    constexpr span& operator=(const span& other) noexcept = default;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets a reference to the front element of this span
    ///
    /// \pre empty() is false
    /// \return reference to front element
    constexpr reference front() const noexcept;

    /// \brief Gets a reference to the back element of this span
    ///
    /// \pre empty() is false
    /// \return reference to back element
    constexpr reference back() const noexcept;

    /// \brief Gets a reference to the element at \p idx
    ///
    /// \pre \p idx is less than size()
    /// \param idx the index
    /// \return reference to the element at \p idx
    constexpr reference operator[](size_type idx) const noexcept;

    /// \brief Gets a pointer to the start of the data
    ///
    /// \return pointer to the data
    constexpr pointer data() const noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of elements in this span
    ///
    /// \return the number of elements in this span
    constexpr size_type size() const noexcept;

    /// \brief Gets the number of bytes in this span
    ///
    /// \return the number of bytes in this span
    constexpr size_type size_bytes() const noexcept;

    /// \brief Queries whether this span is empty
    ///
    /// \return true if this span is empty
    constexpr bool empty() const noexcept;

    //--------------------------------------------------------------------------
    // Subviews
    //--------------------------------------------------------------------------
  public:

    /// \brief Creates a subspan from the first \p Count elements
    ///
    /// \pre A program is ill-formed if \p Count > Extent
    ///
    /// \tparam Count the number of elements to create in the subspan
    /// \return the first \p Count elements
    template <std::size_t Count>
    constexpr span<element_type, Count> first() const;

    /// \brief Creates a subspan from the first \p count elements
    ///
    /// \pre It is undefined behavior if \p count > size()
    ///
    /// \param count the number of elements to create in the subspan
    /// \return the first \p count elements
    constexpr span<element_type> first(size_t count) const;

    //--------------------------------------------------------------------------

    /// \brief Creates a subspan from the last \p Count elements
    ///
    /// \pre A program is ill-formed if \p Count > Extent
    ///
    /// \tparam Count the number of elements to create in the subspan
    /// \return the last \p Count elements
    template <std::size_t Count>
    constexpr span<element_type, Count> last() const;

    /// \brief Creates a subspan from the last \p count elements
    ///
    /// \pre It is undefined behavior if \p count > size()
    ///
    /// \param count the number of elements to create in the subspan
    /// \return the last \p count elements
    constexpr span<element_type> last(size_t count) const;

    //--------------------------------------------------------------------------

    /// \brief Creates a subspan that is \p Count elements long, \p Offset from
    ///        the start of this span
    ///
    /// \tparam Offset the amount to offset the span by
    /// \tparam Count the number of elements to create in the subspan
    /// \return the created subspan
    template <std::size_t Offset, std::size_t Count = dynamic_extent>
    constexpr span<element_type, detail::compute_subspan_size<Extent,Offset,Count>::value>
      subspan() const;

    /// \brief Creates a subspan that is \p count elements long, \p offset from
    ///        the start of this span
    ///
    /// \param offset the amount to offset the span by
    /// \param count the number of elements to create in the subspan
    /// \return the created subspan
    constexpr span<element_type> subspan(std::size_t offset,
                                         std::size_t count = dynamic_extent) const;

    //--------------------------------------------------------------------------
    // Iterators
    //--------------------------------------------------------------------------
  public:

    constexpr iterator begin() const noexcept;
    constexpr iterator end() const noexcept;
    constexpr reverse_iterator rbegin() const noexcept;
    constexpr reverse_iterator rend() const noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    using storage_type = detail::span_storage_type<element_type,Extent>;

    storage_type m_storage;
  };

  //============================================================================
  // non-member functions : class : span
  //============================================================================

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \{
  /// \brief Converts a span \p s to a byte span
  ///
  /// \param s the span to convert
  /// \return a span of the byte range that \p s covered
  template <typename T, std::size_t N>
  span<const std::byte, sizeof(T) * N> as_bytes(span<T, N> s) noexcept;
  template <typename T>
  span<const std::byte> as_bytes(span<T> s) noexcept;
  /// \}

  /// \{
  /// \brief Converts a span \p s to a writable byte span
  ///
  /// \param s the span to convert
  /// \return a span of the byte range that \p s covered
  template <typename T, std::size_t N>
  span<std::byte, sizeof(T) * N> as_writable_bytes(span<T, N> s) noexcept;
  template <typename T>
  span<std::byte> as_writable_bytes(span<T> s) noexcept;
  /// \}

} // namespace alloy::core

template <typename T, std::size_t Extent>
constexpr std::size_t alloy::core::span<T,Extent>::extent;

//==============================================================================
// non-member functions : class : span
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename T, std::size_t Extent>
template <std::size_t UExtent, typename>
inline constexpr
alloy::core::span<T,Extent>::span()
  noexcept
  : m_storage{nullptr, detail::extent_storage<0>{}}
{

}

template <typename T, std::size_t Extent>
template <typename It,
          std::enable_if_t<(Extent == alloy::core::dynamic_extent) &&
                            alloy::core::detail::is_iter_convertible<It, T>::value,int>>
inline constexpr
alloy::core::span<T,Extent>::span(It it, size_type count)
  noexcept
  : m_storage{detail::to_address(it), count}
{

}

template <typename T, std::size_t Extent>
template <typename It,
          std::enable_if_t<(Extent != alloy::core::dynamic_extent) &&
                            alloy::core::detail::is_iter_convertible<It, T>::value,int>>
inline constexpr
alloy::core::span<T,Extent>::span(It it, size_type count)
  noexcept
  : m_storage{detail::to_address(it), count}
{

}

template <typename T, std::size_t Extent>
template <typename It,
          std::enable_if_t<(Extent == alloy::core::dynamic_extent) &&
                            alloy::core::detail::is_iter_convertible<It, T>::value, int>>
inline constexpr
alloy::core::span<T,Extent>::span(It it, It end)
  noexcept
  : m_storage{detail::to_address(it), static_cast<size_type>(end - it)}
{

}

template <typename T, std::size_t Extent>
template <typename It,
          std::enable_if_t<(Extent != alloy::core::dynamic_extent) &&
                              alloy::core::detail::is_iter_convertible<It, T>::value, int>>
inline constexpr
alloy::core::span<T,Extent>::span(It it, It end)
  noexcept
  : m_storage{detail::to_address(it), static_cast<size_type>(end - it)}
{

}

template <typename T, std::size_t Extent>
template <std::size_t N, typename>
inline constexpr
alloy::core::span<T,Extent>::span(element_type (&arr)[N])
  noexcept
  : m_storage{static_cast<element_type*>(arr), detail::extent_storage<N>{}}
{

}

template <typename T, std::size_t Extent>
template <typename U, std::size_t N, typename>
inline constexpr
alloy::core::span<T,Extent>::span(std::array<U, N>& arr)
  noexcept
  : m_storage{arr.data(), detail::extent_storage<N>{}}
{

}

template <typename T, std::size_t Extent>
template <typename U, std::size_t N, typename>
inline constexpr
alloy::core::span<T,Extent>::span(const std::array<U, N>& arr)
  noexcept
  : m_storage{arr.data(), detail::extent_storage<N>{}}
{

}

template <typename T, std::size_t Extent>
template <typename U, std::size_t N, typename>
inline constexpr
alloy::core::span<T,Extent>::span(const span<U, N>& s)
  noexcept
  : m_storage{s.data(), detail::extent_storage<N>{s.size()}}
{

}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

template <typename T, std::size_t Extent>
inline constexpr
typename alloy::core::span<T,Extent>::reference
  alloy::core::span<T,Extent>::front()
  const noexcept
{
  return data()[0];
}

template <typename T, std::size_t Extent>
inline constexpr
typename alloy::core::span<T,Extent>::reference
  alloy::core::span<T,Extent>::back()
  const noexcept
{
  return data()[m_storage.size()-1];
}

template <typename T, std::size_t Extent>
inline constexpr
typename alloy::core::span<T,Extent>::reference
  alloy::core::span<T,Extent>::operator[](size_type idx)
  const noexcept
{
  return data()[idx];
}

template <typename T, std::size_t Extent>
inline constexpr
typename alloy::core::span<T,Extent>::pointer
  alloy::core::span<T,Extent>::data()
  const noexcept
{
  return m_storage.data();
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <typename T, std::size_t Extent>
inline constexpr
typename alloy::core::span<T,Extent>::size_type
  alloy::core::span<T,Extent>::size()
  const noexcept
{
  return m_storage.size();
}

template <typename T, std::size_t Extent>
inline constexpr
typename alloy::core::span<T,Extent>::size_type
  alloy::core::span<T,Extent>::size_bytes()
  const noexcept
{
  return size() * sizeof(T);
}

template <typename T, std::size_t Extent>
inline constexpr
bool alloy::core::span<T,Extent>::empty()
  const noexcept
{
  return size() == 0u;
}

//------------------------------------------------------------------------------
// Subviews
//------------------------------------------------------------------------------

template <typename T, std::size_t Extent>
template <std::size_t Count>
inline constexpr
alloy::core::span<typename alloy::core::span<T, Extent>::element_type, Count>
  alloy::core::span<T, Extent>::first()
  const
{
  static_assert(
    Count <= Extent,
    "A Count larger than Extent is ill-formed"
  );

  return span<element_type, Count>{data(), Count};
}

template <typename T, std::size_t Extent>
inline constexpr
alloy::core::span<typename alloy::core::span<T, Extent>::element_type>
  alloy::core::span<T,Extent>::first(size_t count)
  const
{
  return {data(), count};
}

//------------------------------------------------------------------------------

template <typename T, std::size_t Extent>
template <std::size_t Count>
inline constexpr
alloy::core::span<typename alloy::core::span<T, Extent>::element_type, Count>
  alloy::core::span<T, Extent>::last()
  const
{
  static_assert(
    Count <= Extent,
    "A Count larger than Extent is ill-formed"
  );

  return span<element_type, Count>{data() + (size() - Count), Count};
}

template <typename T, std::size_t Extent>
inline constexpr
alloy::core::span<typename alloy::core::span<T, Extent>::element_type>
  alloy::core::span<T,Extent>::last(size_t count)
  const
{
  return {data() + (size() - count), count};
}

//------------------------------------------------------------------------------

template <typename T, std::size_t Extent>
template <std::size_t Offset, std::size_t Count>
inline constexpr
alloy::core::span<typename alloy::core::span<T, Extent>::element_type, alloy::core::detail::compute_subspan_size<Extent,Offset,Count>::value>
  alloy::core::span<T, Extent>::subspan()
  const
{
  using result_type = span<
    element_type,
    detail::compute_subspan_size<Extent,Offset,Count>::value
  >;

  return result_type{
    data() + Offset,
    (Count == dynamic_extent) ? (size() - Offset) : Count
  };
}

template <typename T, std::size_t Extent>
inline constexpr
alloy::core::span<typename alloy::core::span<T, Extent>::element_type>
  alloy::core::span<T, Extent>::subspan(std::size_t offset, std::size_t count)
  const
{
  return {
    data() + offset,
    (count == dynamic_extent) ? (size() - offset) : count
  };
}

//------------------------------------------------------------------------------
// Iterators
//------------------------------------------------------------------------------

template <typename T, std::size_t Extent>
inline constexpr
typename alloy::core::span<T,Extent>::iterator
  alloy::core::span<T,Extent>::begin()
  const noexcept
{
  return iterator{data()};
}

template <typename T, std::size_t Extent>
inline constexpr
typename alloy::core::span<T,Extent>::iterator
  alloy::core::span<T,Extent>::end()
  const noexcept
{
  return iterator{data() + size()};
}

template <typename T, std::size_t Extent>
inline constexpr
typename alloy::core::span<T,Extent>::reverse_iterator
  alloy::core::span<T,Extent>::rbegin()
  const noexcept
{
  return reverse_iterator(end());
}

template <typename T, std::size_t Extent>
inline constexpr
typename alloy::core::span<T,Extent>::reverse_iterator
  alloy::core::span<T,Extent>::rend()
  const noexcept
{
  return reverse_iterator(begin());
}

//==============================================================================
// non-member functions : class : span
//==============================================================================

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

template <typename T, std::size_t N>
inline alloy::core::span<const std::byte, sizeof(T) * N>
  alloy::core::as_bytes(span<T, N> s)
  noexcept
{
  return span<const std::byte, sizeof(T) * N>{
    reinterpret_cast<const std::byte*>(s.data()), s.size_bytes()
  };
}

template <typename T>
inline alloy::core::span<const std::byte>
  alloy::core::as_bytes(span<T> s)
  noexcept
{
  return span<const std::byte>{
    reinterpret_cast<const std::byte*>(s.data()), s.size_bytes()
  };
}

template <typename T, std::size_t N>
inline alloy::core::span<std::byte, sizeof(T) * N> alloy::core::as_writable_bytes(span<T, N> s)
  noexcept
{
  return span<std::byte, sizeof(T) * N>{
    reinterpret_cast<std::byte*>(s.data()), s.size_bytes()
  };
}

template <typename T>
inline alloy::core::span<std::byte> alloy::core::as_writable_bytes(span<T> s)
  noexcept
{
  return span<std::byte>{
    reinterpret_cast<std::byte*>(s.data()), s.size_bytes()
  };
}

#endif // if __cplusplus >= 202003L

#endif /* ALLOY_CORE_CONTAINER_SPAN_HPP */
