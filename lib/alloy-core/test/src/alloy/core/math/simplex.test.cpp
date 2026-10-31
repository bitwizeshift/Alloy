// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/simplex.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm> // std::ranges::max, std::ranges::minmax
#include <vector>    // std::vector

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Raw Noise
//------------------------------------------------------------------------------

TEST_CASE("simplex::raw_noise( real, real )", "[noise]")
{
  SECTION("Noise is within [-1, 1]")
  {
    // Act
    auto samples = std::vector<real>{};
    for (auto i = -40; i <= 40; ++i) {
      for (auto j = -40; j <= 40; ++j) {
        const auto x = static_cast<real>(i) * real{0.13f};
        const auto y = static_cast<real>(j) * real{0.17f};

        samples.push_back(simplex::raw_noise(x, y));
      }
    }
    const auto [lowest, highest] = std::ranges::minmax(samples);

    // Assert
    SECTION("Never falls below the normalized lower bound") {
      REQUIRE( lowest >= real{-1} );
    }
    SECTION("Never exceeds the normalized upper bound") {
      REQUIRE( highest <= real{1} );
    }
  }

  SECTION("Noise is continuous")
  {
    // Arrange
    const auto step = real{1e-3f};

    // Act
    auto differences = std::vector<real>{};
    for (auto i = -400; i <= 400; ++i) {
      const auto x = static_cast<real>(i) * real{0.0123f};
      const auto y = real{0.37f} - x;

      differences.push_back(
        abs(simplex::raw_noise(x + step, y) - simplex::raw_noise(x, y))
      );
    }
    const auto largest = std::ranges::max(differences);

    // Assert
    REQUIRE( largest < real{0.05f} );
  }

  SECTION("Noise is 0 at the corners of the simplex grid")
  {
    // Arrange
    // The grid corners are the integer points of the skewed space, unskewed
    // by (3 - sqrt(3)) / 6 per unit of i + j.
    const auto unskew = (real{3} - sqrt(real{3})) / real{6};

    // Act
    auto magnitudes = std::vector<real>{};
    for (auto i = -3; i <= 3; ++i) {
      for (auto j = -3; j <= 3; ++j) {
        const auto t = static_cast<real>(i + j) * unskew;
        const auto x = static_cast<real>(i) - t;
        const auto y = static_cast<real>(j) - t;

        magnitudes.push_back(abs(simplex::raw_noise(x, y)));
      }
    }
    const auto largest = std::ranges::max(magnitudes);

    // Assert
    REQUIRE( largest < real{1e-3f} );
  }

  SECTION("Noise is deterministic")
  {
    // Arrange
    const auto x = real{1.5f};
    const auto y = real{-2.25f};

    // Act
    const auto first = simplex::raw_noise(x, y);
    const auto second = simplex::raw_noise(x, y);

    // Assert
    REQUIRE( first == second );
  }
}

TEST_CASE("simplex::raw_noise( real, real, real )", "[noise]")
{
  SECTION("Noise is within [-1, 1]")
  {
    // Act
    auto samples = std::vector<real>{};
    for (auto i = -12; i <= 12; ++i) {
      for (auto j = -12; j <= 12; ++j) {
        for (auto k = -12; k <= 12; ++k) {
          const auto x = static_cast<real>(i) * real{0.13f};
          const auto y = static_cast<real>(j) * real{0.17f};
          const auto z = static_cast<real>(k) * real{0.19f};

          samples.push_back(simplex::raw_noise(x, y, z));
        }
      }
    }
    const auto [lowest, highest] = std::ranges::minmax(samples);

    // Assert
    SECTION("Never falls below the normalized lower bound") {
      REQUIRE( lowest >= real{-1} );
    }
    SECTION("Never exceeds the normalized upper bound") {
      REQUIRE( highest <= real{1} );
    }
  }

  SECTION("Noise is continuous")
  {
    // Arrange
    const auto step = real{1e-3f};

    // Act
    auto differences = std::vector<real>{};
    for (auto i = -400; i <= 400; ++i) {
      const auto x = static_cast<real>(i) * real{0.0123f};
      const auto y = real{0.37f} - x;
      const auto z = real{0.5f} * x;

      differences.push_back(
        abs(simplex::raw_noise(x + step, y, z) - simplex::raw_noise(x, y, z))
      );
    }
    const auto largest = std::ranges::max(differences);

    // Assert
    REQUIRE( largest < real{0.05f} );
  }

  SECTION("Noise is 0 at the corners of the simplex grid")
  {
    // Arrange
    // The grid corners are the integer points of the skewed space, unskewed
    // by 1 / 6 per unit of i + j + k.
    const auto unskew = real{1} / real{6};

    // Act
    auto magnitudes = std::vector<real>{};
    for (auto i = -2; i <= 2; ++i) {
      for (auto j = -2; j <= 2; ++j) {
        for (auto k = -2; k <= 2; ++k) {
          const auto t = static_cast<real>(i + j + k) * unskew;
          const auto x = static_cast<real>(i) - t;
          const auto y = static_cast<real>(j) - t;
          const auto z = static_cast<real>(k) - t;

          magnitudes.push_back(abs(simplex::raw_noise(x, y, z)));
        }
      }
    }
    const auto largest = std::ranges::max(magnitudes);

    // Assert
    REQUIRE( largest < real{1e-3f} );
  }

  SECTION("Noise varies between the corners of the grid")
  {
    // Act
    auto samples = std::vector<real>{};
    for (auto i = 0; i < 1000; ++i) {
      const auto x = static_cast<real>(i) * real{0.0371f};

      samples.push_back(simplex::raw_noise(x, real{0.25f} * x, real{1.3f}));
    }
    const auto [lowest, highest] = std::ranges::minmax(samples);

    // Assert
    SECTION("Produces substantial negative variation") {
      REQUIRE( lowest < real{-0.3f} );
    }
    SECTION("Produces substantial positive variation") {
      REQUIRE( highest > real{0.3f} );
    }
  }
}

//------------------------------------------------------------------------------
// Octave Noise
//------------------------------------------------------------------------------

TEST_CASE("simplex::octave_noise( real, real, real, real, real )", "[noise]")
{
  SECTION("A single octave is the raw noise at the scaled position")
  {
    // Arrange
    const auto expected = simplex::raw_noise(real{0.6f}, real{1.4f});

    // Act
    const auto result = simplex::octave_noise(
      real{1}, real{0.5f}, real{2}, real{0.3f}, real{0.7f}
    );

    // Assert
    REQUIRE( result == expected );
  }

  SECTION("Several octaves stay within [-1, 1]")
  {
    // Act
    auto samples = std::vector<real>{};
    for (auto i = -40; i <= 40; ++i) {
      const auto x = static_cast<real>(i) * real{0.13f};

      samples.push_back(simplex::octave_noise(
        real{4}, real{0.5f}, real{1}, x, real{0.25f}
      ));
    }
    const auto [lowest, highest] = std::ranges::minmax(samples);

    // Assert
    SECTION("Never falls below the normalized lower bound") {
      REQUIRE( lowest >= real{-1} );
    }
    SECTION("Never exceeds the normalized upper bound") {
      REQUIRE( highest <= real{1} );
    }
  }
}

} // namespace alloy::core::test
