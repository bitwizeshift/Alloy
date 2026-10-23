// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/vector/vector2.hpp>

#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector2_constants.hpp>

#include <alloy/core/config.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

#include <limits>
#include <stdexcept>

namespace alloy::core::test {

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("vector2::vector2( real, real )", "[ctor]")
{
  // Arrange
  const auto x = 3.14f;
  const auto y = 1.57f;

  // Act
  const auto sut = alloy::core::vector2{x,y};

  // Assert
  SECTION("Initializes x and y components")
  {
    SECTION("x contains specified x value")
    {
      REQUIRE( sut.x() == x );
    }
    SECTION("y contains specified y value")
    {
      REQUIRE( sut.y() == y );
    }
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("vector2::size()", "[observers]")
{
  // Arrange
  const auto sut = alloy::core::vector2{0,0};

  SECTION("Has size 2")
  {
    // Act
    const auto result = sut.size();

    // Assert
    REQUIRE(result == 2);
  }
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

#if ALLOY_CORE_EXCEPTIONS_ENABLED
TEST_CASE("vector2::at( index_type )", "[observers]")
{
  // Arrange
  const auto sut = alloy::core::vector2{42,1024};

  SECTION("Throws when out of range")
  {
    SECTION("Index < 0")
    {
      // Arrange
      auto threw_expected = false;

      // Act
      try {
        sut.at(-1);
      } catch (const std::logic_error&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
    SECTION("Index > size()")
    {
      // Arrange
      auto threw_expected = false;

      // Act
      try {
        sut.at(sut.size());
      } catch (const std::logic_error&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
  SECTION("Retrieves entry at index")
  {
    SECTION("Index 0 retrieves x()")
    {
      // Act
      const auto result = sut.at(0);

      // Assert
      REQUIRE(result == sut.x());
    }
    SECTION("Index 1 retrieves y()")
    {
      // Act
      const auto result = sut.at(1);

      // Assert
      REQUIRE(result == sut.y());
    }
  }
}
#endif // ALLOY_CORE_EXCEPTIONS_ENABLED

TEST_CASE("vector2::operator[]( index_type )", "[observers]")
{
  // Arrange
  const auto sut = alloy::core::vector2{42,1024};

  SECTION("Retrieves entry at index")
  {
    SECTION("Index 0 retrieves x()")
    {
      // Act
      const auto result = sut[0];

      // Assert
      REQUIRE(result == sut.x());
    }
    SECTION("Index 1 retrieves y()")
    {
      // Act
      const auto result = sut[1];

      // Assert
      REQUIRE(result == sut.y());
    }
  }
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

TEST_CASE("vector2::dot( vector2 )", "[quantifiers]")
{
  // Arrange
  const auto vec1 = alloy::core::vector2{ 1.0f, 2.0f };
  const auto vec2 = alloy::core::vector2{ 2.0f, 1.0f };

  SECTION("Dot product returns sum of multiples")
  {
    // Act
    const auto result = vec1.dot(vec2);

    // Assert
    REQUIRE(result == 4.0f);
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::cross( vector2 )", "[quantifiers]")
{
  SECTION("Cross product is anti-commutative")
  {
    // Arrange
    const auto a = alloy::core::vector2{ 1.0f, 2.0f };
    const auto b = alloy::core::vector2{ 2.0f, 1.0f };

    // a x b == -b x a

    // Act
    const auto result = a.cross(b);
    const auto equivalent = (-b).cross(a);

    // Assert
    REQUIRE(almost_equal(result, equivalent));
  }
  SECTION("Vectors are parallel")
  {
    SECTION("Returns zero")
    {
      // Arrange
      auto vec1 = alloy::core::vector2{ 1.0f, 2.0f };
      auto vec2 = alloy::core::vector2{ 1.0f, 2.0f };

      // Act
      const auto result = vec1.cross(vec2);

      // Assert
      REQUIRE(result == 0.0f);
    }
  }
  SECTION("Vectors are perpendicular")
  {
    // Arrange
    const auto vec1 = alloy::core::vector2{ 1.0f, 0.0f };
    const auto vec2 = alloy::core::vector2{ 0.0f, 1.0f };

    // Act
    const auto result = vec1.cross(vec2);

    // Assert
    SECTION("Returns nonzero")
    {
      REQUIRE( result != 0.0f );
    }
    SECTION("Magnitude of cross-product is product of magnitudes")
    {
      const auto lhs = result;
      const auto rhs = vec1.magnitude() * vec2.magnitude();

      REQUIRE( almost_equal(lhs, rhs) );
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::magnitude()", "[quantifiers]")
{
  SECTION("Vector contains values")
  {
    // pythagorean triple (3,4,5)
    // Arrange
    const auto sut = alloy::core::vector2{ 3.0f, 4.0f };

    SECTION("Returns magnitude of vector")
    {
      // Act
      const auto result = sut.magnitude();

      // Assert
      REQUIRE(almost_equal(result, 5.0f));
    }
  }

  SECTION("Vector contains 0s")
  {
    // Arrange
    const auto sut = alloy::core::vector2{ 0.0f, 0.0f };

    SECTION("Returns 0")
    {
      // Act
      const auto result = sut.magnitude();

      // Assert
      REQUIRE(result == 0.0f);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::midpoint( const vector2& vec )", "[quantifiers]")
{
  // Arrange
  const auto vec1 = alloy::core::vector2{ 0.0, 0.0 };
  const auto vec2 = alloy::core::vector2{ 2.0, 2.0 };

  SECTION("Returns midpoint between two vectors")
  {
    // Act
    const auto result = vec1.midpoint(vec2);

    // Assert
    REQUIRE( almost_equal(result, alloy::core::vector2{1.0,1.0}) );
  }
  SECTION("vector2::midpoint is commutative")
  {
    // Act
    const auto r1 = vec1.midpoint(vec2);
    const auto r2 = vec2.midpoint(vec1);

    // Assert
    REQUIRE( almost_equal(r1,r2) );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::reflection( const vector2& vec )", "[quantifiers]")
{
  SECTION("Reflects over x-axis")
  {
    // Arrange
    const auto x_axis = alloy::core::vector2_constants::unit_y;
    const auto vector = alloy::core::vector2{1.0,1.0};

    // Act
    const auto result = vector.reflection( x_axis );

    // Assert
    REQUIRE( almost_equal( result, alloy::core::vector2{1.0,-1.0}) );
  }

  SECTION("Reflects over y-axis")
  {
    // Arrange
    const auto x_axis = alloy::core::vector2_constants::unit_x;
    const auto vector = alloy::core::vector2{1.0,1.0};

    // Act
    const auto result = vector.reflection( x_axis );

    // Assert
    REQUIRE( almost_equal( result, alloy::core::vector2{-1.0,1.0}) );
  }

  SECTION("Reflects over arbitrary axis")
  {
    // Arrange
    const auto axis   = alloy::core::vector2{ 1.0, 1.0 };
    const auto vector = alloy::core::vector2{ 1.0, 0.0 };

    // Act
    const auto result = vector.reflection( axis );

    // Assert
    REQUIRE( almost_equal(result, alloy::core::vector2{-1.0,-2.0}) );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::projection( const vector2& vec )", "[quantifiers]")
{
  SECTION("Vector is parallel to the projected vector")
  {
    // Arrange
    const auto sut = vector2{2,2};
    const auto onto = vector2{1,1};

    // Act
    const auto result = sut.projection(onto);

    // Assert
    REQUIRE( almost_equal(result, sut) );
  }

  SECTION("Vector is perpendicular to the projected vector")
  {
    // Arrange
    const auto sut = vector2{1,-1};
    const auto onto = vector2{1,1};

    // Act
    const auto result = sut.projection(onto);

    // Assert
    REQUIRE( almost_equal(result, vector2_constants::zero) );
  }

  SECTION("Vector projects onto a unit axis")
  {
    // Arrange
    const auto sut = vector2{2,3};
    const auto onto = vector2_constants::unit_x;

    // Act
    const auto result = sut.projection(onto);

    // Assert
    REQUIRE( almost_equal(result, vector2{2,0}) );
  }

  SECTION("Vector projects onto a non-unit vector")
  {
    // Arrange
    const auto sut = vector2{3,4};
    const auto onto = vector2{0,2};

    // Act
    const auto result = sut.projection(onto);

    // Assert
    REQUIRE( almost_equal(result, vector2{0,4}) );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::rejection( const vector2& vec )", "[quantifiers]")
{
  SECTION("Vector is parallel to the rejected vector")
  {
    // Arrange
    const auto sut = vector2{2,2};
    const auto from = vector2{1,1};

    // Act
    const auto result = sut.rejection(from);

    // Assert
    REQUIRE( almost_equal(result, vector2_constants::zero) );
  }

  SECTION("Vector is perpendicular to the rejected vector")
  {
    // Arrange
    const auto sut = vector2{1,-1};
    const auto from = vector2{1,1};

    // Act
    const auto result = sut.rejection(from);

    // Assert
    REQUIRE( almost_equal(result, sut) );
  }

  SECTION("Vector is rejected from a non-unit vector")
  {
    // Arrange
    const auto sut = vector2{3,4};
    const auto from = vector2{0,2};

    // Act
    const auto result = sut.rejection(from);

    // Assert
    REQUIRE( almost_equal(result, vector2{3,0}) );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::perpendicular()", "[quantifiers]")
{
  // TODO(bitwizeshift): test this
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::normalized()", "[quantifiers]")
{
  // Untested (same as normalize)
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::inverse()", "[quantifiers]")
{
  // Untested (same as invert)
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::angle_between( const vector2& )", "[quantifiers]")
{
  SECTION("Vectors are parallel")
  {
    // Arrange
    const auto vec1 = alloy::core::vector2_constants::unit_x;
    const auto vec2 = alloy::core::vector2_constants::unit_x;

    const auto expected = alloy::core::radian{0};

    // Act
    const auto angle = vec1.angle_between( vec2 );

    // Assert
    REQUIRE( almost_equal(angle, expected) );
  }

  SECTION("Vectors are perpendicular")
  {
    // Arrange
    const auto vec1 = alloy::core::vector2_constants::unit_x;
    const auto vec2 = alloy::core::vector2_constants::unit_y;

    const auto expected = alloy::core::radian_constants::quarter_revolution;

    // Act
    const auto angle = vec1.angle_between( vec2 );

    // Assert
    REQUIRE( almost_equal(angle,expected) );
  }

  SECTION("Vectors are opposite")
  {
    // Arrange
    const auto vec1 = alloy::core::vector2_constants::unit_x;
    const auto vec2 = -vec1;

    const auto expected = alloy::core::radian_constants::half_revolution;

    // Act
    const auto angle = vec1.angle_between( vec2 );

    // Assert
    REQUIRE( almost_equal(angle,expected) );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::angle_to( const vector2& )", "[quantifiers]")
{
  // TODO(bitwizeshift): test this
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

TEST_CASE("vector2::normalize()", "[modifiers]")
{
  SECTION("Vector contains only zeros")
  {
    // Arrange
    auto sut = alloy::core::vector2{0.0f, 0.0f};
    const auto expected = alloy::core::vector2{0.0f,0.0f};

    // Act
    sut.normalize();

    // Assert
    SECTION("Returns zero vector")
    {
      REQUIRE( sut == expected );
    }
  }

  SECTION("Vector is already normalized")
  {
    // Arrange
    auto sut = alloy::core::vector2{
      alloy::core::real{1} / alloy::core::sqrt(2.0f),
      alloy::core::real{1} / alloy::core::sqrt(2.0f)
    };
    const auto expected = sut;

    // Act
    sut.normalize();

    // Assert
    SECTION("Does not alter vector")
    {
      REQUIRE( almost_equal(sut,expected) );
    }
  }

  SECTION("Vector is not normalized")
  {
    // Arrange
    auto sut = alloy::core::vector2{ 1.0f, 1.0f };
    const auto expected = alloy::core::vector2{
      alloy::core::real{1} / alloy::core::sqrt(2.0f),
      alloy::core::real{1} / alloy::core::sqrt(2.0f)
    };

    // Act
    sut.normalize();

    // Assert
    SECTION("Normalizes vector")
    {
      REQUIRE( almost_equal(sut, expected) );
    }
  }

  SECTION("Vector contains near-0 values")
  {
    // Arrange
    const auto epsilon = std::numeric_limits<float>::epsilon();
    auto sut = alloy::core::vector2{epsilon,0.0f};
    const auto expected = alloy::core::vector2{1.0f,0.0f};

    // Act
    sut.normalize();

    // Assert
    SECTION("Normalizes to unit vector")
    {
      REQUIRE( almost_equal(sut, expected) );
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::invert()", "[modifiers]")
{
  SECTION("Vector contains only zeros")
  {
    // Arrange
    auto sut = alloy::core::vector2{0.0f, 0.0f};
    const auto expected = sut;

    // Act
    sut.invert();

    // Assert
    SECTION("Does not alter vector")
    {
      REQUIRE( sut == expected );
    }
  }
  SECTION("Vector contains values")
  {
    // Arrange
    auto sut = alloy::core::vector2{1.0f, 2.0f};
    const auto expected = alloy::core::vector2{-1.0f, -2.0f};

    // Act
    sut.invert();

    // Assert
    SECTION("Inverts the vector")
    {
      REQUIRE( sut == expected );
    }
  }
}

//==============================================================================
// Mathematical Properties
//==============================================================================

TEST_CASE("operator+( vector2, vector2 )", "[arithmetic]")
{
  SECTION("Adds values piecewise")
  {
    // Arrange
    const auto lhs = alloy::core::vector2{1,2};
    const auto rhs = alloy::core::vector2{2,1};
    const auto result = alloy::core::vector2{3,3};

    // Act
    const auto actual = (lhs + rhs);
    const auto equivalent = result;

    // Assert
    REQUIRE(actual == equivalent);
  }
  SECTION("Is commutative")
  {
    // Arrange
    const auto a = alloy::core::vector2{1,2};
    const auto b = alloy::core::vector2{4,5};

    // Act
    const auto actual = (a + b);
    const auto equivalent = (b + a);

    // Assert
    REQUIRE(actual == equivalent);
  }
  SECTION("Is associative")
  {
    // Arrange
    const auto a = alloy::core::vector2{1,2};
    const auto b = alloy::core::vector2{4,5};
    const auto c = alloy::core::vector2{8,9};

    // Act
    const auto actual = ((a + b) + c);
    const auto equivalent = (a + (b + c));

    // Assert
    REQUIRE(actual == equivalent);
  }
  SECTION("Contains identity")
  {
    // Arrange
    const auto a = alloy::core::vector2{1,2};
    const auto identity = alloy::core::vector2{0,0};

    // Act
    const auto actual = (a + identity);
    const auto equivalent = (a);

    // Assert
    REQUIRE(actual == equivalent);
  }
}

TEST_CASE("operator-( vector2, vector2 )", "[arithmetic]")
{
  SECTION("Subtracts values piecewise")
  {
    // Arrange
    const auto lhs = alloy::core::vector2{1,2};
    const auto rhs = alloy::core::vector2{2,1};
    const auto result = alloy::core::vector2{-1,1};

    // Act
    const auto actual = (lhs - rhs);
    const auto equivalent = result;

    // Assert
    REQUIRE(actual == equivalent);
  }
  SECTION("Contains identity")
  {
    // Arrange
    const auto a = alloy::core::vector2{1,2};
    const auto identity = alloy::core::vector2{0,0};

    // Act
    const auto actual = (a - identity);
    const auto equivalent = (a);

    // Assert
    REQUIRE(actual == equivalent);
  }
}

TEST_CASE("operator*( real, vector2 )", "[arithmetic]")
{
  SECTION("Is distributive")
  {
    // Arrange
    const auto constant = alloy::core::real{2};
    const auto a = alloy::core::vector2{1,2};
    const auto b = alloy::core::vector2{2,1};

    // Act
    const auto lhs = (constant * (a + b));
    const auto rhs = (constant * a + constant * b);

    // Assert
    REQUIRE( lhs == rhs );
  }
  SECTION("Contains identity")
  {
    // Arrange
    const auto a = alloy::core::vector2{1,2};
    const auto identity = alloy::core::real{1};

    // Act
    const auto actual = (identity * a);
    const auto equivalent = (a);

    // Assert
    REQUIRE(actual == equivalent);
  }
}

TEST_CASE("operator*( vector2, real )", "[arithmetic]")
{
  SECTION("Is distributive")
  {
    // Arrange
    const auto constant = alloy::core::real{2};
    const auto a = alloy::core::vector2{1,2};
    const auto b = alloy::core::vector2{2,1};

    // Act
    const auto lhs = ((a + b) * constant);
    const auto rhs = (a * constant + b * constant);

    // Assert
    REQUIRE( lhs == rhs );
  }
  SECTION("Contains identity")
  {
    // Arrange
    const auto a = alloy::core::vector2{1,2};
    const auto identity = alloy::core::real{1};

    // Act
    const auto actual = (a * identity);
    const auto equivalent = (a);

    // Assert
    REQUIRE(actual == equivalent);
  }
}
} // namespace alloy::core::test
