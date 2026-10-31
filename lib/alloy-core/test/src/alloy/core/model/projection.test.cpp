// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/model/projection.hpp>

#include <alloy/core/math/angle/degree.hpp>
#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/matrix/matrix4.hpp>
#include <alloy/core/math/matrix/matrix4_constants.hpp>
#include <alloy/core/math/vector/vector4.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {
namespace {

  // Projects the point at x, y, z into normalized device coordinates.
  auto project(const projection& sut, float x, float y, float z) -> vector4
  {
    const auto clip = sut.to_matrix4() * vector4{x, y, z, 1};

    return clip / clip.w();
  }

} // namespace

//------------------------------------------------------------------------------
// view_volume : Static Factories
//------------------------------------------------------------------------------

TEST_CASE("view_volume::make( float, float, float, float, float, float )",
          "[factory]")
{
  // Act
  const auto sut = view_volume::make(-2, 6, -1, 3, 1, 10);

  // Assert
  SECTION("Left is the first bound")
  {
    REQUIRE( sut.horizontal().left == -2.0f );
  }
  SECTION("Right is the second bound")
  {
    REQUIRE( sut.horizontal().right == 6.0f );
  }
  SECTION("Bottom is the third bound")
  {
    REQUIRE( sut.vertical().bottom == -1.0f );
  }
  SECTION("Top is the fourth bound")
  {
    REQUIRE( sut.vertical().top == 3.0f );
  }
  SECTION("Near is the fifth bound")
  {
    REQUIRE( sut.depth().near == 1.0f );
  }
  SECTION("Far is the sixth bound")
  {
    REQUIRE( sut.depth().far == 10.0f );
  }
}

TEST_CASE("view_volume::make_symmetric( float, float, float )", "[factory]")
{
  // Act
  const auto sut = view_volume::make_symmetric(2, 3, 4);

  // Assert
  SECTION("Left is the negated half width")
  {
    REQUIRE( sut.horizontal().left == -2.0f );
  }
  SECTION("Right is the half width")
  {
    REQUIRE( sut.horizontal().right == 2.0f );
  }
  SECTION("Bottom is the negated half height")
  {
    REQUIRE( sut.vertical().bottom == -3.0f );
  }
  SECTION("Top is the half height")
  {
    REQUIRE( sut.vertical().top == 3.0f );
  }
  SECTION("Near is the negated distance")
  {
    REQUIRE( sut.depth().near == -4.0f );
  }
  SECTION("Far is the distance")
  {
    REQUIRE( sut.depth().far == 4.0f );
  }
}

//------------------------------------------------------------------------------
// Static Factories : Identity
//------------------------------------------------------------------------------

TEST_CASE("projection::identity()", "[factory]")
{
  // Act
  const auto sut = projection::identity();
  const auto result = sut.to_matrix4();

  // Assert
  SECTION("Produces the identity matrix")
  {
    REQUIRE( result == matrix4_constants::identity );
  }
}

//------------------------------------------------------------------------------
// Static Factories : Perspective
//------------------------------------------------------------------------------

TEST_CASE("projection::perspective( radian, float, view_volume::depth_bounds )",
          "[factory]")
{
  // Arrange
  const auto sut = projection::perspective(
    radian_constants::quarter_revolution,
    2.0f,
    view_volume::depth_bounds{1.0f, 10.0f}
  );

  SECTION("Maps the center of the near plane to the near face")
  {
    // Act
    const auto result = project(sut, 0, 0, -1);

    // Assert
    REQUIRE( almost_equal(result, vector4{0, 0, -1, 1}, 1e-5f) );
  }
  SECTION("Maps the center of the far plane to the far face")
  {
    // Act
    const auto result = project(sut, 0, 0, -10);

    // Assert
    REQUIRE( almost_equal(result, vector4{0, 0, 1, 1}, 1e-5f) );
  }
  SECTION("Maps the top-right corner of the near plane to the corner")
  {
    // Arrange
    // A quarter revolution of fov spans the same distance as the depth, so
    // the near plane at 1 spans [-1, 1] vertically, and [-2, 2] horizontally.
    const auto x = 2.0f;
    const auto y = 1.0f;

    // Act
    const auto result = project(sut, x, y, -1);

    // Assert
    REQUIRE( almost_equal(result, vector4{1, 1, -1, 1}, 1e-5f) );
  }
}

TEST_CASE("projection::perspective( radian, float, view_volume::depth_bounds, "
          "clip_depth )", "[factory]")
{
  // Arrange
  const auto sut = projection::perspective(
    radian_constants::quarter_revolution,
    2.0f,
    view_volume::depth_bounds{1.0f, 10.0f},
    clip_depth::zero_to_one
  );

  SECTION("Maps the center of the near plane to a depth of zero")
  {
    // Act
    const auto result = project(sut, 0, 0, -1);

    // Assert
    REQUIRE( almost_equal(result, vector4{0, 0, 0, 1}, 1e-5f) );
  }
  SECTION("Maps the center of the far plane to a depth of one")
  {
    // Act
    const auto result = project(sut, 0, 0, -10);

    // Assert
    REQUIRE( almost_equal(result, vector4{0, 0, 1, 1}, 1e-5f) );
  }
}

TEST_CASE("projection::perspective( degree, float, view_volume::depth_bounds )",
          "[factory]")
{
  // Arrange
  const auto expected = projection::perspective(
    radian_constants::quarter_revolution,
    2.0f,
    view_volume::depth_bounds{1.0f, 10.0f}
  ).to_matrix4();

  // Act
  const auto sut = projection::perspective(
    degree{90},
    2.0f,
    view_volume::depth_bounds{1.0f, 10.0f}
  );
  const auto result = sut.to_matrix4();

  // Assert
  SECTION("Matches the projection of the same angle in radians")
  {
    REQUIRE( almost_equal(result, expected, 1e-5f) );
  }
}

//------------------------------------------------------------------------------
// Static Factories : Orthographic
//------------------------------------------------------------------------------

TEST_CASE("projection::orthographic( float, float, float, float, float, float )",
          "[factory]")
{
  // Arrange
  const auto sut = projection::orthographic(-2, 6, -1, 3, 1, 10);

  SECTION("Maps the near bottom-left corner to the near bottom-left face")
  {
    // Act
    const auto result = project(sut, -2, -1, -1);

    // Assert
    REQUIRE( almost_equal(result, vector4{-1, -1, -1, 1}, 1e-5f) );
  }
  SECTION("Maps the far top-right corner to the far top-right face")
  {
    // Act
    const auto result = project(sut, 6, 3, -10);

    // Assert
    REQUIRE( almost_equal(result, vector4{1, 1, 1, 1}, 1e-5f) );
  }
}

TEST_CASE("projection::orthographic( float, float, float, float, float, float, "
          "clip_depth )", "[factory]")
{
  // Arrange
  const auto sut = projection::orthographic(
    -2, 6, -1, 3, 1, 10,
    clip_depth::zero_to_one
  );

  SECTION("Maps the near bottom-left corner to a depth of zero")
  {
    // Act
    const auto result = project(sut, -2, -1, -1);

    // Assert
    REQUIRE( almost_equal(result, vector4{-1, -1, 0, 1}, 1e-5f) );
  }
  SECTION("Maps the far top-right corner to a depth of one")
  {
    // Act
    const auto result = project(sut, 6, 3, -10);

    // Assert
    REQUIRE( almost_equal(result, vector4{1, 1, 1, 1}, 1e-5f) );
  }
}

TEST_CASE("projection::orthographic( const view_volume& )", "[factory]")
{
  // Arrange
  const auto expected = projection::orthographic(-2, 6, -1, 3, 1, 10)
    .to_matrix4();
  const auto volume = view_volume::make(-2, 6, -1, 3, 1, 10);

  // Act
  const auto sut = projection::orthographic(volume);
  const auto result = sut.to_matrix4();

  // Assert
  SECTION("Matches the projection of the same bounds")
  {
    REQUIRE( result == expected );
  }
}

//------------------------------------------------------------------------------
// Adjustment
//------------------------------------------------------------------------------

TEST_CASE("projection::with_aspect_ratio( float )", "[adjustment]")
{
  SECTION("Perspective projection takes the new aspect ratio")
  {
    // Arrange
    const auto expected = projection::perspective(
      radian{1.2f},
      1.0f,
      view_volume::depth_bounds{1.0f, 10.0f}
    ).to_matrix4();
    const auto sut = projection::perspective(
      radian{1.2f},
      2.0f,
      view_volume::depth_bounds{1.0f, 10.0f}
    );

    // Act
    const auto result = sut.with_aspect_ratio(1.0f).to_matrix4();

    // Assert
    REQUIRE( almost_equal(result, expected, 1e-5f) );
  }
  SECTION("Orthographic projection resizes its width about its center")
  {
    // Arrange
    const auto expected = projection::orthographic(0, 4, -1, 3, 1, 10)
      .to_matrix4();
    const auto sut = projection::orthographic(-2, 6, -1, 3, 1, 10);

    // Act
    const auto result = sut.with_aspect_ratio(1.0f).to_matrix4();

    // Assert
    REQUIRE( almost_equal(result, expected, 1e-5f) );
  }
  SECTION("Identity projection is unchanged")
  {
    // Arrange
    const auto sut = projection::identity();

    // Act
    const auto result = sut.with_aspect_ratio(2.0f).to_matrix4();

    // Assert
    REQUIRE( result == matrix4_constants::identity );
  }
}

//------------------------------------------------------------------------------
// Conversion
//------------------------------------------------------------------------------

TEST_CASE("projection::to_inverse_matrix4()", "[conversion]")
{
  SECTION("Undoes a perspective projection")
  {
    // Arrange
    const auto sut = projection::perspective(
      radian{1.2f},
      1.5f,
      view_volume::depth_bounds{0.5f, 100.0f}
    );
    const auto matrix = sut.to_matrix4();

    // Act
    const auto result = sut.to_inverse_matrix4() * matrix;

    // Assert
    REQUIRE( almost_equal(result, matrix4_constants::identity, 1e-4f) );
  }
  SECTION("Undoes a zero-to-one perspective projection")
  {
    // Arrange
    const auto sut = projection::perspective(
      radian{1.2f},
      1.5f,
      view_volume::depth_bounds{0.5f, 100.0f},
      clip_depth::zero_to_one
    );
    const auto matrix = sut.to_matrix4();

    // Act
    const auto result = sut.to_inverse_matrix4() * matrix;

    // Assert
    REQUIRE( almost_equal(result, matrix4_constants::identity, 1e-4f) );
  }
  SECTION("Undoes an orthographic projection")
  {
    // Arrange
    const auto sut = projection::orthographic(-2, 6, -1, 3, 1, 10);
    const auto matrix = sut.to_matrix4();

    // Act
    const auto result = sut.to_inverse_matrix4() * matrix;

    // Assert
    REQUIRE( almost_equal(result, matrix4_constants::identity, 1e-5f) );
  }
  SECTION("Inverts the identity projection to the identity")
  {
    // Arrange
    const auto sut = projection::identity();

    // Act
    const auto result = sut.to_inverse_matrix4();

    // Assert
    REQUIRE( result == matrix4_constants::identity );
  }
}

} // namespace alloy::core::test
