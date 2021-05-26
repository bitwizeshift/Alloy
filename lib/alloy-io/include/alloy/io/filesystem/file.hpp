/*****************************************************************************
 * \file file.hpp
 *
 * \brief
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
#ifndef ALLOY_IO_FILESYSTEM_FILE_HPP
#define ALLOY_IO_FILESYSTEM_FILE_HPP

#include "alloy/io/api.hpp"
#include "alloy/io/filesystem/file_stream_handle.hpp"
#include "alloy/io/buffers/const_buffer.hpp"
#include "alloy/io/buffers/mutable_buffer.hpp"

#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/result.hpp"

#include <system_error>
#include <type_traits>
#include <array>   // std::array
#include <cstdint> // std::uint8_t

namespace alloy::io {

  //===========================================================================
  // class : file
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A file which may be returned by a filesystem
  ///
  /// Files are owners to any file_streams and use RAII to clean up resources
  /// upon completion.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_IO_API file
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using size_type = file_stream::size_type;
    using offset_type = file_stream::offset_type;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Error codes that any basic file may return.
    ///
    /// Specific file implementations may expand on this to include more error
    /// codes/reasons
    ///////////////////////////////////////////////////////////////////////////
    enum class error_code
    {
      eof           = 1, ///< End of file reached
      not_readable  = 2, ///< The file is not able to be read
      not_writeable = 3, ///< The file is not able to be written to
      closed        = 4, ///< The file is already closed
      system_error  = 5, ///< An internal error prevented the operation from
                         ///< completing
    };

    //-------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a closed file object
    file() noexcept;

    /// \brief Constructs a file that owns the specified \p stream that will be
    ///        finalized by \p finalizer
    ///
    /// \param stream the stream to own
    /// \param finalizer the finalizer to finalize the stream
    file(core::not_null<file_stream*> stream,
         core::not_null<file_stream_finalizer*> finalizer) noexcept;

    /// \brief Constructs a file that owns the specified file_stream_handle
    ///
    /// \param handle the handle to own
    explicit file(file_stream_handle handle) noexcept;

    /// \brief Constructs this file by moving the contents of \p other
    ///
    /// \param other the other file to move
    file(file&& other) noexcept = default;
    file(const file&) = delete;

    //-------------------------------------------------------------------------

    ~file();

    //-------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to this
    ///
    /// If this file was open priored to the assignment, it is closed first
    /// before adopting the file from \p other
    ///
    /// \param other the other file to move
    /// \return reference to \c (*this)
    file& operator=(file&& other) noexcept;
    file& operator=(const file&) = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------

    /// \brief Checks whether this file is open
    ///
    /// \return \c true if the file is open
    bool is_open() const noexcept;

    /// \brief Gets the size of this file in bytes
    ///
    /// \return the size of this file in bytes, on success
    core::result<size_type,std::error_code>
      bytes() const noexcept;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Releases the underlying stream handle
    ///
    /// \note The underlying stream will not have resources managed; they need
    ///       to be handled by the underlying manager
    ///
    /// \return the handle
    file_stream_handle release() noexcept;

    //-------------------------------------------------------------------------
    // File API
    //-------------------------------------------------------------------------
  public:

    /// \brief Attempts to close the file
    ///
    /// The only reason this should ever fail is if the file is already closed
    ///
    /// \return void on success
    core::result<void,std::error_code> close() noexcept;

    /// \brief Resets the file cursor back to the start position
    ///
    /// \return void on success
    core::result<void,std::error_code> reset() noexcept;

    /// \brief Flushes the contents to the file, if buffered
    ///
    /// \return void on success
    core::result<void,std::error_code> flush() noexcept;

    /// \brief Skips up to the next N bytes of the file
    ///
    /// \param offset the number of bytes to skip
    /// \return void on success
    core::result<void,std::error_code> skip(offset_type offset) noexcept;

    /// \brief Reads data into the specified \p buffer, returning a buffer the
    ///        buffer that was read
    ///
    /// \param buffer the buffer to read into
    /// \return the buffer to the data that was read
    core::result<mutable_buffer,std::error_code> read(mutable_buffer buffer) noexcept;

    /// \brief Writes data from the specified \p buffer, returning a buffer of
    ///        the bytes that were written
    ///
    /// \param buffer the buffer to write from
    /// \return the buffer to the data that was written
    core::result<const_buffer,std::error_code> write(const_buffer buffer) noexcept;

    //-------------------------------------------------------------------------

    /// \brief Reads an object from this file
    ///
    /// This uses the file_serializer<T> customization point to read arbitrary
    /// types.
    ///
    /// \note Reading objects directly from the file is meant to not only
    ///       provide a means of reading stronger data-types, but also provide
    ///       a portable API for reading from files on systems that might
    ///       otherwise have different endiannesses. Using 'read_object'
    ///       guarantees that integral values read will be written in a
    ///       consistent manner that can be read across systems that might have
    ///       different endiannesses.
    ///
    /// \tparam T the type to read
    /// \return T on success
    template <typename T>
    core::result<T,std::error_code> read_object() noexcept;

    /// \brief Writes an object to this file
    ///
    /// This uses the file_serializer<T> customization point to write arbitrary
    /// types.
    ///
    /// \note Writing objects directly from the file is meant to not only
    ///       provide a means of reading stronger data-types, but also provide
    ///       a portable API for reading from files on systems that might
    ///       otherwise have different endiannesses. Using 'read_object'
    ///       guarantees that integral values read will be written in a
    ///       consistent manner that can be read across systems that might have
    ///       different endiannesses.
    ///
    /// \param v the value to write
    /// \return void on success
    template <typename T>
    core::result<void,std::error_code> write_object(const T& v) noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    file_stream_handle m_handle;
  };

  //===========================================================================
  // non-member functions : enum : file::error_code
  //===========================================================================

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Makes an error code from the given file error code
  ///
  /// \param ec the file error code
  /// \return the std::error_code of the file error
  ALLOY_IO_API
  std::error_code make_error_code(file::error_code ec) noexcept;

  //===========================================================================
  // struct : file_serializer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A customization point for allowing template specializations to
  ///        enable object serialization and deserialization to files
  ///
  /// This is a user customization point that allows consumers to specialize
  /// this structure for user-defined types, so that file::read_object
  /// and file::write_object may interpret how to serialize and deserialize
  /// these types.
  ///
  /// \tparam T the type to serialize
  /////////////////////////////////////////////////////////////////////////////
  template <typename T>
  struct file_serializer
  {
    /// \brief Writes the arbitrary object \p in to the file \p f
    ///
    /// \param f the file to write to
    /// \param in the value to write
    /// \return \c void on success
    static core::result<void,std::error_code> serialize(file& f, const T& in) noexcept;

    /// \brief Reads from the file \p f to \p out
    ///
    /// \param f the file to read from
    /// \return \c void on success
    static core::result<T,std::error_code> deserialize(file& f) noexcept;
  };

} // namespace alloy::io

namespace std {
  template <>
  struct is_error_code_enum<alloy::io::file::error_code> : std::true_type{};
} // namespace std

//=============================================================================
// inline definitions : class : file
//=============================================================================

//-----------------------------------------------------------------------------
// File API
//-----------------------------------------------------------------------------

template <typename T>
inline alloy::core::result<T,std::error_code> alloy::io::file::read_object()
  noexcept
{
  static_assert(
    noexcept(file_serializer<T>::deserialize(*this)),
    "user-defined file_serializer<T>::deserialize requests must be non-throwing. "
    "Please ensure your implementation translates all errors to an "
    "result<T,std::error_code> type and returns them accordingly, then mark the function "
    "'noexcept'."
  );

  return file_serializer<T>::deserialize(*this);
}

template <typename T>
inline alloy::core::result<void,std::error_code> alloy::io::file::write_object(const T& v)
  noexcept
{
  static_assert(
    noexcept(file_serializer<T>::serialize(*this, v)),
    "user-defined file_serializer<T>::serialize requests must be non-throwing. "
    "Please ensure your implementation translates all errors to an "
    "result<void,std::error_code> type and returns them accordingly, then mark the function "
    "'noexcept'."
  );

  return file_serializer<T>::serialize(*this, v);
}

//=============================================================================
// inline definitions : struct : file_serializer<T>
//=============================================================================

template <typename T>
inline alloy::core::result<void,std::error_code>
  alloy::io::file_serializer<T>::serialize(file& f, const T& in)
  noexcept
{
  static_assert(
    std::is_trivially_copyable_v<T>,
    "Only trivially copyable types are serializable by default. "
    "Please define a custom ADL-overload instead"
  );

  // Handle 'enum' as a special case; should be treated like integers
  if constexpr (std::is_enum_v<T>) {
    using underlying_type = std::underlying_type_t<T>;

    return file_serializer<underlying_type>::serialize(
      f,
      static_cast<underlying_type>(in)
    );
  } else {
    auto result = f.write(const_buffer::from_object(in));
    if (!result) {
      return core::fail(result.error());
    }
    return {};
  }
}


template <typename T>
inline alloy::core::result<T,std::error_code>
  alloy::io::file_serializer<T>::deserialize(file& f)
  noexcept
{
  static_assert(
    std::is_trivially_copyable_v<T> &&
    std::is_trivially_default_constructible_v<T>,
    "Only trivially copyable and default-constructible types are "
    "deserializable by default. "
    "Please define a custom ADL-overload instead"
  );

  // Handle 'enum' as a special case; should be treated like integers
  if constexpr (std::is_enum_v<T>) {
    using underlying_type = std::underlying_type_t<T>;

    auto result = file_serializer<underlying_type>::deserialize(f);
    if (!result) {
      return core::fail(result.error());
    }
    return static_cast<T>(*result);
  } else {
    auto object = T{};
    auto result = f.read(mutable_buffer::from_object(object));
    if (!result) {
      return core::fail(result.error());
    }

    return object;
  }
}

//=============================================================================
// inline definitions : specializations : struct : file_serializer
//=============================================================================

//-----------------------------------------------------------------------------
// bool
//-----------------------------------------------------------------------------

template <>
struct alloy::io::file_serializer<bool>
{
  static core::result<void,std::error_code> serialize(file& f, bool in)
    noexcept
  {
    auto result = f.write(const_buffer::from_object(in));
    if (!result) {
      return core::fail(result.error());
    }
    return {};
  }

  static core::result<bool,std::error_code> deserialize(file& f)
    noexcept
  {
    auto object = bool{};
    auto result = f.read(mutable_buffer::from_object(object));
    if (!result) {
      return core::fail(result.error());
    }

    return object;
  }
};

//-----------------------------------------------------------------------------
// std::uint8_t
//-----------------------------------------------------------------------------

template <>
struct alloy::io::file_serializer<std::uint8_t>
{
  static core::result<void,std::error_code> serialize(file& f, std::uint8_t in)
    noexcept
  {
    auto result = f.write(const_buffer::from_object(in));
    if (!result) {
      return core::fail(result.error());
    }
    return {};
  }

  static core::result<std::uint8_t,std::error_code> deserialize(file& f)
    noexcept
  {
    auto object = std::uint8_t{};
    auto result = f.read(mutable_buffer::from_object(object));
    if (!result) {
      return core::fail(result.error());
    }

    return object;
  }
};

//-----------------------------------------------------------------------------
// std::int8_t
//-----------------------------------------------------------------------------

template <>
struct alloy::io::file_serializer<std::int8_t>
{
  static core::result<void,std::error_code> serialize(file& f, std::int8_t in)
    noexcept
  {
    return file_serializer<std::uint8_t>::serialize(
      f,
      static_cast<std::uint8_t>(in)
    );
  }

  static core::result<std::int16_t,std::error_code> deserialize(file& f)
    noexcept
  {
    auto r = file_serializer<std::uint8_t>::deserialize(f);

    if (!r) {
      return core::fail(r.error());
    }
    return static_cast<std::int8_t>(*r);
  }
};

//-----------------------------------------------------------------------------
// std::uint16_t
//-----------------------------------------------------------------------------

template <>
struct alloy::io::file_serializer<std::uint16_t>
{
  static core::result<void,std::error_code> serialize(file& f, std::uint16_t in)
    noexcept
  {
    // Create an array of the byte-sequence in a consistent order
    auto bytes = std::array<std::byte,sizeof(std::uint16_t)> {
      static_cast<std::byte>((0xff00 & in) >> 8),
      static_cast<std::byte>((0x00ff & in)),
    };
    auto result = f.write(const_buffer::from_container(bytes));
    if (!result) {
      return core::fail(result.error());
    }
    return {};
  }

  static core::result<std::uint16_t,std::error_code> deserialize(file& f)
    noexcept
  {
    auto bytes = std::array<std::byte,sizeof(std::uint16_t)>{};
    auto result = f.read(mutable_buffer::from_object(bytes));
    if (!result) {
      return core::fail(result.error());
    }

    // Rebuild the result from the byte sequence
    auto object = std::uint16_t(
      static_cast<std::uint16_t>(bytes[0] << 8) |
      static_cast<std::uint16_t>(bytes[1])
    );
    return object;
  }
};

//-----------------------------------------------------------------------------
// std::int16_t
//-----------------------------------------------------------------------------

template <>
struct alloy::io::file_serializer<std::int16_t>
{
  static core::result<void,std::error_code> serialize(file& f, std::int16_t in)
    noexcept
  {
    return file_serializer<std::uint16_t>::serialize(
      f,
      static_cast<std::uint16_t>(in)
    );
  }

  static core::result<std::int16_t,std::error_code> deserialize(file& f)
    noexcept
  {
    auto r = file_serializer<std::uint16_t>::deserialize(f);

    if (!r) {
      return core::fail(r.error());
    }
    return static_cast<std::int16_t>(*r);
  }
};

//-----------------------------------------------------------------------------
// std::uint32_t
//-----------------------------------------------------------------------------

template <>
struct alloy::io::file_serializer<std::uint32_t>
{
  static core::result<void,std::error_code> serialize(file& f, std::uint32_t in)
    noexcept
  {
    // Create an array of the byte-sequence in a consistent order
    auto bytes = std::array<std::byte,sizeof(std::uint32_t)> {
      static_cast<std::byte>((0xff000000 & in) >> 24),
      static_cast<std::byte>((0x00ff0000 & in) >> 16),
      static_cast<std::byte>((0x0000ff00 & in) >> 8),
      static_cast<std::byte>((0x000000ff & in)),
    };
    auto result = f.write(const_buffer::from_container(bytes));
    if (!result) {
      return core::fail(result.error());
    }
    return {};
  }

  static core::result<std::uint32_t,std::error_code> deserialize(file& f)
    noexcept
  {
    auto bytes = std::array<std::byte,sizeof(std::uint32_t)>{};
    auto result = f.read(mutable_buffer::from_object(bytes));
    if (!result) {
      return core::fail(result.error());
    }

    // Rebuild the result from the byte sequence
    auto object = std::uint32_t(
      static_cast<std::uint32_t>(bytes[0] << 24) |
      static_cast<std::uint32_t>(bytes[1] << 16) |
      static_cast<std::uint32_t>(bytes[2] << 8)  |
      static_cast<std::uint32_t>(bytes[3])
    );
    return object;
  }
};

//-----------------------------------------------------------------------------
// std::int32_t
//-----------------------------------------------------------------------------

template <>
struct alloy::io::file_serializer<std::int32_t>
{
  static core::result<void,std::error_code> serialize(file& f, std::int16_t in)
    noexcept
  {
    return file_serializer<std::uint32_t>::serialize(
      f,
      static_cast<std::uint32_t>(in)
    );
  }

  static core::result<std::int32_t,std::error_code> deserialize(file& f)
    noexcept
  {
    auto r = file_serializer<std::uint32_t>::deserialize(f);

    if (!r) {
      return core::fail(r.error());
    }
    return static_cast<std::int32_t>(*r);
  }
};

//-----------------------------------------------------------------------------
// std::uint64_t
//-----------------------------------------------------------------------------

template <>
struct alloy::io::file_serializer<std::uint64_t>
{
  static core::result<void,std::error_code> serialize(file& f, std::uint64_t in)
    noexcept
  {
    // Create an array of the byte-sequence in a consistent order
    auto bytes = std::array<std::byte,sizeof(std::uint64_t)> {
      static_cast<std::byte>((0xff00000000000000 & in) >> 56),
      static_cast<std::byte>((0x00ff000000000000 & in) >> 48),
      static_cast<std::byte>((0x0000ff0000000000 & in) >> 40),
      static_cast<std::byte>((0x000000ff00000000 & in) >> 32),
      static_cast<std::byte>((0x00000000ff000000 & in) >> 24),
      static_cast<std::byte>((0x0000000000ff0000 & in) >> 16),
      static_cast<std::byte>((0x000000000000ff00 & in) >> 8),
      static_cast<std::byte>((0x00000000000000ff & in)),
    };
    auto result = f.write(const_buffer::from_container(bytes));
    if (!result) {
      return core::fail(result.error());
    }
    return {};
  }

  static core::result<std::uint64_t,std::error_code> deserialize(file& f)
    noexcept
  {
    auto bytes = std::array<std::byte,sizeof(std::uint64_t)>{};
    auto result = f.read(mutable_buffer::from_object(bytes));
    if (!result) {
      return core::fail(result.error());
    }

    // Rebuild the result from the byte sequence
    auto object = std::uint64_t(
      static_cast<std::uint64_t>(bytes[0] << 56) |
      static_cast<std::uint64_t>(bytes[1] << 48) |
      static_cast<std::uint64_t>(bytes[2] << 40) |
      static_cast<std::uint64_t>(bytes[3] << 32) |
      static_cast<std::uint64_t>(bytes[4] << 24) |
      static_cast<std::uint64_t>(bytes[5] << 16) |
      static_cast<std::uint64_t>(bytes[6] << 8)  |
      static_cast<std::uint64_t>(bytes[7])
    );
    return object;
  }
};

//-----------------------------------------------------------------------------
// std::int64_t
//-----------------------------------------------------------------------------

template <>
struct alloy::io::file_serializer<std::int64_t>
{
  static core::result<void,std::error_code> serialize(file& f, std::int16_t in)
    noexcept
  {
    return file_serializer<std::uint64_t>::serialize(
      f,
      static_cast<std::uint64_t>(in)
    );
  }

  static core::result<std::int64_t,std::error_code> deserialize(file& f)
    noexcept
  {
    auto r = file_serializer<std::uint64_t>::deserialize(f);

    if (!r) {
      return core::fail(r.error());
    }
    return static_cast<std::int64_t>(*r);
  }
};

#endif /* ALLOY_IO_FILESYSTEM_FILE_HPP */
