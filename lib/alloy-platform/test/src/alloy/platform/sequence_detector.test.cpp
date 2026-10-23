// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/platform/sequence_detector.hpp>

#include <alloy/core/containers/vector.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace alloy::platform::test {

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

TEST_CASE("sequence_detector<T>::test(const value_type&)", "[modifiers]") {
  // Arrange
  const auto values = core::vector<int>{1,2,3,4};
  auto sut = sequence_detector{values.copy()};

  SECTION("Sequence is not started") {
    SECTION("Testing the first value") {
      // Act
      const auto result = sut.test(values[0]);

      // Assert
      SECTION("Accepts the value") {
        REQUIRE(result);
      }
      SECTION("Starts the sequence") {
        REQUIRE(sut.started());
      }
    }
    SECTION("Testing a different value") {
      // Act
      const auto result = sut.test(42);

      // Assert
      SECTION("Rejects the value") {
        REQUIRE_FALSE(result);
      }
      SECTION("Leaves the sequence unstarted") {
        REQUIRE_FALSE(sut.started());
      }
    }
  }
  SECTION("Sequence is started") {
    // Arrange
    sut.test(values[0]);

    SECTION("Testing the next value") {
      // Act
      const auto result = sut.test(values[1]);

      // Assert
      SECTION("Accepts the value") {
        REQUIRE(result);
      }
      SECTION("Keeps the sequence started") {
        REQUIRE(sut.started());
      }
    }
    SECTION("Testing a different value") {
      // Act
      const auto result = sut.test(values[0]);

      // Assert
      SECTION("Rejects the value") {
        REQUIRE_FALSE(result);
      }
      SECTION("Returns the sequence to its unstarted state") {
        REQUIRE_FALSE(sut.started());
      }
    }
  }
  SECTION("Sequence is completed") {
    // Arrange
    for (const auto value : values) {
      sut.test(value);
    }

    // Act
    const auto result = sut.test(values[0]);

    // Assert
    REQUIRE_FALSE(result);
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("sequence_detector<T>::reset()", "[modifiers]") {
  // Arrange
  const auto values = core::vector<int>{1,2,3,4};
  const auto consumed = GENERATE(0u, 1u, 4u);
  auto sut = sequence_detector{values.copy()};
  for (auto i = 0u; i < consumed; ++i) {
    sut.test(values[i]);
  }

  // Act
  sut.reset();

  // Assert
  SECTION("Sequence is not started") {
    REQUIRE_FALSE(sut.started());
  }
  SECTION("Sequence is not completed") {
    REQUIRE_FALSE(sut.completed());
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("sequence_detector<T>::started()", "[observers]") {
  // Arrange
  const auto values = core::vector<int>{1,2,3,4};
  const auto consumed = GENERATE(0u, 1u, 4u);
  const auto expected = consumed != 0u;
  auto sut = sequence_detector{values.copy()};
  for (auto i = 0u; i < consumed; ++i) {
    sut.test(values[i]);
  }

  // Act
  const auto result = sut.started();

  // Assert
  REQUIRE(result == expected);
}

//-----------------------------------------------------------------------------

TEST_CASE("sequence_detector<T>::completed()", "[observers]") {
  // Arrange
  const auto values = core::vector<int>{1,2,3,4};
  const auto consumed = GENERATE(0u, 1u, 4u);
  const auto expected = consumed == values.size();
  auto sut = sequence_detector{values.copy()};
  for (auto i = 0u; i < consumed; ++i) {
    sut.test(values[i]);
  }

  // Act
  const auto result = sut.completed();

  // Assert
  REQUIRE(result == expected);
}

} // namespace alloy::platform::test
