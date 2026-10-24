// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/model/projection.hpp>

#include <alloy/core/math/angle/degree.hpp>
#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/matrix/matrix4.hpp>
#include <alloy/core/math/matrix/matrix4_constants.hpp>
#include <alloy/core/math/vector/vector4.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {
namespace {

  // Projects the point at x, y, z into normalized device coordinates.
  auto project(const projection& sut, real x, real y, real z) -> vector4
  {
    const auto clip = sut.to_matrix4() * vector4{x, y, z, 1};

    return clip / clip.w();
  }

} // namespace

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

TEST_CASE("projection::perspective( radian, real, view_volume::depth_bounds )",
          "[factory]")
{
  // Arrange
  const auto sut = projection::perspective(
    radian_constants::quarter_revolution,
    real{2},
    view_volume::depth_bounds{real{1}, real{10}}
  );

  SECTION("Maps the center of the near plane to the near face")
  {
    // Act
    const auto result = project(sut, 0, 0, -1);

    // Assert
    REQUIRE( almost_equal(result, vector4{0, 0, -1, 1}, real{1e-5f}) );
  }
  SECTION("Maps the center of the far plane to the far face")
  {
    // Act
    const auto result = project(sut, 0, 0, -10);

    // Assert
    REQUIRE( almost_equal(result, vector4{0, 0, 1, 1}, real{1e-5f}) );
  }
  SECTION("Maps the top-right corner of the near plane to the corner")
  {
    // Arrange
    // A quarter revolution of fov spans the same distance as the depth, so
    // the near plane at 1 spans [-1, 1] vertically, and [-2, 2] horizontally.
    const auto x = real{2};
    const auto y = real{1};

    // Act
    const auto result = project(sut, x, y, -1);

    // Assert
    REQUIRE( almost_equal(result, vector4{1, 1, -1, 1}, real{1e-5f}) );
  }
}

TEST_CASE("projection::perspective( radian, real, view_volume::depth_bounds, "
          "clip_depth )", "[factory]")
{
  // Arrange
  const auto sut = projection::perspective(
    radian_constants::quarter_revolution,
    real{2},
    view_volume::depth_bounds{real{1}, real{10}},
    clip_depth::zero_to_one
  );

  SECTION("Maps the center of the near plane to a depth of zero")
  {
    // Act
    const auto result = project(sut, 0, 0, -1);

    // Assert
    REQUIRE( almost_equal(result, vector4{0, 0, 0, 1}, real{1e-5f}) );
  }
  SECTION("Maps the center of the far plane to a depth of one")
  {
    // Act
    const auto result = project(sut, 0, 0, -10);

    // Assert
    REQUIRE( almost_equal(result, vector4{0, 0, 1, 1}, real{1e-5f}) );
  }
}

TEST_CASE("projection::perspective( degree, real, view_volume::depth_bounds )",
          "[factory]")
{
  // Arrange
  const auto expected = projection::perspective(
    radian_constants::quarter_revolution,
    real{2},
    view_volume::depth_bounds{real{1}, real{10}}
  ).to_matrix4();

  // Act
  const auto sut = projection::perspective(
    degree{90},
    real{2},
    view_volume::depth_bounds{real{1}, real{10}}
  );
  const auto result = sut.to_matrix4();

  // Assert
  SECTION("Matches the projection of the same angle in radians")
  {
    REQUIRE( almost_equal(result, expected, real{1e-5f}) );
  }
}

//------------------------------------------------------------------------------
// Static Factories : Orthographic
//------------------------------------------------------------------------------

TEST_CASE("projection::orthographic( real, real, real, real, real, real )",
          "[factory]")
{
  // Arrange
  const auto sut = projection::orthographic(-2, 6, -1, 3, 1, 10);

  SECTION("Maps the near bottom-left corner to the near bottom-left face")
  {
    // Act
    const auto result = project(sut, -2, -1, -1);

    // Assert
    REQUIRE( almost_equal(result, vector4{-1, -1, -1, 1}, real{1e-5f}) );
  }
  SECTION("Maps the far top-right corner to the far top-right face")
  {
    // Act
    const auto result = project(sut, 6, 3, -10);

    // Assert
    REQUIRE( almost_equal(result, vector4{1, 1, 1, 1}, real{1e-5f}) );
  }
}

TEST_CASE("projection::orthographic( real, real, real, real, real, real, "
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
    REQUIRE( almost_equal(result, vector4{-1, -1, 0, 1}, real{1e-5f}) );
  }
  SECTION("Maps the far top-right corner to a depth of one")
  {
    // Act
    const auto result = project(sut, 6, 3, -10);

    // Assert
    REQUIRE( almost_equal(result, vector4{1, 1, 1, 1}, real{1e-5f}) );
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
// Conversion
//------------------------------------------------------------------------------

TEST_CASE("projection::to_inverse_matrix4()", "[conversion]")
{
  SECTION("Undoes a perspective projection")
  {
    // Arrange
    const auto sut = projection::perspective(
      radian{1.2f},
      real{1.5f},
      view_volume::depth_bounds{real{0.5f}, real{100}}
    );
    const auto matrix = sut.to_matrix4();

    // Act
    const auto result = sut.to_inverse_matrix4() * matrix;

    // Assert
    REQUIRE( almost_equal(result, matrix4_constants::identity, real{1e-4f}) );
  }
  SECTION("Undoes a zero-to-one perspective projection")
  {
    // Arrange
    const auto sut = projection::perspective(
      radian{1.2f},
      real{1.5f},
      view_volume::depth_bounds{real{0.5f}, real{100}},
      clip_depth::zero_to_one
    );
    const auto matrix = sut.to_matrix4();

    // Act
    const auto result = sut.to_inverse_matrix4() * matrix;

    // Assert
    REQUIRE( almost_equal(result, matrix4_constants::identity, real{1e-4f}) );
  }
  SECTION("Undoes an orthographic projection")
  {
    // Arrange
    const auto sut = projection::orthographic(-2, 6, -1, 3, 1, 10);
    const auto matrix = sut.to_matrix4();

    // Act
    const auto result = sut.to_inverse_matrix4() * matrix;

    // Assert
    REQUIRE( almost_equal(result, matrix4_constants::identity, real{1e-5f}) );
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
