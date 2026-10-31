// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/simplex.hpp>

#include <alloy/core/math/math.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm> // std::ranges::max, std::ranges::minmax
#include <vector>    // std::vector

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Raw Noise
//------------------------------------------------------------------------------

TEST_CASE("simplex::raw_noise( float, float )", "[noise]")
{
  SECTION("Noise is within [-1, 1]")
  {
    // Act
    auto samples = std::vector<float>{};
    for (auto i = -40; i <= 40; ++i) {
      for (auto j = -40; j <= 40; ++j) {
        const auto x = static_cast<float>(i) * 0.13f;
        const auto y = static_cast<float>(j) * 0.17f;

        samples.push_back(simplex::raw_noise(x, y));
      }
    }
    const auto [lowest, highest] = std::ranges::minmax(samples);

    // Assert
    SECTION("Never falls below the normalized lower bound") {
      REQUIRE( lowest >= -1.0f );
    }
    SECTION("Never exceeds the normalized upper bound") {
      REQUIRE( highest <= 1.0f );
    }
  }

  SECTION("Noise is continuous")
  {
    // Arrange
    const auto step = 1e-3f;

    // Act
    auto differences = std::vector<float>{};
    for (auto i = -400; i <= 400; ++i) {
      const auto x = static_cast<float>(i) * 0.0123f;
      const auto y = 0.37f - x;

      differences.push_back(
        abs(simplex::raw_noise(x + step, y) - simplex::raw_noise(x, y))
      );
    }
    const auto largest = std::ranges::max(differences);

    // Assert
    REQUIRE( largest < 0.05f );
  }

  SECTION("Noise is 0 at the corners of the simplex grid")
  {
    // Arrange
    // The grid corners are the integer points of the skewed space, unskewed
    // by (3 - sqrt(3)) / 6 per unit of i + j.
    const auto unskew = (3.0f - sqrt(3.0f)) / 6.0f;

    // Act
    auto magnitudes = std::vector<float>{};
    for (auto i = -3; i <= 3; ++i) {
      for (auto j = -3; j <= 3; ++j) {
        const auto t = static_cast<float>(i + j) * unskew;
        const auto x = static_cast<float>(i) - t;
        const auto y = static_cast<float>(j) - t;

        magnitudes.push_back(abs(simplex::raw_noise(x, y)));
      }
    }
    const auto largest = std::ranges::max(magnitudes);

    // Assert
    REQUIRE( largest < 1e-3f );
  }

  SECTION("Noise is deterministic")
  {
    // Arrange
    const auto x = 1.5f;
    const auto y = -2.25f;

    // Act
    const auto first = simplex::raw_noise(x, y);
    const auto second = simplex::raw_noise(x, y);

    // Assert
    REQUIRE( first == second );
  }
}

TEST_CASE("simplex::raw_noise( float, float, float )", "[noise]")
{
  SECTION("Noise is within [-1, 1]")
  {
    // Act
    auto samples = std::vector<float>{};
    for (auto i = -12; i <= 12; ++i) {
      for (auto j = -12; j <= 12; ++j) {
        for (auto k = -12; k <= 12; ++k) {
          const auto x = static_cast<float>(i) * 0.13f;
          const auto y = static_cast<float>(j) * 0.17f;
          const auto z = static_cast<float>(k) * 0.19f;

          samples.push_back(simplex::raw_noise(x, y, z));
        }
      }
    }
    const auto [lowest, highest] = std::ranges::minmax(samples);

    // Assert
    SECTION("Never falls below the normalized lower bound") {
      REQUIRE( lowest >= -1.0f );
    }
    SECTION("Never exceeds the normalized upper bound") {
      REQUIRE( highest <= 1.0f );
    }
  }

  SECTION("Noise is continuous")
  {
    // Arrange
    const auto step = 1e-3f;

    // Act
    auto differences = std::vector<float>{};
    for (auto i = -400; i <= 400; ++i) {
      const auto x = static_cast<float>(i) * 0.0123f;
      const auto y = 0.37f - x;
      const auto z = 0.5f * x;

      differences.push_back(
        abs(simplex::raw_noise(x + step, y, z) - simplex::raw_noise(x, y, z))
      );
    }
    const auto largest = std::ranges::max(differences);

    // Assert
    REQUIRE( largest < 0.05f );
  }

  SECTION("Noise is 0 at the corners of the simplex grid")
  {
    // Arrange
    // The grid corners are the integer points of the skewed space, unskewed
    // by 1 / 6 per unit of i + j + k.
    const auto unskew = 1.0f / 6.0f;

    // Act
    auto magnitudes = std::vector<float>{};
    for (auto i = -2; i <= 2; ++i) {
      for (auto j = -2; j <= 2; ++j) {
        for (auto k = -2; k <= 2; ++k) {
          const auto t = static_cast<float>(i + j + k) * unskew;
          const auto x = static_cast<float>(i) - t;
          const auto y = static_cast<float>(j) - t;
          const auto z = static_cast<float>(k) - t;

          magnitudes.push_back(abs(simplex::raw_noise(x, y, z)));
        }
      }
    }
    const auto largest = std::ranges::max(magnitudes);

    // Assert
    REQUIRE( largest < 1e-3f );
  }

  SECTION("Noise varies between the corners of the grid")
  {
    // Act
    auto samples = std::vector<float>{};
    for (auto i = 0; i < 1000; ++i) {
      const auto x = static_cast<float>(i) * 0.0371f;

      samples.push_back(simplex::raw_noise(x, 0.25f * x, 1.3f));
    }
    const auto [lowest, highest] = std::ranges::minmax(samples);

    // Assert
    SECTION("Produces substantial negative variation") {
      REQUIRE( lowest < -0.3f );
    }
    SECTION("Produces substantial positive variation") {
      REQUIRE( highest > 0.3f );
    }
  }
}

//------------------------------------------------------------------------------
// Octave Noise
//------------------------------------------------------------------------------

TEST_CASE("simplex::octave_noise( float, float, float, float, float )", "[noise]")
{
  SECTION("A single octave is the raw noise at the scaled position")
  {
    // Arrange
    const auto expected = simplex::raw_noise(0.6f, 1.4f);

    // Act
    const auto result = simplex::octave_noise(
      1.0f, 0.5f, 2.0f, 0.3f, 0.7f
    );

    // Assert
    REQUIRE( result == expected );
  }

  SECTION("Several octaves stay within [-1, 1]")
  {
    // Act
    auto samples = std::vector<float>{};
    for (auto i = -40; i <= 40; ++i) {
      const auto x = static_cast<float>(i) * 0.13f;

      samples.push_back(simplex::octave_noise(
        4.0f, 0.5f, 1.0f, x, 0.25f
      ));
    }
    const auto [lowest, highest] = std::ranges::minmax(samples);

    // Assert
    SECTION("Never falls below the normalized lower bound") {
      REQUIRE( lowest >= -1.0f );
    }
    SECTION("Never exceeds the normalized upper bound") {
      REQUIRE( highest <= 1.0f );
    }
  }
}

} // namespace alloy::core::test
