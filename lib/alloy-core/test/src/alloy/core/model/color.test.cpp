// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/model/color.hpp>

#include <alloy/core/model/color_constants.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/types.hpp>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <string>

namespace Catch {
  template<>
  struct StringMaker<alloy::core::color> {
    // Prints 'color' as 4 8-bit component colors.
    // This could also be 16-bit or even floating point -- but neither of these
    // are particularly easy to read.
    static auto convert(const alloy::core::color& value) -> std::string
    {
      return (
        "rgba(" +
          std::to_string(value.r8()) + "," +
          std::to_string(value.g8()) + "," +
          std::to_string(value.b8()) + "," +
          std::to_string(value.a8()) +
        ")"
      );
    }
  };
}

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

// The below tests are assuming that a u8 value can survive a float conversion
// and back when normalized to the range [0,1]. This _should_ hold true for ieee
// floating point; but may need to be disabled if ever ported to different
// hardware. Such tests are labeled "inexact"

TEST_CASE("color::from_rgb8(u8,u8,u8)", "[factory][inexact]") {
  // Arrange
  constexpr auto red   = u8{42};
  constexpr auto green = u8{0x1e};
  constexpr auto blue  = u8{0xe7};

  // Act
  const auto sut = color::from_rgb8(red,green,blue);

  // Assert
  SECTION("Constructed color's red channel is correct") {
    REQUIRE(sut.r8() == red);
  }
  SECTION("Constructed color's green channel is correct") {
    REQUIRE(sut.g8() == green);
  }
  SECTION("Constructed color's blue channel is correct") {
    REQUIRE(sut.b8() == blue);
  }
  SECTION("Constructed color is opaque") {
    REQUIRE(sut.a8() == u8{255u});
  }
}

TEST_CASE("color::from_rgba8(u8,u8,u8,u8)", "[factory][inexact]") {
  // Arrange
  constexpr auto red   = u8{42};
  constexpr auto green = u8{0x1e};
  constexpr auto blue  = u8{0xe7};
  constexpr auto alpha = u8{10};

  // Act
  const auto sut = color::from_rgba8(red,green,blue,alpha);

  // Assert
  SECTION("Constructed color's red channel is correct") {
    REQUIRE(sut.r8() == red);
  }
  SECTION("Constructed color's green channel is correct") {
    REQUIRE(sut.g8() == green);
  }
  SECTION("Constructed color's blue channel is correct") {
    REQUIRE(sut.b8() == blue);
  }
  SECTION("Constructed color's alpha channel is correct") {
    REQUIRE(sut.a8() == alpha);
  }
}

TEST_CASE("color::from_rgb16(u16,u16,u16)", "[factory][inexact]") {
  // Arrange
  constexpr auto red   = u16{42};
  constexpr auto green = u16{0x1ee7};
  constexpr auto blue  = u16{0xdead};

  // Act
  const auto sut = color::from_rgb16(red,green,blue);

  // Assert
  SECTION("Constructed color's red channel is correct") {
    REQUIRE(sut.r16() == red);
  }
  SECTION("Constructed color's green channel is correct") {
    REQUIRE(sut.g16() == green);
  }
  SECTION("Constructed color's blue channel is correct") {
    REQUIRE(sut.b16() == blue);
  }
  SECTION("Constructed color is opaque") {
    REQUIRE(sut.a16() == u16{65535u});
  }
}

TEST_CASE("color::from_rgba16(u16,u16,u16,u16)", "[factory][inexact]") {
  // Arrange
  constexpr auto red   = u16{42};
  constexpr auto green = u16{0x1ee7};
  constexpr auto blue  = u16{0xdead};
  constexpr auto alpha = u16{0xbeef};

  // Act
  const auto sut = color::from_rgba16(red,green,blue,alpha);

  // Assert
  SECTION("Constructed color's red channel is correct") {
    REQUIRE(sut.r16() == red);
  }
  SECTION("Constructed color's green channel is correct") {
    REQUIRE(sut.g16() == green);
  }
  SECTION("Constructed color's blue channel is correct") {
    REQUIRE(sut.b16() == blue);
  }
  SECTION("Constructed color's alpha channel is correct") {
    REQUIRE(sut.a16() == alpha);
  }
}
//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------


TEST_CASE("color::color()","[ctor]") {
  SECTION("Default constructed color is transparent black") {
    // Arrange
    auto expected = color_constants::black;
    expected.a8() = 0u;

    // Act
    const auto sut = color{};

    // Assert
    REQUIRE(sut == expected);
  }
}

TEST_CASE("color::color(real r, real g, real b)","[ctor]") {
  // Arrange
  constexpr auto r = 0.25f;
  constexpr auto g = 0.5f;
  constexpr auto b = 0.75f;

  // Act
  const auto sut = color{r,g,b};

  // Assert
  SECTION("Alpha is opaque by default") {
    REQUIRE(sut.a() == 1.0f);
  }
  SECTION("Color is set to input") {
    SECTION("Red") {
      REQUIRE(sut.r() == r);
    }
    SECTION("Green") {
      REQUIRE(sut.g() == g);
    }
    SECTION("Blue") {
      REQUIRE(sut.b() == b);
    }
  }
}

TEST_CASE("color::color(real r, real g, real b, real a)","[ctor]") {
  // Arrange
  constexpr auto r = 0.25f;
  constexpr auto g = 0.5f;
  constexpr auto b = 0.75f;
  constexpr auto a = 0.125f;

  // Act
  const auto sut = color{r,g,b,a};

  // Assert
  SECTION("Color is set to input") {
    SECTION("Red") {
      REQUIRE(sut.r() == r);
    }
    SECTION("Green") {
      REQUIRE(sut.g() == g);
    }
    SECTION("Blue") {
      REQUIRE(sut.b() == b);
    }
    SECTION("Alpha") {
      REQUIRE(sut.a() == a);
    }
  }
}

TEST_CASE("color::inverted()", "[modifier]") {
  // Arrange
  constexpr auto r = 0.0f;
  constexpr auto g = 0.25f;
  constexpr auto b = 0.5f;
  const auto sut = color{r,g,b};

  // Act
  const auto result = sut.inverted();

  SECTION("Color components are inverted") {
    SECTION("Red is inverted") {
      constexpr auto expected = (1.0f - r);

      // Assert
      REQUIRE(result.r() == expected);
    }
    SECTION("Green is inverted") {
      constexpr auto expected = (1.0f - g);

      // Assert
      REQUIRE(result.g() == expected);
    }
    SECTION("Blue is inverted") {
      constexpr auto expected = (1.0f - b);

      // Assert
      REQUIRE(result.b() == expected);
    }
  }

  // Assert
  SECTION("Alpha is untouched") {
    REQUIRE(sut.a() == result.a());
  }
}

TEST_CASE("color hsb conversion", "[conversion][parametric]") {
  SECTION("Opaque colors survive round trip through hsb conversions") {

    // Convert all color constants
    // Arrange
    const auto input = GENERATE(
      color_constants::white,
      color_constants::silver,
      color_constants::gray,
      color_constants::black,
      color_constants::red,
      color_constants::maroon,
      color_constants::lime,
      color_constants::green,
      color_constants::blue,
      color_constants::navy,
      color_constants::yellow,
      color_constants::orange,
      color_constants::olive,
      color_constants::purple,
      color_constants::fuchsia,
      color_constants::teal,
      color_constants::aqua,
      color_constants::indian_red,
      color_constants::light_coral,
      color_constants::salmon,
      color_constants::dark_salmon,
      color_constants::light_salmon,
      color_constants::crimson,
      color_constants::fire_brick,
      color_constants::dark_red,
      color_constants::pink,
      color_constants::light_pink,
      color_constants::hot_pink,
      color_constants::deep_pink,
      color_constants::medium_violet_red,
      color_constants::pale_violet_red,
      color_constants::coral,
      color_constants::tomato,
      color_constants::orange_red,
      color_constants::dark_orange,
      color_constants::gold,
      color_constants::light_yellow,
      color_constants::lemon_chiffon,
      color_constants::light_goldenrod_yellow,
      color_constants::papaya_whip,
      color_constants::moccasin,
      color_constants::peach_puff,
      color_constants::pale_goldenrod,
      color_constants::khaki,
      color_constants::dark_khaki,
      color_constants::lavender,
      color_constants::thistle,
      color_constants::plum,
      color_constants::violet,
      color_constants::orchid,
      color_constants::magenta,
      color_constants::medium_orchid,
      color_constants::medium_purple,
      color_constants::blue_violet,
      color_constants::dark_violet,
      color_constants::dark_orchid,
      color_constants::dark_magenta,
      color_constants::rebecca_purple,
      color_constants::indigo,
      color_constants::medium_slate_blue,
      color_constants::slate_blue,
      color_constants::dark_slate_blue,
      color_constants::green_yellow,
      color_constants::chartreuse,
      color_constants::lawn_green,
      color_constants::lime_green,
      color_constants::pale_green,
      color_constants::light_green,
      color_constants::medium_spring_green,
      color_constants::spring_green,
      color_constants::medium_sea_green,
      color_constants::sea_green,
      color_constants::forest_green,
      color_constants::dark_green,
      color_constants::yellow_green,
      color_constants::olive_drab,
      color_constants::dark_olive_green,
      color_constants::medium_aquamarine,
      color_constants::dark_sea_green,
      color_constants::light_sea_green,
      color_constants::dark_cyan,
      color_constants::cyan,
      color_constants::light_cyan,
      color_constants::pale_turquoise,
      color_constants::aquamarine,
      color_constants::turquoise,
      color_constants::medium_turquoise,
      color_constants::dark_turquoise,
      color_constants::cadet_blue,
      color_constants::steel_blue,
      color_constants::light_steel_blue,
      color_constants::powder_blue,
      color_constants::light_blue,
      color_constants::sky_blue,
      color_constants::light_sky_blue,
      color_constants::deep_sky_blue,
      color_constants::dodger_blue,
      color_constants::cornflower_blue,
      color_constants::royal_blue,
      color_constants::medium_blue,
      color_constants::dark_blue,
      color_constants::midnight_blue,
      color_constants::cornsilk,
      color_constants::blanched_almond,
      color_constants::bisque,
      color_constants::navajo_white,
      color_constants::wheat,
      color_constants::burly_wood,
      color_constants::tan,
      color_constants::rosy_brown,
      color_constants::sandy_brown,
      color_constants::goldenrod,
      color_constants::dark_goldenrod,
      color_constants::peru,
      color_constants::chocolate,
      color_constants::saddle_brown,
      color_constants::sienna,
      color_constants::brown,
      color_constants::snow,
      color_constants::honeydew,
      color_constants::mint_cream,
      color_constants::azure,
      color_constants::alice_blue,
      color_constants::ghost_white,
      color_constants::white_smoke,
      color_constants::seashell,
      color_constants::beige,
      color_constants::old_lace,
      color_constants::floral_white,
      color_constants::ivory,
      color_constants::antique_white,
      color_constants::linen,
      color_constants::lavender_blush,
      color_constants::misty_rose,
      color_constants::gainsboro,
      color_constants::light_gray,
      color_constants::dark_gray,
      color_constants::dim_gray,
      color_constants::light_slate_gray,
      color_constants::slate_gray,
      color_constants::dark_slate_gray
    );

    // Unfortunately, Catch::StringMaker does not work in the INFO macros; so
    // just call the function manually and concatenate the strings to produce
    // the messages
    const auto& to_string = &Catch::StringMaker<color>::convert;

    SECTION("With input '" + to_string(input) + "'") {
      // Act
      const auto hsb = input.to_hsb();
      const auto output = color::from_hsb(hsb);

      // Assert
      INFO("output = " + to_string(output));

      // Assert
      REQUIRE(almost_equal(input, output));
    }
  }
}

} // namespace alloy::core::test
