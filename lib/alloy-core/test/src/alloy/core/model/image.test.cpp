// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/model/image.hpp>

#include <alloy/core/model/color.hpp>

#include <alloy/core/precision/real.hpp>
#include <alloy/core/types.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

TEST_CASE("image::make_blank_image( dimensions, pixel_format, color )",
          "[factory]")
{
  // Arrange
  const auto background = color{0.999f, 0.0f, 0.0f, 1.0f};

  // Act
  const auto sut = *image::make_blank_image(
    image::dimensions{1u, 1u},
    image::pixel_format::rgba,
    background
  );
  const auto result = sut.get_pixel(0u, 0u).r8();

  // Assert
  SECTION("Rounds each channel to the nearest byte")
  {
    REQUIRE( result == u8{255} );
  }
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

TEST_CASE("image::set_pixel( std::size_t, std::size_t, color )", "[modifier]")
{
  // Arrange
  auto sut = *image::make_blank_image(
    image::dimensions{1u, 1u},
    image::pixel_format::rgba,
    color{0.0f, 0.0f, 0.0f, 1.0f}
  );

  // Act
  sut.set_pixel(0u, 0u, color{0.999f, 0.0f, 0.0f, 1.0f});
  const auto result = sut.get_pixel(0u, 0u).r8();

  // Assert
  SECTION("Rounds each channel to the nearest byte")
  {
    REQUIRE( result == u8{255} );
  }
}

//------------------------------------------------------------------------------
// Accessors
//------------------------------------------------------------------------------

TEST_CASE("image::get_pixel( std::size_t, std::size_t )", "[accessor]")
{
  // Arrange
  const auto sut = *image::make_blank_image(
    image::dimensions{1u, 1u},
    image::pixel_format::rgb,
    color{0.0f, 0.0f, 0.0f, 0.0f}
  );

  // Act
  const auto result = sut.get_pixel(0u, 0u).a();

  // Assert
  SECTION("Pixel of a format without alpha is opaque")
  {
    REQUIRE( result == real{1} );
  }
}

} // namespace alloy::core::test
