#include "alloy/io/filesystem/xor_file_adapter.hpp"

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
    explicit xor_file_stream(alloy::io::file file,
                             std::byte b0,
                             std::byte b1) noexcept;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Returns the size of the file
    ///
    /// \return the size of the file
    alloy::core::expected<size_type> bytes() const noexcept override;

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
    alloy::core::expected<void> reset() noexcept override;

    /// \brief Skips the next \p offset bytes
    ///
    /// This just passes through to the adapted file
    ///
    /// \param offset the offset to skip past
    /// \return the result of skipping
    alloy::core::expected<void>
      skip(offset_type offset) noexcept override;

    /// \brief Reads values from the file and XOR's the result
    ///
    /// \param buffer the buffer to read into
    /// \return a buffer containing the read result
    alloy::core::expected<alloy::io::mutable_buffer>
      read(alloy::io::mutable_buffer buffer) noexcept override;

    /// \brief Writes values into the buffer and XOR's them
    ///
    /// \param buffer the buffer to write
    /// \return a buffer of the written bytes
    alloy::core::expected<alloy::io::const_buffer>
      write(alloy::io::const_buffer buffer) noexcept override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    alloy::io::file m_file;
    std::byte m_byte0;
    std::byte m_byte1;
  };

  //===========================================================================
  // inline definitions : class : xor_file_stream
  //===========================================================================

  //---------------------------------------------------------------------------
  // Constructors
  //---------------------------------------------------------------------------

  xor_file_stream::xor_file_stream(alloy::io::file file,
                                   std::byte b0,
                                   std::byte b1)
    noexcept
    : m_file{std::move(file)},
      m_byte0{b0},
      m_byte1{b1}
  {

  }

  //---------------------------------------------------------------------------
  // Observers
  //---------------------------------------------------------------------------

  alloy::core::expected<alloy::io::file_stream::size_type>
    xor_file_stream::bytes()
    const noexcept
  {
    return m_file.bytes();
  }

  //---------------------------------------------------------------------------
  // File API
  //---------------------------------------------------------------------------

  void xor_file_stream::close()
    noexcept
  {
    m_file.close();
  }

  alloy::core::expected<void> xor_file_stream::reset()
    noexcept
  {
    return m_file.reset();
  }

  alloy::core::expected<void>
    xor_file_stream::skip(offset_type offset)
    noexcept
  {
    return m_file.skip(offset);
  }

  alloy::core::expected<alloy::io::mutable_buffer>
    xor_file_stream::read(alloy::io::mutable_buffer buffer)
    noexcept
  {
    auto result = m_file.read(buffer);
    if (!result) {
      return result;
    }
    auto new_buffer = result.value();

    auto* const end   = new_buffer.data() + new_buffer.size();
    auto* const begin = new_buffer.data();
    auto* it = begin;

    // Run the XOR pass
    for (; it != end; ++it) {
      *it ^= m_byte0;
      *it ^= m_byte1;
    }

    return new_buffer;
  }

  alloy::core::expected<alloy::io::const_buffer>
    xor_file_stream::write(alloy::io::const_buffer buffer)
    noexcept
  {
    // Use thread-local to allow parallel files
    static constexpr auto buffer_size = std::size_t{128u};
    thread_local auto s_buffer = std::array<std::byte,buffer_size>{};

    const auto size = buffer.size();
    const auto* const data = buffer.data();

    auto remaining = buffer.size();
    auto i = 0u;

    while (remaining > 0) {
      const auto to_write = std::min(remaining, buffer_size);

      for (auto j = 0u; j < to_write && i < size; ++i, ++j) {
        s_buffer[j] = data[i] ^ m_byte1;
        s_buffer[j] = data[i] ^ m_byte0;
      }

      auto new_buffer = alloy::io::const_buffer{
        s_buffer.data(),
        to_write
      };
      auto result = m_file.write(new_buffer);

      if (!result) {
        return result;
      }
      remaining -= to_write;
    }

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
    std::move(f),
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