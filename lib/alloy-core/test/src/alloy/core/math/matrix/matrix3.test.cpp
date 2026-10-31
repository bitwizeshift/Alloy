// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/matrix/matrix3.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/math/matrix/matrix3_constants.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm> // std::ranges::equal
#include <array>     // std::array
#include <span>      // std::span

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

TEST_CASE("matrix3::operator[]( index_type )", "[element access]")
{
  // Arrange
  const auto sut = matrix3{
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
  };

  const auto expected = std::array<real,3>{3, 6, 9};

  // Act
  const auto* const column = sut[2];

  // Assert
  REQUIRE( std::ranges::equal(
    std::span{column, 3u}, expected
  ) );
}

TEST_CASE("matrix3::data()", "[observers]")
{
  // Arrange
  const auto sut = matrix3{
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
  };
  const real expected[9] = {1, 4, 7, 2, 5, 8, 3, 6, 9};

  // Act
  const auto* const result = sut.data();

  // Assert
  SECTION("Stores the entries column by column")
  {
    REQUIRE( std::ranges::equal(std::span{result, 9}, expected) );
  }
}

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

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

TEST_CASE("operator*( const matrix3&, const matrix3& )", "[arithmetic]")
{
  // Arrange
  const auto translate = matrix3{
    1, 0, 5,
    0, 1, 0,
    0, 0, 1
  };
  const auto scale = matrix3{
    2, 0, 0,
    0, 2, 0,
    0, 0, 1
  };
  const auto point = vector3{1,0,1};

  // Act
  const auto result = (translate * scale) * point;

  // Assert
  SECTION("Product applies the right matrix first")
  {
    REQUIRE( result == vector3{7,0,1} );
  }
}

TEST_CASE("operator*( const matrix3&, const vector3& )", "[arithmetic]")
{
  // Arrange
  const auto translate = matrix3{
    1, 0, 5,
    0, 1, 3,
    0, 0, 1
  };
  const auto point = vector3{1,2,1};

  // Act
  const auto result = translate * point;

  // Assert
  SECTION("Translation is read from the last column")
  {
    REQUIRE( result == vector3{6,5,1} );
  }
}

TEST_CASE("operator*( const vector3&, const matrix3& )", "[arithmetic]")
{
  // Arrange
  const auto lhs = vector3{1,2,3};
  const auto rhs = matrix3{
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
  };

  // Act
  const auto result = lhs * rhs;

  // Assert
  REQUIRE( result == vector3{30,36,42} );
}

} // namespace alloy::core::test
