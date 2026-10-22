// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/platform/filesystem/memory_filesystem.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/platform/buffers/const_buffer.hpp"
#include "alloy/platform/buffers/mutable_buffer.hpp"
#include "alloy/platform/filesystem/file.hpp"
#include "alloy/platform/filesystem/file_stream.hpp"


#include <algorithm> // std::min
#include <cstddef>
#include <system_error>

namespace {

  //===========================================================================
  // class : memory_file_stream
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A memory file stream
  ///
  /// This file will read and write from memory
  /////////////////////////////////////////////////////////////////////////////
  class memory_file_stream final
    : public alloy::platform::file_stream
  {
    //-------------------------------------------------------------------------
    // Constructors
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a memory file stream that will write into and read
    ///        from the specified \p buffer
    ///
    /// \param buffer the buffer to write into and read from
    explicit memory_file_stream(alloy::platform::mutable_buffer buffer) noexcept;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Returns the size of the mutable buffer
    ///
    /// \return the size of the buffer
    auto bytes()
      const noexcept -> alloy::core::result<size_type,std::error_code> override;

    //-------------------------------------------------------------------------
    // File API
    //-------------------------------------------------------------------------
  public:

    /// \brief Closes this file
    ///
    /// This is a no-op for memory files
    auto close() noexcept -> void override;

    /// \brief Resets the file cursor back to the start position
    ///
    /// \return void on success
    auto reset() noexcept -> alloy::core::result<void,std::error_code> override;

    /// \brief Skips the next \p offset bytes from this file
    ///
    /// \param offset the offset to skip
    /// \return void
    auto skip(offset_type offset)
      noexcept -> alloy::core::result<void,std::error_code> override;

    /// \brief Reads from the buffer
    ///
    /// \param buffer the buffer to read from
    /// \return a buffer to what was read
    auto read(alloy::platform::mutable_buffer buffer)
      noexcept
      -> alloy::core::result<alloy::platform::mutable_buffer,std::error_code>
      override;

    /// \brief Writes memory from the \p buffer into the stored memory
    ///
    /// \param buffer the buffer to write
    /// \return the buffer to what was written
    auto write(alloy::platform::const_buffer buffer)
      noexcept
      -> alloy::core::result<alloy::platform::const_buffer,std::error_code> override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    alloy::platform::mutable_buffer m_buffer;
    std::size_t m_index;
  };

  //===========================================================================
  // inline definitions : class : memory_file_stream
  //===========================================================================

  //---------------------------------------------------------------------------
  // Constructors
  //---------------------------------------------------------------------------

  memory_file_stream::memory_file_stream(alloy::platform::mutable_buffer buffer)
    noexcept
    : m_buffer{buffer},
      m_index{0u}
  {

  }

  //---------------------------------------------------------------------------
  // Observers
  //---------------------------------------------------------------------------

  auto memory_file_stream::bytes()
    const noexcept
    -> alloy::core::result<alloy::platform::file_stream::size_type,std::error_code>
  {
    return m_buffer.size();
  }

  //---------------------------------------------------------------------------
  // File API
  //---------------------------------------------------------------------------

  auto memory_file_stream::close() noexcept -> void
  {
    // Nothing to do
  }

  auto memory_file_stream::reset()
    noexcept -> alloy::core::result<void,std::error_code>
  {
    m_index = 0u;
    return {};
  }

  auto memory_file_stream::skip(offset_type offset)
    noexcept -> alloy::core::result<void,std::error_code>
  {
    const auto next_index = std::min(m_index + offset, m_buffer.size());

    if (next_index == m_index) {
      return alloy::core::fail(alloy::platform::file::error_code::eof);
    }

    m_index = next_index;

    return {};
  }

  auto memory_file_stream::read(alloy::platform::mutable_buffer buffer)
    noexcept -> alloy::core::result<alloy::platform::mutable_buffer,std::error_code>
  {
    if (m_buffer.size() == m_index) {
      return alloy::core::fail(alloy::platform::file::error_code::eof);
    }

    // To write to
    auto* const end   = buffer.data() + buffer.size();
    auto* const begin = buffer.data();

    // To read from
    auto* const data = m_buffer.data();
    const auto size  = m_buffer.size();

    const auto index_before = m_index;
    auto* it = begin;

    for (; it != end && m_index < size; ++it, ++m_index) {
      *it = data[m_index];
    }

    const auto bytes_read = m_index - index_before;

    return alloy::platform::mutable_buffer{
      buffer.data(),
      bytes_read
    };
  }

  auto memory_file_stream::write(alloy::platform::const_buffer buffer)
    noexcept -> alloy::core::result<alloy::platform::const_buffer,std::error_code>
  {
    if (m_buffer.size() == m_index) {
      return alloy::core::fail(alloy::platform::file::error_code::eof);
    }

    // To read from
    auto* const end   = buffer.data() + buffer.size();
    auto* const begin = buffer.data();

    // To write into
    auto* const data = m_buffer.data();
    const auto size  = m_buffer.size();

    const auto index_before = m_index;
    auto* it = begin;

    for (; it != end && m_index < size; ++it, ++m_index) {
      data[m_index] = *it;
    }

    const auto bytes_written = m_index - index_before;

    return alloy::platform::const_buffer{
      buffer.data(),
      bytes_written
    };
  }

} // namespace <anonymous>

//=============================================================================
// definitions : class : memory_filesystem
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

alloy::platform::memory_filesystem::memory_filesystem() noexcept
  : memory_filesystem{core::allocator{}}
{

}


alloy::platform::memory_filesystem::memory_filesystem(core::allocator alloc) noexcept
  : m_allocator{alloc}
{

}

//-----------------------------------------------------------------------------
// Filesystem
//-----------------------------------------------------------------------------

auto alloy::platform::memory_filesystem::open(mutable_buffer buffer) noexcept -> file
{
  auto* const stream = m_allocator.make<::memory_file_stream>(buffer);

  return file{stream, this};
}


auto alloy::platform::memory_filesystem::finalize(file_stream* stream)
  noexcept -> void
{
  auto* const p = static_cast<::memory_file_stream*>(stream);

  m_allocator.dispose<::memory_file_stream>(p);
}
