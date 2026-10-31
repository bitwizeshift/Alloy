// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/matrix/matrix4.hpp>

#include <alloy/core/config.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/matrix/matrix4_constants.hpp>
#include <alloy/core/math/vector/vector4.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm> // std::copy_n, std::ranges::equal
#include <array>     // std::array
#include <span>      // std::span
#include <stdexcept> // std::out_of_range

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("matrix4::matrix4( const float(&)[16] )", "[ctor]")
{
  // Arrange
  const float array[16] = {
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16
  };

  // Act
  const auto sut = matrix4{array};

  // Assert
  SECTION("Reads the array column by column")
  {
    REQUIRE( sut.column(1) == vector4{5, 6, 7, 8} );
  }
}

TEST_CASE("matrix4::matrix4( const float(&)[4][4] )", "[ctor]")
{
  // Arrange
  const float array[4][4] = {
    { 1,  2,  3,  4},
    { 5,  6,  7,  8},
    { 9, 10, 11, 12},
    {13, 14, 15, 16}
  };

  // Act
  const auto sut = matrix4{array};

  // Assert
  SECTION("Indexes the array by column, then row")
  {
    REQUIRE( sut.get(2,1) == array[1][2] );
  }
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

TEST_CASE("matrix4::at( index_type, index_type )", "[element access]")
{
  // Arrange
  const auto sut = matrix4{
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16
  };

  // Act
  const auto result = sut.at(0,3);

  // Assert
  SECTION("Retrieves the entry at the row and column")
  {
    REQUIRE( result == 4.0f );
  }
}

#if ALLOY_CORE_EXCEPTIONS_ENABLED
TEST_CASE("matrix4::at( index_type, index_type ) out of range", "[element access]")
{
  // Arrange
  const auto sut = matrix4{};

  SECTION("Row >= rows")
  {
    // Arrange
    auto threw_expected = false;

    // Act
    try {
      sut.at(4,0);
    } catch (const std::out_of_range&) {
      threw_expected = true;
    }

    // Assert
    REQUIRE(threw_expected);
  }
  SECTION("Column < 0")
  {
    // Arrange
    auto threw_expected = false;

    // Act
    try {
      sut.at(0,-1);
    } catch (const std::out_of_range&) {
      threw_expected = true;
    }

    // Assert
    REQUIRE(threw_expected);
  }
}
#endif // ALLOY_CORE_EXCEPTIONS_ENABLED

TEST_CASE("matrix4::operator[]( index_type )", "[element access]")
{
  // Arrange
  const auto sut = matrix4{
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16
  };

  const auto expected = std::array<float,4>{2, 6, 10, 14};

  // Act
  const auto* const column = sut[1];

  // Assert
  REQUIRE( std::ranges::equal(
    std::span{column, 4u}, expected
  ) );
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

TEST_CASE("matrix4::data()", "[observers]")
{
  SECTION("Stores the entries column by column")
  {
    // Arrange
    const auto sut = matrix4{
      1, 0, 0, 5,
      0, 1, 0, 6,
      0, 0, 1, 7,
      0, 0, 0, 1
    };
    const float expected[16] = {
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      5, 6, 7, 1
    };

    // Act
    const auto* const result = sut.data();

    // Assert
    REQUIRE( std::ranges::equal(std::span{result, 16}, expected) );
  }

  SECTION("Round-trips through the array constructor")
  {
    // Arrange
    const auto sut = matrix4{
       1,  2,  3,  4,
       5,  6,  7,  8,
       9, 10, 11, 12,
      13, 14, 15, 16
    };
    float array[16] = {};

    // Act
    std::copy_n(sut.data(), 16, array);

    // Act
    const auto result = matrix4{array};

    // Assert
    REQUIRE( result == sut );
  }
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

TEST_CASE("matrix4::determinant()", "[quantifiers]")
{
  SECTION("Determinant of a diagonal matrix is the product of its diagonal")
  {
    // Arrange
    const auto sut = matrix4{
      1, 0, 0, 0,
      0, 2, 0, 0,
      0, 0, 3, 0,
      0, 0, 0, 4
    };

    // Act
    const auto result = sut.determinant();

    // Assert
    REQUIRE( almost_equal(result, 24.0f) );
  }

  SECTION("Determinant of a general matrix")
  {
    // Arrange
    const auto sut = matrix4{
       1,  1,  1, -1,
       1,  1, -1,  1,
       1, -1,  1,  1,
      -1,  1,  1,  1
    };

    // Act
    const auto result = sut.determinant();

    // Assert
    REQUIRE( almost_equal(result, -16.0f) );
  }
}

TEST_CASE("matrix4::trace()", "[quantifiers]")
{
  // Arrange
  const auto sut = matrix4{
    1, 9, 9, 9,
    9, 2, 9, 9,
    9, 9, 3, 9,
    9, 9, 9, 4
  };

  // Act
  const auto result = sut.trace();

  // Assert
  REQUIRE( almost_equal(result, 10.0f) );
}

TEST_CASE("matrix4::inverse()", "[quantifiers]")
{
  SECTION("Inverts an affine transform")
  {
    // Arrange
    const auto sut = matrix4{
      2, 0, 0, 1,
      0, 2, 0, 2,
      0, 0, 2, 3,
      0, 0, 0, 1
    };
    const auto expected = matrix4{
      0.5f, 0,    0,    -0.5f,
      0,    0.5f, 0,    -1,
      0,    0,    0.5f, -1.5f,
      0,    0,    0,     1
    };

    // Act
    const auto result = sut.inverse();

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Inverts a general matrix")
  {
    // Arrange
    const auto sut = matrix4{
       1,  1,  1, -1,
       1,  1, -1,  1,
       1, -1,  1,  1,
      -1,  1,  1,  1
    };
    const auto expected = sut * 0.25f;

    // Act
    const auto result = sut.inverse();

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Product with the original matrix is the identity")
  {
    // Arrange
    const auto sut = matrix4{
      4, 0, 0, 0,
      0, 0, 2, 0,
      0, 1, 2, 0,
      1, 0, 0, 1
    };

    // Act
    const auto result = sut * sut.inverse();

    // Assert
    REQUIRE( almost_equal(result, matrix4_constants::identity) );
  }

  SECTION("Singular matrix returns the identity")
  {
    // Arrange
    const auto sut = matrix4{
      1, 2, 3, 4,
      2, 4, 6, 8,
      0, 1, 0, 1,
      1, 0, 1, 0
    };

    // Act
    const auto result = sut.inverse();

    // Assert
    REQUIRE( result == matrix4_constants::identity );
  }
}

TEST_CASE("matrix4::transposed()", "[quantifiers]")
{
  // Arrange
  const auto sut = matrix4{
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16
  };
  const auto expected = matrix4{
    1, 5,  9, 13,
    2, 6, 10, 14,
    3, 7, 11, 15,
    4, 8, 12, 16
  };

  // Act
  const auto result = sut.transposed();

  // Assert
  REQUIRE( result == expected );
}

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

TEST_CASE("operator*( const matrix4&, const matrix4& )", "[arithmetic]")
{
  SECTION("Identity is the multiplicative identity")
  {
    // Arrange
    const auto sut = matrix4{
       1,  2,  3,  4,
       5,  6,  7,  8,
       9, 10, 11, 12,
      13, 14, 15, 16
    };

    // Act
    const auto result = matrix4_constants::identity * sut;

    // Assert
    REQUIRE( result == sut );
  }

  SECTION("Product applies the right matrix first")
  {
    // Arrange
    const auto translate = matrix4{
      1, 0, 0, 5,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
    };
    const auto scale = matrix4{
      2, 0, 0, 0,
      0, 2, 0, 0,
      0, 0, 2, 0,
      0, 0, 0, 1
    };
    const auto point = vector4{1, 0, 0, 1};

    // Act
    const auto result = (translate * scale) * point;

    // Assert
    REQUIRE( result == vector4{7, 0, 0, 1} );
  }
}

TEST_CASE("operator*( const matrix4&, float )", "[arithmetic]")
{
  // Arrange
  const auto sut = matrix4_constants::identity;

  const auto expected = matrix4{
    3, 0, 0, 0,
    0, 3, 0, 0,
    0, 0, 3, 0,
    0, 0, 0, 3
  };

  // Act
  const auto result = sut * 3.0f;

  // Assert
  REQUIRE( result == expected );
}

TEST_CASE("matrix4::operator/=( float )", "[arithmetic]")
{
  // Arrange
  auto sut = matrix4_constants::identity * 4.0f;

  // Act
  sut /= 2.0f;

  // Assert
  REQUIRE( sut == matrix4_constants::identity * 2.0f );
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

TEST_CASE("almost_equal( const matrix4&, const matrix4&, float )", "[comparison]")
{
  // Arrange
  const auto lhs = matrix4_constants::identity;
  auto rhs = matrix4_constants::identity;
  rhs.get(3,0) = 0.05f;

  SECTION("Matrices within tolerance are almost equal")
  {
    // Act
    const auto result = almost_equal(lhs, rhs, 0.1f);

    // Assert
    REQUIRE( result );
  }
  SECTION("Matrices outside tolerance are not almost equal")
  {
    // Act
    const auto result = almost_equal(lhs, rhs, 0.01f);

    // Assert
    REQUIRE_FALSE( result );
  }
}

} // namespace alloy::core::test
