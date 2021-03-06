#include "alloy/io/filesystem/random_filesystem.hpp"
#include "alloy/io/filesystem/file_stream.hpp"
#include "alloy/io/filesystem/file_stream_finalizer.hpp"

#include "alloy/core/intrinsics.hpp" // core::compiler::unused

#include <cstddef> // std::byte
#include <random>  // std::random_device
#include <mutex>   // std::mutex
#include <cstdint> // std::uint16_t

namespace {

  //===========================================================================
  // class : random_file_stream_finalizer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A finalizer for random file streams
  ///
  /// This class is effectively just a no-op
  /////////////////////////////////////////////////////////////////////////////
  class random_file_stream_finalizer final
    : public alloy::io::file_stream_finalizer
  {
    //-------------------------------------------------------------------------
    // Constructors
    //-------------------------------------------------------------------------
  public:

    random_file_stream_finalizer() = default;

    //-------------------------------------------------------------------------
    // Finalizes
    //-------------------------------------------------------------------------
  public:

    /// \brief Finalizes the random file
    ///
    /// This is a no-op
    ///
    /// \param stream the stream to finalize
    void finalize(alloy::io::file_stream* stream) noexcept override;
  };

  //===========================================================================
  // class : random_file_stream
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A random file stream
  ///
  /// This file will always discard writing input, and will read random data
  /////////////////////////////////////////////////////////////////////////////
  class random_file_stream final
    : public alloy::io::file_stream
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using seed_type = std::random_device::result_type;
    using engine_type = std::mt19937;
    using distribution_type = std::uniform_int_distribution<std::uint16_t>;

    //-------------------------------------------------------------------------
    // Constructor
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs this random file stream with the given random seed
    ///
    /// \param seed the seed to use
    explicit random_file_stream(seed_type seed) noexcept;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Returns 0
    ///
    /// \return 0
    alloy::core::result<size_type,std::error_code> bytes() const noexcept override;

    //-------------------------------------------------------------------------
    // File API
    //-------------------------------------------------------------------------
  public:

    /// \brief Closes this file
    ///
    /// This is a no-op for random files
    void close() noexcept override;

    /// \brief Resets the file cursor back to the start position
    ///
    /// \return void on success
    alloy::core::result<void,std::error_code> reset() noexcept override;

    /// \brief Skips the next \p offset bytes
    ///
    /// This is a no-op for random files
    ///
    /// \param offset ignored
    /// \return void
    alloy::core::result<void,std::error_code>
      skip(offset_type offset) noexcept override;

    /// \brief Reads random values into \p buffer
    ///
    /// \param buffer the buffer to read into
    /// \return the buffer
    alloy::core::result<alloy::io::mutable_buffer,std::error_code>
      read(alloy::io::mutable_buffer buffer) noexcept override;

    /// \brief 'Writes' \p buffer to the random file
    ///
    /// This is a no-op
    ///
    /// \param buffer the buffer to write
    /// \return buffer
    alloy::core::result<alloy::io::const_buffer,std::error_code>
      write(alloy::io::const_buffer buffer) noexcept override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    engine_type m_engine;
    distribution_type m_distribution;
  };

  //===========================================================================
  // definitions : class : random_file_stream_finalizer
  //===========================================================================

  void random_file_stream_finalizer::finalize(alloy::io::file_stream* stream)
    noexcept
  {
    alloy::core::compiler::unused(stream);
  }

  //===========================================================================
  // definitions : class : random_file_stream
  //===========================================================================

  //---------------------------------------------------------------------------
  // Constructors
  //---------------------------------------------------------------------------

  random_file_stream::random_file_stream(seed_type seed)
    noexcept
    : m_engine{seed},
      m_distribution{0u,255u}
  {

  }

  //---------------------------------------------------------------------------
  // Observers
  //---------------------------------------------------------------------------

  alloy::core::result<alloy::io::file_stream::size_type,std::error_code>
    random_file_stream::bytes()
    const noexcept
  {
    return 0u;
  }

  //---------------------------------------------------------------------------
  // File API
  //---------------------------------------------------------------------------

  void random_file_stream::close()
    noexcept
  {
    // Nothing to do
  }

  alloy::core::result<void,std::error_code> random_file_stream::reset()
    noexcept
  {
    return {};
  }

  alloy::core::result<void,std::error_code>
    random_file_stream::skip(offset_type offset)
    noexcept
  {
    alloy::core::compiler::unused(offset);

    return {};
  }

  alloy::core::result<alloy::io::mutable_buffer,std::error_code>
    random_file_stream::read(alloy::io::mutable_buffer buffer)
    noexcept
  {
    auto* const end = buffer.data() + buffer.size();
    auto* const begin = buffer.data();

    // Generate random numbers and writes them into the mutable buffer
    for (auto* it = begin; it != end; ++it) {
      const auto v = m_distribution(m_engine);
      *it = std::byte{static_cast<std::uint8_t>(v)};
    }

    return buffer;
  }

  alloy::core::result<alloy::io::const_buffer,std::error_code>
    random_file_stream::write(alloy::io::const_buffer buffer)
    noexcept
  {
    return buffer;
  }

} // namespace <anonymous>

//=============================================================================
// definitions : class : random_filesystem
//=============================================================================

//-----------------------------------------------------------------------------
// Filesystem
//-----------------------------------------------------------------------------

alloy::io::file alloy::io::random_filesystem::open()
  noexcept
{
  static auto s_mutex = std::mutex{};
  static auto s_random_device = std::random_device{};
  static auto s_finalizer = ::random_file_stream_finalizer{};

  // Each separate thread gets its own file stream seeded from a single random
  // device. This is done to ensure thread-safety of the engine, while also
  // ensuring that different threads don't have deterministic values.
  thread_local auto s_file_stream = [&]
  {
    // Ensure that s_random_device is not invoked in parallel
    auto guard = std::lock_guard{s_mutex};
    core::compiler::unused(guard);

    return random_file_stream{s_random_device()};
  }();

  return file{&s_file_stream, &s_finalizer};
}
