/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include "alloy/core/math/clamped.hpp"
#include "alloy/core/math/saturated.hpp"

#include <catch2/catch.hpp>

namespace alloy::core::test {

//=============================================================================
// class : clamped
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

TEST_CASE("clamped<Float>::make(Float)", "[factory]") {
  SECTION("Input exceeds 1.0") {
    const auto sut = clamped<float>::make(2.0f);

    SECTION("Results in overflow error") {
      REQUIRE(sut == fail(clamped<float>::error::overflow));
    }
  }
  SECTION("Input precedes 0.0") {
    const auto sut = clamped<float>::make(2.0f);

    SECTION("Results in underflow error") {
      REQUIRE(sut == fail(clamped<float>::error::overflow));
    }
  }
  SECTION("Input is between 1.0 and 0.0") {
    const auto expected = 0.5f;
    const auto sut = clamped<float>::make(expected);

    SECTION("Results in a clamped containing the input") {
      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("clamped<Float>::make_unchecked(Float)", "[factory]") {
  const auto expected = 0.5f;
  const auto sut = clamped<float>::make(expected);

  SECTION("Results in a clamped containing the input") {
    REQUIRE(sut == expected);
  }
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("clamped<Float>::clamped()", "[ctor]") {
  SECTION("Constructor is trivial") {
    STATIC_REQUIRE(std::is_trivially_constructible_v<clamped<float>>);
  }
  SECTION("Constructs a default-initialized underlying float") {
    const auto sut = clamped<float>{};

    REQUIRE(sut.value() == 0.0f);
  }
}

TEST_CASE("clamped<Float>::clamped(const clamped&)", "[ctor]") {
  SECTION("Constructor is trivial") {
    STATIC_REQUIRE(std::is_trivially_copy_constructible_v<clamped<float>>);
  }
  SECTION("Copies underlying float") {
    const auto sut = clamped<float>::make_unchecked(0.5);
    const auto result = sut;

    REQUIRE(sut == result);
  }
}

TEST_CASE("clamped<Float>::clamped(const saturated<UFloat>&)", "[ctor]") {
  SECTION("Copies underlying float") {
    const auto to_copy = saturated<float>::make_unchecked(0.5);
    const auto sut = clamped<float>{to_copy};

    REQUIRE(sut == to_copy.value());
  }
}

TEST_CASE("clamped<Float>::clamped(const clamped<UFloat>&)", "[ctor]") {
  SECTION("Copies underlying float") {
    const auto to_copy = clamped<double>::make_unchecked(0.5);
    const auto sut = clamped<float>{to_copy};

    REQUIRE(sut == static_cast<float>(to_copy.value()));
  }
}

//-----------------------------------------------------------------------------
// Assignment
//-----------------------------------------------------------------------------

TEST_CASE("clamped<Float>::operator=(const clamped&)", "[assign]") {
  SECTION("Assignment is trivial") {
    STATIC_REQUIRE(std::is_trivially_copy_assignable_v<clamped<float>>);
  }
  SECTION("Assigns the underlying value") {
    const auto copy = clamped<float>::make_unchecked(0.5);
    auto sut = clamped<float>{};
    compiler::unused(sut);

    sut = copy;

    REQUIRE(sut == copy);
  }
}
} // namespace alloy::core::test
