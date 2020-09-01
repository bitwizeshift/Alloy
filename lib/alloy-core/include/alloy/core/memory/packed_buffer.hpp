////////////////////////////////////////////////////////////////////////////////
/// \file packed_buffer.hpp
///
/// \brief TODO(Bitwize): Add description
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

#ifndef ALLOY_CORE_MEMORY_PACKED_BUFFER_HPP
#define ALLOY_CORE_MEMORY_PACKED_BUFFER_HPP

#include "alloy/core/api.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/expected.hpp"
#include "alloy/core/utilities/casts.hpp"    // narrow_cast

#include "alloy/core/memory/data_quantity.hpp" // bytes
#include "alloy/core/memory/allocator.hpp"

#include "alloy/core/containers/vector.hpp"
#include "alloy/core/containers/span.hpp"

#include <cstddef>      // std::byte
#include <system_error> // std::error_code
#include <type_traits>  // std::true_type

namespace alloy::core {

  //===========================================================================
  // class : packed_buffer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A buffer of tightly packed bytes that represents a possibly
  ///        heterogeneous store of objects.
  ///
  /// All writes are stored sequentially at the end of the buffer.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API packed_buffer
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using value_type      = std::byte;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;

    using size_type = bytes;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a packed buffer that does not contain any data
    packed_buffer() noexcept = default;

    /// \brief Constructs a packed buffer that uses the specified allocator
    ///        \p alloc for memory management
    ///
    /// \param alloc the allocator to use
    explicit packed_buffer(allocator alloc) noexcept;

    /// \brief Constructs a packed buffer from an existing vector of bytes
    ///
    /// \param buffer the buffer to use
    explicit packed_buffer(vector<std::byte> buffer) noexcept;

    /// \brief Moves the contents of \p other to this
    ///
    /// \param other the other buffer to move
    packed_buffer(packed_buffer&& other) noexcept = default;

    // Deleted copy constructor
    packed_buffer(const packed_buffer&) = delete;

    //-------------------------------------------------------------------------

    /// \brief Moves the contents of \p other to this
    ///
    /// \param other the other buffer to move
    /// \return reference to `(*this)`
    auto operator=(packed_buffer&& other) noexcept -> packed_buffer& = default;

    // Deleted copy assignment
    auto operator=(const packed_buffer&) -> packed_buffer& = delete;

    //-------------------------------------------------------------------------

    /// \brief Copies this packed_buffer using the same allocator
    ///
    /// \return a copy of this packed_buffer
    auto copy() const -> packed_buffer;

    /// \brief Copies this packed_buffer using the requested allocator \p alloc
    ///
    /// \param alloc the allocator to use
    /// \return a copy of this packed_buffer
    auto copy(allocator alloc) const -> packed_buffer;

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    auto data() const noexcept -> const_pointer;

    /// \brief Gets a byte at the specified \p index
    ///
    /// \pre index must be >= 0, and <= size()
    /// \param index the index to retrieve the byte from
    /// \return the byte
    auto operator[](std::ptrdiff_t index) const noexcept -> value_type;

    //-------------------------------------------------------------------------
    // Capacity
    //-------------------------------------------------------------------------
  public:

    /// \brief Reserves space for \p new_size bytes
    ///
    /// \param new_size the number of bytes to reserve space for
    auto reserve(bytes new_size) -> void;

    /// \brief The number of bytes this packed_buffer is actually using
    ///
    /// \return the number of bytes
    auto size() const noexcept -> bytes;

    /// \brief The number of bytes available for this packed_buffer to use
    ///        without resizing
    ///
    /// \return the capacity
    auto capacity() const noexcept -> bytes;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Pushes a single byte into the packed_buffer
    ///
    /// \param b the byte to push
    auto push_byte(std::byte b) -> void;

    /// \brief Pushes a series of bytes into the packed_buffer
    ///
    /// \param bytes the bytes to push
    auto push_bytes(span<const std::byte> bytes) -> void;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    vector<std::byte> m_buffer;
  };

  //===========================================================================
  // non-member functions : class : packed_buffer
  //===========================================================================

  //---------------------------------------------------------------------------
  // Equality
  //---------------------------------------------------------------------------

  auto operator==(const packed_buffer& lhs, const packed_buffer& rhs) noexcept -> bool;
  auto operator!=(const packed_buffer& lhs, const packed_buffer& rhs) noexcept -> bool;

  //===========================================================================
  // class : packed_buffer_writer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Provides a mutable writer view to a packed_buffer.
  ///
  /// This introduces an interface for writing any objects to the packed_buffer
  /// rather than simply bytes. Different user-defined `Packer` types may be
  /// specified to allow for non-trivial objects to be packed densely into the
  /// underlying buffer.
  ///
  /// \note The packed_buffer_writer class does not own a buffer; it simply
  ///       adapts and mutates the buffer. Care must be taken to ensure that
  ///       any buffer captured by this writer will not go out of scope by the
  ///       time that it is used!
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API packed_buffer_writer
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    // Not default-constructible
    packed_buffer_writer() = delete;

    /// \brief Constructs a writer around the packed_buffer object
    ///
    /// \pre \p buffer is not null
    ///
    /// \pre \p buffer must not exceed the scope of this writer
    /// \param buffer the buffer to view
    explicit packed_buffer_writer(not_null<packed_buffer*> buffer) noexcept;

    // Not copyable or moveable
    packed_buffer_writer(packed_buffer_writer&&) = delete;
    packed_buffer_writer(const packed_buffer_writer&) = delete;

    //-------------------------------------------------------------------------

    // Not copyable or moveable
    auto operator=(packed_buffer_writer&&) -> packed_buffer_writer& = delete;
    auto operator=(const packed_buffer_writer&) -> packed_buffer_writer& = delete;

    //-------------------------------------------------------------------------
    // Writing
    //-------------------------------------------------------------------------
  public:

    /// \brief Writes raw bytes to the underlying packed_buffer
    ///
    /// This operation can only fail if not enough memory is available
    /// \param source the buffer of data to write
    auto write_bytes(span<const std::byte> source) -> void;

    /// \brief Writes a trivially copyable object to the underlying
    ///        packed_buffer by ignoring any alignment requirements, as if by
    ///        memcpy
    ///
    /// \param value the value to write
    template <typename T>
    auto pack_object(const T& value) noexcept -> void;

    /// \brief Writes an object \p value to the underlying packed_buffer using
    ///        the specified \p packer for arranging the bytes.
    ///
    /// This overload allows for custom, possibly non-trivial objects to be
    /// packed and unpacked from the underlying buffer as needed.
    ///
    /// \param value the value to write
    /// \param packer the packager to use to write to the buffer
    template <typename T, typename Packer>
    auto pack_object(const T& value, Packer&& packer) noexcept -> void;

    /// \brief Writes a range of trivially copyable objects to the underlying
    ///        packed_buffer by ignoring any alignment requirements, as if by
    ///        memcpy
    ///
    /// \param first the start of the range
    /// \param last the end of the range
    template <typename InputIt>
    auto pack_objects(InputIt first, InputIt last) noexcept -> void;

    /// \brief Writes a range of objects to the underlying packed_buffer by
    ///        using the specified \p packer for arranging the bytes.
    ///
    /// This overload allows for custom, possibly non-trivial objects to be
    /// packed and unpacked from the underlying buffer as needed.
    ///
    /// \param first the start of the range
    /// \param last the end of the range
    template <typename InputIt, typename Packer>
    auto pack_objects(InputIt first, InputIt last, Packer&& packer) noexcept -> void;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    not_null<packed_buffer*> m_buffer;
  };

  //===========================================================================
  // class : packed_buffer_reader
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Provides an immutable reader view over top of a packed_buffer.
  ///
  /// This view allows for reading arbitrary, possibly packed objects, from a
  /// contiguous sequence of bytes denoted by the packed_buffer.
  ///
  /// This introduces an interface for writing any objects to the packed_buffer
  /// rather than simply bytes. Different user-defined `Packer` types may be
  /// specified to allow for non-trivial objects to be packed densely into the
  /// underlying buffer.
  ///
  /// \note The packed_buffer_reader class does not own a buffer; it simply
  ///       adapts and reads the buffer. Care must be taken to ensure that
  ///       any buffer captured by this reader will not go out of scope by the
  ///       time that it is used!
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API packed_buffer_reader
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    enum class error {
      out_of_bytes = 1, ///< Not enough bytes were read
    };

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    // Not default-constructible
    packed_buffer_reader() = delete;

    /// \brief Constructs a reader from a referenced packed_buffer
    ///
    /// \param buffer the buffer to read from
    explicit packed_buffer_reader(not_null<const packed_buffer*> buffer) noexcept;

    // Not copyable or moveable
    packed_buffer_reader(packed_buffer_reader&&) = delete;
    packed_buffer_reader(const packed_buffer_reader&) = delete;

    //-------------------------------------------------------------------------

    // Not copyable or moveable
    auto operator=(packed_buffer_reader&&) -> packed_buffer_reader& = delete;
    auto operator=(const packed_buffer_reader&) -> packed_buffer_reader& = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Retrieves the underlying index from this packed buffer
    ///
    /// \return the index
    auto index() const noexcept -> std::size_t;

    /// \brief Seeks the internal read pointer to the specified \p index
    ///
    /// \param index the index to seek to
    auto seek(std::size_t index) noexcept -> void;

    //-------------------------------------------------------------------------
    // Reading
    //-------------------------------------------------------------------------
  public:

    /// \brief Reads up to destination.size() bytes from the underlying
    ///        packed_buffer, returning the span of read bytes
    ///
    /// \param destination the buffer to write to
    /// \return the span of read bytes
    auto read_bytes(span<std::byte> destination) noexcept -> span<std::byte>;

    /// \brief Reads a trivially copyable object to the underlying
    ///        packed_buffer by ignoring any alignment requirements, as if by
    ///        memcpy
    ///
    /// \tparam T the type of object to read
    /// \return a T object on success
    template <typename T>
    auto unpack_object() noexcept -> expected<T>;

    /// \brief Reads an object \p value to the underlying packed_buffer using
    ///        the specified \p packer for arranging the bytes.
    ///
    /// This overload allows for custom, possibly non-trivial objects to be
    /// packed and unpacked from the underlying buffer as needed.
    ///
    /// \tparam T the type of object to read
    /// \param packer the packager to use to read from the buffer
    template <typename T, typename Packer>
    auto unpack_object(Packer&& packer) noexcept -> expected<T>;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    not_null<const packed_buffer*> m_buffer;
    std::size_t m_read_index;
  };

  //===========================================================================
  // non-member functions : class : packed_buffer_reader
  //===========================================================================

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Makes an error_code from a packed_buffer_error \p e
  ///
  /// \param e the error
  /// \return the error_code
  ALLOY_CORE_API
  auto make_error_code(packed_buffer_reader::error e) noexcept -> std::error_code;

  //===========================================================================
  // class : trivial_object_packer<T>
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A packer for trivial objects
  ///
  /// This simply packs all objects as a copied series of bytes without
  /// actually unwrapping each individual object. If there is padding
  /// between each object, this padding will be written as well.
  /////////////////////////////////////////////////////////////////////////////
  template <typename T>
  struct trivial_object_packer
  {
    static_assert(std::is_trivially_copyable_v<T>);

    /// \brief Packs the object \p v into the \p buffer
    ///
    /// \param buffer the buffer writer
    /// \param v the value to write
    auto pack(packed_buffer_writer& buffer, const T& v) const noexcept -> void;

    /// \brief Unpacks a T object from the \p buffer
    ///
    /// \param buffer the buffer reader
    /// \return a T value on success
    auto unpack(packed_buffer_reader& buffer) const noexcept -> expected<T>;
  };

} // namespace alloy::core

namespace std {
  template <>
  struct is_error_code_enum<alloy::core::packed_buffer_reader::error> : std::true_type{};
} // namespace std

//=============================================================================
// class : packed_buffer
//=============================================================================

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE auto
  alloy::core::packed_buffer::operator[](std::ptrdiff_t index)
  const noexcept -> value_type
{
  ALLOY_ASSERT(index >= 0 && static_cast<std::size_t>(index) <= m_buffer.size());

  return m_buffer[static_cast<std::size_t>(index)];
}

ALLOY_FORCE_INLINE auto
  alloy::core::packed_buffer::data()
  const noexcept -> const_pointer
{
  return m_buffer.data();
}

//-----------------------------------------------------------------------------
// Capacity
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE auto
  alloy::core::packed_buffer::size()
  const noexcept -> size_type
{
  return bytes{m_buffer.size()};
}

ALLOY_FORCE_INLINE auto
  alloy::core::packed_buffer::capacity()
  const noexcept -> size_type
{
  return bytes{m_buffer.capacity()};
}

//=============================================================================
// non-member functions : class : packed_buffer
//=============================================================================

//-----------------------------------------------------------------------------
// Equality
//-----------------------------------------------------------------------------

inline auto
  alloy::core::operator==(const packed_buffer& lhs, const packed_buffer& rhs)
  noexcept -> bool
{
  if (lhs.size() != rhs.size()) {
    return false;
  }
  const auto size = narrow_cast<std::size_t>(lhs.size().count());
  return std::memcmp(lhs.data(), rhs.data(), size) == 0;
}

inline auto
  alloy::core::operator!=(const packed_buffer& lhs, const packed_buffer& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

//=============================================================================
// class : packed_buffer_writer
//=============================================================================

//-----------------------------------------------------------------------------
// Writing
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE auto
  alloy::core::packed_buffer_writer::write_bytes(span<const std::byte> source)
  -> void
{
  m_buffer->push_bytes(source);
}

template <typename T>
ALLOY_FORCE_INLINE auto
  alloy::core::packed_buffer_writer::pack_object(const T& value)
  noexcept -> void
{
  pack_object(value, trivial_object_packer<T>{});
}

template <typename T, typename Packer>
ALLOY_FORCE_INLINE auto
  alloy::core::packed_buffer_writer::pack_object(const T& value,
                                                 Packer&& packer)
  noexcept -> void
{
  packer.pack(*this, value);
}

template<typename InputIt>
inline auto
  alloy::core::packed_buffer_writer::pack_objects(InputIt first, InputIt last)
  noexcept -> void
{
  using underlying_type = typename std::iterator_traits<InputIt>::value_type;

  pack_objects(first, last, trivial_object_packer<underlying_type>{});
}

template <typename InputIt, typename Packer>
inline auto
  alloy::core::packed_buffer_writer::pack_objects(InputIt first,
                                                  InputIt last,
                                                  Packer&& packer)
  noexcept -> void
{
  for (auto it = first; it != last; ++it) {
    packer.pack(*this, *it);
  }
}

//=============================================================================
// class : packed_buffer_reader
//=============================================================================

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE auto
  alloy::core::packed_buffer_reader::index()
  const noexcept -> std::size_t
{
  return m_read_index;
}

//-----------------------------------------------------------------------------
// Reading
//-----------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE auto
  alloy::core::packed_buffer_reader::unpack_object()
  noexcept -> expected<T>
{
  return unpack_object<T>(trivial_object_packer<T>{});
}

template <typename T, typename Packer>
ALLOY_FORCE_INLINE auto
  alloy::core::packed_buffer_reader::unpack_object(Packer&& packer)
  noexcept -> expected<T>
{
  return packer.unpack(*this);
}

//=============================================================================
// class : trivial_object_packer<T>
//=============================================================================

template <typename T>
ALLOY_FORCE_INLINE auto
  alloy::core::trivial_object_packer<T>::pack(packed_buffer_writer& buffer,
                                              const T& v)
  const noexcept -> void
{
  auto s = span<const T>{&v, std::size_t{1}};

  buffer.write_bytes(as_bytes(s));
}

template <typename T>
inline auto
  alloy::core::trivial_object_packer<T>::unpack(packed_buffer_reader& buffer)
  const noexcept -> expected<T>
{
  auto out = T{};
  auto s = span<T>{&out,std::size_t{1}};
  auto input = as_writable_bytes(s);
  const auto result = buffer.read_bytes(input);

  if (input.size() != result.size()) {
    return unexpected(packed_buffer_reader::error::out_of_bytes);
  }

  return out;
}

#endif /* ALLOY_CORE_MEMORY_PACKED_BUFFER_HPP */
