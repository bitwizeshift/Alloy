/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include "alloy/core/math/angle/basic_angle.hpp"

#include <catch2/catch.hpp>

namespace alloy::core::test {
namespace {
struct base_100_unit {
  static inline constexpr auto revolution()
    noexcept -> real
  {
    return 100;
  }
};

struct base_200_unit {
  static inline constexpr auto revolution()
    noexcept -> real
  {
    return 200;
  }
};
} // namespace <anonymous>

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("basic_angle", "[requirements]") {
  SECTION("Is trivially destructible") {
    STATIC_REQUIRE(std::is_trivially_destructible_v<basic_angle<base_100_unit>>);
  }
  SECTION("Is trivially copyable") {
    STATIC_REQUIRE(std::is_trivially_copyable_v<basic_angle<base_100_unit>>);
  }
}

TEST_CASE("basic_angle::basic_angle()", "[ctor]") {
  // Arrange / Act
  const auto sut = basic_angle<base_100_unit>{};

  // Assert
  SECTION("Default-constructs with zero angle value") {
    REQUIRE(sut.value() == 0);
  }
}

TEST_CASE("basic_angle::basic_angle(real)", "[ctor]") {
  // Arrange
  const auto angle = real{50};

  // Act
  const auto sut = basic_angle<base_100_unit>{angle};

  // Assert
  SECTION("Constructs angle with specified value") {
    REQUIRE(sut.value() == angle);
  }
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

TEST_CASE("basic_angle::constrain()", "[modifier]") {
  SECTION("Angle is positive") {
    SECTION("Angle is more than 1 revolution") {
      // Arrange
      const auto revolutions = GENERATE(1.0f, 2.0f, 3.0f, 4.0f);
      const auto delta = (base_100_unit::revolution() / 4);
      const auto angle = (revolutions * base_100_unit::revolution()) + delta;
      const auto expected = basic_angle<base_100_unit>{delta};

      auto sut = basic_angle<base_100_unit>{angle};

      // Act
      sut.constrain();

      // Assert
      SECTION("Angle is constrained to within 1 positive revolution") {
        REQUIRE(almost_equal(sut, expected));
      }
    }
    SECTION("Angle is less than 1 revolution") {
      // Arrange
      const auto angle = (base_100_unit::revolution() / 4);
      const auto expected = basic_angle<base_100_unit>{angle};

      auto sut = basic_angle<base_100_unit>{angle};

      // Act
      sut.constrain();

      // Assert
      SECTION("Angle is constrained to within 1 positive revolution") {
        REQUIRE(almost_equal(sut, expected));
      }
    }
  }
  SECTION("Angle is negative") {
    SECTION("Angle is more than 1 revolution") {
      // Arrange
      const auto revolutions = GENERATE(1.0f, 2.0f, 3.0f, 4.0f);
      const auto delta = (base_100_unit::revolution() / 4);
      const auto angle = -((revolutions * base_100_unit::revolution()) + delta);
      const auto expected = basic_angle<base_100_unit>{
        base_100_unit::revolution() - delta
      };

      auto sut = basic_angle<base_100_unit>{angle};

      // Act
      sut.constrain();

      // Assert
      SECTION("Angle is constrained to within 1 positive revolution") {
        REQUIRE(almost_equal(sut, expected));
      }
    }
    SECTION("Angle is less than 1 revolution") {
      // Arrange
      const auto delta = (base_100_unit::revolution() / 4);
      const auto angle = -delta;
      const auto expected = basic_angle<base_100_unit>{
        base_100_unit::revolution() - delta
      };

      auto sut = basic_angle<base_100_unit>{angle};

      // Act
      sut.constrain();

      // Assert
      SECTION("Angle is constrained to within 1 positive revolution") {
        REQUIRE(almost_equal(sut, expected));
      }
    }
  }
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

TEST_CASE("basic_angle::constrained()", "[observers]") {
  SECTION("Angle is positive") {
    SECTION("Angle is more than 1 revolution") {
      // Arrange
      const auto revolutions = GENERATE(1.0f, 2.0f, 3.0f, 4.0f);
      const auto delta = (base_100_unit::revolution() / 4);
      const auto angle = (revolutions * base_100_unit::revolution()) + delta;
      const auto expected = basic_angle<base_100_unit>{delta};

      auto sut = basic_angle<base_100_unit>{angle};

      // Act
      const auto result = sut.constrained();

      // Assert
      SECTION("Angle is constrained to within 1 positive revolution") {
        REQUIRE(almost_equal(result, expected));
      }
    }
    SECTION("Angle is less than 1 revolution") {
      // Arrange
      const auto angle = (base_100_unit::revolution() / 4);
      const auto expected = basic_angle<base_100_unit>{angle};

      auto sut = basic_angle<base_100_unit>{angle};

      // Act
      const auto result = sut.constrained();

      // Assert
      SECTION("Angle is constrained to within 1 positive revolution") {
        REQUIRE(almost_equal(result, expected));
      }
    }
  }
  SECTION("Angle is negative") {
    SECTION("Angle is more than 1 revolution") {
      // Arrange
      const auto revolutions = GENERATE(1.0f, 2.0f, 3.0f, 4.0f);
      const auto delta = (base_100_unit::revolution() / 4);
      const auto angle = -((revolutions * base_100_unit::revolution()) + delta);
      const auto expected = basic_angle<base_100_unit>{
        base_100_unit::revolution() - delta
      };

      auto sut = basic_angle<base_100_unit>{angle};

      // Act
      const auto result = sut.constrained();

      // Assert
      SECTION("Angle is constrained to within 1 positive revolution") {
        REQUIRE(almost_equal(result, expected));
      }
    }
    SECTION("Angle is less than 1 revolution") {
      // Arrange
      const auto delta = (base_100_unit::revolution() / 4);
      const auto angle = -delta;
      const auto expected = basic_angle<base_100_unit>{
        base_100_unit::revolution() - delta
      };

      auto sut = basic_angle<base_100_unit>{angle};

      // Act
      const auto result = sut.constrained();

      // Assert
      SECTION("Angle is constrained to within 1 positive revolution") {
        REQUIRE(almost_equal(result, expected));
      }
    }
  }
}

TEST_CASE("base_angle::revolutions()", "[observers]") {
  // Arrange
  const auto revolutions = GENERATE(0.0f,1.0f,2.0f,3.0f,4.0f,5.0f);
  const auto angle = revolutions * base_100_unit::revolution();

  const auto sut = basic_angle<base_100_unit>{angle};

  // Act
  const auto result = std::floor(sut.revolutions());

  // Assert
  SECTION("Counts the number of revolutions") {
    REQUIRE(almost_equal(result, revolutions));
  }
}

//==============================================================================
// non-member functions : basic_angle
//==============================================================================

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

TEST_CASE("to_angle<To>(From)") {
  // Arrange
  using src_unit = base_100_unit;
  using src_angle = basic_angle<src_unit>;

  const auto revolutions = GENERATE(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
  const auto angle = (src_unit::revolution() / 4) * revolutions;

  const auto src = src_angle{angle};

  SECTION("Destination type is the same as source") {
    // Arrange
    using dest_unit = base_100_unit;
    using dest_angle = basic_angle<dest_unit>;

    // Act
    const auto result = to_angle<dest_angle>(src);

    // Assert
    SECTION("Returns input") {
      REQUIRE(result == src);
    }
  }
  SECTION("Destination type is different from source") {
    // Arrange
    using dest_unit = base_200_unit;
    using dest_angle = basic_angle<dest_unit>;

    const auto expected = dest_angle{angle * 2};

    // Act
    const auto result = to_angle<dest_angle>(src);

    // Assert
    SECTION("Converts to destination angle") {
      REQUIRE(almost_equal(result, expected));
    }
  }
}

} // namespace alloy::core::test
