// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/pass/render_batch.hpp>

#include <alloy/render/model/model_spec.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>   // std::array
#include <cstddef> // std::size_t
#include <span>    // std::span

namespace alloy::render::test {
namespace {

struct instance
{
  float x;
  float y;
};

} // namespace <anonymous>

TEST_CASE("render_batch::add(model_handle, std::span<const Instance>)", "[modifier]") {
  // Arrange
  auto sut = render_batch{};
  const auto first = std::array{instance{1.0f, 2.0f}};
  const auto second = std::array{instance{3.0f, 4.0f}, instance{5.0f, 6.0f}};

  SECTION("Counts the instances of an entry") {
    // Act
    sut.add(model_handle{}, std::span{second});

    // Assert
    REQUIRE(sut.entries()[0].instance_count == 2u);
  }
  SECTION("Places each entry after the instances of the last") {
    // Act
    sut.add(model_handle{}, std::span{first});
    sut.add(model_handle{}, std::span{second});

    // Assert
    REQUIRE(sut.entries()[1].offset == sizeof(instance));
  }
  SECTION("Keeps every instance together") {
    // Act
    sut.add(model_handle{}, std::span{first});
    sut.add(model_handle{}, std::span{second});

    // Assert
    REQUIRE(sut.instance_data().size() == 3u * sizeof(instance));
  }
  SECTION("No instances adds nothing") {
    // Act
    sut.add(model_handle{}, std::span<const instance>{});

    // Assert
    REQUIRE(sut.empty());
  }
}

TEST_CASE("render_batch::clear()", "[modifier]") {
  // Arrange
  auto sut = render_batch{};
  const auto instances = std::array{instance{1.0f, 2.0f}};
  sut.add(model_handle{}, std::span{instances});

  SECTION("Removes every entry") {
    // Act
    sut.clear();

    // Assert
    REQUIRE(sut.empty());
  }
  SECTION("Removes every instance") {
    // Act
    sut.clear();

    // Assert
    REQUIRE(sut.instance_data().empty());
  }
}

} // namespace alloy::render::test
