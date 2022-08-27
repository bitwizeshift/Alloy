/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include "alloy/core/utilities/hash.hpp"

#include <catch2/catch.hpp>

namespace alloy::core::test {
namespace {

  struct std_hashable
  {
    unsigned value;
    bool* on_hash;

    auto hash() const noexcept -> hash_type {
      *on_hash = true;

      return hash_value(value);
    }
  };

  //----------------------------------------------------------------------------

  struct adl_hashable{};

  inline constexpr auto test_sentinel = hash_type{0xdeadbeef};

  auto hash_value(const adl_hashable&) noexcept -> hash_type {
    return test_sentinel;
  }

  //----------------------------------------------------------------------------

  struct aggregate_hashable
  {
    long a;
    long b;
  };

  //----------------------------------------------------------------------------

  class unique_representable_hashable
  {
  public:
    unique_representable_hashable() = default;
  private:
    [[maybe_unused]]
    std::uint64_t a = 42;
    [[maybe_unused]]
    std::uint32_t b = 100;
    [[maybe_unused]]
    std::uint8_t c[sizeof(std::uint32_t)] = {};
  };

  static_assert(std::has_unique_object_representations_v<unique_representable_hashable>);
  static_assert(!std::is_aggregate_v<unique_representable_hashable>);

} // namespace <anonymous>
} // namespace alloy::core::test
namespace std {
  template <>
  struct hash<alloy::core::test::std_hashable> {
    auto operator()(const alloy::core::test::std_hashable& v) const noexcept {
      return v.hash();
    }
  };
} // namespace std

namespace alloy::core::test {

TEST_CASE("hash_value(const T&)") {
  using alloy::core::hash_value;

  SECTION("With std::hash-able value") {
    SECTION("Calls std::hash") {

      auto was_called = false;
      auto sut = std_hashable{42, &was_called};

      auto result = hash_value(sut);
      (void) result;

      REQUIRE(was_called);
    }
    SECTION("Provides the same hash as std::hash would") {
      const auto value = int{42};

      const auto r1 = hash_value(value);
      const auto r2 = std::hash<int>{}(value);

      REQUIRE(r1 == r2);
    }
  }

  SECTION("With overloaded hash_value") {
    SECTION("Calls overload") {
      auto sut = adl_hashable{};

      const auto result = hash_value(sut);

      REQUIRE(result == test_sentinel);
    }
  }

  SECTION("With aggregate") {
    SECTION("Hashes and combines each value") {
      auto sut = aggregate_hashable{
        42,
        1337
      };

      const auto expected = hash_values(sut.a, sut.b);
      const auto actual   = hash_value(sut);

      REQUIRE(expected == actual);
    }
  }
  SECTION("With uniquely representable object") {
    SECTION("Hashes the raw bytes") {
      auto sut = unique_representable_hashable{};

      const auto result = hash_value(sut);

      REQUIRE(result != hash_type{0u});
    }
  }
}

TEST_CASE("hash_value(const T(&)[N])") {
  using alloy::core::hash_value;

  SECTION("Hashes and combines each value") {
    long input[2] = {42, 0x1337};

    const auto result   = hash_value(input);
    const auto expected = hash_values(input[0], input[1]);

    REQUIRE(result == expected);
  }
}

TEST_CASE("hash_object(const T&)") {
  SECTION("With std::hash-able value") {
    SECTION("ADL-calls the std::hash implementation") {

      auto was_called = false;
      auto sut = std_hashable{42, &was_called};

      auto result = hash_object(sut);
      (void) result;

      REQUIRE(was_called);
    }
  }

  SECTION("With overloaded hash_value") {
    SECTION("ADL-calls the overload") {
      auto sut = adl_hashable{};

      const auto result = hash_object(sut);

      REQUIRE(result == test_sentinel);
    }
  }
}

} // namespace alloy::core::test
