/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include <alloy/core/utilities/casts.hpp>

#include <catch2/catch.hpp>
#include <cstdint> // std::uint8_t, std::uint64_t
#include <cstddef> // std::byte
#include <algorithm> // std::equal

namespace alloy::core::test {
namespace {

// Not really all that much logic to test here...
TEST_CASE("implicit_cast")
{
  SECTION("Performs implicit cast")
  {
    const auto from = std::uint64_t{42};

    const auto to = implicit_cast<std::uint8_t>(from);

    REQUIRE(to == from);
  }
}

TEST_CASE("narrow_cast")
{
  SECTION("Allows casting without loss of precision")
  {
    const auto from = std::uint64_t{64};

    const auto to = narrow_cast<std::uint8_t>(from);

    REQUIRE(to == from);
  }

  SECTION("Allows casting with loss of precision")
  {
    const auto from = std::uint64_t{0xffffffff};

    const auto to = narrow_cast<std::uint8_t>(from);

    REQUIRE(to != from);
  }
}

TEST_CASE("safe_narrow_cast")
{
  SECTION("Allows casting without loss of precision")
  {
    const auto from = std::uint64_t{64};

    const auto to = checked_narrow_cast<std::int8_t>(from);

    REQUIRE(to == from);
  }

  SECTION("Does not allow casting with loss of precision")
  {
    // Not testable under current Catch2 versions.
  }
}

struct Base{
  virtual ~Base() = default;
};
struct Derived : public Base{};

TEST_CASE("pointer_cast")
{
  SECTION("Allows casts to proper base")
  {
    auto x = Derived{};
    auto* p = static_cast<Base*>(&x);

    auto* result = pointer_cast<Derived*>(p);

    REQUIRE(p == result);
  }

  SECTION("Fails to cast to improper base")
  {
    // Not testable under current Catch2 versions.
  }
}

TEST_CASE("bit_cast")
{
  SECTION("Casts to the underlying byte representation")
  {
    static_assert(sizeof(std::uint32_t) == sizeof(float));

    const auto from = std::uint32_t{0x01234567};
    const auto to = bit_cast<float>(from);

    const auto* begin1 = reinterpret_cast<const std::byte*>(&from);
    const auto* end1   = begin1 + sizeof(from);
    const auto* begin2 = reinterpret_cast<const std::byte*>(&to);
    const auto* end2   = begin2 + sizeof(to);

    REQUIRE(std::equal(begin1, end1, begin2, end2));
  }
}

//-----------------------------------------------------------------------------
// Signed / Unsigned Casts
//-----------------------------------------------------------------------------

TEST_CASE("as_signed")
{
  SECTION("With signed integral input")
  {
    const auto from = std::int32_t{42};
    const auto to = as_signed(from);

    SECTION("Value is unchanged")
    {
      REQUIRE(from == to);
    }

    SECTION("Result is signed")
    {
      REQUIRE(std::is_signed_v<decltype(to)>);
    }
  }

  SECTION("With unsigned integral input")
  {
    SECTION("Input is out of signed range")
    {
      const auto from = std::numeric_limits<std::uint32_t>::max();
      const auto to = as_signed(from);

      SECTION("Result has different sign than input")
      {
        REQUIRE((to < 0) != (from < 0));
      }

      SECTION("Value is the signed-representation of the unsigned value")
      {
        REQUIRE(to == static_cast<std::int32_t>(from));
      }

      SECTION("Result is signed")
      {
        REQUIRE(std::is_signed_v<decltype(to)>);
      }
    }

    SECTION("Input is within signed range")
    {
      const auto from = std::uint32_t{42};
      const auto to = as_signed(from);

      SECTION("Result has same sign as input")
      {
        REQUIRE((to < 0) == (from < 0));
      }

      SECTION("Result contains the same value as the input")
      {
        REQUIRE(to == from);
      }

      SECTION("Result is signed")
      {
        REQUIRE(std::is_signed_v<decltype(to)>);
      }
    }
  }

  SECTION("With enum input")
  {
    const auto from = std::byte{42};
    const auto to = as_signed(from);

    SECTION("Value is (signed) integral representation of enum value")
    {
      REQUIRE(static_cast<std::int8_t>(from) == to);
    }

    SECTION("Result is signed")
    {
      REQUIRE(std::is_signed_v<decltype(to)>);
    }
  }
}


TEST_CASE("as_unsigned")
{
  SECTION("With signed integral input")
  {
    SECTION("Input is out of signed range")
    {
      const auto from = std::numeric_limits<std::int32_t>::min();
      const auto to = as_unsigned(from);

      SECTION("Result has different sign than input")
      {
        REQUIRE((to < 0) != (from < 0));
      }

      SECTION("Value is the unsigned-representation of the nsigned value")
      {
        REQUIRE(to == static_cast<std::uint32_t>(from));
      }

      SECTION("Result is unsigned")
      {
        REQUIRE(std::is_unsigned_v<decltype(to)>);
      }
    }

    SECTION("Input is within signed range")
    {
      const auto from = std::uint32_t{42};
      const auto to = as_unsigned(from);

      SECTION("Result has same sign as input")
      {
        REQUIRE((to < 0) == (from < 0));
      }

      SECTION("Result contains the same value as the input")
      {
        REQUIRE(to == from);
      }

      SECTION("Result is unsigned")
      {
        REQUIRE(std::is_unsigned_v<decltype(to)>);
      }
    }
  }

  SECTION("With unsigned integral input")
  {
    const auto from = std::uint32_t{42};
    const auto to = as_unsigned(from);

    SECTION("Value is unchanged")
    {
      REQUIRE(from == to);
    }

    SECTION("Result is unsigned")
    {
      REQUIRE(std::is_unsigned_v<decltype(to)>);
    }
  }

  SECTION("With enum input")
  {
    const auto from = std::byte{42};
    const auto to = as_unsigned(from);

    SECTION("Value is integral representation of enum value")
    {
      REQUIRE(static_cast<std::uint8_t>(from) == to);
    }

    SECTION("Result is unsigned")
    {
      REQUIRE(std::is_unsigned_v<decltype(to)>);
    }
  }
}


} // namespace <anonymous>
} // namespace alloy::core::test
