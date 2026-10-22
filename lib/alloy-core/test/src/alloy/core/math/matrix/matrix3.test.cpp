// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/matrix/matrix3.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/math/matrix/matrix3_constants.hpp>

#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

TEST_CASE("matrix3::determinant()", "[quantifiers]")
{
  // Arrange
  const auto sut = matrix3{
    2, 0, 1,
    1, 3, 2,
    1, 1, 2
  };

  // Act
  const auto result = sut.determinant();

  // Assert
  REQUIRE( almost_equal(result, real{6}) );
}

TEST_CASE("matrix3::trace()", "[quantifiers]")
{
  // Arrange
  const auto sut = matrix3{
    2, 0, 1,
    1, 3, 2,
    1, 1, 2
  };

  // Act
  const auto result = sut.trace();

  // Assert
  REQUIRE( almost_equal(result, real{7}) );
}

TEST_CASE("matrix3::inverse()", "[quantifiers]")
{
  SECTION("Inverts a non-symmetric matrix")
  {
    // Arrange
    const auto sut = matrix3{
      1, 2, 3,
      0, 1, 4,
      5, 6, 0
    };
    const auto expected = matrix3{
      -24,  18,  5,
       20, -15, -4,
       -5,   4,  1
    };

    // Act
    const auto result = sut.inverse();

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Product with the original matrix is the identity")
  {
    // Arrange
    const auto sut = matrix3{
      2, 0, 1,
      1, 3, 2,
      1, 1, 2
    };

    // Act
    const auto result = sut.inverse() * sut;

    // Assert
    REQUIRE( almost_equal(result, matrix3_constants::identity) );
  }

  SECTION("Singular matrix returns the identity")
  {
    // Arrange
    const auto sut = matrix3{
      1, 2, 3,
      2, 4, 6,
      0, 1, 1
    };

    // Act
    const auto result = sut.inverse();

    // Assert
    REQUIRE( result == matrix3_constants::identity );
  }
}

TEST_CASE("matrix3::transposed()", "[quantifiers]")
{
  // Arrange
  const auto sut = matrix3{
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
  };
  const auto expected = matrix3{
    1, 4, 7,
    2, 5, 8,
    3, 6, 9
  };

  // Act
  const auto result = sut.transposed();

  // Assert
  REQUIRE( result == expected );
}

} // namespace alloy::core::test
