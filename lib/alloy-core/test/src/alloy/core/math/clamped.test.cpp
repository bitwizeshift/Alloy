// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/clamped.hpp>

#include <alloy/core/math/saturated.hpp>

#include <alloy/core/intrinsics.hpp>
#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>

#include <functional> // std::hash
#include <limits>     // std::numeric_limits
#include <type_traits>

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
  SECTION("Input is NaN") {
    // Arrange
    const auto value = std::numeric_limits<float>::quiet_NaN();

    // Act
    const auto sut = clamped<float>::make(value);

    // Assert
    SECTION("Results in not a number error") {
      REQUIRE(sut == fail(clamped<float>::error::not_a_number));
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

//-----------------------------------------------------------------------------
// Hashing
//-----------------------------------------------------------------------------

TEST_CASE("std::hash<clamped<Float>>", "[hash]") {
  // Arrange
  const auto hasher = std::hash<clamped<float>>{};
  const auto value = clamped<float>::make_unchecked(0.5f);
  const auto expected = std::hash<float>{}(0.5f);

  // Act
  const auto result = hasher(value);

  // Assert
  SECTION("Hashes the underlying value") {
    REQUIRE(result == expected);
  }
}

//-----------------------------------------------------------------------------
// Comparisons
//-----------------------------------------------------------------------------

struct clamped_only_equality_comparable {
  friend auto operator==(clamped_only_equality_comparable, float) -> bool;
};

template <typename L, typename R>
concept clamped_greater_equal_comparable = requires(const L& lhs, const R& rhs) {
  lhs >= rhs;
};

TEST_CASE("operator>=(const T&, clamped<Float>)", "[comparison]") {
  SECTION("Requires T to be ordered against Float") {
    // Assert
    STATIC_REQUIRE_FALSE(
      clamped_greater_equal_comparable<
        clamped_only_equality_comparable,
        clamped<float>
      >
    );
  }
}

} // namespace alloy::core::test
