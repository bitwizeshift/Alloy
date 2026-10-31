// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/vector/vector3.hpp>

#include <alloy/core/config.hpp>
#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector3_constants.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

#include <limits>
#include <stdexcept>

namespace alloy::core::test {

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("vector3::vector3( real, real, real )", "[ctor]")
{
  // Arrange
  const auto x = 3.14f;
  const auto y = 1.57f;
  const auto z = 0.25f;

  // Act
  const auto sut = vector3{x,y,z};

  // Assert
  SECTION("Initializes x, y, and z components")
  {
    SECTION("x contains specified x value")
    {
      REQUIRE( sut.x() == x );
    }
    SECTION("y contains specified y value")
    {
      REQUIRE( sut.y() == y );
    }
    SECTION("z contains specified z value")
    {
      REQUIRE( sut.z() == z );
    }
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("vector3::size()", "[observers]")
{
  // Arrange
  const auto sut = vector3{0,0,0};

  SECTION("Has size 3")
  {
    // Act
    const auto result = sut.size();

    // Assert
    REQUIRE(result == 3);
  }
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

#if ALLOY_CORE_EXCEPTIONS_ENABLED
TEST_CASE("vector3::at( index_type )", "[observers]")
{
  // Arrange
  const auto sut = vector3{1,42,1024};

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
    SECTION("Index 2 retrieves z()")
    {
      // Act
      const auto result = sut.at(2);

      // Assert
      REQUIRE(result == sut.z());
    }
  }
}
#endif // ALLOY_CORE_EXCEPTIONS_ENABLED

TEST_CASE("vector3::operator[]( index_type )", "[observers]")
{
  // Arrange
  const auto sut = vector3{1,42,1024};

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
    SECTION("Index 2 retrieves z()")
    {
      // Act
      const auto result = sut[2];

      // Assert
      REQUIRE(result == sut.z());
    }
  }
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

TEST_CASE("vector3::dot( vector3 )", "[quantifiers]")
{
  // Arrange
  const auto vec1 = vector3{ 1.0f, 2.0f, 1.0f };
  const auto vec2 = vector3{ 2.0f, 1.0f, 2.0f };

  SECTION("Dot product returns sum of multiples")
  {
    // Act
    const auto result = vec1.dot(vec2);

    // Assert
    REQUIRE(result == 6.0f);
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector3::cross( vector3 )", "[quantifiers]")
{
  SECTION("Cross product is anti-commutative")
  {
    // Arrange
    const auto a = vector3{ 1.0f, 0.0f, 0.0f };
    const auto b = vector3{ 0.0f, 1.0f, 0.0f };

    // a x b == -b x a

    // Act
    const auto result = a.cross(b);
    const auto equivalent = (-b).cross(a);

    // Assert
    REQUIRE(almost_equal(result, equivalent));
  }
  SECTION("Trigonometry product holds")
  {
    // Arrange
    const auto a = vector3{ 1.0f, 0.0f, 0.0f };
    const auto b = vector3{ 0.0f, 1.0f, 0.0f };

    const auto descriminant = (1 - a.dot(b) * a.dot(b));

    // Act
    const auto lhs = (a.cross(b)).magnitude();
    const auto rhs = a.magnitude() * b.magnitude() * sqrt(descriminant);

    // |a x b| == |a||b|*sqrt(1-(a dot b)^2)

    // Assert
    REQUIRE( almost_equal(lhs, rhs) );
  }
  SECTION("Vectors are parallel")
  {
    SECTION("Returns zero vector")
    {
      // Arrange
      const auto vec1 = vector3{ 1.0f, 1.0f, 1.0f };
      const auto vec2 = vector3{ 1.0f, 1.0f, 1.0f };

      const auto expected = vector3{ 0.0f, 0.0f, 0.0f };

      // Act
      const auto result = vec1.cross(vec2);

      // Assert
      REQUIRE( almost_equal(result, expected) );
    }
  }
  SECTION("Vectors are perpendicular")
  {
    SECTION("Magnitude of cross-product is product of magnitudes")
    {
      // Arrange
      const auto vec1 = vector3{ 1.0f, 0.0f, 0.0f };
      const auto vec2 = vector3{ 0.0f, 1.0f, 0.0f };

      // Act
      const auto result = vec1.cross(vec2);

      const auto lhs = result.magnitude();
      const auto rhs = vec1.magnitude() * vec2.magnitude();

      // Assert
      REQUIRE( almost_equal(lhs, rhs) );
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector3::magnitude()", "[quantifiers]")
{
  SECTION("Vector contains values")
  {
    // pythagorean quadruple (1,2,2,3)
    // Arrange
    const auto sut = vector3{ 1.0f, 2.0f, 2.0f };

    SECTION("Returns magnitude of vector")
    {
      // Act
      const auto result = sut.magnitude();

      // Assert
      REQUIRE(almost_equal(result, 3.0f));
    }
  }

  SECTION("Vector contains 0s")
  {
    // Arrange
    const auto sut = vector3{ 0.0f, 0.0f, 0.0f };

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

TEST_CASE("vector3::midpoint( const vector3& vec )", "[quantifiers]")
{
  // Arrange
  const auto vec1 = vector3{ 0.0, 0.0, 0.0 };
  const auto vec2 = vector3{ 2.0, 2.0, 2.0 };

  SECTION("Returns midpoint between two vectors")
  {
    // Act
    const auto result = vec1.midpoint(vec2);

    // Assert
    REQUIRE( almost_equal(result, vector3{1.0, 1.0, 1.0}) );
  }
  SECTION("midpoint is commutative")
  {
    // Act
    const auto r1 = vec1.midpoint(vec2);
    const auto r2 = vec2.midpoint(vec1);

    // Assert
    REQUIRE( almost_equal(r1, r2) );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector3::projection( const vector3& vec )", "[quantifiers]")
{
  SECTION("Vector is parallel to the projected vector")
  {
    // Arrange
    const auto sut = vector3{2,2,2};
    const auto onto = vector3{1,1,1};

    // Act
    const auto result = sut.projection(onto);

    // Assert
    REQUIRE( almost_equal(result, sut) );
  }

  SECTION("Vector is perpendicular to the projected vector")
  {
    // Arrange
    const auto sut = vector3{1,-1,0};
    const auto onto = vector3{1,1,1};

    // Act
    const auto result = sut.projection(onto);

    // Assert
    REQUIRE( almost_equal(result, vector3_constants::zero) );
  }

  SECTION("Vector projects onto a non-unit vector")
  {
    // Arrange
    const auto sut = vector3{3,4,5};
    const auto onto = vector3{0,0,2};

    // Act
    const auto result = sut.projection(onto);

    // Assert
    REQUIRE( almost_equal(result, vector3{0,0,5}) );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector3::rejection( const vector3& vec )", "[quantifiers]")
{
  SECTION("Vector is parallel to the rejected vector")
  {
    // Arrange
    const auto sut = vector3{2,2,2};
    const auto from = vector3{1,1,1};

    // Act
    const auto result = sut.rejection(from);

    // Assert
    REQUIRE( almost_equal(result, vector3_constants::zero) );
  }

  SECTION("Vector is rejected from a non-unit vector")
  {
    // Arrange
    const auto sut = vector3{3,4,5};
    const auto from = vector3{0,0,2};

    // Act
    const auto result = sut.rejection(from);

    // Assert
    REQUIRE( almost_equal(result, vector3{3,4,0}) );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector3::normalized()", "[quantifiers]")
{
  // TODO(bitwizeshift): test this
}

//-----------------------------------------------------------------------------

TEST_CASE("vector3::inverse()", "[quantifiers]")
{
  // TODO(bitwizeshift): test this
}

//-----------------------------------------------------------------------------

TEST_CASE("vector3::angle_between( const vector3& )", "[quantifiers]")
{
  SECTION("Vectors are parallel")
  {
    // Arrange
    const auto vec1 = vector3_constants::unit_x;
    const auto vec2 = vector3_constants::unit_x;

    const auto expected = alloy::core::radian{0};

    // Act
    const auto angle = vec1.angle_between( vec2 );

    // Assert
    REQUIRE( almost_equal(angle, expected) );
  }

  SECTION("Vectors are perpendicular")
  {
    // Arrange
    const auto vec1 = vector3_constants::unit_x;
    const auto vec2 = vector3_constants::unit_z;

    const auto expected = alloy::core::radian_constants::quarter_revolution;

    // Act
    const auto angle = vec1.angle_between( vec2 );

    // Assert
    REQUIRE( almost_equal(angle,expected) );
  }

  SECTION("Vectors are opposite")
  {
    // Arrange
    const auto vec1 = vector3_constants::unit_z;
    const auto vec2 = -vec1;

    const auto expected = alloy::core::radian_constants::half_revolution;

    // Act
    const auto angle = vec1.angle_between( vec2 );

    // Assert
    REQUIRE( almost_equal(angle,expected) );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector3::angle_to( const vector3& )", "[quantifiers]")
{
  // TODO(bitwizeshift): test this
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

TEST_CASE("vector3::normalize()", "[modifiers]")
{
  SECTION("Vector contains only zeros")
  {
    // Arrange
    auto sut = vector3{0.0f, 0.0f, 0.0f};
    const auto expected = vector3{0.0f, 0.0f, 0.0f};

    // Act
    sut.normalize();

    // Assert
    SECTION("Returns zero vector")
    {
      REQUIRE( sut == expected );
    }
  }

  SECTION("Vector contains values")
  {
    SECTION("Vector is already normalized")
    {
      // Arrange
      auto sut = vector3{
        alloy::core::real{1} / alloy::core::sqrt(3.0f),
        alloy::core::real{1} / alloy::core::sqrt(3.0f),
        alloy::core::real{1} / alloy::core::sqrt(3.0f)
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
      auto sut = vector3{ 1.0f, 1.0f, 1.0f };
      const auto expected = vector3{
        alloy::core::real{1} / alloy::core::sqrt(3.0f),
        alloy::core::real{1} / alloy::core::sqrt(3.0f),
        alloy::core::real{1} / alloy::core::sqrt(3.0f)
      };

      // Act
      sut.normalize();

      // Assert
      SECTION("Normalizes vector")
      {
        REQUIRE( almost_equal(sut, expected) );
      }
    }
  }

  SECTION("Vector contains near-0 values")
  {
    // Arrange
    const auto epsilon = std::numeric_limits<float>::epsilon();

    auto sut = vector3{epsilon,0.0f,0.0f};
    const auto expected = vector3{1.0f,0.0f,0.0f};

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

TEST_CASE("vector3::invert()", "[modifiers]")
{
  SECTION("Vector contains only zeros")
  {
    // Arrange
    auto sut = vector3{0.0f,0.0f,0.0f};
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
    auto sut = vector3{1.0f, 2.0f, 3.0f};
    const auto expected = vector3{-1.0f, -2.0f, -3.0f};

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

TEST_CASE("operator+( vector3, vector3 )", "[arithmetic]")
{
  SECTION("Adds values piecewise")
  {
    // Arrange
    const auto lhs = vector3{1,2,3};
    const auto rhs = vector3{2,1,4};
    const auto result = vector3{3,3,7};

    // Act
    const auto actual = (lhs + rhs);
    const auto equivalent = result;

    // Assert
    REQUIRE(actual == equivalent);
  }
  SECTION("Is commutative")
  {
    // Arrange
    const auto a = vector3{1,2,3};
    const auto b = vector3{4,5,6};

    // Act
    const auto actual = (a + b);
    const auto equivalent = (b + a);

    // Assert
    REQUIRE(actual == equivalent);
  }
  SECTION("Is associative")
  {
    // Arrange
    const auto a = vector3{1,2,3};
    const auto b = vector3{4,5,6};
    const auto c = vector3{7,8,9};

    // Act
    const auto actual = ((a + b) + c);
    const auto equivalent = (a + (b + c));

    // Assert
    REQUIRE(actual == equivalent);
  }
  SECTION("Contains identity")
  {
    // Arrange
    const auto a = vector3{1,2,3};
    const auto identity = vector3{0,0,0};

    // Act
    const auto actual = (a + identity);
    const auto equivalent = (a);

    // Assert
    REQUIRE(actual == equivalent);
  }
}

TEST_CASE("operator-( vector3, vector3 )", "[arithmetic]")
{
  SECTION("Subtracts values piecewise")
  {
    // Arrange
    const auto lhs = vector3{1,2,1};
    const auto rhs = vector3{2,1,2};
    const auto result = vector3{-1,1,-1};

    // Act
    const auto actual = (lhs - rhs);
    const auto equivalent = result;

    // Assert
    REQUIRE(actual == equivalent);
  }
  SECTION("Contains identity")
  {
    // Arrange
    const auto a = vector3{1,2,3};
    const auto identity = vector3{0,0,0};

    // Act
    const auto actual = (a - identity);
    const auto equivalent = (a);

    // Assert
    REQUIRE(actual == equivalent);
  }
}

TEST_CASE("operator*( real, vector3 )", "[arithmetic]")
{
  SECTION("Is distributive")
  {
    // Arrange
    const auto constant = alloy::core::real{2};
    const auto a = vector3{1,2,1};
    const auto b = vector3{2,1,2};

    // Act
    const auto lhs = (constant * (a + b));
    const auto rhs = (constant * a + constant * b);

    // Assert
    REQUIRE( lhs == rhs );
  }
  SECTION("Contains identity")
  {
    // Arrange
    const auto a = vector3{1,2,3};
    const auto identity = alloy::core::real{1};

    // Act
    const auto actual = (identity * a);
    const auto equivalent = (a);

    // Assert
    REQUIRE(actual == equivalent);
  }
}

TEST_CASE("operator*( vector3, real )", "[arithmetic]")
{
  SECTION("Is distributive")
  {
    // Arrange
    const auto constant = alloy::core::real{2};
    const auto a = vector3{1,2,1};
    const auto b = vector3{2,1,2};

    // Act
    const auto lhs = ((a + b) * constant);
    const auto rhs = (a * constant + b * constant);

    // Assert
    REQUIRE( lhs == rhs );
  }
  SECTION("Contains identity")
  {
    // Arrange
    const auto a = vector3{1,2,3};
    const auto identity = alloy::core::real{1};

    // Act
    const auto actual = (a * identity);
    const auto equivalent = (a);

    // Assert
    REQUIRE(actual == equivalent);
  }
}
} // namespace alloy::core::test
