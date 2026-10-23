// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/vector/vector_utilities.hpp>

#include <alloy/core/math/vector/vector2.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/math/vector/vector4.hpp>

#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//-----------------------------------------------------------------------------
// Traits
//-----------------------------------------------------------------------------

TEST_CASE("is_vector<T>", "[traits]")
{
  SECTION("Vector types are vectors")
  {
    SECTION("Recognizes two-dimensional vectors") {
      // Act
      constexpr auto property_holds = is_vector_v<vector2>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Recognizes three-dimensional vectors") {
      // Act
      constexpr auto property_holds = is_vector_v<vector3>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Recognizes four-dimensional vectors") {
      // Act
      constexpr auto property_holds = is_vector_v<vector4>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
  SECTION("Scalar types are not vectors")
  {
    // Act
    constexpr auto property_holds = is_vector_v<real>;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

TEST_CASE("vector_traits<T>", "[traits]")
{
  SECTION("Reads the components that the vector has")
  {
    // Arrange
    const auto vec = vector3{1,2,3};

    // Act
    const auto z = vector_traits<vector3>::z(vec);

    // Assert
    REQUIRE( z == real{3} );
  }

  SECTION("Reads 0 for components that the vector lacks")
  {
    // Arrange
    const auto vec = vector2{1,2};

    // Act
    const auto z = vector_traits<vector2>::z(vec);
    const auto w = vector_traits<vector2>::w(vec);

    // Assert
    SECTION("Supplies zero for the missing third dimension") {
      REQUIRE( z == real{0} );
    }
    SECTION("Supplies zero for the missing fourth dimension") {
      REQUIRE( w == real{0} );
    }
  }
}

//-----------------------------------------------------------------------------
// Casts
//-----------------------------------------------------------------------------

TEST_CASE("to_vector2( Vector )", "[casts]")
{
  // Arrange
  const auto vec = vector4{1,2,3,4};

  // Act
  const auto result = to_vector2(vec);

  // Assert
  SECTION("Discards components beyond the second")
  {
    REQUIRE( result == vector2{1,2} );
  }
}

TEST_CASE("to_vector3( Vector )", "[casts]")
{
  SECTION("Discards components beyond the third")
  {
    // Arrange
    const auto vec = vector4{1,2,3,4};

    // Act
    const auto result = to_vector3(vec);

    // Assert
    REQUIRE( result == vector3{1,2,3} );
  }

  SECTION("Sets missing components to 0")
  {
    // Arrange
    const auto vec = vector2{1,2};

    // Act
    const auto result = to_vector3(vec);

    // Assert
    REQUIRE( result == vector3{1,2,0} );
  }
}

TEST_CASE("to_vector4( Vector )", "[casts]")
{
  SECTION("Copies every component of a vector4")
  {
    // Arrange
    const auto vec = vector4{1,2,3,4};

    // Act
    const auto result = to_vector4(vec);

    // Assert
    REQUIRE( result == vec );
  }

  SECTION("Sets missing components to 0")
  {
    // Arrange
    const auto vec = vector3{1,2,3};

    // Act
    const auto result = to_vector4(vec);

    // Assert
    REQUIRE( result == vector4{1,2,3,0} );
  }
}

} // namespace alloy::core::test
