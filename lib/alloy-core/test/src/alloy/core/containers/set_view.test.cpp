// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/containers/set_view.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <set>
#include <unordered_set>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("set_view::set_view()", "[ctor]") {
  // Act
  const auto sut = set_view<int>{};

  // Assert
  SECTION("Constructs empty view") {
    REQUIRE(sut.empty());
  }
}

TEMPLATE_TEST_CASE("set_view::set_view(const Sap&)", "[ctor]",
  (std::set<int>),
  (std::unordered_set<int>)
) {
  // Arrange
  auto set = TestType{};

  // Act
  const auto sut = set_view<int>{set};

  // Assert
  SECTION("Contains same number of entries as viewed set") {
    REQUIRE(sut.size() == set.size());
  }
}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

TEST_CASE("set_view::size() on an unbound view", "[capacity]") {
  SECTION("View is not viewing a set") {
    // Arrange
    const auto sut = set_view<int>{};

    SECTION("Has zero size") {
      // Act
      const auto result = sut.size();

      // Assert
      REQUIRE(result == 0u);
    }
  }
}

TEMPLATE_TEST_CASE("set_view::size()", "[capacity]",
  (std::set<int>),
  (std::unordered_set<int>)
) {
  SECTION("View is viewing a set") {
    // Arrange
    auto set = TestType{};

    const auto sut = set_view<int>{set};

    SECTION("Viewed set contains entries") {
      for (auto i = 0; i < 10; ++i) {
        set.insert(i);
      }
      SECTION("Contains same number of entries as viewed set") {
        // Act
        const auto result = sut.size();
        const auto equivalent = set.size();

        // Assert
        REQUIRE(result == equivalent);
      }
    }
    SECTION("Viewed set is empty") {
      SECTION("Contains same number of entries as viewed set") {
        // Act
        const auto result = sut.size();
        const auto equivalent = set.size();

        // Assert
        REQUIRE(result == equivalent);
      }
    }
  }
}

TEST_CASE("set_view::empty() on an unbound view", "[capacity]") {
  SECTION("View is not viewing a set") {
    // Arrange
    const auto sut = set_view<int>{};

    SECTION("View is empty") {
      // Act
      const auto result = sut.empty();

      // Assert
      REQUIRE(result);
    }
  }
}

TEMPLATE_TEST_CASE("set_view::empty()", "[capacity]",
  (std::set<int>),
  (std::unordered_set<int>)
) {
  SECTION("View is viewing a set") {
    // Arrange
    auto set = TestType{};

    const auto sut = set_view<int>{set};

    SECTION("Viewed set contains entries") {
      for (auto i = 0; i < 10; ++i) {
        set.insert(i);
      }
      SECTION("View is not empty") {
        // Act
        const auto result = sut.empty();

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Viewed set is empty") {
      SECTION("View is empty") {
        // Act
        const auto result = sut.empty();

        // Assert
        REQUIRE(result);
      }
    }
  }
}

//------------------------------------------------------------------------------
// Lookup
//------------------------------------------------------------------------------

TEST_CASE("set_view::count(const Key&) on an unbound view", "[lookup]") {
  // Arrange
  static constexpr auto value = 42;
  SECTION("View is not viewing a set") {
    const auto sut = set_view<int>{};

    SECTION("View counts 0 entries") {
      // Act
      const auto result = sut.count(value);

      // Assert
      REQUIRE(result == 0u);
    }
  }
}

TEMPLATE_TEST_CASE("set_view::count(const Key&)", "[lookup]",
  (std::set<int>),
  (std::unordered_set<int>)
) {
  // Arrange
  static constexpr auto value = 42;
  SECTION("View is viewing a set") {
    auto set = TestType{};

    const auto sut = set_view<int>{set};

    SECTION("Viewed set contains value") {
      set.insert(value);
      SECTION("View counts 1 entry") {
        // Act
        const auto result = sut.count(value);

        // Assert
        REQUIRE(result == 1u);
      }
    }
    SECTION("Viewed set is empty") {
      SECTION("View counts 0 entries") {
        // Act
        const auto result = sut.count(value);

        // Assert
        REQUIRE(result == 0u);
      }
    }
  }
}

TEST_CASE("set_view::contains(const Key&) on an unbound view", "[lookup]") {
  // Arrange
  static constexpr auto value = 42;
  SECTION("View is not viewing a set") {
    const auto sut = set_view<int>{};

    SECTION("View does not contain value") {
      // Act
      const auto result = sut.contains(value);

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEMPLATE_TEST_CASE("set_view::contains(const Key&)", "[lookup]",
  (std::set<int>),
  (std::unordered_set<int>)
) {
  // Arrange
  static constexpr auto value = 42;
  SECTION("View is viewing a set") {
    auto set = TestType{};

    const auto sut = set_view<int>{set};

    SECTION("Viewed set contains value") {
      set.insert(value);
      SECTION("View contains value") {
        // Act
        const auto result = sut.contains(value);

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Viewed set is empty") {
      SECTION("View does not contain value") {
        // Act
        const auto result = sut.contains(value);

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

//------------------------------------------------------------------------------
// Iteration
//------------------------------------------------------------------------------

TEST_CASE("set_view::for_each(Callback&&) on an unbound view", "[iteration]") {
  SECTION("View is not viewing a map") {
    // Arrange
    const auto sut = set_view<int>{};

    SECTION("View does not iterate anything") {
      auto iterations = 0u;

      // Act
      sut.for_each([&iterations](const auto&){
        ++iterations;
      });

      // Assert
      REQUIRE(iterations == 0u);
    }
  }
}

TEMPLATE_TEST_CASE("set_view::for_each(Callback&&)", "[iteration]",
  (std::set<int>),
  (std::unordered_set<int>)
) {
  SECTION("View is viewing a map") {
    // Arrange
    auto set = TestType{};

    const auto sut = set_view<int>{set};

    SECTION("Viewed map contains value") {
      set.insert(42);
      set.insert(0xdead);
      set.insert(0xbeef);

      auto entries = TestType{};

      // Act
      sut.for_each([&entries](const auto& v){
        entries.insert(v);
      });

      // Assert
      SECTION("Viewed set iterates all underlying key/value pairs") {
        REQUIRE(set == entries);
      }
    }
    SECTION("Viewed set is empty") {
      SECTION("View does not iterate anything") {
        auto iterations = 0u;

        // Act
        sut.for_each([&iterations](const auto&){
          ++iterations;
        });

        // Assert
        REQUIRE(iterations == 0u);
      }
    }
  }
}


} // namespace alloy::core::test
