// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/vector/vector4.hpp>

#include <alloy/core/config.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector4_constants.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>     // std::array
#include <stdexcept>

namespace alloy::core::test {

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("vector4::vector4()", "[ctor]")
{
  // Act
  const auto sut = vector4{};

  // Assert
  SECTION("Every component is 0")
  {
    REQUIRE( sut == vector4_constants::zero );
  }
}

TEST_CASE("vector4::vector4( float, float, float, float )", "[ctor]")
{
  // Act
  const auto sut = vector4{1,2,3,4};

  // Assert
  SECTION("x contains specified x value")
  {
    REQUIRE( sut.x() == 1.0f );
  }
  SECTION("y contains specified y value")
  {
    REQUIRE( sut.y() == 2.0f );
  }
  SECTION("z contains specified z value")
  {
    REQUIRE( sut.z() == 3.0f );
  }
  SECTION("w contains specified w value")
  {
    REQUIRE( sut.w() == 4.0f );
  }
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

#if ALLOY_CORE_EXCEPTIONS_ENABLED
TEST_CASE("vector4::at( index_type )", "[observers]")
{
  // Arrange
  const auto sut = vector4{1,2,3,4};

  SECTION("Index < 0 throws")
  {
    // Arrange
    auto threw_expected = false;

    // Act
    try {
      sut.at(-1);
    } catch (const std::out_of_range&) {
      threw_expected = true;
    }

    // Assert
    REQUIRE(threw_expected);
  }
  SECTION("Index >= size() throws")
  {
    // Arrange
    auto threw_expected = false;

    // Act
    try {
      sut.at(sut.size());
    } catch (const std::out_of_range&) {
      threw_expected = true;
    }

    // Assert
    REQUIRE(threw_expected);
  }
  SECTION("Index 3 retrieves w()")
  {
    // Act
    const auto result = sut.at(3);

    // Assert
    REQUIRE( result == sut.w() );
  }
}
#endif // ALLOY_CORE_EXCEPTIONS_ENABLED

TEST_CASE("get<N>( vector4 )", "[observers]")
{
  // Arrange
  const auto sut = vector4{1,2,3,4};

  const auto expected = std::array{1.0f, 2.0f, 3.0f, 4.0f};

  // Act
  const auto [x, y, z, w] = sut;

  const auto result = std::array{x, y, z, w};

  // Assert
  REQUIRE( result == expected );
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

TEST_CASE("vector4::dot( vector4 )", "[quantifiers]")
{
  // Arrange
  const auto sut = vector4{1,2,3,4};
  const auto other = vector4{5,6,7,8};

  // Act
  const auto result = sut.dot(other);

  // Assert
  SECTION("Sums the products of each pair of components")
  {
    REQUIRE( result == 70.0f );
  }
}

TEST_CASE("vector4::cross( vector4 )", "[quantifiers]")
{
  SECTION("Crosses the xyz components")
  {
    // Arrange
    const auto sut = vector4_constants::unit_x;
    const auto other = vector4_constants::unit_y;

    // Act
    const auto result = sut.cross(other);

    // Assert
    REQUIRE( result == vector4_constants::unit_z );
  }

  SECTION("Clears the w component")
  {
    // Arrange
    const auto sut = vector4{1,0,0,5};
    const auto other = vector4{0,1,0,7};

    // Act
    const auto result = sut.cross(other);

    // Assert
    REQUIRE( result.w() == 0.0f );
  }
}

TEST_CASE("vector4::magnitude()", "[quantifiers]")
{
  // Arrange
  const auto sut = vector4{1,1,1,1};

  // Act
  const auto result = sut.magnitude();

  // Assert
  SECTION("Is the square root of the sum of the squared components")
  {
    REQUIRE( almost_equal(result, 2.0f) );
  }
}

TEST_CASE("vector4::midpoint( const vector4& )", "[quantifiers]")
{
  // Arrange
  const auto sut = vector4{0,2,4,6};
  const auto other = vector4{2,4,6,8};

  // Act
  const auto result = sut.midpoint(other);

  // Assert
  SECTION("Averages each pair of components")
  {
    REQUIRE( result == vector4{1,3,5,7} );
  }
}

TEST_CASE("vector4::projection( const vector4& )", "[quantifiers]")
{
  SECTION("Vector is perpendicular to the projected vector")
  {
    // Arrange
    const auto sut = vector4{1,-1,0,0};
    const auto onto = vector4{1,1,1,1};

    // Act
    const auto result = sut.projection(onto);

    // Assert
    REQUIRE( almost_equal(result, vector4_constants::zero) );
  }

  SECTION("Vector projects onto a non-unit vector")
  {
    // Arrange
    const auto sut = vector4{3,4,5,6};
    const auto onto = vector4{0,0,0,2};

    // Act
    const auto result = sut.projection(onto);

    // Assert
    REQUIRE( almost_equal(result, vector4{0,0,0,6}) );
  }
}

TEST_CASE("vector4::rejection( const vector4& )", "[quantifiers]")
{
  // Arrange
  const auto sut = vector4{3,4,5,6};
  const auto from = vector4{0,0,0,2};

  // Act
  const auto result = sut.rejection(from);

  // Assert
  SECTION("Removes the component along the rejected vector")
  {
    REQUIRE( almost_equal(result, vector4{3,4,5,0}) );
  }
}

TEST_CASE("vector4::inverse()", "[quantifiers]")
{
  // Arrange
  const auto sut = vector4{1,-2,3,-4};

  // Act
  const auto result = sut.inverse();

  // Assert
  SECTION("Negates every component")
  {
    REQUIRE( result == vector4{-1,2,-3,4} );
  }
}

TEST_CASE("vector4::is_normalized()", "[quantifiers]")
{
  SECTION("Unit vector is normalized")
  {
    // Arrange
    const auto sut = vector4_constants::unit_w;

    // Act
    const auto result = sut.is_normalized();

    // Assert
    REQUIRE( result );
  }

  SECTION("Non-unit vector is not normalized")
  {
    // Arrange
    const auto sut = vector4{1,1,0,0};

    // Act
    const auto result = sut.is_normalized();

    // Assert
    REQUIRE_FALSE( result );
  }
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

TEST_CASE("vector4::normalize()", "[modifiers]")
{
  SECTION("Non-unit vector becomes unit length")
  {
    // Arrange
    auto sut = vector4{2,0,0,2};

    // Act
    sut.normalize();

    // Assert
    REQUIRE( sut.is_normalized() );
  }

  SECTION("Direction is preserved")
  {
    // Arrange
    auto sut = vector4{0,0,0,5};

    // Act
    sut.normalize();

    // Assert
    REQUIRE( almost_equal(sut, vector4_constants::unit_w) );
  }

  SECTION("Zero vector is left unchanged")
  {
    // Arrange
    auto sut = vector4_constants::zero;

    // Act
    sut.normalize();

    // Assert
    REQUIRE( sut == vector4_constants::zero );
  }
}

//-----------------------------------------------------------------------------
// Binary Operators
//-----------------------------------------------------------------------------

TEST_CASE("operator+( vector4, vector4 )", "[arithmetic]")
{
  // Arrange
  const auto lhs = vector4{1,2,3,4};
  const auto rhs = vector4{4,3,2,1};

  // Act
  const auto result = lhs + rhs;

  // Assert
  SECTION("Adds each pair of components")
  {
    REQUIRE( result == vector4{5,5,5,5} );
  }
}

TEST_CASE("operator-( vector4, vector4 )", "[arithmetic]")
{
  // Arrange
  const auto lhs = vector4{1,2,3,4};
  const auto rhs = vector4{4,3,2,1};

  // Act
  const auto result = lhs - rhs;

  SECTION("Subtracts each pair of components")
  {
    // Act
    const auto actual = result;
    const auto equivalent = vector4{-3,-1,1,3};

    // Assert
    REQUIRE(actual == equivalent);
  }
}

TEST_CASE("operator*( vector4, float )", "[arithmetic]")
{
  // Arrange
  const auto lhs = vector4{1,2,3,4};

  // Act
  const auto result = lhs * 2.0f;

  // Assert
  SECTION("Scales every component")
  {
    REQUIRE( result == vector4{2,4,6,8} );
  }
}

TEST_CASE("operator/( vector4, float )", "[arithmetic]")
{
  // Arrange
  const auto lhs = vector4{2,4,6,8};

  // Act
  const auto result = lhs / 2.0f;

  // Assert
  SECTION("Divides every component")
  {
    REQUIRE( result == vector4{1,2,3,4} );
  }
}

//-----------------------------------------------------------------------------
// Comparisons
//-----------------------------------------------------------------------------

TEST_CASE("almost_equal( vector4, vector4, float )", "[comparison]")
{
  // Arrange
  const auto lhs = vector4{1,2,3,4};

  SECTION("Vectors within tolerance are almost equal")
  {
    // Arrange
    const auto rhs = vector4{1,2,3,4.05f};

    // Act
    const auto result = almost_equal(lhs, rhs, 0.1f);

    // Assert
    REQUIRE( result );
  }

  SECTION("Vectors outside tolerance are not almost equal")
  {
    // Arrange
    const auto rhs = vector4{1,2,3,4.5f};

    // Act
    const auto result = almost_equal(lhs, rhs, 0.1f);

    // Assert
    REQUIRE_FALSE( result );
  }
}

} // namespace alloy::core::test
