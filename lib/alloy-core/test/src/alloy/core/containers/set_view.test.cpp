/*
  The MIT License (MIT)

  Copyright (c) 2022 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <alloy/core/containers/set_view.hpp>

#include <alloy/core/utilities/result.hpp>

#include <catch2/catch.hpp>
#include <set>
#include <unordered_set>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("set_view::set_view()", "[ctor]") {
  const auto sut = set_view<int>{};

  SECTION("Constructs empty view") {
    REQUIRE(sut.empty());
  }
}

TEMPLATE_TEST_CASE("set_view::set_view(const Sap&)", "[ctor]",
  (std::set<int>),
  (std::unordered_set<int>)
) {
  auto set = TestType{};

  const auto sut = set_view<int>{set};

  SECTION("Contains same number of entries as viewed set") {
    REQUIRE(sut.size() == set.size());
  }
}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

TEMPLATE_TEST_CASE("set_view::size()", "[capacity]",
  void,
  (std::set<int>),
  (std::unordered_set<int>)
) {
  if constexpr (std::is_same_v<void,TestType>) {
    SECTION("View is not viewing a set") {
      const auto sut = set_view<int>{};

      SECTION("Has zero size") {
        REQUIRE(sut.size() == 0u);
      }
    }
  } else {
    SECTION("View is viewing a set") {
      auto set = TestType{};

      const auto sut = set_view<int>{set};

      SECTION("Viewed set contains entries") {
        for (auto i = 0; i < 10; ++i) {
          set.insert(i);
        }
        SECTION("Contains same number of entries as viewed set") {
          REQUIRE(sut.size() == set.size());
        }
      }
      SECTION("Viewed set is empty") {
        SECTION("Contains same number of entries as viewed set") {
          REQUIRE(sut.size() == set.size());
        }
      }
    }
  }
}

TEMPLATE_TEST_CASE("set_view::empty()", "[capacity]",
  void,
  (std::set<int>),
  (std::unordered_set<int>)
) {
  if constexpr (std::is_same_v<void,TestType>) {
    SECTION("View is not viewing a set") {
      const auto sut = set_view<int>{};

      SECTION("View is empty") {
        REQUIRE(sut.empty());
      }
    }
  } else {
    SECTION("View is viewing a set") {
      auto set = TestType{};

      const auto sut = set_view<int>{set};

      SECTION("Viewed set contains entries") {
        for (auto i = 0; i < 10; ++i) {
          set.insert(i);
        }
        SECTION("View is not empty") {
          REQUIRE_FALSE(sut.empty());
        }
      }
      SECTION("Viewed set is empty") {
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

TEMPLATE_TEST_CASE("set_view::count(const Key&)", "[lookup]",
  void,
  (std::set<int>),
  (std::unordered_set<int>)
) {
  static constexpr auto value = 42;

  if constexpr (std::is_same_v<void,TestType>) {
    SECTION("View is not viewing a set") {
      const auto sut = set_view<int>{};

      SECTION("View counts 0 entries") {
        REQUIRE(sut.count(value) == 0u);
      }
    }
  } else {
    SECTION("View is viewing a set") {
      auto set = TestType{};

      const auto sut = set_view<int>{set};

      SECTION("Viewed set contains value") {
        set.insert(value);
        SECTION("View counts 1 entry") {
          REQUIRE(sut.count(value) == 1u);
        }
      }
      SECTION("Viewed set is empty") {
        SECTION("View counts 0 entries") {
          REQUIRE(sut.count(value) == 0u);
        }
      }
    }
  }
}

TEMPLATE_TEST_CASE("set_view::contains(const Key&)", "[lookup]",
  void,
  (std::set<int>),
  (std::unordered_set<int>)
) {
  static constexpr auto value = 42;

  if constexpr (std::is_same_v<void,TestType>) {
    SECTION("View is not viewing a set") {
      const auto sut = set_view<int>{};

      SECTION("View does not contain value") {
        REQUIRE_FALSE(sut.contains(value));
      }
    }
  } else {
    SECTION("View is viewing a set") {
      auto set = TestType{};

      const auto sut = set_view<int>{set};

      SECTION("Viewed set contains value") {
        set.insert(value);
        SECTION("View contains value") {
          REQUIRE(sut.contains(value));
        }
      }
      SECTION("Viewed set is empty") {
        SECTION("View does not contain value") {
          REQUIRE_FALSE(sut.contains(value));
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
// Iteration
//------------------------------------------------------------------------------

TEMPLATE_TEST_CASE("set_view::for_each(Callback&&)", "[iteration]",
  void,
  (std::set<int>),
  (std::unordered_set<int>)
) {
  if constexpr (std::is_same_v<void,TestType>) {
    SECTION("View is not viewing a map") {
      const auto sut = set_view<int>{};

      SECTION("View does not iterate anything") {
        auto iterations = 0u;
        sut.for_each([&iterations](const auto&){
          ++iterations;
        });
        REQUIRE(iterations == 0u);
      }
    }
  } else {
    SECTION("View is viewing a map") {
      auto set = TestType{};

      const auto sut = set_view<int>{set};

      SECTION("Viewed map contains value") {
        set.insert(42);
        set.insert(0xdead);
        set.insert(0xbeef);

        auto entries = TestType{};

        sut.for_each([&entries](const auto& v){
          entries.insert(v);
        });

        SECTION("Viewed set iterates all underlying key/value pairs") {
          REQUIRE(set == entries);
        }
      }
      SECTION("Viewed set is empty") {
        SECTION("View does not iterate anything") {
          auto iterations = 0u;
          sut.for_each([&iterations](const auto&){
            ++iterations;
          });
          REQUIRE(iterations == 0u);
        }
      }
    }
  }
}


} // namespace alloy::core::test
