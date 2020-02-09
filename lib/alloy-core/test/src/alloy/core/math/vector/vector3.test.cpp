#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/math/vector/vector_utilities.hpp>
#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/math.hpp>

#include <catch2/catch.hpp>

namespace alloy::core::test {

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("vector3::vector3( real, real, real )", "[ctor]")
{
  const auto x = 3.14f;
  const auto y = 1.57f;
  const auto z = 0.25f;
  const auto vec = vector3{x,y,z};

  SECTION("Initializes x, y, and z components")
  {
    SECTION("x contains specified x value")
    {
      REQUIRE( vec.x() == x );
    }
    SECTION("y contains specified y value")
    {
      REQUIRE( vec.y() == y );
    }
    SECTION("z contains specified z value")
    {
      REQUIRE( vec.z() == z );
    }
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("vector3::size()", "[observers]")
{
  const auto vec = vector3{0,0,0};

  SECTION("Has size 3")
  {
    REQUIRE( vec.size() == 3 );
  }
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

#if ALLOY_CORE_EXCEPTIONS_ENABLED
TEST_CASE("vector3::at( index_type )", "[observers]")
{
  const auto vec = vector3{1,42,1024};

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
    SECTION("Index 2 retrieves z()")
    {
      REQUIRE( vec[2] == vec.z() );
    }
  }
}
#endif // ALLOY_CORE_EXCEPTIONS_ENABLED

TEST_CASE("vector3::operator[]( index_type )", "[observers]")
{
  const auto vec = vector3{1,42,1024};

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
    SECTION("Index 2 retrieves z()")
    {
      REQUIRE( vec[2] == vec.z() );
    }
  }
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

TEST_CASE("vector3::dot( vector3 )", "[quantifiers]")
{
  const auto vec1 = vector3{ 1.0f, 2.0f, 1.0f };
  const auto vec2 = vector3{ 2.0f, 1.0f, 2.0f };

  SECTION("Dot product returns sum of multiples")
  {
    REQUIRE( vec1.dot(vec2) == 6.0f );
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector3::cross( vector3 )", "[quantifiers]")
{
  SECTION("Cross product is anti-commutative")
  {
    const auto a = vector3{ 1.0f, 0.0f, 0.0f };
    const auto b = vector3{ 0.0f, 1.0f, 0.0f };

    // a x b == -b x a
    REQUIRE( almost_equal(a.cross(b), (-b).cross(a)) );
  }
  SECTION("Trigonometry product holds")
  {
    const auto a = vector3{ 1.0f, 0.0f, 0.0f };
    const auto b = vector3{ 0.0f, 1.0f, 0.0f };

    const auto descriminant = (1 - a.dot(b) * a.dot(b));
    const auto lhs = (a.cross(b)).magnitude();
    const auto rhs = a.magnitude() * b.magnitude() * sqrt(descriminant);

    // |a x b| == |a||b|*sqrt(1-(a dot b)^2)
    REQUIRE( almost_equal(lhs, rhs) );
  }
  SECTION("Vectors are parallel")
  {
    SECTION("Returns zero vector")
    {
      const auto vec1 = vector3{ 1.0f, 1.0f, 1.0f };
      const auto vec2 = vector3{ 1.0f, 1.0f, 1.0f };

      const auto result = vec1.cross(vec2);
      const auto expected = vector3{ 0.0f, 0.0f, 0.0f };

      REQUIRE( almost_equal(result, expected) );
    }
  }
  SECTION("Vectors are perpendicular")
  {
    SECTION("Magnitude of cross-product is product of magnitudes")
    {
      const auto vec1 = vector3{ 1.0f, 0.0f, 0.0f };
      const auto vec2 = vector3{ 0.0f, 1.0f, 0.0f };

      const auto result = vec1.cross(vec2);

      const auto lhs = result.magnitude();
      const auto rhs = vec1.magnitude() * vec2.magnitude();

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
    const auto vec = vector3{ 1.0f, 2.0f, 2.0f };

    SECTION("Returns magnitude of vector")
    {
      REQUIRE( almost_equal( vec.magnitude(), 3.0f ) );
    }
  }

  SECTION("Vector contains 0s")
  {
    const auto vec = vector3{ 0.0f, 0.0f, 0.0f };

    SECTION("Returns 0")
    {
      REQUIRE( vec.magnitude() == 0.0f );
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector3::midpoint( const vector3& vec )", "[quantifiers]")
{
  const auto vec1 = vector3{ 0.0, 0.0, 0.0 };
  const auto vec2 = vector3{ 2.0, 2.0, 2.0 };

  SECTION("Returns midpoint between two vectors")
  {
    const auto result = vec1.midpoint(vec2);

    REQUIRE( almost_equal(result, vector3{1.0, 1.0, 1.0}) );
  }
  SECTION("midpoint is commutative")
  {
    const auto r1 = vec1.midpoint(vec2);
    const auto r2 = vec2.midpoint(vec1);

    REQUIRE( almost_equal(r1, r2) );
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
    const auto vec1 = vector3::unit_x;
    const auto vec2 = vector3::unit_x;

    const auto angle = vec1.angle_between( vec2 );
    const auto expected = alloy::core::radian{0};

    REQUIRE( almost_equal(angle, expected) );
  }

  SECTION("Vectors are perpendicular")
  {
    const auto vec1 = vector3::unit_x;
    const auto vec2 = vector3::unit_z;

    const auto angle = vec1.angle_between( vec2 );
    const auto expected = alloy::core::radian_constants::quarter_revolution;

    REQUIRE( almost_equal(angle,expected) );
  }

  SECTION("Vectors are opposite")
  {
    const auto vec1 = vector3::unit_z;
    const auto vec2 = -vec1;

    const auto angle = vec1.angle_between( vec2 );
    const auto expected = alloy::core::radian_constants::half_revolution;

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
    auto vec = vector3{0.0f, 0.0f, 0.0f};
    const auto expected = vector3{0.0f, 0.0f, 0.0f};

    vec.normalize();

    SECTION("Returns zero vector")
    {
      REQUIRE( vec == expected );
    }
  }

  SECTION("Vector contains values")
  {
    SECTION("Vector is already normalized")
    {
      auto vec = vector3{
        alloy::core::real{1} / alloy::core::sqrt(3.0f),
        alloy::core::real{1} / alloy::core::sqrt(3.0f),
        alloy::core::real{1} / alloy::core::sqrt(3.0f)
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
      auto vec = vector3{ 1.0f, 1.0f, 1.0f };
      const auto expected = vector3{
        alloy::core::real{1} / alloy::core::sqrt(3.0f),
        alloy::core::real{1} / alloy::core::sqrt(3.0f),
        alloy::core::real{1} / alloy::core::sqrt(3.0f)
      };

      vec.normalize();

      SECTION("Normalizes vector")
      {
        REQUIRE( almost_equal(vec, expected) );
      }
    }
  }

  SECTION("Vector contains near-0 values")
  {
    const auto epsilon = std::numeric_limits<float>::epsilon();

    auto vec = vector3{epsilon,0.0f,0.0f};
    const auto expected = vector3{1.0f,0.0f,0.0f};

    vec.normalize();

    SECTION("Normalizes to unit vector")
    {
      REQUIRE( almost_equal(vec, expected) );
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("vector3::invert()", "[modifiers]")
{
  SECTION("Vector contains only zeros")
  {
    auto vec = vector3{0.0f,0.0f,0.0f};
    const auto expected = vec;

    vec.invert();

    SECTION("Does not alter vector")
    {
      REQUIRE( vec == expected );
    }
  }
  SECTION("Vector contains values")
  {
    auto vec = vector3{1.0f, 2.0f, 3.0f};
    const auto expected = vector3{-1.0f, -2.0f, -3.0f};

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

TEST_CASE("operator+( vector3, vector3 )", "[arithmetic]")
{
  SECTION("Adds values piecewise")
  {
    const auto lhs = vector3{1,2,3};
    const auto rhs = vector3{2,1,4};
    const auto result = vector3{3,3,7};

    REQUIRE( (lhs + rhs) == result );
  }
  SECTION("Is commutative")
  {
    const auto a = vector3{1,2,3};
    const auto b = vector3{4,5,6};

    REQUIRE( (a + b) == (b + a) );
  }
  SECTION("Is associative")
  {
    const auto a = vector3{1,2,3};
    const auto b = vector3{4,5,6};
    const auto c = vector3{7,8,9};

    REQUIRE( ((a + b) + c) == (a + (b + c)) );
  }
  SECTION("Contains identity")
  {
    const auto a = vector3{1,2,3};
    const auto identity = vector3{0,0,0};

    REQUIRE( (a + identity) == (a) );
  }
}

TEST_CASE("operator-( vector3, vector3 )", "[arithmetic]")
{
  SECTION("Subtracts values piecewise")
  {
    const auto lhs = vector3{1,2,1};
    const auto rhs = vector3{2,1,2};
    const auto result = vector3{-1,1,-1};

    REQUIRE( (lhs - rhs) == result );
  }
  SECTION("Contains identity")
  {
    const auto a = vector3{1,2,3};
    const auto identity = vector3{0,0,0};

    REQUIRE( (a - identity) == (a) );
  }
}

TEST_CASE("operator*( real, vector3 )", "[arithmetic]")
{
  SECTION("Is distributive")
  {
    const auto constant = alloy::core::real{2};
    const auto a = vector3{1,2,1};
    const auto b = vector3{2,1,2};

    const auto lhs = (constant * (a + b));
    const auto rhs = (constant * a + constant * b);

    REQUIRE( lhs == rhs );
  }
  SECTION("Contains identity")
  {
    const auto a = vector3{1,2,3};
    const auto identity = alloy::core::real{1};

    REQUIRE( (identity * a) == (a) );
  }
}

TEST_CASE("operator*( vector3, real )", "[arithmetic]")
{
  SECTION("Is distributive")
  {
    const auto constant = alloy::core::real{2};
    const auto a = vector3{1,2,1};
    const auto b = vector3{2,1,2};

    const auto lhs = ((a + b) * constant);
    const auto rhs = (a * constant + b * constant);

    REQUIRE( lhs == rhs );
  }
  SECTION("Contains identity")
  {
    const auto a = vector3{1,2,3};
    const auto identity = alloy::core::real{1};

    REQUIRE( (a * identity) == (a) );
  }
}
} // namespace alloy::core::test