/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include <alloy/core/containers/map_view.hpp>

#include <alloy/core/utilities/result.hpp>

#include <catch2/catch.hpp>
#include <map>
#include <unordered_map>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("map_view::map_view()", "[ctor]") {
  const auto sut = map_view<int,int>{};

  SECTION("Constructs empty view") {
    REQUIRE(sut.empty());
  }
}

TEMPLATE_TEST_CASE("map_view::map_view(const Map&)", "[ctor]",
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  auto map = TestType{};

  const auto sut = map_view<int,int>{map};

  SECTION("Contains same number of entries as viewed map") {
    REQUIRE(sut.size() == map.size());
  }
}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

TEMPLATE_TEST_CASE("map_view::size()", "[capacity]",
  void,
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  if constexpr (std::is_same_v<void,TestType>) {
    SECTION("View is not viewing a map") {
      const auto sut = map_view<int,int>{};

      SECTION("Has zero size") {
        REQUIRE(sut.size() == 0u);
      }
    }
  } else {
    SECTION("View is viewing a map") {
      auto map = TestType{};

      const auto sut = map_view<int,int>{map};

      SECTION("Viewed map contains entries") {
        // insert random data
        for (auto i = 0; i < 10; ++i) {
          map[i] = 2 * i + 1;
        }
        SECTION("Contains same number of entries as viewed map") {
          REQUIRE(sut.size() == map.size());
        }
      }
      SECTION("Viewed map is empty") {
        SECTION("Contains same number of entries as viewed map") {
          REQUIRE(sut.size() == map.size());
        }
      }
    }
  }
}

TEMPLATE_TEST_CASE("map_view::empty()", "[capacity]",
  void,
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  if constexpr (std::is_same_v<void,TestType>) {
    SECTION("View is not viewing a map") {
      const auto sut = map_view<int,int>{};

      SECTION("View is empty") {
        REQUIRE(sut.empty());
      }
    }
  } else {
    SECTION("View is viewing a map") {
      auto map = TestType{};

      const auto sut = map_view<int,int>{map};

      SECTION("Viewed map contains entries") {
        // insert random data
        for (auto i = 0; i < 10; ++i) {
          map[i] = 2 * i + 1;
        }
        SECTION("View is not empty") {
          REQUIRE_FALSE(sut.empty());
        }
      }
      SECTION("Viewed map is empty") {
        SECTION("View is empty") {
          REQUIRE(sut.empty());
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
// Lookup
//------------------------------------------------------------------------------

TEMPLATE_TEST_CASE("map_view::count(const Key&)", "[lookup]",
  void,
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  static constexpr auto value = 42;

  if constexpr (std::is_same_v<void,TestType>) {
    SECTION("View is not viewing a map") {
      const auto sut = map_view<int,int>{};

      SECTION("View counts 0 entries") {
        REQUIRE(sut.count(value) == 0u);
      }
    }
  } else {
    SECTION("View is viewing a map") {
      auto map = TestType{};

      const auto sut = map_view<int,int>{map};

      SECTION("Viewed map contains value") {
        map[value] = value;
        SECTION("View counts 1 entry") {
          REQUIRE(sut.count(value) == 1u);
        }
      }
      SECTION("Viewed map is empty") {
        SECTION("View counts 0 entries") {
          REQUIRE(sut.count(value) == 0u);
        }
      }
    }
  }
}

TEMPLATE_TEST_CASE("map_view::contains(const Key&)", "[lookup]",
  void,
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  static constexpr auto value = 42;

  if constexpr (std::is_same_v<void,TestType>) {
    SECTION("View is not viewing a map") {
      const auto sut = map_view<int,int>{};

      SECTION("View does not contain value") {
        REQUIRE_FALSE(sut.contains(value));
      }
    }
  } else {
    SECTION("View is viewing a map") {
      auto map = TestType{};

      const auto sut = map_view<int,int>{map};

      SECTION("Viewed map contains value") {
        map[value] = value;
        SECTION("View contains value") {
          REQUIRE(sut.contains(value));
        }
      }
      SECTION("Viewed map is empty") {
        SECTION("View does not contain value") {
          REQUIRE_FALSE(sut.contains(value));
        }
      }
    }
  }
}

TEMPLATE_TEST_CASE("map_view::at(const Key&)", "[lookup]",
  void,
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  static constexpr auto value = 42;

  if constexpr (std::is_same_v<void,TestType>) {
    SECTION("View is not viewing a map") {
      const auto sut = map_view<int,int>{};

      SECTION("View does not contain value") {
        REQUIRE(sut.at(value) == fail(map_view_lookup_error::key_not_found));
      }
    }
  } else {
    SECTION("View is viewing a map") {
      auto map = TestType{};

      const auto sut = map_view<int,int>{map};

      SECTION("Viewed map contains value") {
        map[value] = value;
        SECTION("View returns reference to value") {
          REQUIRE(&sut.at(value).value() == &map.at(value));
        }
      }
      SECTION("Viewed map is empty") {
        SECTION("View returns lookup error") {
          REQUIRE(sut.at(value) == fail(map_view_lookup_error::key_not_found));
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
// Iteration
//------------------------------------------------------------------------------

TEMPLATE_TEST_CASE("map_view::for_each(Callback&&)", "[iteration]",
  void,
  (std::map<int,int>),
  (std::unordered_map<int,int>)
) {
  if constexpr (std::is_same_v<void,TestType>) {
    SECTION("View is not viewing a map") {
      const auto sut = map_view<int,int>{};

      SECTION("View does not iterate anything") {
        auto iterations = 0u;
        sut.for_each([&iterations](const auto&, const auto&){
          ++iterations;
        });
        REQUIRE(iterations == 0u);
      }
    }
  } else {
    SECTION("View is viewing a map") {
      auto map = TestType{};

      const auto sut = map_view<int,int>{map};

      SECTION("Viewed map contains value") {
        map[42] = 1;
        map[0xdead] = 0xbeef;

        auto entries = TestType{};

        sut.for_each([&entries](const auto& k, const auto& v){
          entries[k] = v;
        });

        SECTION("Viewed map iterates all underlying key/value pairs") {
          REQUIRE(map == entries);
        }
      }
      SECTION("Viewed map is empty") {
        SECTION("View does not iterate anything") {
          auto iterations = 0u;
          sut.for_each([&iterations](const auto&, const auto&){
            ++iterations;
          });
          REQUIRE(iterations == 0u);
        }
      }
    }
  }
}

} // namespace alloy::core::test
