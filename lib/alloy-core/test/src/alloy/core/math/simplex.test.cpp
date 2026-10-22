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
    REQUIRE( lowest >= real{-1} );
    REQUIRE( highest <= real{1} );
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

} // namespace alloy::core::test
