#include <alloy/core/math/vector/vector2.hpp>
#include <alloy/core/math/vector/vector_utilities.hpp>
#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/math.hpp>

#include <catch2/catch.hpp>

using alloy::core::almost_equal;

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("vector2::vector2( real, real )", "[ctor]")
{
  const auto x = 3.14f;
  const auto y = 1.57f;
  const auto vec = alloy::core::vector2{x,y};

  SECTION("Initializes x and y components")
  {
    SECTION("x contains specified x value")
    {
      REQUIRE( vec.x() == x );
    }
    SECTION("y contains specified y value")
    {
      REQUIRE( vec.y() == y );
    }
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("vector2::size()", "[observers]")
{
  const auto vec = alloy::core::vector2{0,0};

  SECTION("Has size 2")
  {
    REQUIRE( vec.size() == 2 );
  }
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

#if ALLOY_ENABLE_EXCEPTIONS
TEST_CASE("vector2::at( index_type )", "[observers]")
{
  const auto vec = alloy::core::vector2{42,1024};

  SECTION("Throws when out of range")
  {
    SECTION("Index < 0")
    {
      REQUIRE_THROWS_AS(vec.at(-1),std::logic_error);
    }
    SECTION("Index > size()")
    {
      REQUIRE_THROWS_AS(vec.at(vec.size()),std::logic_error);
    }
  }
  SECTION("Retrieves entry at index")
  {
    SECTION("Index 0 retrieves x()")
    {
      REQUIRE( vec[0] == vec.x() );
    }
    SECTION("Index 1 retrieves y()")
    {
      REQUIRE( vec[1] == vec.y() );
    }
  }
}
#endif // ALLOY_ENABLE_EXCEPTIONS

TEST_CASE("vector2::operator[]( index_type )", "[observers]")
{
  const auto vec = alloy::core::vector2{42,1024};

  SECTION("Retrieves entry at index")
  {
    SECTION("Index 0 retrieves x()")
    {
      REQUIRE( vec[0] == vec.x() );
    }
    SECTION("Index 1 retrieves y()")
    {
      REQUIRE( vec[1] == vec.y() );
    }
  }
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

TEST_CASE("vector2::dot( vector2 )", "[quantifiers]")
{
  const auto vec1 = alloy::core::vector2{ 1.0f, 2.0f };
  const auto vec2 = alloy::core::vector2{ 2.0f, 1.0f };

  SECTION("Dot product returns sum of multiples")
  {
    REQUIRE( vec1.dot(vec2) == 4.0f );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::cross( vector2 )", "[quantifiers]")
{
  SECTION("Cross product is anti-commutative")
  {
    const auto a = alloy::core::vector2{ 1.0f, 2.0f };
    const auto b = alloy::core::vector2{ 2.0f, 1.0f };

    // a x b == -b x a
    REQUIRE( almost_equal(a.cross(b), (-b).cross(a)) );
  }
  SECTION("Vectors are parallel")
  {
    SECTION("Returns zero")
    {
      auto vec1 = alloy::core::vector2{ 1.0f, 2.0f };
      auto vec2 = alloy::core::vector2{ 1.0f, 2.0f };

      REQUIRE( vec1.cross(vec2) == 0.0f );
    }
  }
  SECTION("Vectors are perpendicular")
  {
    const auto vec1 = alloy::core::vector2{ 1.0f, 0.0f };
    const auto vec2 = alloy::core::vector2{ 0.0f, 1.0f };

    const auto result = vec1.cross(vec2);

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
    const auto vec = alloy::core::vector2{ 3.0f, 4.0f };

    SECTION("Returns magnitude of vector")
    {
      REQUIRE( almost_equal( vec.magnitude(), 5.0f ) );
    }
  }

  SECTION("Vector contains 0s")
  {
    const auto vec = alloy::core::vector2{ 0.0f, 0.0f };

    SECTION("Returns 0")
    {
      REQUIRE( vec.magnitude() == 0.0f );
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::midpoint( const vector2& vec )", "[quantifiers]")
{
  const auto vec1 = alloy::core::vector2{ 0.0, 0.0 };
  const auto vec2 = alloy::core::vector2{ 2.0, 2.0 };

  SECTION("Returns midpoint between two vectors")
  {
    const auto result = vec1.midpoint(vec2);

    REQUIRE( almost_equal(result, alloy::core::vector2{1.0,1.0}) );
  }
  SECTION("vector2::midpoint is commutative")
  {
    const auto r1 = vec1.midpoint(vec2);
    const auto r2 = vec2.midpoint(vec1);

    REQUIRE( almost_equal(r1,r2) );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::reflection( const vector2& vec )", "[quantifiers]")
{
  SECTION("Reflects over x-axis")
  {
    const auto x_axis = alloy::core::vector2::unit_y;
    const auto vector = alloy::core::vector2{1.0,1.0};

    const auto result = vector.reflection( x_axis );

    REQUIRE( almost_equal( result, alloy::core::vector2{1.0,-1.0}) );
  }

  SECTION("Reflects over y-axis")
  {
    const auto x_axis = alloy::core::vector2::unit_x;
    const auto vector = alloy::core::vector2{1.0,1.0};

    const auto result = vector.reflection( x_axis );

    REQUIRE( almost_equal( result, alloy::core::vector2{-1.0,1.0}) );
  }

  SECTION("Reflects over arbitrary axis")
  {
    const auto axis   = alloy::core::vector2{ 1.0, 1.0 };
    const auto vector = alloy::core::vector2{ 1.0, 0.0 };

    const auto result = vector.reflection( axis );

    REQUIRE( almost_equal(result, alloy::core::vector2{-1.0,-2.0}) );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::projection( const vector2& vec )", "[quantifiers]")
{
  SECTION("Projection onto x-axis")
  {
//    const auto axis = alloy::core::vector2::unit_y;

    SECTION("Vector is parallel to axis")
    {
      // TODO(bitwizeshift): test this condition
    }

    SECTION("Vector is perpendicular to axis")
    {
      // TODO(bitwizeshift): test this condition
    }

    SECTION("Vector can project on axis")
    {
      // TODO(bitwizeshift): test this condition
    }
  }

  SECTION("Projection onto y-axis")
  {
//    const auto axis = alloy::core::vector2::unit_x;

    SECTION("Vector is parallel to axis")
    {
      // TODO(bitwizeshift): test this condition
    }

    SECTION("Vector is perpendicular to axis")
    {
      // TODO(bitwizeshift): test this condition
    }

    SECTION("Vector can project on axis")
    {
      // TODO(bitwizeshift): test this condition
    }
  }

  SECTION("Projection onto arbitrary axis")
  {
//    const auto axis = alloy::core::vector2{1,1}.normalize();

    SECTION("Vector is parallel to axis")
    {
      // TODO(bitwizeshift): test this condition
    }

    SECTION("Vector is perpendicular to axis")
    {
      // TODO(bitwizeshift): test this condition
    }

    SECTION("Vector can project on axis")
    {
      // TODO(bitwizeshift): test this condition
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::rejection( const vector2& vec )", "[quantifiers]")
{
  // TODO(bitwizeshift): test this
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
    const auto vec1 = alloy::core::vector2::unit_x;
    const auto vec2 = alloy::core::vector2::unit_x;

    const auto angle = vec1.angle_between( vec2 );
    const auto expected = alloy::core::radian{0};

    REQUIRE( almost_equal(angle, expected) );
  }

  SECTION("Vectors are perpendicular")
  {
    const auto vec1 = alloy::core::vector2::unit_x;
    const auto vec2 = alloy::core::vector2::unit_y;

    const auto angle = vec1.angle_between( vec2 );
    const auto expected = alloy::core::radian_constants::quarter_revolution;

    REQUIRE( almost_equal(angle,expected) );
  }

  SECTION("Vectors are opposite")
  {
    const auto vec1 = alloy::core::vector2::unit_x;
    const auto vec2 = -vec1;

    const auto angle = vec1.angle_between( vec2 );
    const auto expected = alloy::core::radian_constants::half_revolution;

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
    auto vec = alloy::core::vector2{0.0f, 0.0f};
    const auto expected = alloy::core::vector2{0.0f,0.0f};

    vec.normalize();

    SECTION("Returns zero vector")
    {
      REQUIRE( vec == expected );
    }
  }

  SECTION("Vector is already normalized")
  {
    auto vec = alloy::core::vector2{
      alloy::core::real{1} / alloy::core::sqrt(2.0f),
      alloy::core::real{1} / alloy::core::sqrt(2.0f)
    };
    const auto expected = vec;

    vec.normalize();

    SECTION("Does not alter vector")
    {
      REQUIRE( almost_equal(vec,expected) );
    }
  }

  SECTION("Vector is not normalized")
  {
    auto vec = alloy::core::vector2{ 1.0f, 1.0f };
    const auto expected = alloy::core::vector2{
      alloy::core::real{1} / alloy::core::sqrt(2.0f),
      alloy::core::real{1} / alloy::core::sqrt(2.0f)
    };

    vec.normalize();

    SECTION("Normalizes vector")
    {
      REQUIRE( almost_equal(vec, expected) );
    }
  }

  SECTION("Vector contains near-0 values")
  {
    const auto epsilon = std::numeric_limits<float>::epsilon();
    auto vec = alloy::core::vector2{epsilon,0.0f};
    const auto expected = alloy::core::vector2{1.0f,0.0f};

    vec.normalize();

    SECTION("Normalizes to unit vector")
    {
      REQUIRE( almost_equal(vec, expected) );
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector2::invert()", "[modifiers]")
{
  SECTION("Vector contains only zeros")
  {
    auto vec = alloy::core::vector2{0.0f, 0.0f};
    const auto expected = vec;

    vec.invert();

    SECTION("Does not alter vector")
    {
      REQUIRE( vec == expected );
    }
  }
  SECTION("Vector contains values")
  {
    auto vec = alloy::core::vector2{1.0f, 2.0f};
    const auto expected = alloy::core::vector2{-1.0f, -2.0f};

    vec.invert();

    SECTION("Inverts the vector")
    {
      REQUIRE( vec == expected );
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
    const auto lhs = alloy::core::vector2{1,2};
    const auto rhs = alloy::core::vector2{2,1};
    const auto result = alloy::core::vector2{3,3};

    REQUIRE( (lhs + rhs) == result );
  }
  SECTION("Is commutative")
  {
    const auto a = alloy::core::vector2{1,2};
    const auto b = alloy::core::vector2{4,5};

    REQUIRE( (a + b) == (b + a) );
  }
  SECTION("Is associative")
  {
    const auto a = alloy::core::vector2{1,2};
    const auto b = alloy::core::vector2{4,5};
    const auto c = alloy::core::vector2{8,9};

    REQUIRE( ((a + b) + c) == (a + (b + c)) );
  }
  SECTION("Contains identity")
  {
    const auto a = alloy::core::vector2{1,2};
    const auto identity = alloy::core::vector2{0,0};

    REQUIRE( (a + identity) == (a) );
  }
}

TEST_CASE("operator-( vector2, vector2 )", "[arithmetic]")
{
  SECTION("Subtracts values piecewise")
  {
    const auto lhs = alloy::core::vector2{1,2};
    const auto rhs = alloy::core::vector2{2,1};
    const auto result = alloy::core::vector2{-1,1};

    REQUIRE( (lhs - rhs) == result );
  }
  SECTION("Contains identity")
  {
    const auto a = alloy::core::vector2{1,2};
    const auto identity = alloy::core::vector2{0,0};

    REQUIRE( (a - identity) == (a) );
  }
}

TEST_CASE("operator*( real, vector2 )", "[arithmetic]")
{
  SECTION("Is distributive")
  {
    const auto constant = alloy::core::real{2};
    const auto a = alloy::core::vector2{1,2};
    const auto b = alloy::core::vector2{2,1};

    const auto lhs = (constant * (a + b));
    const auto rhs = (constant * a + constant * b);

    REQUIRE( lhs == rhs );
  }
  SECTION("Contains identity")
  {
    const auto a = alloy::core::vector2{1,2};
    const auto identity = alloy::core::real{1};

    REQUIRE( (identity * a) == (a) );
  }
}

TEST_CASE("operator*( vector2, real )", "[arithmetic]")
{
  SECTION("Is distributive")
  {
    const auto constant = alloy::core::real{2};
    const auto a = alloy::core::vector2{1,2};
    const auto b = alloy::core::vector2{2,1};

    const auto lhs = ((a + b) * constant);
    const auto rhs = (a * constant + b * constant);

    REQUIRE( lhs == rhs );
  }
  SECTION("Contains identity")
  {
    const auto a = alloy::core::vector2{1,2};
    const auto identity = alloy::core::real{1};

    REQUIRE( (a * identity) == (a) );
  }
}