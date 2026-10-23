// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/io/buffered_reader.hpp>

#include <alloy/core/io/reader.hpp>
#include "test_doubles.hpp"

#include <alloy/core/containers/vector.hpp>
#include <alloy/core/intrinsics.hpp>
#include <alloy/core/memory/allocator.hpp>
#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <algorithm>    // std::equal
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

/// \brief Reads every byte of \p source with reads of \p request bytes
///
/// \param source the reader to read from
/// \param request the number of bytes to request from each read
/// \return the bytes that were read
auto read_all_by(reader& source, std::size_t request) -> std::vector<std::byte>
{
  auto out = std::vector<std::byte>{};
  auto chunk = std::vector<std::byte>(request);
  while (true) {
    const auto bytes_read = source.read(chunk);
    REQUIRE( bytes_read.has_value() );
    if (*bytes_read == 0u) {
      return out;
    }
    const auto bytes = std::span{chunk}.first(*bytes_read);
    out.insert(out.end(), bytes.begin(), bytes.end());
  }
}

//=============================================================================
// class : immovable_reader
//=============================================================================

/// \brief A reader that is neither move constructible nor move assignable
class immovable_reader final : public reader
{
public:

  immovable_reader() noexcept = default;
  immovable_reader(immovable_reader&&) = delete;

  auto operator=(immovable_reader&&) -> immovable_reader& = delete;

private:

  auto do_read(std::span<std::byte>)
    noexcept -> result<std::size_t,std::error_code> override
  {
    return 0u;
  }
};

//=============================================================================
// class : unassignable_reader
//=============================================================================

/// \brief A reader that is move constructible, but not move assignable
class unassignable_reader final : public reader
{
public:

  unassignable_reader() noexcept = default;
  unassignable_reader(unassignable_reader&&) noexcept = default;

  auto operator=(unassignable_reader&&) -> unassignable_reader& = delete;

private:

  auto do_read(std::span<std::byte>)
    noexcept -> result<std::size_t,std::error_code> override
  {
    return 0u;
  }
};

//=============================================================================
// class : buffered_reader
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("buffered_reader::buffered_reader(U&&, std::size_t, allocator)",
          "[ctor]")
{
  // Arrange
  const auto input = sequence_bytes(16u);

  SECTION("Capacity is not specified")
  {
    // Arrange
    auto source = fake_reader{input};

    // Act
    const auto sut = buffered_reader{std::move(source)};

    // Assert
    SECTION("Uses the default capacity")
    {
      using sut_type = buffered_reader<fake_reader>;

      REQUIRE( sut.capacity() == sut_type::default_capacity );
    }
    SECTION("Starts with an empty buffer")
    {
      REQUIRE( sut.buffer().empty() );
    }
    SECTION("Owns the source")
    {
      REQUIRE( sut.source().remaining() == input.size() );
    }
    SECTION("Does not read from the source yet")
    {
      REQUIRE( sut.source().read_count() == 0u );
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
    const auto sut = buffered_reader{
      fake_reader{input},
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

  SECTION("Source is an lvalue")
  {
    // Arrange
    auto source = fake_reader{input};
    source.set_chunk_size(4u);

    // Act
    auto sut = buffered_reader{source};

    // Assert
    SECTION("Owns a copy of the source")
    {
      REQUIRE( &sut.source() != &source );
    }
    SECTION("Does not read from the original source")
    {
      // Act
      const auto result = sut.fill_buffer();

      // Assert
      SECTION("Fills the owned buffer successfully") {
        REQUIRE( result.has_value() );
      }
      SECTION("Leaves the original source unused") {
        REQUIRE( source.read_count() == 0u );
      }
    }
  }
}

TEST_CASE("buffered_reader::buffered_reader(std::in_place_t, Args&&...)",
          "[ctor]")
{
  // Arrange
  const auto input = sequence_bytes(16u);

  // Act
  const auto sut = buffered_reader<fake_reader>{
    std::in_place,
    std::span<const std::byte>{input}
  };

  // Assert
  SECTION("Uses the default capacity")
  {
    REQUIRE( sut.capacity() == buffered_reader<fake_reader>::default_capacity );
  }
  SECTION("Constructs the source from the arguments")
  {
    REQUIRE( sut.source().remaining() == input.size() );
  }
  SECTION("Starts with an empty buffer")
  {
    REQUIRE( sut.buffer().empty() );
  }
}

TEST_CASE("buffered_reader::buffered_reader(std::allocator_arg_t, allocator, "
          "std::size_t, std::in_place_t, Args&&...)", "[ctor]")
{
  // Arrange
  const auto input = sequence_bytes(16u);
  auto resource = counting_resource{};

  // Act
  auto sut = buffered_reader<fake_reader>{
    std::allocator_arg,
    allocator{&resource},
    8u,
    std::in_place,
    std::span<const std::byte>{input}
  };

  // Assert
  SECTION("Uses the requested capacity")
  {
    REQUIRE( sut.capacity() == 8u );
  }
  SECTION("Constructs the source from the arguments")
  {
    REQUIRE( sut.source().remaining() == input.size() );
  }
  SECTION("Does not allocate before the first use")
  {
    REQUIRE( resource.allocations() == 0u );
  }
  SECTION("Allocates the buffer when it is first used")
  {
    // Act
    const auto result = sut.fill_buffer();

    // Assert
    SECTION("The operation succeeds") {
      REQUIRE( result.has_value() );
    }
    SECTION("The buffer is allocated once") {
      REQUIRE( resource.allocations() == 1u );
    }
  }
}

TEST_CASE("buffered_reader deduction guides", "[ctor]")
{
  // Arrange
  const auto input = sequence_bytes(16u);

  SECTION("Source is an lvalue")
  {
    // Arrange
    auto source = fake_reader{input};

    // Act
    auto sut = buffered_reader{source};

    SECTION("Deduces the type of the source")
    {
      // Act
      constexpr auto property_holds =
        std::is_same_v<decltype(sut), buffered_reader<fake_reader>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("Source is a const lvalue")
  {
    // Arrange
    const auto source = fake_reader{input};

    // Act
    auto sut = buffered_reader{source};

    SECTION("Deduces the type of the source without const")
    {
      // Act
      constexpr auto property_holds =
        std::is_same_v<decltype(sut), buffered_reader<fake_reader>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("Source is an rvalue")
  {
    // Act
    auto sut = buffered_reader{fake_reader{input}};

    SECTION("Deduces the type of the source")
    {
      // Act
      constexpr auto property_holds =
        std::is_same_v<decltype(sut), buffered_reader<fake_reader>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("Source and capacity are given")
  {
    // Arrange
    auto source = fake_reader{input};

    // Act
    auto sut = buffered_reader{source, 8u};

    SECTION("Deduces the type of the source")
    {
      // Act
      constexpr auto property_holds =
        std::is_same_v<decltype(sut), buffered_reader<fake_reader>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("Source, capacity, and allocator are given")
  {
    // Arrange
    auto source = fake_reader{input};

    // Act
    auto sut = buffered_reader{source, 8u, allocator{}};

    SECTION("Deduces the type of the source")
    {
      // Act
      constexpr auto property_holds =
        std::is_same_v<decltype(sut), buffered_reader<fake_reader>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
}

TEST_CASE("buffered_reader::buffered_reader(buffered_reader&&)", "[ctor]")
{
  SECTION("Source is move constructible")
  {
    // Arrange
    const auto input = sequence_bytes(16u);
    auto resource = counting_resource{};
    auto other = buffered_reader{fake_reader{input}, 8u, allocator{&resource}};
    compiler::unused(other.fill_buffer());
    other.consume(3u);
    const auto expected = other.buffer();

    // Act
    auto sut = std::move(other);

    // Assert
    SECTION("Takes the capacity of the other reader")
    {
      REQUIRE( sut.capacity() == 8u );
    }
    SECTION("Takes the buffered bytes of the other reader")
    {
      SECTION("Retains the original buffer allocation") {
        REQUIRE( sut.buffer().data() == expected.data() );
      }
      SECTION("Retains the number of pending bytes") {
        REQUIRE( sut.buffer().size() == expected.size() );
      }
    }
    SECTION("Takes the source of the other reader")
    {
      REQUIRE( sut.source().remaining() == input.size() - 8u );
    }
    SECTION("Leaves the other reader without a buffer")
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
  }

  SECTION("Source is not move constructible")
  {
    // Act
    constexpr auto property_holds =
      std::is_move_constructible_v<buffered_reader<immovable_reader>>;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

TEST_CASE("buffered_reader::~buffered_reader()", "[dtor]")
{
  // Arrange
  const auto input = sequence_bytes(16u);
  auto resource = counting_resource{};

  SECTION("Buffer was allocated")
  {
    // Arrange
    auto sut = buffered_reader{fake_reader{input}, 8u, allocator{&resource}};
    compiler::unused(sut.fill_buffer());

    // Act
    {
      auto destroyed = std::move(sut);
    }

    // Assert
    SECTION("Releases the buffer once")
    {
      REQUIRE( resource.deallocations() == 1u );
    }
  }

  SECTION("Buffer was never allocated")
  {
    // Act
    {
      auto sut = buffered_reader{fake_reader{input}, 8u, allocator{&resource}};
    }

    // Assert
    SECTION("Does not release a buffer")
    {
      REQUIRE( resource.deallocations() == 0u );
    }
  }

  SECTION("Reader was moved from")
  {
    // Arrange
    auto sut = buffered_reader{fake_reader{input}, 8u, allocator{&resource}};
    compiler::unused(sut.fill_buffer());

    // Act
    {
      auto destroyed = std::move(sut);
      auto other = std::move(destroyed);
    }

    // Assert
    SECTION("Releases the buffer only once")
    {
      REQUIRE( resource.deallocations() == 1u );
    }
  }
}

TEST_CASE("buffered_reader::operator=(buffered_reader&&)", "[assignment]")
{
  SECTION("Source is move assignable")
  {
    // Arrange
    const auto input = sequence_bytes(16u);
    const auto other_input = sequence_bytes(32u);
    auto resource = counting_resource{};
    auto sut = buffered_reader{fake_reader{input}, 4u, allocator{&resource}};
    auto other = buffered_reader{
      fake_reader{other_input},
      8u,
      allocator{&resource}
    };
    compiler::unused(sut.fill_buffer());
    compiler::unused(other.fill_buffer());
    const auto expected = other.buffer();

    // Act
    sut = std::move(other);

    // Assert
    SECTION("Releases the previous buffer")
    {
      REQUIRE( resource.deallocations() == 1u );
    }
    SECTION("Takes the capacity of the other reader")
    {
      REQUIRE( sut.capacity() == 8u );
    }
    SECTION("Takes the buffered bytes of the other reader")
    {
      SECTION("Retains the original buffer allocation") {
        REQUIRE( sut.buffer().data() == expected.data() );
      }
      SECTION("Retains the number of pending bytes") {
        REQUIRE( sut.buffer().size() == expected.size() );
      }
    }
    SECTION("Takes the source of the other reader")
    {
      REQUIRE( sut.source().remaining() == other_input.size() - 8u );
    }
    SECTION("Leaves the other reader without a buffer")
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

  SECTION("Source is not move assignable")
  {
    SECTION("Move construction does not require source assignment") {
      // Act
      constexpr auto property_holds =
        std::is_move_constructible_v<buffered_reader<unassignable_reader>>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Move assignment requires an assignable source") {
      // Act
      constexpr auto property_holds =
        std::is_move_assignable_v<buffered_reader<unassignable_reader>>;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

//-----------------------------------------------------------------------------
// Buffering
//-----------------------------------------------------------------------------

TEST_CASE("buffered_reader::fill_buffer()", "[buffering]")
{
  // Arrange
  static constexpr auto capacity = std::size_t{16};
  const auto input = sequence_bytes(40u);

  SECTION("Buffer is empty")
  {
    // Arrange
    const auto chunk_size = GENERATE(std::size_t{5}, std::size_t{100});
    auto source = fake_reader{input};
    source.set_chunk_size(chunk_size);
    auto sut = buffered_reader{std::move(source), capacity};

    // Act
    const auto result = sut.fill_buffer();

    // Assert
    SECTION("Reads from the source once")
    {
      REQUIRE( sut.source().read_count() == 1u );
    }
    SECTION("Requests the whole capacity from the source")
    {
      REQUIRE( sut.source().last_request_size() == capacity );
    }
    SECTION("Returns the bytes that were read")
    {
      REQUIRE( result.has_value() );
      SECTION("Exposes the bytes returned by the source") {
        REQUIRE( result->size() == std::min(chunk_size, capacity) );
      }
      SECTION("Preserves the leading source bytes in order") {
        REQUIRE( std::equal(result->begin(), result->end(), input.begin()) );
      }
    }
    SECTION("Returns the buffered bytes")
    {
      REQUIRE( result.has_value() );
      SECTION("Returns a view into the owned buffer") {
        REQUIRE( result->data() == sut.buffer().data() );
      }
      SECTION("Returns a view covering every buffered byte") {
        REQUIRE( result->size() == sut.buffer().size() );
      }
    }
  }

  SECTION("Buffer has bytes")
  {
    // Arrange
    auto source = fake_reader{input};
    auto sut = buffered_reader{std::move(source), capacity};
    compiler::unused(sut.fill_buffer());
    sut.consume(3u);

    // Act
    const auto result = sut.fill_buffer();

    // Assert
    SECTION("Does not read from the source again")
    {
      REQUIRE( sut.source().read_count() == 1u );
    }
    SECTION("Returns the bytes that are still buffered")
    {
      REQUIRE( result.has_value() );
      SECTION("Excludes consumed bytes from the returned range") {
        REQUIRE( result->size() == capacity - 3u );
      }
      SECTION("Begins with the first unconsumed source byte") {
        REQUIRE( std::equal(result->begin(), result->end(), input.begin() + 3) );
      }
    }
  }

  SECTION("Every buffered byte was consumed")
  {
    // Arrange
    auto source = fake_reader{input};
    auto sut = buffered_reader{std::move(source), capacity};
    compiler::unused(sut.fill_buffer());
    sut.consume(capacity);

    // Act
    const auto result = sut.fill_buffer();

    // Assert
    SECTION("Reads from the source again")
    {
      REQUIRE( sut.source().read_count() == 2u );
    }
    SECTION("Returns the next bytes of the source")
    {
      REQUIRE( result.has_value() );
      SECTION("Fills the buffer to its capacity") {
        REQUIRE( result->size() == capacity );
      }
      SECTION("Continues after the previously consumed buffer") {
        REQUIRE(
          std::equal(result->begin(), result->end(), input.begin() + capacity)
        );
      }
    }
  }

  SECTION("Source has ended")
  {
    // Arrange
    auto source = fake_reader{std::span<const std::byte>{}};
    auto sut = buffered_reader{std::move(source), capacity};

    // Act
    const auto result = sut.fill_buffer();

    // Assert
    SECTION("Returns no bytes")
    {
      REQUIRE( result.has_value() );
      SECTION("Reports no buffered bytes at end of stream") {
        REQUIRE( result->empty() );
      }
    }
  }

  SECTION("Source fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto source = fake_reader{std::span<const std::byte>{}};
    source.fail_at_end(expected);
    auto sut = buffered_reader{std::move(source), capacity};

    // Act
    const auto result = sut.fill_buffer();

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result == fail(expected) );
    }
  }

  SECTION("Buffer cannot be allocated")
  {
    // Arrange
    auto resource = counting_resource{};
    resource.fail_allocations();
    auto sut = buffered_reader{
      fake_reader{input},
      capacity,
      allocator{&resource}
    };

    // Act
    const auto result = sut.fill_buffer();

    // Assert
    SECTION("Fails with not enough memory")
    {
      REQUIRE( result == fail(std::errc::not_enough_memory) );
    }
    SECTION("Does not read from the source")
    {
      REQUIRE( sut.source().read_count() == 0u );
    }
  }

  SECTION("Buffer was allocated by a previous fill")
  {
    // Arrange
    auto resource = counting_resource{};
    auto sut = buffered_reader{
      fake_reader{input},
      capacity,
      allocator{&resource}
    };
    compiler::unused(sut.fill_buffer());
    sut.consume(capacity);

    // Act
    const auto result = sut.fill_buffer();

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

TEST_CASE("buffered_reader::consume(std::size_t)", "[buffering]")
{
  // Arrange
  static constexpr auto capacity = std::size_t{16};
  const auto input = sequence_bytes(40u);
  const auto n = GENERATE(std::size_t{0}, std::size_t{1}, capacity);
  auto source = fake_reader{input};
  auto sut = buffered_reader{std::move(source), capacity};
  compiler::unused(sut.fill_buffer());

  // Act
  sut.consume(n);

  // Assert
  SECTION("Removes the bytes from the front of the buffer")
  {
    const auto buffer = sut.buffer();

    REQUIRE( std::ranges::equal(
      buffer, std::span{input}.subspan(n, capacity - n)
    ) );
  }
  SECTION("Does not read from the source")
  {
    REQUIRE( sut.source().read_count() == 1u );
  }
}

TEST_CASE("buffered_reader::discard_buffer()", "[buffering]")
{
  // Arrange
  static constexpr auto capacity = std::size_t{16};
  const auto input = sequence_bytes(40u);
  auto source = fake_reader{input};
  auto sut = buffered_reader{std::move(source), capacity};
  compiler::unused(sut.fill_buffer());

  // Act
  sut.discard_buffer();

  // Assert
  SECTION("Empties the buffer")
  {
    REQUIRE( sut.buffer().empty() );
  }
  SECTION("Continues reading after the discarded bytes")
  {
    auto storage = std::array<std::byte,1>{};
    const auto result = sut.read(storage);

    REQUIRE( result.has_value() );
    SECTION("Continues reading after the consumed buffer") {
      REQUIRE( storage[0] == input[capacity] );
    }
  }
}

//-----------------------------------------------------------------------------
// Reading
//-----------------------------------------------------------------------------

TEST_CASE("buffered_reader::read(std::span<std::byte>)", "[reading]")
{
  // Arrange
  static constexpr auto capacity = std::size_t{16};
  const auto input = sequence_bytes(300u);

  SECTION("Reads are interleaved with refills")
  {
    // Arrange
    const auto chunk_size = GENERATE(
      std::size_t{1}, std::size_t{5}, std::size_t{16}, std::size_t{1000}
    );
    const auto request = GENERATE(
      std::size_t{1}, std::size_t{7}, std::size_t{15}, std::size_t{16},
      std::size_t{33}
    );
    auto source = fake_reader{input};
    source.set_chunk_size(chunk_size);
    auto sut = buffered_reader{std::move(source), capacity};

    // Act
    const auto output = read_all_by(sut, request);

    // Assert
    SECTION("Reads every byte of the source in order")
    {
      REQUIRE(
        std::equal(output.begin(), output.end(), input.begin(), input.end())
      );
    }
  }

  SECTION("Read is smaller than the buffer, which is empty")
  {
    // Arrange
    auto source = fake_reader{input};
    auto sut = buffered_reader{std::move(source), capacity};
    auto storage = std::array<std::byte,4>{};

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Returns the number of bytes requested")
    {
      REQUIRE( result == storage.size() );
    }
    SECTION("Writes the first bytes of the source")
    {
      REQUIRE( std::equal(storage.begin(), storage.end(), input.begin()) );
    }
    SECTION("Fills the whole buffer with one read")
    {
      SECTION("Satisfies the request with one source read") {
        REQUIRE( sut.source().read_count() == 1u );
      }
      SECTION("Requests a full buffer from the source") {
        REQUIRE( sut.source().last_request_size() == capacity );
      }
    }
    SECTION("Keeps the rest of the read in the buffer")
    {
      REQUIRE( sut.buffer().size() == capacity - storage.size() );
    }
  }

  SECTION("Read is smaller than what is buffered")
  {
    // Arrange
    auto source = fake_reader{input};
    auto sut = buffered_reader{std::move(source), capacity};
    auto storage = std::array<std::byte,4>{};
    compiler::unused(sut.read(storage));

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Returns the number of bytes requested")
    {
      REQUIRE( result == storage.size() );
    }
    SECTION("Writes the next bytes of the source")
    {
      REQUIRE( std::equal(storage.begin(), storage.end(), input.begin() + 4) );
    }
    SECTION("Does not read from the source again")
    {
      REQUIRE( sut.source().read_count() == 1u );
    }
  }

  SECTION("Read is larger than what is buffered")
  {
    // Arrange
    auto source = fake_reader{input};
    auto sut = buffered_reader{std::move(source), capacity};
    auto storage = std::array<std::byte,64>{};
    compiler::unused(sut.fill_buffer());
    sut.consume(10u);

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Returns only the buffered bytes")
    {
      REQUIRE( result == capacity - 10u );
    }
    SECTION("Writes the buffered bytes")
    {
      REQUIRE(
        std::equal(storage.begin(), storage.begin() + 6, input.begin() + 10)
      );
    }
    SECTION("Does not read from the source again")
    {
      REQUIRE( sut.source().read_count() == 1u );
    }
    SECTION("Empties the buffer")
    {
      REQUIRE( sut.buffer().empty() );
    }
  }

  SECTION("Read is at least as large as the buffer, which is empty")
  {
    // Arrange
    const auto request = GENERATE(capacity, capacity + 1u, capacity * 4u);
    auto source = fake_reader{input};
    auto sut = buffered_reader{std::move(source), capacity};
    auto storage = std::vector<std::byte>(request);

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Reads directly into the buffer of the caller")
    {
      SECTION("Satisfies the request with one source read") {
        REQUIRE( sut.source().read_count() == 1u );
      }
      SECTION("Requests the bytes directly from the source") {
        REQUIRE( sut.source().last_request_size() == request );
      }
    }
    SECTION("Returns the number of bytes read")
    {
      REQUIRE( result == request );
    }
    SECTION("Writes the first bytes of the source")
    {
      REQUIRE( std::equal(storage.begin(), storage.end(), input.begin()) );
    }
    SECTION("Leaves the buffer empty")
    {
      REQUIRE( sut.buffer().empty() );
    }
  }

  SECTION("Read is empty and bytes are buffered")
  {
    // Arrange
    auto source = fake_reader{input};
    auto sut = buffered_reader{std::move(source), capacity};
    compiler::unused(sut.fill_buffer());

    // Act
    const auto result = sut.read(std::span<std::byte>{});

    // Assert
    SECTION("Returns zero bytes read")
    {
      REQUIRE( result == 0u );
    }
    SECTION("Does not read from the source again")
    {
      REQUIRE( sut.source().read_count() == 1u );
    }
    SECTION("Keeps the buffered bytes")
    {
      REQUIRE( sut.buffer().size() == capacity );
    }
  }

  SECTION("Source has ended")
  {
    // Arrange
    const auto request = GENERATE(std::size_t{1}, capacity);
    auto source = fake_reader{std::span<const std::byte>{}};
    auto sut = buffered_reader{std::move(source), capacity};
    auto storage = std::vector<std::byte>(request);

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Returns zero bytes read")
    {
      REQUIRE( result == 0u );
    }
  }

  SECTION("Source fails")
  {
    // Arrange
    const auto request = GENERATE(std::size_t{1}, capacity);
    const auto expected = std::make_error_code(std::errc::io_error);
    auto source = fake_reader{std::span<const std::byte>{}};
    source.fail_at_end(expected);
    auto sut = buffered_reader{std::move(source), capacity};
    auto storage = std::vector<std::byte>(request);

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result == fail(expected) );
    }
    SECTION("Leaves the buffer empty")
    {
      REQUIRE( sut.buffer().empty() );
    }
  }

  SECTION("Buffer cannot be allocated")
  {
    // Arrange
    auto resource = counting_resource{};
    resource.fail_allocations();
    auto sut = buffered_reader{
      fake_reader{input},
      capacity,
      allocator{&resource}
    };
    auto storage = std::array<std::byte,4>{};

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Fails with not enough memory")
    {
      REQUIRE( result == fail(std::errc::not_enough_memory) );
    }
    SECTION("Does not read from the source")
    {
      REQUIRE( sut.source().read_count() == 0u );
    }
  }

  SECTION("Read bypasses a buffer that cannot be allocated")
  {
    // Arrange
    auto resource = counting_resource{};
    resource.fail_allocations();
    auto sut = buffered_reader{
      fake_reader{input},
      capacity,
      allocator{&resource}
    };
    auto storage = std::vector<std::byte>(capacity);

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Reads directly into the buffer of the caller")
    {
      REQUIRE( result.has_value() );
      SECTION("Reports the number of bytes copied before the failure") {
        REQUIRE( *result == capacity );
      }
      SECTION("Copies the source prefix without changing its order") {
        REQUIRE( std::equal(storage.begin(), storage.end(), input.begin()) );
      }
    }
  }
}

TEST_CASE("buffered_reader::read_until(std::byte, "
          "not_null<vector<std::byte>*>)",
          "[reading]")
{
  // Arrange
  static constexpr auto delimiter = std::byte{10};
  const auto input = sequence_bytes(40u);

  SECTION("Source contains the delimiter")
  {
    // Arrange
    const auto capacity = GENERATE(
      std::size_t{1}, std::size_t{3}, std::size_t{11}, std::size_t{64}
    );
    auto source = fake_reader{input};
    auto sut = buffered_reader{std::move(source), capacity};
    auto out = vector<std::byte>{};

    // Act
    const auto result = sut.read_until(delimiter, &out);

    // Assert
    SECTION("Returns the number of bytes up to and including the delimiter")
    {
      REQUIRE( result == 11u );
    }
    SECTION("Appends the bytes up to and including the delimiter")
    {
      REQUIRE( std::ranges::equal(
        out, std::span{input}.subspan(0u, 11u)
      ) );
    }
    SECTION("Continues reading after the delimiter")
    {
      auto storage = std::array<std::byte,1>{};
      const auto next = sut.read(storage);

      REQUIRE( next.has_value() );
      SECTION("Leaves the next byte available after the delimiter") {
        REQUIRE( storage[0] == input[11] );
      }
    }
  }

  SECTION("Delimiter is the next byte")
  {
    // Arrange
    auto source = fake_reader{std::span{input}.subspan(10u)};
    auto sut = buffered_reader{std::move(source), 16u};
    auto out = vector<std::byte>{};

    // Act
    const auto result = sut.read_until(delimiter, &out);

    // Assert
    SECTION("Appends only the delimiter")
    {
      REQUIRE( result.has_value() );
      SECTION("Counts the delimiter as a copied byte") {
        REQUIRE( *result == 1u );
      }
      SECTION("Copies only the delimiter when it is the first byte") {
        REQUIRE( out.size() == 1u );
      }
      SECTION("Includes the delimiter in the copied output") {
        REQUIRE( out[0] == delimiter );
      }
    }
  }

  SECTION("Source does not contain the delimiter")
  {
    // Arrange
    const auto capacity = GENERATE(std::size_t{3}, std::size_t{64});
    auto source = fake_reader{std::span{input}.first(10u)};
    auto sut = buffered_reader{std::move(source), capacity};
    auto out = vector<std::byte>{};

    // Act
    const auto result = sut.read_until(delimiter, &out);

    // Assert
    SECTION("Appends every byte of the source")
    {
      REQUIRE( result.has_value() );
      SECTION("Reports the bytes copied before end of stream") {
        REQUIRE( *result == 10u );
      }
      SECTION("Copies all available bytes before end of stream") {
        REQUIRE(
          std::equal(out.begin(), out.end(), input.begin(), input.begin() + 10)
        );
      }
    }
  }

  SECTION("Output already has contents")
  {
    // Arrange
    const auto existing = sequence_bytes(3u);
    auto source = fake_reader{input};
    auto sut = buffered_reader{std::move(source), 16u};
    auto out = vector<std::byte>{existing.begin(), existing.end()};

    // Act
    const auto result = sut.read_until(delimiter, &out);

    // Assert
    SECTION("Returns only the number of bytes read")
    {
      REQUIRE( result == 11u );
    }
    SECTION("Keeps the existing contents")
    {
      REQUIRE( std::equal(existing.begin(), existing.end(), out.begin()) );
    }
    SECTION("Appends the bytes after the existing contents")
    {
      REQUIRE( std::equal(out.begin() + 3, out.end(), input.begin()) );
    }
  }

  SECTION("Source fails before the delimiter")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto source = fake_reader{std::span{input}.first(5u)};
    source.fail_at_end(expected);
    auto sut = buffered_reader{std::move(source), 3u};
    auto out = vector<std::byte>{};

    // Act
    const auto result = sut.read_until(delimiter, &out);

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result == fail(expected) );
    }
    SECTION("Keeps the bytes read before the failure")
    {
      REQUIRE(
        std::equal(out.begin(), out.end(), input.begin(), input.begin() + 5)
      );
    }
  }
}

TEST_CASE("buffered_reader::skip_until(std::byte)", "[reading]")
{
  // Arrange
  static constexpr auto delimiter = std::byte{10};
  const auto input = sequence_bytes(40u);

  SECTION("Source contains the delimiter")
  {
    // Arrange
    const auto capacity = GENERATE(
      std::size_t{1}, std::size_t{3}, std::size_t{11}, std::size_t{64}
    );
    auto source = fake_reader{input};
    auto sut = buffered_reader{std::move(source), capacity};

    // Act
    const auto result = sut.skip_until(delimiter);

    // Assert
    SECTION("Returns the number of bytes up to and including the delimiter")
    {
      REQUIRE( result == 11u );
    }
    SECTION("Continues reading after the delimiter")
    {
      auto storage = std::array<std::byte,1>{};
      const auto next = sut.read(storage);

      REQUIRE( next.has_value() );
      SECTION("Leaves the next byte available after the delimiter") {
        REQUIRE( storage[0] == input[11] );
      }
    }
  }

  SECTION("Delimiter repeats")
  {
    // Arrange
    const auto repeated = std::array<std::byte,3>{
      delimiter, delimiter, delimiter
    };
    auto source = fake_reader{repeated};
    auto sut = buffered_reader{std::move(source), 16u};

    // Act
    const auto result = sut.skip_until(delimiter);

    // Assert
    SECTION("Skips only the first delimiter")
    {
      REQUIRE( result.has_value() );
      SECTION("Counts the delimiter as a copied byte") {
        REQUIRE( *result == 1u );
      }
      SECTION("Keeps bytes following the delimiter buffered") {
        REQUIRE( sut.buffer().size() == 2u );
      }
    }
  }

  SECTION("Source does not contain the delimiter")
  {
    // Arrange
    auto source = fake_reader{std::span{input}.first(10u)};
    auto sut = buffered_reader{std::move(source), 3u};

    // Act
    const auto result = sut.skip_until(delimiter);

    // Assert
    SECTION("Skips every byte of the source")
    {
      REQUIRE( result.has_value() );
      SECTION("Reports the bytes copied before end of stream") {
        REQUIRE( *result == 10u );
      }
      SECTION("Consumes the source when no delimiter is found") {
        REQUIRE( sut.source().remaining() == 0u );
      }
    }
  }

  SECTION("Source fails before the delimiter")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto source = fake_reader{std::span{input}.first(5u)};
    source.fail_at_end(expected);
    auto sut = buffered_reader{std::move(source), 3u};

    // Act
    const auto result = sut.skip_until(delimiter);

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result == fail(expected) );
    }
  }
}

//=============================================================================
// alias : indirect_buffered_reader
//=============================================================================

TEST_CASE("indirect_buffered_reader::read(std::span<std::byte>)", "[reading]")
{
  // Arrange
  static constexpr auto capacity = std::size_t{16};
  const auto input = sequence_bytes(40u);
  auto source = fake_reader{input};
  auto sut = indirect_buffered_reader{&source, capacity};
  auto storage = std::array<std::byte,4>{};

  // Act
  const auto result = sut.read(storage);

  // Assert
  SECTION("Reads from the source that it refers to")
  {
    SECTION("Refers to the supplied source") {
      REQUIRE( &sut.source().source() == &source );
    }
    SECTION("Reads through the supplied source once") {
      REQUIRE( source.read_count() == 1u );
    }
  }
  SECTION("Fills the whole buffer with one read")
  {
    SECTION("Fills the buffer through the supplied source") {
      REQUIRE( source.last_request_size() == capacity );
    }
    SECTION("Buffers the bytes beyond the requested output") {
      REQUIRE( sut.buffer().size() == capacity - storage.size() );
    }
  }
  SECTION("Writes the first bytes of the source")
  {
    REQUIRE( result.has_value() );
    SECTION("Reports a completely filled output buffer") {
      REQUIRE( *result == storage.size() );
    }
    SECTION("Copies the source prefix without changing its order") {
      REQUIRE( std::equal(storage.begin(), storage.end(), input.begin()) );
    }
  }
}

} // namespace
} // namespace alloy::core::test
