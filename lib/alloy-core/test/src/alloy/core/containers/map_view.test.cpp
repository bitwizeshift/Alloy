// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/containers/map_view.hpp>

#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <map>
#include <unordered_map>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("map_view::map_view()", "[ctor]") {
  // Act
  const auto sut = map_view<int,int>{};

  // Assert
  SECTION("Constructs empty view") {
    REQUIRE(sut.empty());
  }
}

TEMPLATE_TEST_CASE("map_view::map_view(const Map&)", "[ctor]",
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  // Arrange
  auto map = TestType{};

  // Act
  const auto sut = map_view<int,int>{map};

  // Assert
  SECTION("Contains same number of entries as viewed map") {
    REQUIRE(sut.size() == map.size());
  }
}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

TEST_CASE("map_view::size() on an unbound view", "[capacity]") {
  SECTION("View is not viewing a map") {
    // Arrange
    const auto sut = map_view<int,int>{};

    SECTION("Has zero size") {
      // Act
      const auto result = sut.size();

      // Assert
      REQUIRE(result == 0u);
    }
  }
}

TEMPLATE_TEST_CASE("map_view::size()", "[capacity]",
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  SECTION("View is viewing a map") {
    // Arrange
    auto map = TestType{};

    const auto sut = map_view<int,int>{map};

    SECTION("Viewed map contains entries") {
      // insert random data
      for (auto i = 0; i < 10; ++i) {
        map[i] = 2 * i + 1;
      }
      SECTION("Contains same number of entries as viewed map") {
        // Act
        const auto result = sut.size();
        const auto equivalent = map.size();

        // Assert
        REQUIRE(result == equivalent);
      }
    }
    SECTION("Viewed map is empty") {
      SECTION("Contains same number of entries as viewed map") {
        // Act
        const auto result = sut.size();
        const auto equivalent = map.size();

        // Assert
        REQUIRE(result == equivalent);
      }
    }
  }
}

TEST_CASE("map_view::empty() on an unbound view", "[capacity]") {
  SECTION("View is not viewing a map") {
    // Arrange
    const auto sut = map_view<int,int>{};

    SECTION("View is empty") {
      // Act
      const auto result = sut.empty();

      // Assert
      REQUIRE(result);
    }
  }
}

TEMPLATE_TEST_CASE("map_view::empty()", "[capacity]",
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  SECTION("View is viewing a map") {
    // Arrange
    auto map = TestType{};

    const auto sut = map_view<int,int>{map};

    SECTION("Viewed map contains entries") {
      // insert random data
      for (auto i = 0; i < 10; ++i) {
        map[i] = 2 * i + 1;
      }
      SECTION("View is not empty") {
        // Act
        const auto result = sut.empty();

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Viewed map is empty") {
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

TEST_CASE("map_view::count(const Key&) on an unbound view", "[lookup]") {
  // Arrange
  static constexpr auto value = 42;
  SECTION("View is not viewing a map") {
    const auto sut = map_view<int,int>{};

    SECTION("View counts 0 entries") {
      // Act
      const auto result = sut.count(value);

      // Assert
      REQUIRE(result == 0u);
    }
  }
}

TEMPLATE_TEST_CASE("map_view::count(const Key&)", "[lookup]",
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  // Arrange
  static constexpr auto value = 42;
  SECTION("View is viewing a map") {
    auto map = TestType{};

    const auto sut = map_view<int,int>{map};

    SECTION("Viewed map contains value") {
      map[value] = value;
      SECTION("View counts 1 entry") {
        // Act
        const auto result = sut.count(value);

        // Assert
        REQUIRE(result == 1u);
      }
    }
    SECTION("Viewed map is empty") {
      SECTION("View counts 0 entries") {
        // Act
        const auto result = sut.count(value);

        // Assert
        REQUIRE(result == 0u);
      }
    }
  }
}

TEST_CASE("map_view::contains(const Key&) on an unbound view", "[lookup]") {
  // Arrange
  static constexpr auto value = 42;
  SECTION("View is not viewing a map") {
    const auto sut = map_view<int,int>{};

    SECTION("View does not contain value") {
      // Act
      const auto result = sut.contains(value);

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEMPLATE_TEST_CASE("map_view::contains(const Key&)", "[lookup]",
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  // Arrange
  static constexpr auto value = 42;
  SECTION("View is viewing a map") {
    auto map = TestType{};

    const auto sut = map_view<int,int>{map};

    SECTION("Viewed map contains value") {
      map[value] = value;
      SECTION("View contains value") {
        // Act
        const auto result = sut.contains(value);

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Viewed map is empty") {
      SECTION("View does not contain value") {
        // Act
        const auto result = sut.contains(value);

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

TEST_CASE("map_view::at(const Key&) on an unbound view", "[lookup]") {
  // Arrange
  static constexpr auto value = 42;
  SECTION("View is not viewing a map") {
    const auto sut = map_view<int,int>{};

    SECTION("View does not contain value") {
      // Act
      const auto result = sut.at(value);

      // Assert
      REQUIRE(result == fail(map_view_lookup_error::key_not_found));
    }
  }
}

TEMPLATE_TEST_CASE("map_view::at(const Key&)", "[lookup]",
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  // Arrange
  static constexpr auto value = 42;
  SECTION("View is viewing a map") {
    auto map = TestType{};

    const auto sut = map_view<int,int>{map};

    SECTION("Viewed map contains value") {
      map[value] = value;
      SECTION("View returns reference to value") {
        // Act
        const auto result = &sut.at(value).value();
        const auto equivalent = &map.at(value);

        // Assert
        REQUIRE(result == equivalent);
      }
    }
    SECTION("Viewed map is empty") {
      SECTION("View returns lookup error") {
        // Act
        const auto result = sut.at(value);

        // Assert
        REQUIRE(result == fail(map_view_lookup_error::key_not_found));
      }
    }
  }
}

//------------------------------------------------------------------------------
// Iteration
//------------------------------------------------------------------------------

TEST_CASE("map_view::for_each(Callback&&) on an unbound view", "[iteration]") {
  SECTION("View is not viewing a map") {
    // Arrange
    const auto sut = map_view<int,int>{};

    SECTION("View does not iterate anything") {
      auto iterations = 0u;

      // Act
      sut.for_each([&iterations](const auto&, const auto&){
        ++iterations;
      });

      // Assert
      REQUIRE(iterations == 0u);
    }
  }
}

TEMPLATE_TEST_CASE("map_view::for_each(Callback&&)", "[iteration]",
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  SECTION("View is viewing a map") {
    // Arrange
    auto map = TestType{};

    const auto sut = map_view<int,int>{map};

    SECTION("Viewed map contains value") {
      map[42] = 1;
      map[0xdead] = 0xbeef;

      auto entries = TestType{};

      // Act
      sut.for_each([&entries](const auto& k, const auto& v){
        entries[k] = v;
      });

      // Assert
      SECTION("Viewed map iterates all underlying key/value pairs") {
        REQUIRE(map == entries);
      }
    }
    SECTION("Viewed map is empty") {
      SECTION("View does not iterate anything") {
        auto iterations = 0u;

        // Act
        sut.for_each([&iterations](const auto&, const auto&){
          ++iterations;
        });

        // Assert
        REQUIRE(iterations == 0u);
      }
    }
  }
}

} // namespace alloy::core::test
