// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/model/transform.hpp>

#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/interpolation.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/math/vector/vector3_constants.hpp>
#include <alloy/core/math/vector/vector4.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Conversion
//------------------------------------------------------------------------------

TEST_CASE("transform::to_matrix4()", "[conversion]")
{
  // Arrange
  auto sut = transform{};
  sut.scale(2);
  sut.rotate_around_axis(
    radian_constants::quarter_revolution,
    vector3_constants::unit_z
  );
  sut.translate(10, 0, 0);
  const auto point = vector4{1, 0, 0, 1};

  // Act
  const auto result = sut.to_matrix4() * point;

  // Assert
  SECTION("Scales, then rotates, then translates")
  {
    REQUIRE( almost_equal(result, vector4{10, 2, 0, 1}, real{1e-5}) );
  }
}

//------------------------------------------------------------------------------
// Interpolation
//------------------------------------------------------------------------------

TEST_CASE("interpolator<transform>", "[interpolation]")
{
  // Arrange
  const auto start = transform{};
  auto end = transform{};
  end.set_origin(2, 4, 6);

  // Act
  const auto result = interpolation::linear(start, end, real{0.5});

  // Assert
  SECTION("Interpolates the translation")
  {
    REQUIRE( almost_equal(result.translation(), vector3{1, 2, 3}) );
  }
}

} // namespace alloy::core::test
