// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/matrix/matrix_utilities.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Casting : From matrix2
//------------------------------------------------------------------------------

TEST_CASE("matrix_cast<matrix2>( const matrix2& )", "[casts]")
{
  // Arrange
  const auto sut = matrix2{
    1, 2,
    3, 4
  };

  // Act
  const auto result = matrix_cast<matrix2>(sut);

  // Assert
  SECTION("Returns an equal matrix")
  {
    REQUIRE( result == sut );
  }
}

TEST_CASE("matrix_cast<matrix3>( const matrix2& )", "[casts]")
{
  // Arrange
  const auto sut = matrix2{
    1, 2,
    3, 4
  };
  const auto expected = matrix3{
    1, 2, 0,
    3, 4, 0,
    0, 0, 1
  };

  // Act
  const auto result = matrix_cast<matrix3>(sut);

  // Assert
  SECTION("Copies the entries into the upper-left of an identity matrix")
  {
    REQUIRE( result == expected );
  }
}

TEST_CASE("matrix_cast<matrix4>( const matrix2& )", "[casts]")
{
  // Arrange
  const auto sut = matrix2{
    1, 2,
    3, 4
  };
  const auto expected = matrix4{
    1, 2, 0, 0,
    3, 4, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };

  // Act
  const auto result = matrix_cast<matrix4>(sut);

  // Assert
  SECTION("Copies the entries into the upper-left of an identity matrix")
  {
    REQUIRE( result == expected );
  }
}

//------------------------------------------------------------------------------
// Casting : From matrix3
//------------------------------------------------------------------------------

TEST_CASE("matrix_cast<matrix2>( const matrix3& )", "[casts]")
{
  // Arrange
  const auto sut = matrix3{
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
  };
  const auto expected = matrix2{
    1, 2,
    4, 5
  };

  // Act
  const auto result = matrix_cast<matrix2>(sut);

  // Assert
  SECTION("Truncates to the upper-left entries")
  {
    REQUIRE( result == expected );
  }
}

TEST_CASE("matrix_cast<matrix3>( const matrix3& )", "[casts]")
{
  // Arrange
  const auto sut = matrix3{
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
  };

  // Act
  const auto result = matrix_cast<matrix3>(sut);

  // Assert
  SECTION("Returns an equal matrix")
  {
    REQUIRE( result == sut );
  }
}

TEST_CASE("matrix_cast<matrix4>( const matrix3& )", "[casts]")
{
  // Arrange
  const auto sut = matrix3{
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
  };
  const auto expected = matrix4{
    1, 2, 3, 0,
    4, 5, 6, 0,
    7, 8, 9, 0,
    0, 0, 0, 1
  };

  // Act
  const auto result = matrix_cast<matrix4>(sut);

  // Assert
  SECTION("Copies the entries into the upper-left of an identity matrix")
  {
    REQUIRE( result == expected );
  }
}

//------------------------------------------------------------------------------
// Casting : From matrix4
//------------------------------------------------------------------------------

TEST_CASE("matrix_cast<matrix2>( const matrix4& )", "[casts]")
{
  // Arrange
  const auto sut = matrix4{
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16
  };
  const auto expected = matrix2{
    1, 2,
    5, 6
  };

  // Act
  const auto result = matrix_cast<matrix2>(sut);

  // Assert
  SECTION("Truncates to the upper-left entries")
  {
    REQUIRE( result == expected );
  }
}

TEST_CASE("matrix_cast<matrix3>( const matrix4& )", "[casts]")
{
  // Arrange
  const auto sut = matrix4{
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16
  };
  const auto expected = matrix3{
    1,  2,  3,
    5,  6,  7,
    9, 10, 11
  };

  // Act
  const auto result = matrix_cast<matrix3>(sut);

  // Assert
  SECTION("Truncates to the upper-left entries")
  {
    REQUIRE( result == expected );
  }
}

TEST_CASE("matrix_cast<matrix4>( const matrix4& )", "[casts]")
{
  // Arrange
  const auto sut = matrix4{
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16
  };

  // Act
  const auto result = matrix_cast<matrix4>(sut);

  // Assert
  SECTION("Returns an equal matrix")
  {
    REQUIRE( result == sut );
  }
}

} // namespace alloy::core::test
