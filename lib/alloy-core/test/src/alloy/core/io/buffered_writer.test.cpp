// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include "test_doubles.hpp"

#include <alloy/core/io/buffered_writer.hpp>
#include <alloy/core/io/error.hpp>
#include <alloy/core/memory/allocator.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <algorithm>    // std::equal, std::min
#include <array>        // std::array
#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code, std::errc
#include <utility>      // std::move
#include <vector>       // std::vector

namespace alloy::core::test {
namespace {

//=============================================================================
// non-member functions
//=============================================================================

/// \brief Writes every byte of \p bytes to \p destination with writes of at
///        most \p request bytes
///
/// \param destination the writer to write to
/// \param bytes the bytes to write
/// \param request the largest number of bytes to give to each write
auto write_all_by(
  writer& destination,
  std::span<const std::byte> bytes,
  std::size_t request
) -> void
{
  while (!bytes.empty()) {
    const auto count = std::min(request, bytes.size());
    REQUIRE( write_all(destination, bytes.first(count)).has_value() );
    bytes = bytes.subspan(count);
  }
}

/// \brief Makes a buffered_writer of \p destination that holds \p capacity
///        bytes
///
/// \param capacity the capacity of the buffer
/// \param destination the writer to write to
/// \return the buffered_writer
auto make_buffered_writer(std::size_t capacity, writer& destination)
  -> buffered_writer
{
  auto made = buffered_writer::with_capacity(capacity, &destination);
  REQUIRE( made.has_value() );
  return std::move(made).value();
}

//=============================================================================
// class : buffered_writer
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("buffered_writer::from_writer(not_null<writer*>, allocator)",
          "[ctor]")
{
  auto storage = std::array<std::byte,16>{};

  SECTION("Allocation succeeds")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    const auto sut = buffered_writer::from_writer(&destination);

    // Assert
    SECTION("Uses the default capacity")
    {
      REQUIRE( sut.has_value() );
      REQUIRE( sut->capacity() == buffered_writer::default_capacity );
    }
    SECTION("Starts with an empty buffer")
    {
      REQUIRE( sut.has_value() );
      REQUIRE( sut->buffer().empty() );
    }
    SECTION("Writes to the destination")
    {
      REQUIRE( sut.has_value() );
      REQUIRE( &sut->destination() == &destination );
    }
    SECTION("Does not write to the destination yet")
    {
      REQUIRE( destination.write_count() == 0u );
    }
  }

  SECTION("Allocation fails")
  {
    // Arrange
    auto destination = fake_writer{storage};
    auto resource = counting_resource{};
    resource.fail_allocations();

    // Act
    const auto sut = buffered_writer::from_writer(
      &destination,
      allocator{&resource}
    );

    // Assert
    SECTION("Fails with not enough memory")
    {
      REQUIRE( sut.has_error() );
      REQUIRE( sut.error() == std::errc::not_enough_memory );
    }
  }
}

TEST_CASE("buffered_writer::with_capacity(std::size_t, not_null<writer*>, "
          "allocator)", "[ctor]")
{
  auto storage = std::array<std::byte,16>{};

  SECTION("Allocation succeeds")
  {
    // Arrange
    const auto capacity = GENERATE(
      std::size_t{1}, std::size_t{16}, std::size_t{4096}
    );
    auto destination = fake_writer{storage};
    auto resource = counting_resource{};

    // Act
    const auto sut = buffered_writer::with_capacity(
      capacity,
      &destination,
      allocator{&resource}
    );

    // Assert
    SECTION("Uses the requested capacity")
    {
      REQUIRE( sut.has_value() );
      REQUIRE( sut->capacity() == capacity );
    }
    SECTION("Allocates the buffer from the allocator once")
    {
      REQUIRE( resource.allocations() == 1u );
    }
    SECTION("Starts with an empty buffer")
    {
      REQUIRE( sut.has_value() );
      REQUIRE( sut->buffer().empty() );
    }
  }

  SECTION("Allocation fails")
  {
    // Arrange
    auto destination = fake_writer{storage};
    auto resource = counting_resource{};
    resource.fail_allocations();

    // Act
    const auto sut = buffered_writer::with_capacity(
      16u,
      &destination,
      allocator{&resource}
    );

    // Assert
    SECTION("Fails with not enough memory")
    {
      REQUIRE( sut.has_error() );
      REQUIRE( sut.error() == std::errc::not_enough_memory );
    }
  }
}

TEST_CASE("buffered_writer::buffered_writer(buffered_writer&&)", "[ctor]")
{
  // Arrange
  const auto input = sequence_bytes(3u);
  auto storage = std::array<std::byte,16>{};
  auto destination = fake_writer{storage};
  auto resource = counting_resource{};
  auto made = buffered_writer::with_capacity(
    8u,
    &destination,
    allocator{&resource}
  );
  REQUIRE( made.has_value() );
  auto& other = *made;
  REQUIRE( other.write(input).has_value() );
  const auto expected = other.buffer();

  // Act
  auto sut = std::move(other);

  // Assert
  SECTION("Takes the capacity of the other writer")
  {
    REQUIRE( sut.capacity() == 8u );
  }
  SECTION("Takes the buffered bytes of the other writer")
  {
    REQUIRE( sut.buffer().data() == expected.data() );
    REQUIRE( sut.buffer().size() == expected.size() );
  }
  SECTION("Takes the destination of the other writer")
  {
    REQUIRE( &sut.destination() == &destination );
  }
  SECTION("Leaves the other writer without a buffer")
  {
    // NOLINTNEXTLINE(bugprone-use-after-move)
    REQUIRE( other.capacity() == 0u );
    REQUIRE( other.buffer().empty() );
  }
  SECTION("Does not allocate another buffer")
  {
    REQUIRE( resource.allocations() == 1u );
  }
}

TEST_CASE("buffered_writer::~buffered_writer()", "[dtor]")
{
  const auto input = sequence_bytes(3u);
  auto storage = std::array<std::byte,16>{};
  auto destination = fake_writer{storage};
  auto resource = counting_resource{};

  SECTION("Writer has buffered bytes")
  {
    // Arrange
    auto made = buffered_writer::with_capacity(
      8u,
      &destination,
      allocator{&resource}
    );
    REQUIRE( made.has_value() );
    REQUIRE( made->write(input).has_value() );

    // Act
    {
      auto sut = std::move(made).value();
    }

    // Assert
    SECTION("Releases the buffer once")
    {
      REQUIRE( resource.deallocations() == 1u );
    }
    SECTION("Flushes the buffered bytes")
    {
      const auto written = destination.written();

      REQUIRE(
        std::equal(written.begin(), written.end(), input.begin(), input.end())
      );
    }
  }

  SECTION("Writer was moved from")
  {
    // Arrange
    auto made = buffered_writer::with_capacity(
      8u,
      &destination,
      allocator{&resource}
    );
    REQUIRE( made.has_value() );

    // Act
    {
      auto sut = std::move(made).value();
      auto other = std::move(sut);
    }

    // Assert
    SECTION("Releases the buffer only once")
    {
      REQUIRE( resource.deallocations() == 1u );
    }
  }
}

TEST_CASE("buffered_writer::operator=(buffered_writer&&)", "[assignment]")
{
  // Arrange
  const auto input = sequence_bytes(3u);
  auto storage = std::array<std::byte,16>{};
  auto other_storage = std::array<std::byte,16>{};
  auto destination = fake_writer{storage};
  auto other_destination = fake_writer{other_storage};
  auto resource = counting_resource{};
  auto sut_made = buffered_writer::with_capacity(
    4u,
    &destination,
    allocator{&resource}
  );
  auto other_made = buffered_writer::with_capacity(
    8u,
    &other_destination,
    allocator{&resource}
  );
  REQUIRE( sut_made.has_value() );
  REQUIRE( other_made.has_value() );
  auto& sut = *sut_made;
  auto& other = *other_made;
  REQUIRE( other.write(input).has_value() );
  const auto expected = other.buffer();

  // Act
  sut = std::move(other);

  // Assert
  SECTION("Releases the previous buffer")
  {
    REQUIRE( resource.deallocations() == 1u );
  }
  SECTION("Takes the capacity of the other writer")
  {
    REQUIRE( sut.capacity() == 8u );
  }
  SECTION("Takes the buffered bytes of the other writer")
  {
    REQUIRE( sut.buffer().data() == expected.data() );
    REQUIRE( sut.buffer().size() == expected.size() );
  }
  SECTION("Takes the destination of the other writer")
  {
    REQUIRE( &sut.destination() == &other_destination );
  }
  SECTION("Leaves the other writer without a buffer")
  {
    // NOLINTNEXTLINE(bugprone-use-after-move)
    REQUIRE( other.capacity() == 0u );
    REQUIRE( other.buffer().empty() );
  }
}

//-----------------------------------------------------------------------------
// Buffering
//-----------------------------------------------------------------------------

TEST_CASE("buffered_writer::flush()", "[buffering]")
{
  static constexpr auto capacity = std::size_t{16};
  const auto input = sequence_bytes(10u);

  SECTION("Buffer has bytes")
  {
    // Arrange
    const auto chunk_size = GENERATE(
      std::size_t{1}, std::size_t{3}, std::size_t{100}
    );
    auto storage = std::array<std::byte,32>{};
    auto destination = fake_writer{storage};
    destination.set_chunk_size(chunk_size);
    auto sut = make_buffered_writer(capacity, destination);
    REQUIRE( sut.write(input).has_value() );

    // Act
    const auto result = sut.flush();

    // Assert
    SECTION("Succeeds")
    {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes every buffered byte in order")
    {
      const auto written = destination.written();

      REQUIRE(
        std::equal(written.begin(), written.end(), input.begin(), input.end())
      );
    }
    SECTION("Empties the buffer")
    {
      REQUIRE( sut.buffer().empty() );
    }
  }

  SECTION("Buffer is empty")
  {
    // Arrange
    auto storage = std::array<std::byte,32>{};
    auto destination = fake_writer{storage};
    auto sut = make_buffered_writer(capacity, destination);

    // Act
    const auto result = sut.flush();

    // Assert
    SECTION("Succeeds")
    {
      REQUIRE( result.has_value() );
    }
    SECTION("Does not write to the destination")
    {
      REQUIRE( destination.write_count() == 0u );
    }
  }

  SECTION("Destination stops accepting bytes")
  {
    // Arrange
    auto storage = std::array<std::byte,4>{};
    auto destination = fake_writer{storage};
    auto sut = make_buffered_writer(capacity, destination);
    REQUIRE( sut.write(input).has_value() );

    // Act
    const auto result = sut.flush();

    // Assert
    SECTION("Fails with write zero")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == io_error::write_zero );
    }
    SECTION("Keeps the bytes that were not written")
    {
      const auto buffer = sut.buffer();

      REQUIRE(
        std::equal(buffer.begin(), buffer.end(), input.begin() + 4, input.end())
      );
    }
  }

  SECTION("Destination fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto storage = std::array<std::byte,4>{};
    auto destination = fake_writer{storage};
    destination.fail_when_full(expected);
    auto sut = make_buffered_writer(capacity, destination);
    REQUIRE( sut.write(input).has_value() );

    // Act
    const auto result = sut.flush();

    // Assert
    SECTION("Returns the error of the destination")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == expected );
    }
    SECTION("Keeps the bytes that were not written")
    {
      const auto buffer = sut.buffer();

      REQUIRE(
        std::equal(buffer.begin(), buffer.end(), input.begin() + 4, input.end())
      );
    }
  }

  SECTION("Flushing is retried after a failure")
  {
    // Arrange
    auto storage = std::array<std::byte,16>{};
    auto destination = fake_writer{storage};
    destination.set_chunk_size(0u);
    auto sut = make_buffered_writer(capacity, destination);
    REQUIRE( sut.write(input).has_value() );
    REQUIRE( sut.flush().has_error() );
    destination.set_chunk_size(3u);

    // Act
    const auto result = sut.flush();

    // Assert
    SECTION("Writes every buffered byte in order")
    {
      const auto written = destination.written();

      REQUIRE( result.has_value() );
      REQUIRE(
        std::equal(written.begin(), written.end(), input.begin(), input.end())
      );
    }
  }
}

//-----------------------------------------------------------------------------
// Writing
//-----------------------------------------------------------------------------

TEST_CASE("buffered_writer::write(std::span<const std::byte>)", "[writing]")
{
  static constexpr auto capacity = std::size_t{16};
  const auto input = sequence_bytes(300u);

  SECTION("Writes are interleaved with flushes")
  {
    // Arrange
    const auto chunk_size = GENERATE(
      std::size_t{1}, std::size_t{5}, std::size_t{16}, std::size_t{1000}
    );
    const auto request = GENERATE(
      std::size_t{1}, std::size_t{7}, std::size_t{15}, std::size_t{16},
      std::size_t{33}
    );
    auto storage = std::vector<std::byte>(input.size());
    auto destination = fake_writer{storage};
    destination.set_chunk_size(chunk_size);
    auto sut = make_buffered_writer(capacity, destination);

    // Act
    write_all_by(sut, input, request);
    const auto result = sut.flush();

    // Assert
    SECTION("Writes every byte to the destination in order")
    {
      const auto written = destination.written();

      REQUIRE( result.has_value() );
      REQUIRE(
        std::equal(written.begin(), written.end(), input.begin(), input.end())
      );
    }
  }

  SECTION("Write fits in the buffer")
  {
    // Arrange
    auto storage = std::vector<std::byte>(input.size());
    auto destination = fake_writer{storage};
    auto sut = make_buffered_writer(capacity, destination);
    const auto bytes = std::span{input}.first(capacity - 1u);

    // Act
    const auto result = sut.write(bytes);

    // Assert
    SECTION("Returns the number of bytes given")
    {
      REQUIRE( result.has_value() );
      REQUIRE( *result == bytes.size() );
    }
    SECTION("Holds the bytes in the buffer")
    {
      const auto buffer = sut.buffer();

      REQUIRE(
        std::equal(buffer.begin(), buffer.end(), bytes.begin(), bytes.end())
      );
    }
    SECTION("Does not write to the destination")
    {
      REQUIRE( destination.write_count() == 0u );
    }
  }

  SECTION("Write does not fit in the space left in the buffer")
  {
    // Arrange
    auto storage = std::vector<std::byte>(input.size());
    auto destination = fake_writer{storage};
    auto sut = make_buffered_writer(capacity, destination);
    REQUIRE( sut.write(std::span{input}.first(10u)).has_value() );
    const auto bytes = std::span{input}.subspan(10u, 6u);

    // Act
    const auto result = sut.write(bytes);

    // Assert
    SECTION("Returns the number of bytes given")
    {
      REQUIRE( result.has_value() );
      REQUIRE( *result == bytes.size() );
    }
    SECTION("Writes the previously buffered bytes to the destination")
    {
      const auto written = destination.written();

      REQUIRE( written.size() == 10u );
      REQUIRE( std::equal(written.begin(), written.end(), input.begin()) );
    }
    SECTION("Holds the new bytes in the buffer")
    {
      const auto buffer = sut.buffer();

      REQUIRE(
        std::equal(buffer.begin(), buffer.end(), bytes.begin(), bytes.end())
      );
    }
  }

  SECTION("Write is at least as large as the buffer, which is empty")
  {
    // Arrange
    const auto request = GENERATE(capacity, capacity + 1u, capacity * 4u);
    auto storage = std::vector<std::byte>(input.size());
    auto destination = fake_writer{storage};
    auto sut = make_buffered_writer(capacity, destination);
    const auto bytes = std::span{input}.first(request);

    // Act
    const auto result = sut.write(bytes);

    // Assert
    SECTION("Writes directly from the buffer of the caller")
    {
      REQUIRE( destination.write_count() == 1u );
      REQUIRE( destination.last_request_size() == request );
    }
    SECTION("Returns the number of bytes written")
    {
      REQUIRE( result.has_value() );
      REQUIRE( *result == request );
    }
    SECTION("Leaves the buffer empty")
    {
      REQUIRE( sut.buffer().empty() );
    }
  }

  SECTION("Write is at least as large as the buffer, which has bytes")
  {
    // Arrange
    auto storage = std::vector<std::byte>(input.size());
    auto destination = fake_writer{storage};
    auto sut = make_buffered_writer(capacity, destination);
    REQUIRE( sut.write(std::span{input}.first(4u)).has_value() );
    const auto bytes = std::span{input}.subspan(4u, capacity);

    // Act
    const auto result = sut.write(bytes);

    // Assert
    SECTION("Returns the number of bytes written")
    {
      REQUIRE( result.has_value() );
      REQUIRE( *result == bytes.size() );
    }
    SECTION("Writes the buffered bytes before the new bytes")
    {
      const auto written = destination.written();

      REQUIRE( written.size() == 4u + capacity );
      REQUIRE( std::equal(written.begin(), written.end(), input.begin()) );
    }
    SECTION("Leaves the buffer empty")
    {
      REQUIRE( sut.buffer().empty() );
    }
  }

  SECTION("Destination fails while making space")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto storage = std::array<std::byte,4>{};
    auto destination = fake_writer{storage};
    destination.fail_when_full(expected);
    auto sut = make_buffered_writer(capacity, destination);
    REQUIRE( sut.write(std::span{input}.first(10u)).has_value() );

    // Act
    const auto result = sut.write(std::span{input}.subspan(10u, 6u));

    // Assert
    SECTION("Returns the error of the destination")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == expected );
    }
    SECTION("Keeps the bytes that were not written")
    {
      const auto buffer = sut.buffer();

      REQUIRE(
        std::equal(
          buffer.begin(), buffer.end(), input.begin() + 4, input.begin() + 10
        )
      );
    }
  }

  SECTION("Destination fails on a write that bypasses the buffer")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto destination = fake_writer{std::span<std::byte>{}};
    destination.fail_when_full(expected);
    auto sut = make_buffered_writer(capacity, destination);

    // Act
    const auto result = sut.write(std::span{input}.first(capacity));

    // Assert
    SECTION("Returns the error of the destination")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == expected );
    }
  }
}

} // namespace
} // namespace alloy::core::test
