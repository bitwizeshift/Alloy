// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/model/cached_transform.hpp>

#include <alloy/core/intrinsics.hpp>
#include <alloy/core/math/matrix/matrix4.hpp>
#include <alloy/core/utilities/not_null.hpp>

#include <catch2/catch_test_macros.hpp>

#include <memory>  // std::unique_ptr, std::make_unique
#include <utility> // std::as_const, std::in_place, std::move

namespace alloy::core::test {
namespace {

  // A transform that counts how many times its matrix is extracted, and
  // writes its value into the first entry of the matrix.
  struct counting_transform
  {
    float value;
    int* extractions;

    auto extract_matrix(not_null<matrix4*> out) const noexcept -> void
    {
      ++(*extractions);
      (*out) = matrix4{};
      (*out)[0][0] = value;
    }
  };

  // A transform that can only be moved, which writes its value into the
  // first entry of the matrix.
  struct move_only_transform
  {
    std::unique_ptr<float> value;

    auto extract_matrix(not_null<matrix4*> out) const noexcept -> void
    {
      (*out) = matrix4{};
      (*out)[0][0] = *value;
    }
  };

} // namespace

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("cached_transform::cached_transform( Transform&& )", "[ctor]")
{
  // Arrange
  auto transform = move_only_transform{std::make_unique<float>(3.0f)};

  // Act
  auto sut = cached_transform{std::move(transform)};
  const auto result = sut.matrix();

  // Assert
  SECTION("Takes ownership of a move-only transform")
  {
    REQUIRE( result[0][0] == 3.0f );
  }
}

TEST_CASE("cached_transform::cached_transform( std::in_place_t, Args&&... )",
          "[ctor]")
{
  // Arrange
  auto extractions = 0;

  // Act
  const auto sut = cached_transform<counting_transform>{
    std::in_place,
    3.0f,
    &extractions
  };
  const auto result = sut.value().value;

  // Assert
  SECTION("Constructs the transform from the arguments")
  {
    REQUIRE( result == 3.0f );
  }
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

TEST_CASE("cached_transform::operator*() &", "[element access]")
{
  // Arrange
  auto extractions = 0;
  auto sut = cached_transform{counting_transform{1.0f, &extractions}};
  sut.matrix();

  // Act
  (*sut).value = 2.0f;
  const auto result = sut.matrix();

  // Assert
  SECTION("Marks the matrix as stale")
  {
    REQUIRE( result[0][0] == 2.0f );
  }
}

TEST_CASE("cached_transform::operator*() &&", "[element access]")
{
  // Arrange
  auto sut = cached_transform{
    move_only_transform{std::make_unique<float>(3.0f)}
  };

  // Act
  const auto result = *std::move(sut);

  // Assert
  SECTION("Moves the transform out")
  {
    REQUIRE( *result.value == 3.0f );
  }
}

TEST_CASE("cached_transform::operator->() const", "[element access]")
{
  // Arrange
  auto extractions = 0;
  auto sut = cached_transform{counting_transform{1.0f, &extractions}};
  sut.matrix();

  // Act
  compiler::unused(std::as_const(sut)->value);
  sut.matrix();

  // Assert
  SECTION("Does not mark the matrix as stale")
  {
    REQUIRE( extractions == 1 );
  }
}

TEST_CASE("cached_transform::operator*() const &", "[element access]")
{
  // Arrange
  auto extractions = 0;
  auto sut = cached_transform{counting_transform{1.0f, &extractions}};
  sut.matrix();

  // Act
  compiler::unused((*std::as_const(sut)).value);
  sut.matrix();

  // Assert
  SECTION("Does not mark the matrix as stale")
  {
    REQUIRE( extractions == 1 );
  }
}

//------------------------------------------------------------------------------
// Conversion
//------------------------------------------------------------------------------

TEST_CASE("cached_transform::matrix()", "[conversion]")
{
  // Arrange
  auto extractions = 0;
  auto sut = cached_transform{counting_transform{1.0f, &extractions}};

  SECTION("First call computes the matrix")
  {
    // Act
    const auto result = sut.matrix();

    // Assert
    REQUIRE( result[0][0] == 1.0f );
  }
  SECTION("Unmodified transform is not recomputed")
  {
    // Act
    sut.matrix();
    sut.matrix();

    // Assert
    REQUIRE( extractions == 1 );
  }
  SECTION("Modified transform is recomputed once")
  {
    // Arrange
    sut.matrix();

    // Act
    sut->value = 2.0f;
    sut.matrix();
    sut.matrix();

    // Assert
    REQUIRE( extractions == 2 );
  }
  SECTION("Modified transform produces the updated matrix")
  {
    // Arrange
    sut.matrix();

    // Act
    sut->value = 2.0f;
    const auto result = sut.matrix();

    // Assert
    REQUIRE( result[0][0] == 2.0f );
  }
}

} // namespace alloy::core::test
