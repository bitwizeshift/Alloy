#include "alloy/io/filesystem/crc16_file_adapter.hpp"

#include "alloy/core/intrinsics.hpp"

#include <array>     // std::array
#include <algorithm> // std::min
#include <cstdint>   // std::uint16_t
#include <cstddef>   // std::byte
#include <string>

namespace {

  //===========================================================================
  // class : error_category
  //===========================================================================

  class error_category final
    : public std::error_category
  {
  public:

    const char* name() const noexcept override
    {
      return "crc16 file error";
    }

    std::string message(int ec) const override
    {
      using error_code = alloy::io::crc16_file_adapter::error_code;
      using std::literals::string_literals::operator""s;

      const auto code = static_cast<error_code>(ec);

      switch (code) {
        case error_code::bad_checksum: {
          return "file contains invalid checksum"s;
        }
        case error_code::unable_to_compute_checksum: {
          return "unable to compute checksum"s;
        }
        case error_code::unable_to_read_checksum: {
          return "unable to read checksum from file"s;
        }
        case error_code::unable_to_write_checksum: {
          return "unable to write checksum to file"s;
        }
        default: break;
      }
      return "unknown error code"s;
    }
  };

  //===========================================================================
  // class : crc16_file_stream
  //===========================================================================

  class crc16_file_stream final
    : public alloy::io::file_stream
  {

    using error_code = alloy::io::crc16_file_adapter::error_code;

    static constexpr auto header_size = std::size_t{2u};

    //-------------------------------------------------------------------------
    // Constructor
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs this CRC16 file stream from the given file
    ///
    /// \param file the file to adapt
    explicit crc16_file_stream(alloy::io::file file) noexcept;

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
    bool m_is_valid;

    //-------------------------------------------------------------------------
    // Checksum
    //-------------------------------------------------------------------------
  private:

    alloy::core::expected<std::uint16_t>
      read_checksum() noexcept;

    alloy::core::expected<void>
      write_checksum(std::uint16_t checksum) noexcept;

    alloy::core::expected<std::uint16_t> compute_checksum() noexcept;
  };

  //===========================================================================
  // inline definitions : class : crc16_file_stream
  //===========================================================================

  //---------------------------------------------------------------------------
  // Constructors
  //---------------------------------------------------------------------------

  crc16_file_stream::crc16_file_stream(alloy::io::file file)
    noexcept
    : m_file{std::move(file)},
      m_is_valid{false}
  {
    auto expected = read_checksum();
    if (!expected) {
      return;
    }

    auto actual = compute_checksum();
    if (!actual) {
      return;
    }

    m_is_valid = (actual.value() == expected.value());

    reset();
  }

  //---------------------------------------------------------------------------
  // Observers
  //---------------------------------------------------------------------------

  alloy::core::expected<alloy::io::file_stream::size_type>
    crc16_file_stream::bytes()
    const noexcept
  {
    if (ALLOY_UNLIKELY(!m_is_valid)) {
      return alloy::core::unexpected(error_code::bad_checksum);
    }

    auto result = m_file.bytes();
    if (ALLOY_UNLIKELY(!result)) {
      return alloy::core::unexpected(result.error());
    }

    return result.value() - header_size;
  }

  //---------------------------------------------------------------------------
  // File API
  //---------------------------------------------------------------------------

  void crc16_file_stream::close()
    noexcept
  {
    // Only write checksum if the file has a correct checksum
    if (ALLOY_LIKELY(m_is_valid)) {
      const auto checksum = compute_checksum();

      if (ALLOY_UNLIKELY(!checksum)) {
        write_checksum(checksum.value());
      }
    }

    m_file.close();
  }


  alloy::core::expected<void> crc16_file_stream::reset()
    noexcept
  {
    if (ALLOY_UNLIKELY(!m_is_valid)) {
      return alloy::core::unexpected(error_code::bad_checksum);
    }

    auto result = m_file.reset();
    if (ALLOY_UNLIKELY(!result)) {
      return result;
    }
    return m_file.skip(header_size);
  }


  alloy::core::expected<void>
    crc16_file_stream::skip(offset_type offset)
    noexcept
  {
    if (ALLOY_UNLIKELY(!m_is_valid)) {
      return alloy::core::unexpected(error_code::bad_checksum);
    }
    return m_file.skip(offset);
  }


  alloy::core::expected<alloy::io::mutable_buffer>
    crc16_file_stream::read(alloy::io::mutable_buffer buffer)
    noexcept
  {
    if (ALLOY_UNLIKELY(!m_is_valid)) {
      return alloy::core::unexpected(error_code::bad_checksum);
    }
    return m_file.read(buffer);
  }


  alloy::core::expected<alloy::io::const_buffer>
    crc16_file_stream::write(alloy::io::const_buffer buffer)
    noexcept
  {
    if (ALLOY_UNLIKELY(!m_is_valid)) {
      return alloy::core::unexpected(error_code::bad_checksum);
    }
    return m_file.write(buffer);
  }

  //---------------------------------------------------------------------------
  // Checksum
  //---------------------------------------------------------------------------

  alloy::core::expected<std::uint16_t>
    crc16_file_stream::read_checksum()
    noexcept
  {
    auto storage = std::array<std::byte,header_size>{};
    auto buffer = alloy::io::mutable_buffer::from_container(storage);

    m_file.reset();

    auto result = m_file.read(buffer);
    if (!result) {
      return alloy::core::unexpected(error_code::unable_to_read_checksum);
    }
    buffer = result.value();
    if (buffer.size() != header_size) {
      return alloy::core::unexpected(error_code::unable_to_read_checksum);
    }
    const auto b0 = static_cast<std::uint16_t>(buffer.data()[0]);
    const auto b1 = static_cast<std::uint16_t>(buffer.data()[1]);

    const auto checksum = static_cast<std::uint16_t>((b0 << 8u) | b1);

    return checksum;
  }


  alloy::core::expected<void>
    crc16_file_stream::write_checksum(std::uint16_t checksum)
    noexcept
  {
    auto storage = std::array<std::byte,header_size>{};

    const auto b0 = static_cast<std::uint8_t>((checksum & 0xff00) >> 8u);
    const auto b1 = static_cast<std::uint8_t>((checksum & 0x00ff));
    storage[0] = std::byte{b0};
    storage[1] = std::byte{b1};

    auto buffer = alloy::io::const_buffer::from_container(storage);

    // Write the checksum at the beginning of the file
    m_file.reset();
    auto result = m_file.write(buffer);

    if (!result) {
      return alloy::core::unexpected(error_code::unable_to_write_checksum);
    }
    buffer = result.value();

    if (buffer.size() != header_size) {
      return alloy::core::unexpected(error_code::unable_to_write_checksum);
    }
    return {};
  }


  alloy::core::expected<std::uint16_t>
    crc16_file_stream::compute_checksum()
    noexcept
  {
    static constexpr auto buffer_size = std::size_t{128};
    static constexpr auto crc16 = std::uint16_t{0x8005};

    thread_local auto s_buffer = std::array<std::byte,buffer_size>{};

    reset();

    auto out = std::uint16_t{0};
    auto bits_read = std::uint32_t{};
    auto bit_flag = std::uint32_t{};

    auto bytes_result = m_file.bytes();

    if (!bytes_result) {
      return alloy::core::unexpected(bytes_result.error());
    }

    auto size = bytes_result.value();

    while (size > 0u) {
      auto buffer = alloy::io::mutable_buffer::from_container(s_buffer);

      auto result = m_file.read(buffer);

      if (!result) {
        if (result.error() == alloy::io::file::error_code::eof) {
          break;
        }
        return alloy::core::unexpected(result.error());
      }
      buffer = result.value();

      auto i = std::size_t{0u};
      auto* const data = buffer.data();

      for (; i < buffer.size(); ++i) {
        bit_flag = out >> 15u;

        out <<= 1u;
        out |= (static_cast<std::uint16_t>(data[i]) >> bits_read) & 1u;

        bits_read++;
        if (bits_read > 7u) {
          bits_read = 0u;
          size--;
        }

        if (bit_flag) {
          out ^= crc16;
        }
      }
    }

    auto i = std::uint32_t{0u};

    for (; i < 16u; ++i) {
      bit_flag = out >> 15;
      out <<= 1;
      if (bit_flag) {

      }
      out ^= crc16;
    }

    auto crc = std::uint32_t{0};
    i = std::uint32_t{0x8000};
    auto j = std::uint32_t{0x0001};

    for (; i != 0; i >>=1, j <<= 1) {
      if (i & out) {
        crc |= j;
      }
    }

    return crc;
  }
} // namespace <anonymous>

//=============================================================================
// definitions : class : crc16_file_adapter
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

alloy::io::crc16_file_adapter::crc16_file_adapter()
  noexcept
  : crc16_file_adapter{
      core::allocator{}
    }
{

}


alloy::io::crc16_file_adapter::crc16_file_adapter(core::allocator alloc)
  noexcept
  : m_allocator{alloc}
{

}

//-----------------------------------------------------------------------------
// Virtual Hooks
//-----------------------------------------------------------------------------

alloy::io::file alloy::io::crc16_file_adapter::do_adapt(file f)
  noexcept
{
  auto* stream = m_allocator.make<::crc16_file_stream>(
    std::move(f)
  );

  return file{stream, this};
}

void alloy::io::crc16_file_adapter::finalize(file_stream* stream)
  noexcept
{
  auto* p = static_cast<::crc16_file_stream*>(stream);

  m_allocator.dispose<::crc16_file_stream>(p);
}

//=============================================================================
// definitions : non-member functions : class : crc16_file_adapter
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

std::error_code alloy::io::make_error_code(crc16_file_adapter::error_code ec)
  noexcept
{
  static auto s_category = ::error_category{};

  return std::error_code{static_cast<int>(ec), s_category};
}
