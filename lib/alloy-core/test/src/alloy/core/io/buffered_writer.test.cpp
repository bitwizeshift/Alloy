// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/io/buffered_writer.hpp>

#include <alloy/core/io/error.hpp>
#include <alloy/core/io/writer.hpp>
#include "test_doubles.hpp"

#include <alloy/core/intrinsics.hpp>
#include <alloy/core/memory/allocator.hpp>
#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <algorithm>    // std::equal, std::min
#include <array>        // std::array
#include <cstddef>      // std::byte, std::size_t
#include <memory>       // std::allocator_arg
#include <span>         // std::span
#include <system_error> // std::error_code, std::errc
#include <type_traits>  // std::is_same_v, std::is_move_constructible_v
#include <utility>      // std::move, std::in_place
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

//=============================================================================
// class : immovable_writer
//=============================================================================

/// \brief A writer that is neither move constructible nor move assignable
class immovable_writer final : public writer
{
public:

  immovable_writer() noexcept = default;
  immovable_writer(immovable_writer&&) = delete;

  auto operator=(immovable_writer&&) -> immovable_writer& = delete;

private:

  auto do_write(std::span<const std::byte>)
    noexcept -> result<std::size_t,std::error_code> override
  {
    return 0u;
  }
};

//=============================================================================
// class : unassignable_writer
//=============================================================================

/// \brief A writer that is move constructible, but not move assignable
class unassignable_writer final : public writer
{
public:

  unassignable_writer() noexcept = default;
  unassignable_writer(unassignable_writer&&) noexcept = default;

  auto operator=(unassignable_writer&&) -> unassignable_writer& = delete;

private:

  auto do_write(std::span<const std::byte>)
    noexcept -> result<std::size_t,std::error_code> override
  {
    return 0u;
  }
};

//=============================================================================
// class : buffered_writer
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("buffered_writer::buffered_writer(U&&, std::size_t, allocator)",
          "[ctor]")
{
  // Arrange
  auto storage = std::array<std::byte,16>{};

  SECTION("Capacity is not specified")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    const auto sut = buffered_writer{std::move(destination)};

    // Assert
    SECTION("Uses the default capacity")
    {
      using sut_type = buffered_writer<fake_writer>;

      REQUIRE( sut.capacity() == sut_type::default_capacity );
    }
    SECTION("Starts with an empty buffer")
    {
      REQUIRE( sut.buffer().empty() );
    }
    SECTION("Owns the destination")
    {
      REQUIRE( sut.destination().written().data() == storage.data() );
    }
    SECTION("Does not write to the destination yet")
    {
      REQUIRE( sut.destination().write_count() == 0u );
    }
  }

  SECTION("Capacity and allocator are specified")
  {
    // Arrange
    const auto capacity = GENERATE(
      std::size_t{1}, std::size_t{16}, std::size_t{4096}
    );
    auto resource = counting_resource{};

    // Act
    const auto sut = buffered_writer{
      fake_writer{storage},
      capacity,
      allocator{&resource}
    };

    // Assert
    SECTION("Uses the requested capacity")
    {
      REQUIRE( sut.capacity() == capacity );
    }
    SECTION("Does not allocate the buffer yet")
    {
      REQUIRE( resource.allocations() == 0u );
    }
    SECTION("Starts with an empty buffer")
    {
      REQUIRE( sut.buffer().empty() );
    }
  }

  SECTION("Destination is an lvalue")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    auto sut = buffered_writer{destination, 4u};

    // Assert
    SECTION("Owns a copy of the destination")
    {
      REQUIRE( &sut.destination() != &destination );
    }
    SECTION("Does not write to the original destination")
    {
      const auto input = sequence_bytes(8u);

      // Act
      const auto result = sut.write(input);

      // Assert
      SECTION("Accepts bytes into the owned buffer") {
        REQUIRE( result.has_value() );
      }
      SECTION("Leaves the original destination unused") {
        REQUIRE( destination.write_count() == 0u );
      }
    }
  }
}

TEST_CASE("buffered_writer::buffered_writer(std::in_place_t, Args&&...)",
          "[ctor]")
{
  // Arrange
  auto storage = std::array<std::byte,16>{};

  // Act
  const auto sut = buffered_writer<fake_writer>{
    std::in_place,
    std::span<std::byte>{storage}
  };

  // Assert
  SECTION("Uses the default capacity")
  {
    using sut_type = buffered_writer<fake_writer>;

    REQUIRE( sut.capacity() == sut_type::default_capacity );
  }
  SECTION("Constructs the destination from the arguments")
  {
    REQUIRE( sut.destination().written().data() == storage.data() );
  }
  SECTION("Starts with an empty buffer")
  {
    REQUIRE( sut.buffer().empty() );
  }
}

TEST_CASE("buffered_writer::buffered_writer(std::allocator_arg_t, allocator, "
          "std::size_t, std::in_place_t, Args&&...)", "[ctor]")
{
  // Arrange
  const auto input = sequence_bytes(3u);
  auto storage = std::array<std::byte,16>{};
  auto resource = counting_resource{};

  // Act
  auto sut = buffered_writer<fake_writer>{
    std::allocator_arg,
    allocator{&resource},
    8u,
    std::in_place,
    std::span<std::byte>{storage}
  };

  // Assert
  SECTION("Uses the requested capacity")
  {
    REQUIRE( sut.capacity() == 8u );
  }
  SECTION("Constructs the destination from the arguments")
  {
    REQUIRE( sut.destination().written().data() == storage.data() );
  }
  SECTION("Does not allocate before the first use")
  {
    REQUIRE( resource.allocations() == 0u );
  }
  SECTION("Allocates the buffer when it is first used")
  {
    // Act
    const auto result = sut.write(input);

    // Assert
    SECTION("The operation succeeds") {
      REQUIRE( result.has_value() );
    }
    SECTION("The buffer is allocated once") {
      REQUIRE( resource.allocations() == 1u );
    }
  }
}

TEST_CASE("buffered_writer deduction guides", "[ctor]")
{
  // Arrange
  auto storage = std::array<std::byte,16>{};

  SECTION("Destination is an lvalue")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    auto sut = buffered_writer{destination};

    SECTION("Deduces the type of the destination")
    {
      // Act
      constexpr auto property_holds =
        std::is_same_v<decltype(sut), buffered_writer<fake_writer>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("Destination is a const lvalue")
  {
    // Arrange
    const auto destination = fake_writer{storage};

    // Act
    auto sut = buffered_writer{destination};

    SECTION("Deduces the type of the destination without const")
    {
      // Act
      constexpr auto property_holds =
        std::is_same_v<decltype(sut), buffered_writer<fake_writer>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("Destination is an rvalue")
  {
    // Act
    auto sut = buffered_writer{fake_writer{storage}};

    SECTION("Deduces the type of the destination")
    {
      // Act
      constexpr auto property_holds =
        std::is_same_v<decltype(sut), buffered_writer<fake_writer>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("Destination and capacity are given")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    auto sut = buffered_writer{destination, 8u};

    SECTION("Deduces the type of the destination")
    {
      // Act
      constexpr auto property_holds =
        std::is_same_v<decltype(sut), buffered_writer<fake_writer>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("Destination, capacity, and allocator are given")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    auto sut = buffered_writer{destination, 8u, allocator{}};

    SECTION("Deduces the type of the destination")
    {
      // Act
      constexpr auto property_holds =
        std::is_same_v<decltype(sut), buffered_writer<fake_writer>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
}

TEST_CASE("buffered_writer::buffered_writer(buffered_writer&&)", "[ctor]")
{
  SECTION("Destination is move constructible")
  {
    // Arrange
    const auto input = sequence_bytes(3u);
    auto storage = std::array<std::byte,16>{};
    auto resource = counting_resource{};
    auto other = buffered_writer{
      fake_writer{storage},
      8u,
      allocator{&resource}
    };
    compiler::unused(other.write(input));
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
      SECTION("Retains the original buffer allocation") {
        REQUIRE( sut.buffer().data() == expected.data() );
      }
      SECTION("Retains the number of pending bytes") {
        REQUIRE( sut.buffer().size() == expected.size() );
      }
    }
    SECTION("Takes the destination of the other writer")
    {
      REQUIRE( sut.destination().written().data() == storage.data() );
    }
    SECTION("Leaves the other writer without a buffer")
    {
      SECTION("Releases ownership of the moved-from allocation") {
        // NOLINTNEXTLINE(bugprone-use-after-move)
        REQUIRE( other.capacity() == 0u );
      }
      SECTION("Leaves no pending bytes in the moved-from object") {
        // Moved-from buffers remain valid and empty.
        // NOLINTNEXTLINE(bugprone-use-after-move)
        REQUIRE( other.buffer().empty() );
      }
    }
    SECTION("Does not allocate another buffer")
    {
      REQUIRE( resource.allocations() == 1u );
    }
    SECTION("Does not write to the destination")
    {
      REQUIRE( sut.destination().write_count() == 0u );
    }
  }

  SECTION("Destination is not move constructible")
  {
    // Act
    constexpr auto property_holds =
      std::is_move_constructible_v<buffered_writer<immovable_writer>>;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

TEST_CASE("buffered_writer::~buffered_writer()", "[dtor]")
{
  // Arrange
  const auto input = sequence_bytes(3u);
  auto storage = std::array<std::byte,16>{};
  auto destination = fake_writer{storage};
  auto resource = counting_resource{};

  SECTION("Writer has buffered bytes")
  {
    // Act
    {
      auto sut = buffered_writer{
        indirect_writer{&destination},
        8u,
        allocator{&resource}
      };

      // Assert
      REQUIRE( sut.write(input).has_value() );
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

  SECTION("Buffer was never allocated")
  {
    // Act
    {
      auto sut = buffered_writer{
        indirect_writer{&destination},
        8u,
        allocator{&resource}
      };
    }

    // Assert
    SECTION("Does not release a buffer")
    {
      REQUIRE( resource.deallocations() == 0u );
    }
    SECTION("Does not write to the destination")
    {
      REQUIRE( destination.write_count() == 0u );
    }
  }

  SECTION("Writer was moved from")
  {
    // Act
    {
      auto sut = buffered_writer{
        indirect_writer{&destination},
        8u,
        allocator{&resource}
      };

      // Assert
      REQUIRE( sut.write(input).has_value() );
      auto other = std::move(sut);
    }

    // Assert
    SECTION("Releases the buffer only once")
    {
      REQUIRE( resource.deallocations() == 1u );
    }
    SECTION("Flushes the buffered bytes only once")
    {
      SECTION("Flushes the old destination once before assignment") {
        REQUIRE( destination.write_count() == 1u );
      }
      SECTION("Flushes every byte pending in the old destination") {
        REQUIRE( destination.written().size() == input.size() );
      }
    }
  }
}

TEST_CASE("buffered_writer::operator=(buffered_writer&&)", "[assignment]")
{
  SECTION("Destination is move assignable")
  {
    // Arrange
    const auto input = sequence_bytes(3u);
    auto storage = std::array<std::byte,16>{};
    auto other_storage = std::array<std::byte,16>{};
    auto destination = fake_writer{storage};
    auto other_destination = fake_writer{other_storage};
    auto resource = counting_resource{};
    auto sut = buffered_writer{
      indirect_writer{&destination},
      4u,
      allocator{&resource}
    };
    auto other = buffered_writer{
      indirect_writer{&other_destination},
      8u,
      allocator{&resource}
    };
    compiler::unused(sut.write(std::span{input}.first(2u)));
    compiler::unused(other.write(input));
    const auto expected = other.buffer();

    // Act
    sut = std::move(other);

    // Assert
    SECTION("Flushes the previous buffered bytes to the previous destination")
    {
      const auto written = destination.written();

      REQUIRE( std::ranges::equal(
        written, std::span{input}.subspan(0u, 2u)
      ) );
    }
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
      SECTION("Retains the original buffer allocation") {
        REQUIRE( sut.buffer().data() == expected.data() );
      }
      SECTION("Retains the number of pending bytes") {
        REQUIRE( sut.buffer().size() == expected.size() );
      }
    }
    SECTION("Takes the destination of the other writer")
    {
      REQUIRE( &sut.destination().destination() == &other_destination );
    }
    SECTION("Does not write to the new destination")
    {
      REQUIRE( other_destination.write_count() == 0u );
    }
    SECTION("Leaves the other writer without a buffer")
    {
      SECTION("Releases ownership of the moved-from allocation") {
        // NOLINTNEXTLINE(bugprone-use-after-move)
        REQUIRE( other.capacity() == 0u );
      }
      SECTION("Leaves no pending bytes in the moved-from object") {
        // Moved-from buffers remain valid and empty.
        // NOLINTNEXTLINE(bugprone-use-after-move)
        REQUIRE( other.buffer().empty() );
      }
    }
  }

  SECTION("Destination is not move assignable")
  {
    SECTION("Move construction does not require destination assignment") {
      // Act
      constexpr auto property_holds =
        std::is_move_constructible_v<buffered_writer<unassignable_writer>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Move assignment requires an assignable destination") {
      // Act
      constexpr auto property_holds =
        std::is_move_assignable_v<buffered_writer<unassignable_writer>>;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

//-----------------------------------------------------------------------------
// Buffering
//-----------------------------------------------------------------------------

TEST_CASE("buffered_writer::flush()", "[buffering]")
{
  // Arrange
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
    auto sut = buffered_writer{std::move(destination), capacity};
    compiler::unused(sut.write(input));

    // Act
    const auto result = sut.flush();

    // Assert
    SECTION("Succeeds")
    {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes every buffered byte in order")
    {
      const auto written = sut.destination().written();

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
    auto sut = buffered_writer{std::move(destination), capacity};

    // Act
    const auto result = sut.flush();

    // Assert
    SECTION("Succeeds")
    {
      REQUIRE( result.has_value() );
    }
    SECTION("Does not write to the destination")
    {
      REQUIRE( sut.destination().write_count() == 0u );
    }
  }

  SECTION("Destination stops accepting bytes")
  {
    // Arrange
    auto storage = std::array<std::byte,4>{};
    auto destination = fake_writer{storage};
    auto sut = buffered_writer{std::move(destination), capacity};
    compiler::unused(sut.write(input));

    // Act
    const auto result = sut.flush();

    // Assert
    SECTION("Fails with write zero")
    {
      REQUIRE( result == fail(io_error::write_zero) );
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
    auto sut = buffered_writer{std::move(destination), capacity};
    compiler::unused(sut.write(input));

    // Act
    const auto result = sut.flush();

    // Assert
    SECTION("Returns the error of the destination")
    {
      REQUIRE( result == fail(expected) );
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
    auto sut = buffered_writer{std::move(destination), capacity};
    compiler::unused(sut.write(input));
    compiler::unused(sut.flush());
    sut.destination().set_chunk_size(3u);

    // Act
    const auto result = sut.flush();

    // Assert
    SECTION("Writes every buffered byte in order")
    {
      const auto written = sut.destination().written();

      REQUIRE( result.has_value() );
      SECTION("Preserves the order of all written bytes") {
        REQUIRE(
          std::equal(written.begin(), written.end(), input.begin(), input.end())
        );
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Writing
//-----------------------------------------------------------------------------

TEST_CASE("buffered_writer::write(std::span<const std::byte>)", "[writing]")
{
  // Arrange
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
    auto sut = buffered_writer{std::move(destination), capacity};

    // Act
    write_all_by(sut, input, request);
    const auto result = sut.flush();

    // Assert
    SECTION("Writes every byte to the destination in order")
    {
      const auto written = sut.destination().written();

      REQUIRE( result.has_value() );
      SECTION("Preserves the order of all written bytes") {
        REQUIRE(
          std::equal(written.begin(), written.end(), input.begin(), input.end())
        );
      }
    }
  }

  SECTION("Write fits in the buffer")
  {
    // Arrange
    auto storage = std::vector<std::byte>(input.size());
    auto destination = fake_writer{storage};
    auto sut = buffered_writer{std::move(destination), capacity};
    const auto bytes = std::span{input}.first(capacity - 1u);

    // Act
    const auto result = sut.write(bytes);

    // Assert
    SECTION("Returns the number of bytes given")
    {
      REQUIRE( result == bytes.size() );
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
      REQUIRE( sut.destination().write_count() == 0u );
    }
  }

  SECTION("Write does not fit in the space left in the buffer")
  {
    // Arrange
    auto storage = std::vector<std::byte>(input.size());
    auto destination = fake_writer{storage};
    auto sut = buffered_writer{std::move(destination), capacity};
    compiler::unused(sut.write(std::span{input}.first(10u)));
    const auto bytes = std::span{input}.subspan(10u, 6u);

    // Act
    const auto result = sut.write(bytes);

    // Assert
    SECTION("Returns the number of bytes given")
    {
      REQUIRE( result == bytes.size() );
    }
    SECTION("Writes the previously buffered bytes to the destination")
    {
      const auto written = sut.destination().written();

      REQUIRE( std::ranges::equal(
        written, std::span{input}.subspan(0u, 10u)
      ) );
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
    auto sut = buffered_writer{std::move(destination), capacity};
    const auto bytes = std::span{input}.first(request);

    // Act
    const auto result = sut.write(bytes);

    // Assert
    SECTION("Writes directly from the buffer of the caller")
    {
      SECTION("Satisfies the request with one destination write") {
        REQUIRE( sut.destination().write_count() == 1u );
      }
      SECTION("Writes the request directly to the destination") {
        REQUIRE( sut.destination().last_request_size() == request );
      }
    }
    SECTION("Returns the number of bytes written")
    {
      REQUIRE( result == request );
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
    auto sut = buffered_writer{std::move(destination), capacity};
    compiler::unused(sut.write(std::span{input}.first(4u)));
    const auto bytes = std::span{input}.subspan(4u, capacity);

    // Act
    const auto result = sut.write(bytes);

    // Assert
    SECTION("Returns the number of bytes written")
    {
      REQUIRE( result == bytes.size() );
    }
    SECTION("Writes the buffered bytes before the new bytes")
    {
      const auto written = sut.destination().written();

      REQUIRE( std::ranges::equal(
        written, std::span{input}.subspan(0u, 4u + capacity)
      ) );
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
    auto sut = buffered_writer{std::move(destination), capacity};
    compiler::unused(sut.write(std::span{input}.first(10u)));

    // Act
    const auto result = sut.write(std::span{input}.subspan(10u, 6u));

    // Assert
    SECTION("Returns the error of the destination")
    {
      REQUIRE( result == fail(expected) );
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
    auto sut = buffered_writer{std::move(destination), capacity};

    // Act
    const auto result = sut.write(std::span{input}.first(capacity));

    // Assert
    SECTION("Returns the error of the destination")
    {
      REQUIRE( result == fail(expected) );
    }
  }

  SECTION("Buffer cannot be allocated")
  {
    // Arrange
    auto storage = std::vector<std::byte>(input.size());
    auto resource = counting_resource{};
    resource.fail_allocations();
    auto sut = buffered_writer{
      fake_writer{storage},
      capacity,
      allocator{&resource}
    };

    // Act
    const auto result = sut.write(std::span{input}.first(4u));

    // Assert
    SECTION("Fails with not enough memory")
    {
      REQUIRE( result == fail(std::errc::not_enough_memory) );
    }
    SECTION("Does not write to the destination")
    {
      REQUIRE( sut.destination().write_count() == 0u );
    }
  }

  SECTION("Write bypasses a buffer that cannot be allocated")
  {
    // Arrange
    auto storage = std::vector<std::byte>(input.size());
    auto resource = counting_resource{};
    resource.fail_allocations();
    auto sut = buffered_writer{
      fake_writer{storage},
      capacity,
      allocator{&resource}
    };
    const auto bytes = std::span{input}.first(capacity);

    // Act
    const auto result = sut.write(bytes);

    // Assert
    SECTION("Writes directly from the buffer of the caller")
    {
      const auto written = sut.destination().written();

      REQUIRE( result.has_value() );
      SECTION("Reports the bytes accepted before the write failure") {
        REQUIRE( *result == capacity );
      }
      SECTION("Writes the accepted bytes in their original order") {
        REQUIRE(
          std::equal(written.begin(), written.end(), bytes.begin(), bytes.end())
        );
      }
    }
  }

  SECTION("Buffer was allocated by a previous write")
  {
    // Arrange
    auto storage = std::vector<std::byte>(input.size());
    auto resource = counting_resource{};
    auto sut = buffered_writer{
      fake_writer{storage},
      capacity,
      allocator{&resource}
    };
    compiler::unused(sut.write(std::span{input}.first(10u)));

    // Act
    const auto result = sut.write(std::span{input}.subspan(10u, 10u));

    // Assert
    SECTION("Reuses the buffer")
    {
      REQUIRE( result.has_value() );
      SECTION("Reuses the allocation for subsequent I/O") {
        REQUIRE( resource.allocations() == 1u );
      }
    }
  }
}

//=============================================================================
// alias : indirect_buffered_writer
//=============================================================================

TEST_CASE("indirect_buffered_writer::write(std::span<const std::byte>)",
          "[writing]")
{
  // Arrange
  static constexpr auto capacity = std::size_t{16};
  const auto input = sequence_bytes(capacity + 4u);
  auto storage = std::vector<std::byte>(input.size());
  auto destination = fake_writer{storage};
  auto sut = indirect_buffered_writer{&destination, capacity};
  compiler::unused(sut.write(std::span{input}.first(4u)));

  // Act
  const auto result = sut.write(std::span{input}.subspan(4u));

  // Assert
  SECTION("Writes to the destination that it refers to")
  {
    REQUIRE( &sut.destination().destination() == &destination );
  }
  SECTION("Writes every byte to the destination in order")
  {
    const auto written = destination.written();

    REQUIRE( result.has_value() );
    SECTION("Preserves the order of all written bytes") {
      REQUIRE(
        std::equal(written.begin(), written.end(), input.begin(), input.end())
      );
    }
  }
}

} // namespace
} // namespace alloy::core::test
