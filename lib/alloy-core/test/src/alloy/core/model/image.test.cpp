// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/model/image.hpp>

#include <alloy/core/containers/vector.hpp>
#include <alloy/core/model/color.hpp>
#include <alloy/core/types.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef> // std::byte, std::size_t
#include <utility> // std::move

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

TEST_CASE("image::make_image_from_buffer( vector<std::byte>, dimensions, "
          "pixel_format )", "[factory]")
{
  SECTION("Buffer that matches the dimensions makes an image")
  {
    // Arrange
    auto buffer = vector<std::byte>(std::size_t{2u} * 3u * 4u);

    // Act
    const auto result = image::make_image_from_buffer(
      std::move(buffer),
      image::dimensions{2u, 3u},
      image::pixel_format::rgba
    );

    // Assert
    REQUIRE( result.has_value() );
  }
  SECTION("Zero width is a bad width")
  {
    // Arrange
    auto buffer = vector<std::byte>{};

    // Act
    const auto result = image::make_image_from_buffer(
      std::move(buffer),
      image::dimensions{0u, 3u},
      image::pixel_format::rgba
    );

    // Assert
    REQUIRE( result.error() == image::error_code::bad_width );
  }
  SECTION("Zero height is a bad height")
  {
    // Arrange
    auto buffer = vector<std::byte>{};

    // Act
    const auto result = image::make_image_from_buffer(
      std::move(buffer),
      image::dimensions{2u, 0u},
      image::pixel_format::rgba
    );

    // Assert
    REQUIRE( result.error() == image::error_code::bad_height );
  }
  SECTION("Buffer that does not match the dimensions is incorrect")
  {
    // Arrange
    auto buffer = vector<std::byte>(std::size_t{2u} * 3u * 3u);

    // Act
    const auto result = image::make_image_from_buffer(
      std::move(buffer),
      image::dimensions{2u, 3u},
      image::pixel_format::rgba
    );

    // Assert
    REQUIRE( result.error() == image::error_code::incorrect_dimensions );
  }
  SECTION("Unknown pixel format is invalid")
  {
    // Arrange
    auto buffer = vector<std::byte>(std::size_t{2u} * 3u * 4u);

    // Act
    const auto result = image::make_image_from_buffer(
      std::move(buffer),
      image::dimensions{2u, 3u},
      static_cast<image::pixel_format>(42)
    );

    // Assert
    REQUIRE( result.error() == image::error_code::invalid_pixel_format );
  }
}

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

TEST_CASE("image::set_pixel( std::size_t, std::size_t, color ) "
          "with pixel_format::bgr", "[modifier]")
{
  // Arrange
  auto sut = *image::make_blank_image(
    image::dimensions{2u, 2u},
    image::pixel_format::bgr,
    color{0.0f, 0.0f, 0.0f, 1.0f}
  );

  // Act
  sut.set_pixel(1u, 1u, color::from_rgb8(0x11u, 0x22u, 0x33u));
  const auto offset = ((std::size_t{1u} * 2u) + 1u) * 3u;
  const auto* const pixel = sut.data() + offset;

  // Assert
  SECTION("Stores blue first")
  {
    REQUIRE( pixel[0] == std::byte{0x33u} );
  }
  SECTION("Stores green second")
  {
    REQUIRE( pixel[1] == std::byte{0x22u} );
  }
  SECTION("Stores red third")
  {
    REQUIRE( pixel[2] == std::byte{0x11u} );
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
    REQUIRE( result == 1.0f );
  }
}

TEST_CASE("image::get_pixel( std::size_t, std::size_t ) "
          "with pixel_format::bgra", "[accessor]")
{
  // Arrange
  const auto expected = color::from_rgba8(0x11u, 0x22u, 0x33u, 0x44u);
  auto sut = *image::make_blank_image(
    image::dimensions{2u, 2u},
    image::pixel_format::bgra,
    color{0.0f, 0.0f, 0.0f, 1.0f}
  );
  sut.set_pixel(1u, 0u, expected);

  // Act
  const auto result = sut.get_pixel(1u, 0u);

  // Assert
  SECTION("Reads back the pixel that was set")
  {
    REQUIRE( result.to_rgba8() == expected.to_rgba8() );
  }
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

TEST_CASE("image::copy()", "[observer]")
{
  // Arrange
  const auto expected = color::from_rgba8(0x11u, 0x22u, 0x33u, 0x44u);
  auto sut = *image::make_blank_image(
    image::dimensions{2u, 2u},
    image::pixel_format::rgba,
    color{0.0f, 0.0f, 0.0f, 1.0f}
  );
  sut.set_pixel(1u, 1u, expected);

  // Act
  const auto result = sut.copy();

  // Assert
  SECTION("Copies the pixels")
  {
    REQUIRE( result.get_pixel(1u, 1u).to_rgba8() == expected.to_rgba8() );
  }
}

} // namespace alloy::core::test
