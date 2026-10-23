// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/saturated.hpp>

#include <alloy/core/intrinsics.hpp>

#include <catch2/catch_test_macros.hpp>

#include <functional> // std::hash
#include <limits>     // std::numeric_limits
#include <type_traits>

namespace alloy::core::test {

//=============================================================================
// class : saturated
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

TEST_CASE("saturated<Float>::make(Float)", "[factory]") {
  SECTION("Input is above 1.0") {
    // Act
    const auto sut = saturated<float>::make(1.5f);

    // Assert
    SECTION("Value is clamped to 1.0") {
      REQUIRE(sut.value() == 1.0f);
    }
  }

  SECTION("Input is below 0.0") {
    // Act
    const auto sut = saturated<float>::make(-1.0f);

    // Assert
    SECTION("Value is clamped to 0.0") {
      REQUIRE(sut.value() == 0.0f);
    }
  }

  SECTION("Input is between 0.0 and 1.0") {
    // Arrange
    const auto value = 0.5f;

    // Act
    const auto sut = saturated<float>::make(value);

    // Assert
    SECTION("Value is passed through") {
      REQUIRE(sut.value() == value);
    }
  }

  SECTION("Input is NaN") {
    // Arrange
    const auto value = std::numeric_limits<float>::quiet_NaN();

    // Act
    const auto sut = saturated<float>::make(value);

    // Assert
    SECTION("Value saturates to 0.0") {
      REQUIRE(sut.value() == 0.0f);
    }
  }
}

TEST_CASE("saturated<Float>::make_unchecked(Float)", "[factory]") {
  // Arrange
  const auto value = 0.5f;

  // Act
  const auto sut = saturated<float>::make_unchecked(value);

  // Assert
  SECTION("Value is passed through (assumed valid)") {
    REQUIRE(sut.value() == value);
  }
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("saturated<Float>::saturated()", "[ctor]") {
  SECTION("Constructor is trivial") {
    // Act
    constexpr auto property_holds =
      std::is_trivially_constructible_v<saturated<float>>;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Constructs a default-initialized underlying float") {
    // Act
    const auto sut = saturated<float>{};

    // Assert
    REQUIRE(sut.value() == 0.0f);
  }
}

TEST_CASE("saturated<Float>::saturated(const saturated&)", "[ctor]") {
  SECTION("Constructor is trivial") {
    // Act
    constexpr auto property_holds =
      std::is_trivially_copy_constructible_v<saturated<float>>;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Copies underlying float") {
    // Arrange
    const auto source = saturated<float>::make(0.5);

    // Act
    const auto sut = source;

    // Assert
    REQUIRE(sut == source);
  }
}

TEST_CASE("saturated<Float>::saturated(const saturated<UFloat>&)", "[ctor]") {
  SECTION("Copies underlying float") {
    // Arrange
    const auto to_copy = saturated<double>::make_unchecked(0.5);

    // Act
    const auto sut = saturated<float>{to_copy};

    // Assert
    REQUIRE(sut == static_cast<float>(to_copy.value()));
  }
}

//-----------------------------------------------------------------------------
// Assignment
//-----------------------------------------------------------------------------

TEST_CASE("saturated<Float>::operator=(const saturated&)", "[assign]") {
  SECTION("Assignment is trivial") {
    // Act
    constexpr auto property_holds =
      std::is_trivially_copy_assignable_v<saturated<float>>;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Assigns the underlying value") {
    // Arrange
    const auto copy = saturated<float>::make(0.5);
    auto sut = saturated<float>{};
    compiler::unused(sut);

    // Act
    sut = copy;

    // Assert
    REQUIRE(sut == copy);
  }
}

//-----------------------------------------------------------------------------
// Arithmetic Operators
//-----------------------------------------------------------------------------

TEST_CASE("saturated<Float>::operator-()", "[arithmetic]") {
  SECTION("Always saturates to 0.0") {
    // Act
    const auto sut = -saturated<float>::make(0.5f);

    // Assert
    REQUIRE(sut == 0.0f);
  }
}

TEST_CASE("saturated<Float>::operator+()", "[arithmetic]") {
  SECTION("Returns the input") {
    // Arrange
    const auto copy = saturated<float>::make(0.5f);

    // Act
    const auto sut  = +copy;

    // Assert
    REQUIRE(sut == copy);
  }
}

TEST_CASE("saturated<Float>::operator+=(saturated<Float>)", "[arithmetic]") {
  SECTION("Sum exceeds 1.0") {
    SECTION("Saturates value to 1.0") {
      // Arrange
      auto sut = saturated<float>::make(0.9f);

      // Act
      sut += saturated<float>::make_unchecked(0.5f);

      // Assert
      REQUIRE(sut == 1.0f);
    }
  }
  SECTION("Sum does not exceed 1.0") {
    SECTION("Returns sum") {
      // Arrange
      const auto value = 0.25f;
      const auto expected = value + value;

      auto sut = saturated<float>::make(value);

      // Act
      sut += saturated<float>::make_unchecked(value);

      // Assert
      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("saturated<Float>::operator+=(element_type)", "[arithmetic]") {
  SECTION("Sum exceeds 1.0") {
    SECTION("Saturates value to 1.0") {
      // Arrange
      auto sut = saturated<float>::make(0.9f);

      // Act
      sut += 0.5f;

      // Assert
      REQUIRE(sut == 1.0f);
    }
  }
  SECTION("Sum does not exceed 1.0") {
    SECTION("Returns sum") {
      // Arrange
      const auto value = 0.25f;
      const auto expected = value + value;

      auto sut = saturated<float>::make(value);

      // Act
      sut += value;

      // Assert
      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("saturated<Float>::operator-=(saturated<Float>)", "[arithmetic]") {
  SECTION("Difference precedes 0.0") {
    SECTION("Saturates value to 0.0") {
      // Arrange
      auto sut = saturated<float>::make(0.1f);

      // Act
      sut -= saturated<float>::make_unchecked(0.9f);

      // Assert
      REQUIRE(sut == 0.0f);
    }
  }
  SECTION("Difference does not precede 0.0") {
    SECTION("Returns difference") {
      // Arrange
      const auto value = 0.25f;
      const auto expected = value - 0.1f;

      auto sut = saturated<float>::make(value);

      // Act
      sut -= saturated<float>::make_unchecked(0.1f);

      // Assert
      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("saturated<Float>::operator-=(element_type)", "[arithmetic]") {
  SECTION("Difference precedes 0.0") {
    SECTION("Saturates value to 0.0") {
      // Arrange
      auto sut = saturated<float>::make(0.1f);

      // Act
      sut -= 0.9f;

      // Assert
      REQUIRE(sut == 0.0f);
    }
  }
  SECTION("Difference does not precede 0.0") {
    SECTION("Returns difference") {
      // Arrange
      const auto value = 0.25f;
      const auto expected = value - 0.1f;

      auto sut = saturated<float>::make(value);

      // Act
      sut -= 0.1f;

      // Assert
      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("saturated<Float>::operator*=(saturated<Float>)", "[arithmetic]") {
  SECTION("Returns product") {
    // Arrange
    const auto value = 0.25f;
    const auto expected = value * value;

    auto sut = saturated<float>::make(value);

    // Act
    sut *= saturated<float>::make(value);

    // Assert
    REQUIRE(sut == expected);
  }
}

TEST_CASE("saturated<Float>::operator*=(element_type)", "[arithmetic]") {
  SECTION("Product is between 0.0 and 1.0") {
    SECTION("Returns product") {
      // Arrange
      const auto value = 0.25f;
      const auto expected = value * 1.5f;

      auto sut = saturated<float>::make(value);

      // Act
      sut *= 1.5f;

      // Assert
      REQUIRE(sut == expected);
    }
  }
  SECTION("Product exceeds 1.0") {
    SECTION("Returns 1.0") {
      // Arrange
      auto sut = saturated<float>::make(0.5f);

      // Act
      sut *= 100.0f;

      // Assert
      REQUIRE(sut == 1.0f);
    }
  }
  SECTION("Product precedes 0.0") {
    SECTION("Returns 0.0") {
      // Arrange
      auto sut = saturated<float>::make(0.5f);

      // Act
      sut *= -100.0f;

      // Assert
      REQUIRE(sut == 0.0f);
    }
  }
}

TEST_CASE("saturated<Float>::operator/=(saturated<Float>)", "[arithmetic]") {
  SECTION("Quotient exceeds 1.0") {
    SECTION("Saturates value to 1.0") {
      // Arrange
      auto sut = saturated<float>::make(0.5f);

      // Act
      sut /= saturated<float>::make(0.1f);

      // Assert
      REQUIRE(sut == 1.0f);
    }
  }
  SECTION("Quotient does not exceed 1.0") {
    SECTION("Returns quotient") {
      // Arrange
      const auto value = 0.25f;
      const auto expected = value / 0.9f;

      auto sut = saturated<float>::make(value);

      // Act
      sut /= saturated<float>::make(0.9f);

      // Assert
      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("saturated<Float>::operator/=(element_type)", "[arithmetic]") {
  SECTION("Quotient exceeds 1.0") {
    SECTION("Saturates value to 1.0") {
      // Arrange
      auto sut = saturated<float>::make(0.5f);

      // Act
      sut /= 0.1f;

      // Assert
      REQUIRE(sut == 1.0f);
    }
  }
  SECTION("Quotient precedes 0.0") {
    SECTION("Saturates value to 0.0") {
      // Arrange
      auto sut = saturated<float>::make(0.5f);

      // Act
      sut /= -1.0f;

      // Assert
      REQUIRE(sut == 0.0f);
    }
  }
  SECTION("Quotient does not exceed 1.0") {
    SECTION("Returns quotient") {
      // Arrange
      const auto value = 0.25f;
      const auto expected = value / 0.9f;

      auto sut = saturated<float>::make(value);

      // Act
      sut /= 0.9f;

      // Assert
      REQUIRE(sut == expected);
    }
  }
}

//=============================================================================
// non-member functions : class : saturated
//=============================================================================

//-----------------------------------------------------------------------------
// Comparison
//-----------------------------------------------------------------------------

TEST_CASE("operator==(saturated<Float>, saturated<Float>)", "[comparison]") {
  SECTION("Values are equal") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = lhs;

      // Act
      const auto result = lhs == rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Values are not equal") {
    SECTION("Returns false") {
      // Arrange
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      // Act
      const auto result = lhs == rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("operator!=(saturated<Float>, saturated<Float>)", "[comparison]") {
  SECTION("Values are equal") {
    SECTION("Returns false") {
      // Arrange
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = lhs;

      // Act
      const auto result = lhs != rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Values are not equal") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      // Act
      const auto result = lhs != rhs;

      // Assert
      REQUIRE(result);
    }
  }
}

TEST_CASE("operator<(saturated<Float>,saturated<Float>)", "[comparison]") {
  SECTION("lhs is less than rhs") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = saturated<float>::make(0.0f);
      const auto rhs = saturated<float>::make(0.5f);

      // Act
      const auto result = lhs < rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("rhs is less than lhs") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      // Act
      const auto result = lhs < rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("operator>(saturated<Float>,saturated<Float>)", "[comparison]") {
  SECTION("lhs is less than rhs") {
    SECTION("Returns false") {
      // Arrange
      const auto lhs = saturated<float>::make(0.0f);
      const auto rhs = saturated<float>::make(0.5f);

      // Act
      const auto result = lhs > rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("rhs is less than lhs") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      // Act
      const auto result = lhs > rhs;

      // Assert
      REQUIRE(result);
    }
  }
}

TEST_CASE("operator<=(saturated<Float>,saturated<Float>)", "[comparison]") {
  SECTION("lhs is less than rhs") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = saturated<float>::make(0.0f);
      const auto rhs = saturated<float>::make(0.5f);

      // Act
      const auto result = lhs <=rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("rhs is less than lhs") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      // Act
      const auto result = lhs <= rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("operator>=(saturated<Float>,saturated<Float>)", "[comparison]") {
  SECTION("lhs is less than rhs") {
    SECTION("Returns false") {
      // Arrange
      const auto lhs = saturated<float>::make(0.0f);
      const auto rhs = saturated<float>::make(0.5f);

      // Act
      const auto result = lhs >= rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("rhs is less than lhs") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      // Act
      const auto result = lhs >= rhs;

      // Assert
      REQUIRE(result);
    }
  }
}

//-----------------------------------------------------------------------------
// Hashing
//-----------------------------------------------------------------------------

TEST_CASE("std::hash<saturated<Float>>", "[hash]") {
  // Arrange
  const auto hasher = std::hash<saturated<float>>{};
  const auto value = saturated<float>::make_unchecked(0.5f);
  const auto expected = std::hash<float>{}(0.5f);

  // Act
  const auto result = hasher(value);

  // Assert
  SECTION("Hashes the underlying value") {
    REQUIRE(result == expected);
  }
}

//-----------------------------------------------------------------------------
// Comparisons
//-----------------------------------------------------------------------------

struct saturated_only_equality_comparable {
  friend auto operator==(saturated_only_equality_comparable, float) -> bool;
};

template <typename L, typename R>
concept saturated_greater_equal_comparable = requires(const L& lhs, const R& rhs) {
  lhs >= rhs;
};

TEST_CASE("operator>=(const T&, saturated<Float>)", "[comparison]") {
  SECTION("Requires T to be ordered against Float") {
    // Act
    constexpr auto property_holds = saturated_greater_equal_comparable<
        saturated_only_equality_comparable,
        saturated<float>
      >;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

} // namespace alloy::core::test
