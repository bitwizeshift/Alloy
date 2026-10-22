// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/matrix/matrix2.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/math/matrix/matrix2_constants.hpp>
#include <alloy/core/math/vector/vector2.hpp>

#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

TEST_CASE("matrix2::determinant()", "[quantifiers]")
{
  // Arrange
  const auto sut = matrix2{
    4, 7,
    2, 6
  };

  // Act
  const auto result = sut.determinant();

  // Assert
  REQUIRE( almost_equal(result, real{10}) );
}

TEST_CASE("matrix2::trace()", "[quantifiers]")
{
  // Arrange
  const auto sut = matrix2{
    4, 7,
    2, 6
  };

  // Act
  const auto result = sut.trace();

  // Assert
  REQUIRE( almost_equal(result, real{10}) );
}

TEST_CASE("matrix2::inverse()", "[quantifiers]")
{
  SECTION("Inverts a shear")
  {
    // Arrange
    const auto sut = matrix2{
      1, 2,
      0, 1
    };
    const auto expected = matrix2{
      1, -2,
      0,  1
    };

    // Act
    const auto result = sut.inverse();

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Product with the original matrix is the identity")
  {
    // Arrange
    const auto sut = matrix2{
      4, 7,
      2, 6
    };

    // Act
    const auto result = sut.inverse() * sut;

    // Assert
    REQUIRE( almost_equal(result, matrix2_constants::identity) );
  }

  SECTION("Singular matrix returns the identity")
  {
    // Arrange
    const auto sut = matrix2{
      1, 2,
      2, 4
    };

    // Act
    const auto result = sut.inverse();

    // Assert
    REQUIRE( result == matrix2_constants::identity );
  }
}

TEST_CASE("matrix2::transposed()", "[quantifiers]")
{
  // Arrange
  const auto sut = matrix2{
    1, 2,
    3, 4
  };
  const auto expected = matrix2{
    1, 3,
    2, 4
  };

  // Act
  const auto result = sut.transposed();

  // Assert
  REQUIRE( result == expected );
}

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

TEST_CASE("operator*( const matrix2&, const matrix2& )", "[arithmetic]")
{
  SECTION("Computes the matrix product")
  {
    // Arrange
    const auto lhs = matrix2{
      1, 2,
      3, 4
    };
    const auto rhs = matrix2{
      5, 6,
      7, 8
    };
    const auto expected = matrix2{
      19, 22,
      43, 50
    };

    // Act
    const auto result = lhs * rhs;

    // Assert
    REQUIRE( result == expected );
  }

  SECTION("Product applies the right matrix first")
  {
    // Arrange
    const auto rotate = matrix2{
      0, -1,
      1,  0
    };
    const auto scale_x = matrix2{
      2, 0,
      0, 1
    };
    const auto vec = vector2{1,0};

    // Act
    const auto result = (rotate * scale_x) * vec;

    // Assert
    REQUIRE( result == vector2{0,2} );
  }
}

TEST_CASE("operator*( const matrix2&, const vector2& )", "[arithmetic]")
{
  // Arrange
  const auto lhs = matrix2{
    1, 2,
    3, 4
  };
  const auto rhs = vector2{1,2};

  // Act
  const auto result = lhs * rhs;

  // Assert
  REQUIRE( result == vector2{5,11} );
}

TEST_CASE("operator*( const vector2&, const matrix2& )", "[arithmetic]")
{
  // Arrange
  const auto lhs = vector2{1,2};
  const auto rhs = matrix2{
    1, 2,
    3, 4
  };

  // Act
  const auto result = lhs * rhs;

  // Assert
  REQUIRE( result == vector2{7,10} );
}

} // namespace alloy::core::test
