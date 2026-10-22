// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/matrix/matrix4.hpp>

#include <alloy/core/math/vector/vector4.hpp>

#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm> // std::copy_n, std::ranges::equal
#include <span>      // std::span

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("matrix4::matrix4( const real(&)[16] )", "[ctor]")
{
  // Arrange
  const real array[16] = {
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

TEST_CASE("matrix4::matrix4( const real(&)[4][4] )", "[ctor]")
{
  // Arrange
  const real array[4][4] = {
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
    REQUIRE( result == real{4} );
  }
}

TEST_CASE("matrix4::operator[]( index_type )", "[element access]")
{
  // Arrange
  const auto sut = matrix4{
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16
  };

  // Act
  const auto* const column = sut[1];

  // Assert
  SECTION("Retrieves the column at the index")
  {
    REQUIRE( column[0] == real{2} );
    REQUIRE( column[1] == real{6} );
    REQUIRE( column[2] == real{10} );
    REQUIRE( column[3] == real{14} );
  }
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
    const real expected[16] = {
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
    real array[16] = {};
    std::copy_n(sut.data(), 16, array);

    // Act
    const auto result = matrix4{array};

    // Assert
    REQUIRE( result == sut );
  }
}

} // namespace alloy::core::test
