// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/texture/texture_spec.hpp>

#include <alloy/render/error.hpp>
#include <alloy/render/texture/texture_format.hpp>

#include <alloy/core/model/color.hpp>
#include <alloy/core/model/image.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef> // std::byte
#include <vector>  // std::vector

namespace alloy::render::test {
namespace {

auto make_image(core::image::pixel_format format) -> core::image
{
  return core::image::make_blank_image(
    {.width = 4u, .height = 2u},
    format,
    core::color{0.0f, 0.0f, 0.0f}
  ).value();
}

} // namespace <anonymous>

TEST_CASE("texture_spec::texture_2d(...)", "[factory][static]") {
  SECTION("Without initial data has a single mip level") {
    // Act
    const auto sut = texture_spec::texture_2d(texture_format::r8_unorm, 4u, 2u);

    // Assert
    REQUIRE(sut->mip_levels() == 1u);
  }
  SECTION("Zero width") {
    // Act
    const auto sut = texture_spec::texture_2d(texture_format::r8_unorm, 0u, 2u);

    // Assert
    REQUIRE(sut.error().code() == render_errc::invalid_texture_spec);
  }
  SECTION("Depth format") {
    // Act
    const auto sut = texture_spec::texture_2d(
      texture_format::depth32_float, 4u, 2u
    );

    // Assert
    REQUIRE(sut.error().code() == render_errc::invalid_texture_spec);
  }
  SECTION("Initial data that matches the extent") {
    // Arrange
    const auto data = std::vector<std::byte>(32u);

    // Act
    const auto sut = texture_spec::texture_2d(
      texture_format::rgba8_unorm, 4u, 2u, data
    );

    // Assert
    REQUIRE(sut.has_value());
  }
  SECTION("Initial data that does not match the extent") {
    // Arrange
    const auto data = std::vector<std::byte>(31u);

    // Act
    const auto sut = texture_spec::texture_2d(
      texture_format::rgba8_unorm, 4u, 2u, data
    );

    // Assert
    REQUIRE(sut.error().code() == render_errc::invalid_texture_spec);
  }
  SECTION("Full chain uses the largest of width and height") {
    // Act
    const auto sut = texture_spec::texture_2d(
      texture_format::r8_unorm, 16u, 64u, {}, mip_chain::full
    );

    // Assert
    REQUIRE(sut->mip_levels() == 7u);
  }
  SECTION("Full chain initial data holds every level") {
    // Arrange
    const auto data = std::vector<std::byte>(8u + 2u + 1u);

    // Act
    const auto sut = texture_spec::texture_2d(
      texture_format::r8_unorm, 4u, 2u, data, mip_chain::full
    );

    // Assert
    REQUIRE(sut.has_value());
  }
  SECTION("Generated chain initial data holds only the base level") {
    // Arrange
    const auto data = std::vector<std::byte>(8u);

    // Act
    const auto sut = texture_spec::texture_2d(
      texture_format::r8_unorm, 4u, 2u, data, mip_chain::generated
    );

    // Assert
    REQUIRE(sut->generates_mipmaps());
  }
  SECTION("Generated chain without initial data") {
    // Act
    const auto sut = texture_spec::texture_2d(
      texture_format::r8_unorm, 4u, 2u, {}, mip_chain::generated
    );

    // Assert
    REQUIRE(sut.error().code() == render_errc::invalid_texture_spec);
  }
  SECTION("Generated chain of an integer format") {
    // Arrange
    const auto data = std::vector<std::byte>(32u);

    // Act
    const auto sut = texture_spec::texture_2d(
      texture_format::r32_uint, 4u, 2u, data, mip_chain::generated
    );

    // Assert
    REQUIRE(sut.error().code() == render_errc::invalid_texture_spec);
  }
}

TEST_CASE("texture_spec::texture_2d_array(...)", "[factory][static]") {
  SECTION("Zero layers") {
    // Act
    const auto sut = texture_spec::texture_2d_array(
      texture_format::r8_unorm, 4u, 4u, 0u
    );

    // Assert
    REQUIRE(sut.error().code() == render_errc::invalid_texture_spec);
  }
}

TEST_CASE("texture_spec::texture_3d(...)", "[factory][static]") {
  SECTION("Full chain includes the depth") {
    // Act
    const auto sut = texture_spec::texture_3d(
      texture_format::r8_unorm, 4u, 4u, 32u, {}, mip_chain::full
    );

    // Assert
    REQUIRE(sut->mip_levels() == 6u);
  }
  SECTION("Full chain initial data shrinks the depth") {
    // Arrange
    const auto data = std::vector<std::byte>(8u + 1u);

    // Act
    const auto sut = texture_spec::texture_3d(
      texture_format::r8_unorm, 2u, 2u, 2u, data, mip_chain::full
    );

    // Assert
    REQUIRE(sut.has_value());
  }
}

TEST_CASE("texture_spec::cube(...)", "[factory][static]") {
  SECTION("Has a layer per face") {
    // Act
    const auto sut = texture_spec::cube(texture_format::r8_unorm, 2u);

    // Assert
    REQUIRE(sut->array_layers() == 6u);
  }
  SECTION("Initial data holds every face") {
    // Arrange
    const auto data = std::vector<std::byte>(24u);

    // Act
    const auto sut = texture_spec::cube(texture_format::r8_unorm, 2u, data);

    // Assert
    REQUIRE(sut.has_value());
  }
}

TEST_CASE("texture_spec::cube_array(...)", "[factory][static]") {
  SECTION("Has a layer per face of every cube") {
    // Act
    const auto sut = texture_spec::cube_array(texture_format::r8_unorm, 2u, 3u);

    // Assert
    REQUIRE(sut->array_layers() == 18u);
  }
  SECTION("Zero cubes") {
    // Act
    const auto sut = texture_spec::cube_array(texture_format::r8_unorm, 2u, 0u);

    // Assert
    REQUIRE(sut.error().code() == render_errc::invalid_texture_spec);
  }
}

TEST_CASE("texture_spec::render_target(...)", "[factory][static]") {
  SECTION("Depth format is usable as a depth target") {
    // Act
    const auto sut = texture_spec::render_target(
      texture_format::depth32_float, 16u, 16u
    );

    // Assert
    REQUIRE(sut->usage().test(texture_usage::depth_stencil));
  }
  SECTION("Color format is usable as a color target") {
    // Act
    const auto sut = texture_spec::render_target(
      texture_format::rgba8_unorm, 16u, 16u
    );

    // Assert
    REQUIRE(sut->usage().test(texture_usage::render_target));
  }
  SECTION("Is sampled") {
    // Act
    const auto sut = texture_spec::render_target(
      texture_format::rgba8_unorm, 16u, 16u
    );

    // Assert
    REQUIRE(sut->usage().test(texture_usage::sampled));
  }
  SECTION("Keeps the sample count") {
    // Act
    const auto sut = texture_spec::render_target(
      texture_format::rgba8_unorm, 16u, 16u, sample_count::x4
    );

    // Assert
    REQUIRE(sut->samples() == sample_count::x4);
  }
  SECTION("Zero height") {
    // Act
    const auto sut = texture_spec::render_target(
      texture_format::rgba8_unorm, 16u, 0u
    );

    // Assert
    REQUIRE(sut.error().code() == render_errc::invalid_texture_spec);
  }
}

TEST_CASE("texture_spec::from_image(const core::image&)", "[factory][static]") {
  SECTION("RGBA image") {
    // Arrange
    const auto image = make_image(core::image::pixel_format::rgba);

    // Act
    const auto sut = texture_spec::from_image(image);

    // Assert
    REQUIRE(sut->format() == texture_format::rgba8_unorm);
  }
  SECTION("BGRA image") {
    // Arrange
    const auto image = make_image(core::image::pixel_format::bgra);

    // Act
    const auto sut = texture_spec::from_image(image);

    // Assert
    REQUIRE(sut->format() == texture_format::bgra8_unorm);
  }
  SECTION("RGBA image views every pixel") {
    // Arrange
    const auto image = make_image(core::image::pixel_format::rgba);

    // Act
    const auto sut = texture_spec::from_image(image);

    // Assert
    REQUIRE(sut->initial_data().size() == 32u);
  }
  SECTION("RGB image") {
    // Arrange
    const auto image = make_image(core::image::pixel_format::rgb);

    // Act
    const auto sut = texture_spec::from_image(image);

    // Assert
    REQUIRE(sut.error().code() == render_errc::unsupported_texture_format);
  }
  SECTION("BGR image") {
    // Arrange
    const auto image = make_image(core::image::pixel_format::bgr);

    // Act
    const auto sut = texture_spec::from_image(image);

    // Assert
    REQUIRE(sut.error().code() == render_errc::unsupported_texture_format);
  }
  SECTION("Generated chain views only the base level") {
    // Arrange
    const auto image = make_image(core::image::pixel_format::rgba);

    // Act
    const auto sut = texture_spec::from_image(image, mip_chain::generated);

    // Assert
    REQUIRE(sut->initial_data().size() == 32u);
  }
}

TEST_CASE("texture_spec::mip_extent(std::uint32_t)", "[observer]") {
  SECTION("Halves each axis per level") {
    // Arrange
    const auto sut = texture_spec::texture_2d(
      texture_format::r8_unorm, 16u, 8u, {}, mip_chain::full
    ).value();

    // Act
    const auto result = sut.mip_extent(2u);

    // Assert
    REQUIRE(result.width == 4u);
  }
  SECTION("Clamps each axis to 1") {
    // Arrange
    const auto sut = texture_spec::texture_2d(
      texture_format::r8_unorm, 16u, 8u, {}, mip_chain::full
    ).value();

    // Act
    const auto result = sut.mip_extent(4u);

    // Assert
    REQUIRE(result.height == 1u);
  }
}

TEST_CASE("texture_spec::mip_size(std::uint32_t)", "[observer]") {
  SECTION("Includes every array layer") {
    // Arrange
    const auto sut = texture_spec::cube(texture_format::rgba8_unorm, 2u).value();

    // Act
    const auto result = sut.mip_size(0u);

    // Assert
    REQUIRE(result == 96u);
  }
}

} // namespace alloy::render::test
