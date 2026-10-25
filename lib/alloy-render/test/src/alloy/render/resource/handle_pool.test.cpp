// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/resource/handle_pool.hpp>

#include <alloy/render/resource/resource_handle.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::render::test {
namespace {

struct test_tag {};

using test_pool = handle_pool<test_tag, int>;
using test_handle = resource_handle<test_tag>;

} // namespace <anonymous>

TEST_CASE("handle_pool::insert(Object)", "[modifier]") {
  SECTION("Returns a non-null handle") {
    // Arrange
    auto sut = test_pool{};

    // Act
    const auto handle = sut.insert(42);

    // Assert
    REQUIRE_FALSE(handle.is_null());
  }
  SECTION("Returns distinct handles for each object") {
    // Arrange
    auto sut = test_pool{};
    const auto first = sut.insert(1);

    // Act
    const auto second = sut.insert(2);

    // Assert
    REQUIRE(first != second);
  }
  SECTION("Returns a distinct handle when reusing an erased slot") {
    // Arrange
    auto sut = test_pool{};
    const auto erased = sut.insert(1);
    sut.erase(erased);

    // Act
    const auto reused = sut.insert(2);

    // Assert
    REQUIRE(reused != erased);
  }
  SECTION("Increases the size") {
    // Arrange
    auto sut = test_pool{};

    // Act
    sut.insert(1);

    // Assert
    REQUIRE(sut.size() == 1u);
  }
}

TEST_CASE("handle_pool::find(handle_type)", "[element access]") {
  SECTION("Handle of a live object") {
    // Arrange
    auto sut = test_pool{};
    const auto handle = sut.insert(42);

    // Act
    const auto* const result = sut.find(handle);

    // Assert
    REQUIRE(*result == 42);
  }
  SECTION("Null handle") {
    // Arrange
    auto sut = test_pool{};
    sut.insert(42);

    // Act
    const auto* const result = sut.find(test_handle{});

    // Assert
    REQUIRE(result == nullptr);
  }
  SECTION("Handle of an erased object") {
    // Arrange
    auto sut = test_pool{};
    const auto handle = sut.insert(42);
    sut.erase(handle);

    // Act
    const auto* const result = sut.find(handle);

    // Assert
    REQUIRE(result == nullptr);
  }
  SECTION("Handle of an erased object whose slot was reused") {
    // Arrange
    auto sut = test_pool{};
    const auto handle = sut.insert(1);
    sut.erase(handle);
    sut.insert(2);

    // Act
    const auto* const result = sut.find(handle);

    // Assert
    REQUIRE(result == nullptr);
  }
  SECTION("Handle from another pool with more slots") {
    // Arrange
    auto other = test_pool{};
    other.insert(1);
    const auto handle = other.insert(2);
    auto sut = test_pool{};
    sut.insert(3);

    // Act
    const auto* const result = sut.find(handle);

    // Assert
    REQUIRE(result == nullptr);
  }
}

TEST_CASE("handle_pool::erase(handle_type)", "[modifier]") {
  SECTION("Handle of a live object") {
    // Arrange
    auto sut = test_pool{};
    const auto handle = sut.insert(42);

    // Act
    const auto result = sut.erase(handle);

    // Assert
    REQUIRE(result == 42);
  }
  SECTION("Handle of a live object decreases the size") {
    // Arrange
    auto sut = test_pool{};
    const auto handle = sut.insert(42);

    // Act
    sut.erase(handle);

    // Assert
    REQUIRE(sut.empty());
  }
  SECTION("Handle of an erased object") {
    // Arrange
    auto sut = test_pool{};
    const auto handle = sut.insert(42);
    sut.erase(handle);

    // Act
    const auto result = sut.erase(handle);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
  SECTION("Null handle") {
    // Arrange
    auto sut = test_pool{};
    sut.insert(42);

    // Act
    const auto result = sut.erase(test_handle{});

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
}

TEST_CASE("resource_handle::is_null()", "[observer]") {
  SECTION("Default constructed handle") {
    // Arrange
    const auto sut = test_handle{};

    // Act
    const auto result = sut.is_null();

    // Assert
    REQUIRE(result);
  }
}

} // namespace alloy::render::test
