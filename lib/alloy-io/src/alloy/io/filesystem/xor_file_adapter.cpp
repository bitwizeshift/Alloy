#include "alloy/io/filesystem/xor_file_adapter.hpp"

#include "alloy/io/filesystem/file_stream_handle.hpp"

#include <array>     // std::array
#include <algorithm> // std::min

namespace {

  //===========================================================================
  // class : xor_file_stream
  //===========================================================================

  class xor_file_stream final
    : public alloy::io::file_stream
  {
    //-------------------------------------------------------------------------
    // Constructor
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs this xor file stream from the underlying file, and
    ///        the bytes to perform the XOR with
    ///
    /// \param file the file to adapt
    /// \param b0 the first byte to XOR
    /// \param b1 the second byte to XOR
    explicit xor_file_stream(alloy::io::file_stream_handle file,
                             std::byte b0,
                             std::byte b1) noexcept;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Returns the size of the file
    ///
    /// \return the size of the file
    alloy::core::result<size_type,std::error_code> bytes() const noexcept override;

    //-------------------------------------------------------------------------
    // File API
    //-------------------------------------------------------------------------
  public:

    /// \brief Closes this file
    ///
    /// This just passes through to the adapted file
    void close() noexcept override;

    /// \brief Resets the file cursor back to the start position
    ///
    /// \return void on success
    alloy::core::result<void,std::error_code> reset() noexcept override;

    /// \brief Skips the next \p offset bytes
    ///
    /// This just passes through to the adapted file
    ///
    /// \param offset the offset to skip past
    /// \return the result of skipping
    alloy::core::result<void,std::error_code>
      skip(offset_type offset) noexcept override;

    /// \brief Reads values from the file and XOR's the result
    ///
    /// \param buffer the buffer to read into
    /// \return a buffer containing the read result
    alloy::core::result<alloy::io::mutable_buffer,std::error_code>
      read(alloy::io::mutable_buffer buffer) noexcept override;

    /// \brief Writes values into the buffer and XOR's them
    ///
    /// \param buffer the buffer to write
    /// \return a buffer of the written bytes
    alloy::core::result<alloy::io::const_buffer,std::error_code>
      write(alloy::io::const_buffer buffer) noexcept override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    alloy::io::file_stream_handle m_handle;
    std::byte m_byte0;
    std::byte m_byte1;
  };

  //===========================================================================
  // inline definitions : class : xor_file_stream
  //===========================================================================

  //---------------------------------------------------------------------------
  // Constructors
  //---------------------------------------------------------------------------

  xor_file_stream::xor_file_stream(alloy::io::file_stream_handle file,
                                   std::byte b0,
                                   std::byte b1)
    noexcept
    : m_handle{std::move(file)},
      m_byte0{b0},
      m_byte1{b1}
  {

  }

  //---------------------------------------------------------------------------
  // Observers
  //---------------------------------------------------------------------------

  alloy::core::result<alloy::io::file_stream::size_type,std::error_code>
    xor_file_stream::bytes()
    const noexcept
  {
    return m_handle->bytes();
  }

  //---------------------------------------------------------------------------
  // File API
  //---------------------------------------------------------------------------

  void xor_file_stream::close()
    noexcept
  {
    m_handle->close();
  }

  alloy::core::result<void,std::error_code> xor_file_stream::reset()
    noexcept
  {
    return m_handle->reset();
  }

  alloy::core::result<void,std::error_code>
    xor_file_stream::skip(offset_type offset)
    noexcept
  {
    return m_handle->skip(offset);
  }

  alloy::core::result<alloy::io::mutable_buffer,std::error_code>
    xor_file_stream::read(alloy::io::mutable_buffer buffer)
    noexcept
  {
    auto result = m_handle->read(buffer);
    if (!result) {
      return result;
    }
    auto new_buffer = result.value();

    auto* const end   = new_buffer.data() + new_buffer.size();
    auto* const begin = new_buffer.data();
    auto* it = begin;

    // Run the XOR pass
    for (; it != end; ++it) {
      *it ^= m_byte1;
      *it ^= m_byte0;
    }

    return new_buffer;
  }

  alloy::core::result<alloy::io::const_buffer,std::error_code>
    xor_file_stream::write(alloy::io::const_buffer buffer)
    noexcept
  {
    // Use thread-local to allow parallel files
    static constexpr auto buffer_size = std::size_t{128u};
    thread_local auto s_buffer = std::array<std::byte,buffer_size>{};

    const auto size = buffer.size();
    const auto* const data = buffer.data();

    auto remaining = buffer.size();
    auto total_written = std::size_t{0u};
    auto i = std::size_t{0u};

    // Writing requires a mutable buffer, but file::write only exposes const
    // buffers. Internally, this uses a thread-local buffer of 128 bytes, and
    // writes are done in chunks as a result. This gets rebuilt at the end
    // to provide a seamless result to the consumer.
    while (remaining > 0) {
      const auto to_write = std::min(remaining, buffer_size);

      // Translate buffers by running XOR pass over bytes
      for (auto j = 0u; j < to_write && i < size; ++i, ++j) {
        s_buffer[j] = data[i];
        s_buffer[j] ^= m_byte0;
        s_buffer[j] ^= m_byte1;
      }

      auto new_buffer = alloy::io::const_buffer{
        s_buffer.data(),
        to_write
      };
      auto result = m_handle->write(new_buffer);

      if (!result) {
        return result;
      }
      remaining -= result->size();
      total_written += result->size();

      // If we somehow don't write the entire thing, return the total amount
      // that was written.
      if (ALLOY_UNLIKELY(result->size() < to_write)) {
        return alloy::io::const_buffer{buffer.data(), total_written};
      }
    }

    // If we get this far, we wrote the whole buffer!
    return buffer;
  }
} // namespace <anonymous>

//=============================================================================
// definitions : class : xor_file_adapter
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

alloy::io::xor_file_adapter::xor_file_adapter()
  noexcept
  : xor_file_adapter{
      std::byte{58u},
      std::byte{129u},
      core::allocator{}
    }
{

}


alloy::io::xor_file_adapter::xor_file_adapter(core::allocator alloc)
  noexcept
  : xor_file_adapter{
      std::byte{58u},
      std::byte{129u},
      alloc
    }
{

}


alloy::io::xor_file_adapter::xor_file_adapter(std::byte b0,
                                              std::byte b1)
  noexcept
  : xor_file_adapter{
      b0,
      b1,
      core::allocator{}
    }
{

}


alloy::io::xor_file_adapter::xor_file_adapter(std::byte b0,
                                              std::byte b1,
                                              core::allocator alloc)
  noexcept
  : m_allocator{alloc},
    m_byte0{b0},
    m_byte1{b1}
{

}

//-----------------------------------------------------------------------------
// Virtual Hooks
//-----------------------------------------------------------------------------

alloy::io::file alloy::io::xor_file_adapter::do_adapt(file f)
  noexcept
{
  auto* stream = m_allocator.make<::xor_file_stream>(
    f.release(),
    m_byte0,
    m_byte1
  );

  return file{stream, this};
}

void alloy::io::xor_file_adapter::finalize(file_stream* stream)
  noexcept
{
  auto* p = static_cast<::xor_file_stream*>(stream);

  m_allocator.dispose<::xor_file_stream>(p);
}
