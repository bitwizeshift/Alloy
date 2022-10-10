/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include "alloy/core/math/simd.hpp"
#include "alloy/core/math/math_constants.hpp"
#include "alloy/core/math/math.hpp"

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <array>

namespace alloy::core::test {

//-----------------------------------------------------------------------------
// Factories
//-----------------------------------------------------------------------------

TEMPLATE_TEST_CASE("simd<T>::load(span<f32,4>)", "[factory]", f32, u32, s32) {
  // Arrange
  const auto values = simd_aligned_array<TestType, 4>{{
    1, 2, 3, 4
  }};

  // Act
  const auto sut = simd<TestType>::load(values);

  // Assert
  SECTION("First lane contains correct value") {
    REQUIRE(sut.template lane<0>() == values[0]);
  }
  SECTION("Second lane contains correct value") {
    REQUIRE(sut.template lane<1>() == values[1]);
  }
  SECTION("Third lane contains correct value") {
    REQUIRE(sut.template lane<2>() == values[2]);
  }
  SECTION("Fourth lane contains correct value") {
    REQUIRE(sut.template lane<3>() == values[3]);
  }
}

TEMPLATE_TEST_CASE("simd<T>::load_unaligned(span<f32,4>)", "[factory]", f32, u32, s32) {
  // Arrange
  const auto values = std::array<TestType, 4>{{
    1, 2, 3, 4
  }};

  // Act
  const auto sut = simd<TestType>::load_unaligned(values);

  // Assert
  SECTION("First lane contains correct value") {
    REQUIRE(sut.template lane<0>() == values[0]);
  }
  SECTION("Second lane contains correct value") {
    REQUIRE(sut.template lane<1>() == values[1]);
  }
  SECTION("Third lane contains correct value") {
    REQUIRE(sut.template lane<2>() == values[2]);
  }
  SECTION("Fourth lane contains correct value") {
    REQUIRE(sut.template lane<3>() == values[3]);
  }
}

TEMPLATE_TEST_CASE("simd<T>::spread_scalar(T)", "[factory]", f32, u32, s32) {
  // Arrange
  const auto value = TestType{4};

  // Act
  const auto sut = simd<TestType>::spread_scalar(value);

  // Assert
  SECTION("First lane is set to value") {
    REQUIRE(sut.template lane<0>() == value);
  }
  SECTION("Second lane is set to value") {
    REQUIRE(sut.template lane<1>() == value);
  }
  SECTION("Third lane is set to value") {
    REQUIRE(sut.template lane<2>() == value);
  }
  SECTION("Fourth lane is set to value") {
    REQUIRE(sut.template lane<3>() == value);
  }

}

TEMPLATE_TEST_CASE("simd<T>::spread_from_lane<N>(simd<T>)", "[factory]", f32, u32, s32) {
  // Arrange
  const auto base = simd<TestType>::set(1, 2, 3, 4);

  SECTION("Spreading first lane") {
    // Arrange
    constexpr auto lane = 0;

    // Act
    const auto sut = simd<TestType>::template spread_from_lane<0>(base);

    // Assert
    SECTION("First lane contains correct value") {
      REQUIRE(sut.template lane<0>() == base.template lane<lane>());
    }
    SECTION("Second lane contains correct value") {
      REQUIRE(sut.template lane<1>() == base.template lane<lane>());
    }
    SECTION("Third lane contains correct value") {
      REQUIRE(sut.template lane<2>() == base.template lane<lane>());
    }
    SECTION("Fourth lane contains correct value") {
      REQUIRE(sut.template lane<3>() == base.template lane<lane>());
    }
  }
  SECTION("Spreading second lane") {
    // Arrange
    constexpr auto lane = 1;

    // Act
    const auto sut = simd<TestType>::template spread_from_lane<lane>(base);

    // Assert
    SECTION("First lane contains correct value") {
      REQUIRE(sut.template lane<0>() == base.template lane<lane>());
    }
    SECTION("Second lane contains correct value") {
      REQUIRE(sut.template lane<1>() == base.template lane<lane>());
    }
    SECTION("Third lane contains correct value") {
      REQUIRE(sut.template lane<2>() == base.template lane<lane>());
    }
    SECTION("Fourth lane contains correct value") {
      REQUIRE(sut.template lane<3>() == base.template lane<lane>());
    }
  }
  SECTION("Spreading third lane") {
    // Arrange
    constexpr auto lane = 2;

    // Act
    const auto sut = simd<TestType>::template spread_from_lane<lane>(base);

    // Assert
    SECTION("First lane contains correct value") {
      REQUIRE(sut.template lane<0>() == base.template lane<lane>());
    }
    SECTION("Second lane contains correct value") {
      REQUIRE(sut.template lane<1>() == base.template lane<lane>());
    }
    SECTION("Third lane contains correct value") {
      REQUIRE(sut.template lane<2>() == base.template lane<lane>());
    }
    SECTION("Fourth lane contains correct value") {
      REQUIRE(sut.template lane<3>() == base.template lane<lane>());
    }
  }
  SECTION("Spreading fourth lane") {
    // Arrange
    constexpr auto lane = 3;

    // Act
    const auto sut = simd<TestType>::template spread_from_lane<lane>(base);

    // Assert
    SECTION("First lane contains correct value") {
      REQUIRE(sut.template lane<0>() == base.template lane<lane>());
    }
    SECTION("Second lane contains correct value") {
      REQUIRE(sut.template lane<1>() == base.template lane<lane>());
    }
    SECTION("Third lane contains correct value") {
      REQUIRE(sut.template lane<2>() == base.template lane<lane>());
    }
    SECTION("Fourth lane contains correct value") {
      REQUIRE(sut.template lane<3>() == base.template lane<lane>());
    }
  }
}

TEMPLATE_TEST_CASE("simd<T>::set(T, T, T, T)", "[factory]", f32, u32, s32) {
  // Arrange
  const auto values = std::array<TestType, 4>{{
    1, 2, 3, 4
  }};

  // Act
  const auto sut = simd<f32>::set(values[0], values[1], values[2], values[3]);

  // Assert
  SECTION("First lane contains correct value") {
    REQUIRE(sut.template lane<0>() == values[0]);
  }
  SECTION("Second lane contains correct value") {
    REQUIRE(sut.template lane<1>() == values[1]);
  }
  SECTION("Third lane contains correct value") {
    REQUIRE(sut.template lane<2>() == values[2]);
  }
  SECTION("Fourth lane contains correct value") {
    REQUIRE(sut.template lane<3>() == values[3]);
  }
}

//-----------------------------------------------------------------------------
// Unary Arithmetic
//-----------------------------------------------------------------------------

TEST_CASE("simd_negate(simd<f32>)", "[unary]") {
  // Arrange
  const auto v0 = f32{-5};
  const auto v1 = f32{13};
  const auto v2 = f32{-9};
  const auto v3 = f32{17};
  const auto sut = simd<f32>::set(v0, v1, v2, v3);

  // Act
  const auto result = simd_negate(sut);

  // Assert
  SECTION("Values are negated") {
    SECTION("First lane is negated") {
      REQUIRE(result.lane<0>() == -v0);
    }
    SECTION("Second lane is negated") {
      REQUIRE(result.lane<1>() == -v1);
    }
    SECTION("Third lane is negated") {
      REQUIRE(result.lane<2>() == -v2);
    }
    SECTION("Fourth lane is negated") {
      REQUIRE(result.lane<3>() == -v3);
    }
  }
}

TEST_CASE("simd_abs(simd<f32>)", "[unary]") {
  // Arrange
  const auto v0 = f32{-5};
  const auto v1 = f32{13};
  const auto v2 = f32{-9};
  const auto v3 = f32{17};
  const auto sut = simd<f32>::set(v0, v1, v2, v3);

  // Act
  const auto result = simd_abs(sut);

  // Assert
  SECTION("Values become positive") {
    SECTION("First lane is negated") {
      REQUIRE(result.lane<0>() == abs(v0));
    }
    SECTION("Second lane is negated") {
      REQUIRE(result.lane<1>() == abs(v1));
    }
    SECTION("Third lane is negated") {
      REQUIRE(result.lane<2>() == abs(v2));
    }
    SECTION("Fourth lane is negated") {
      REQUIRE(result.lane<3>() == abs(v3));
    }
  }
}

//-----------------------------------------------------------------------------
// Arithmetic
//-----------------------------------------------------------------------------

TEMPLATE_TEST_CASE("simd_add(simd<T>,simd<T>)", "[arithmetic]", f32, s32, u32) {
  // Arrange
  const auto lhs = simd<TestType>::set(1,2,3,4);
  const auto rhs = simd<TestType>::set(4,3,2,1);
  const auto expected = simd<TestType>::set(
    lhs.template lane<0>() + rhs.template lane<0>(),
    lhs.template lane<1>() + rhs.template lane<1>(),
    lhs.template lane<2>() + rhs.template lane<2>(),
    lhs.template lane<3>() + rhs.template lane<3>()
  );

  // Act
  const auto result = simd_add(lhs, rhs);

  // Assert
  SECTION("Result is sum of two SIMD elements") {
    SECTION("First lane is sum of lhs.lane<0>() and rhs.lane<0>()") {
      REQUIRE(result.template lane<0>() == expected.template lane<0>());
    }
    SECTION("Second lane is sum of lhs.lane<1>() and rhs.lane<1>()") {
      REQUIRE(result.template lane<1>() == expected.template lane<1>());
    }
    SECTION("Third lane is sum of lhs.lane<2>() and rhs.lane<2>()") {
      REQUIRE(result.template lane<2>() == expected.template lane<2>());
    }
    SECTION("Fourth lane is sum of lhs.lane<3>() and rhs.lane<3>()") {
      REQUIRE(result.template lane<3>() == expected.template lane<3>());
    }
  }
}

TEMPLATE_TEST_CASE("simd_subtract(simd<T>,simd<T>)", "[arithmetic]", f32, s32, u32) {
  // Arrange
  const auto lhs = simd<TestType>::set(1,2,3,4);
  const auto rhs = simd<TestType>::set(4,3,2,1);
  const auto expected = simd<TestType>::set(
    lhs.template lane<0>() - rhs.template lane<0>(),
    lhs.template lane<1>() - rhs.template lane<1>(),
    lhs.template lane<2>() - rhs.template lane<2>(),
    lhs.template lane<3>() - rhs.template lane<3>()
  );

  // Act
  const auto result = simd_subtract(lhs, rhs);

  // Assert
  SECTION("Result is difference of two SIMD elements") {
    SECTION("First lane is difference of lhs.lane<0>() and rhs.lane<0>()") {
      REQUIRE(result.template lane<0>() == expected.template lane<0>());
    }
    SECTION("Second lane is difference of lhs.lane<1>() and rhs.lane<1>()") {
      REQUIRE(result.template lane<1>() == expected.template lane<1>());
    }
    SECTION("Third lane is difference of lhs.lane<2>() and rhs.lane<2>()") {
      REQUIRE(result.template lane<2>() == expected.template lane<2>());
    }
    SECTION("Fourth lane is difference of lhs.lane<3>() and rhs.lane<3>()") {
      REQUIRE(result.template lane<3>() == expected.template lane<3>());
    }
  }
}

TEMPLATE_TEST_CASE("simd_multiply(simd<T>,simd<T>)", "[arithmetic]", f32, s32, u32) {
  // Arrange
  const auto lhs = simd<TestType>::set(1,2,3,4);
  const auto rhs = simd<TestType>::set(4,3,2,1);
  const auto expected = simd<TestType>::set(
    lhs.template lane<0>() * rhs.template lane<0>(),
    lhs.template lane<1>() * rhs.template lane<1>(),
    lhs.template lane<2>() * rhs.template lane<2>(),
    lhs.template lane<3>() * rhs.template lane<3>()
  );

  // Act
  const auto result = simd_multiply(lhs, rhs);

  // Assert
  SECTION("Result is the product of two SIMD elements") {
    SECTION("First lane is the product of lhs.lane<0>() and rhs.lane<0>()") {
      REQUIRE(result.template lane<0>() == expected.template lane<0>());
    }
    SECTION("Second lane is the product of lhs.lane<1>() and rhs.lane<1>()") {
      REQUIRE(result.template lane<1>() == expected.template lane<1>());
    }
    SECTION("Third lane is the product of lhs.lane<2>() and rhs.lane<2>()") {
      REQUIRE(result.template lane<2>() == expected.template lane<2>());
    }
    SECTION("Fourth lane is the product of lhs.lane<3>() and rhs.lane<3>()") {
      REQUIRE(result.template lane<3>() == expected.template lane<3>());
    }
  }
}

TEST_CASE("simd_multiply_scalar(simd<f32>,f32)", "[arithmetic]") {
  // Arrange
  const auto lhs = simd<f32>::set(1,2,3,4);
  const auto rhs = f32{10};
  const auto expected = simd<f32>::set(
    lhs.lane<0>() * rhs,
    lhs.lane<1>() * rhs,
    lhs.lane<2>() * rhs,
    lhs.lane<3>() * rhs
  );

  // Act
  const auto result = simd_multiply_scalar(lhs, rhs);

  // Assert
  SECTION("Result is the product of the SIMD element and a spread scalar") {
    SECTION("First lane is the product of lhs.lane<0>() and rhs") {
      REQUIRE(result.lane<0>() == expected.lane<0>());
    }
    SECTION("Second lane is the product of lhs.lane<1>() and rhs") {
      REQUIRE(result.lane<1>() == expected.lane<1>());
    }
    SECTION("Third lane is the product of lhs.lane<2>() and rhs") {
      REQUIRE(result.lane<2>() == expected.lane<2>());
    }
    SECTION("Fourth lane is the product of lhs.lane<3>() and rhs") {
      REQUIRE(result.lane<3>() == expected.lane<3>());
    }
  }
}

TEST_CASE("simd_divide(simd<f32>,simd<f32>)", "[arithmetic]") {
  // Arrange
  const auto lhs = simd<f32>::set(1,2,3,4);
  const auto rhs = simd<f32>::set(4,3,2,1);
  const auto expected = simd<f32>::set(
    lhs.lane<0>() / rhs.lane<0>(),
    lhs.lane<1>() / rhs.lane<1>(),
    lhs.lane<2>() / rhs.lane<2>(),
    lhs.lane<3>() / rhs.lane<3>()
  );

  // Act
  const auto result = simd_divide(lhs, rhs);

  // Assert
  SECTION("Result is the quotient of the SIMD element and a spread scalar") {
    SECTION("First lane is the quotient of lhs.lane<0>() and rhs") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>()));
    }
    SECTION("Second lane is the quotient of lhs.lane<1>() and rhs") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>()));
    }
    SECTION("Third lane is the quotient of lhs.lane<2>() and rhs") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>()));
    }
    SECTION("Fourth lane is the quotient of lhs.lane<3>() and rhs") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>()));
    }
  }
}

TEST_CASE("simd_abs_difference(simd<f32>,simd<f32>)", "[arithmetic]") {
  // Arrange
  const auto lhs = simd<f32>::set(1,2,3,4);
  const auto rhs = simd<f32>::set(4,3,2,1);
  const auto expected = simd<f32>::set(
    abs(lhs.lane<0>() - rhs.lane<0>()),
    abs(lhs.lane<1>() - rhs.lane<1>()),
    abs(lhs.lane<2>() - rhs.lane<2>()),
    abs(lhs.lane<3>() - rhs.lane<3>())
  );

  // Act
  const auto result = simd_abs_difference(lhs, rhs);

  // Assert
  SECTION("Result is the absolute difference of the SIMD elements") {
    SECTION("First lane is the absolute difference of lhs.lane<0>() and rhs.lane<0>()") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>()));
    }
    SECTION("Second lane is the absolute difference of lhs.lane<1>() and rhs.lane<1>()") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>()));
    }
    SECTION("Third lane is the absolute difference of lhs.lane<2>() and rhs.lane<2>()") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>()));
    }
    SECTION("Fourth lane is the absolute difference of lhs.lane<3>() and rhs.lane<3>()") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>()));
    }
  }
}

//-----------------------------------------------------------------------------
// Fused Arithmetic
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Comparisons
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Math and Algorithms
//-----------------------------------------------------------------------------

TEST_CASE("simd_fast_reciprocol(simd<f32>)", "[algorithm][math]") {
  // Arrange
  const auto sut = simd<f32>::set(1,2,3,4);
  const auto tolerance = f32{1e-2};
  const auto expected = simd<f32>::set(
    f32{1} / sut.lane<0>(),
    f32{1} / sut.lane<1>(),
    f32{1} / sut.lane<2>(),
    f32{1} / sut.lane<3>()
  );

  // Act
  const auto result = simd_fast_reciprocol(sut);

  // Assert
  SECTION("Result is an approximation of the reciprocol") {
    SECTION("First lane is accurate to within 2 decimal places") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>(),tolerance));
    }
    SECTION("Second lane is accurate to within 2 decimal places") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>(),tolerance));
    }
    SECTION("Third lane is accurate to within 2 decimal places") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>(),tolerance));
    }
    SECTION("Fourth lane is accurate to within 2 decimal places") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>(),tolerance));
    }
  }
}

TEST_CASE("simd_reciprocol(simd<f32>)", "[algorithm][math]") {
  // Arrange
  const auto sut = simd<f32>::set(1,2,3,4);
  const auto tolerance = f32{1e-5};
  const auto expected = simd<f32>::set(
    f32{1} / sut.lane<0>(),
    f32{1} / sut.lane<1>(),
    f32{1} / sut.lane<2>(),
    f32{1} / sut.lane<3>()
  );

  // Act
  const auto result = simd_reciprocol(sut);

  // Assert
  SECTION("Result is an approximation of the reciprocol") {
    SECTION("First lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>(),tolerance));
    }
    SECTION("Second lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>(),tolerance));
    }
    SECTION("Third lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>(),tolerance));
    }
    SECTION("Fourth lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>(),tolerance));
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("simd_fast_sqrt(simd<f32>)", "[algorithm][math]") {
  // Arrange
  const auto sut = simd<f32>::set(1,3,5,7);
  const auto tolerance = f32{1e-2};
  const auto expected = simd<f32>::set(
    sqrt(sut.lane<0>()),
    sqrt(sut.lane<1>()),
    sqrt(sut.lane<2>()),
    sqrt(sut.lane<3>())
  );

  // Act
  const auto result = simd_fast_sqrt(sut);

  // Assert
  SECTION("Result is an approximation of the reciprocol") {
    SECTION("First lane is accurate to within at least 2 decimal places") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>(),tolerance));
    }
    SECTION("Second lane is accurate to within at least 2 decimal places") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>(),tolerance));
    }
    SECTION("Third lane is accurate to within at least 2 decimal places") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>(),tolerance));
    }
    SECTION("Fourth lane is accurate to within at least 2 decimal places") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>(),tolerance));
    }
  }
}

TEST_CASE("simd_sqrt(simd<f32>)", "[algorithm][math]") {
  // Arrange
  const auto sut = simd<f32>::set(1,3,5,7);
  const auto tolerance = f32{1e-5};
  const auto expected = simd<f32>::set(
    sqrt(sut.lane<0>()),
    sqrt(sut.lane<1>()),
    sqrt(sut.lane<2>()),
    sqrt(sut.lane<3>())
  );

  // Act
  const auto result = simd_sqrt(sut);

  // Assert
  SECTION("Result is an approximation of the reciprocol") {
    SECTION("First lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>(),tolerance));
    }
    SECTION("Second lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>(),tolerance));
    }
    SECTION("Third lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>(),tolerance));
    }
    SECTION("Fourth lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>(),tolerance));
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("simd_fast_reciprocol_sqrt(simd<f32>)", "[algorithm][math]") {
  // Arrange
  const auto sut = simd<f32>::set(1,3,5,7);
  const auto tolerance = f32{1e-2};
  const auto expected = simd<f32>::set(
    f32{1} / sqrt(sut.lane<0>()),
    f32{1} / sqrt(sut.lane<1>()),
    f32{1} / sqrt(sut.lane<2>()),
    f32{1} / sqrt(sut.lane<3>())
  );

  // Act
  const auto result = simd_fast_reciprocol_sqrt(sut);

  // Assert
  SECTION("Result is an approximation of the reciprocol") {
    SECTION("First lane is accurate to within at least 2 decimal places") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>(),tolerance));
    }
    SECTION("Second lane is accurate to within at least 2 decimal places") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>(),tolerance));
    }
    SECTION("Third lane is accurate to within at least 2 decimal places") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>(),tolerance));
    }
    SECTION("Fourth lane is accurate to within at least 2 decimal places") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>(),tolerance));
    }
  }
}

TEST_CASE("simd_reciprocol_sqrt(simd<f32>)", "[algorithm][math]") {
  // Arrange
  const auto sut = simd<f32>::set(1,3,5,7);
  const auto tolerance = f32{1e-5};
  const auto expected = simd<f32>::set(
    f32{1} / sqrt(sut.lane<0>()),
    f32{1} / sqrt(sut.lane<1>()),
    f32{1} / sqrt(sut.lane<2>()),
    f32{1} / sqrt(sut.lane<3>())
  );

  // Act
  const auto result = simd_reciprocol_sqrt(sut);

  // Assert
  SECTION("Result is an approximation of the reciprocol") {
    SECTION("First lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>(),tolerance));
    }
    SECTION("Second lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>(),tolerance));
    }
    SECTION("Third lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>(),tolerance));
    }
    SECTION("Fourth lane is accurate to within at least 5 decimal places") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>(),tolerance));
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("simd_min(simd<f32>)", "[algorithm][math]") {
  // Arrange
  const auto lhs = simd<f32>::set(1,20,3,42);
  const auto rhs = simd<f32>::set(4,3,17,1);
  const auto expected = simd<f32>::set(
    std::min(lhs.lane<0>(), rhs.lane<0>()),
    std::min(lhs.lane<1>(), rhs.lane<1>()),
    std::min(lhs.lane<2>(), rhs.lane<2>()),
    std::min(lhs.lane<3>(), rhs.lane<3>())
  );

  // Act
  const auto result = simd_min(lhs, rhs);

  // Assert
  SECTION("Result vector contains min value from each lane") {
    SECTION("First lane contains min value of lhs and rhs") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>()));
    }
    SECTION("Second lane contains min value of lhs and rhs") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>()));
    }
    SECTION("Third lane contains min value of lhs and rhs") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>()));
    }
    SECTION("Fourth lane contains min value of lhs and rhs") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>()));
    }
  }
}

TEST_CASE("simd_max(simd<f32>)", "[algorithm][math]") {
  // Arrange
  const auto lhs = simd<f32>::set(1,20,3,42);
  const auto rhs = simd<f32>::set(4,3,17,1);
  const auto expected = simd<f32>::set(
    std::max(lhs.lane<0>(), rhs.lane<0>()),
    std::max(lhs.lane<1>(), rhs.lane<1>()),
    std::max(lhs.lane<2>(), rhs.lane<2>()),
    std::max(lhs.lane<3>(), rhs.lane<3>())
  );

  // Act
  const auto result = simd_max(lhs, rhs);

  // Assert
  SECTION("Result vector contains max value from each lane") {
    SECTION("First lane contains max value of lhs and rhs") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>()));
    }
    SECTION("Second lane contains max value of lhs and rhs") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>()));
    }
    SECTION("Third lane contains max value of lhs and rhs") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>()));
    }
    SECTION("Fourth lane contains max value of lhs and rhs") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>()));
    }
  }
}

//-----------------------------------------------------------------------------

namespace {
  template <std::size_t Lane>
  auto unrolled_dot4(span<const simd<f32>,4> lhs, span<const simd<f32>,4> rhs)
    noexcept -> f32
  {
    auto result = f32{0};
    for (auto i = 0u; i < 4u; ++i) {
      result += lhs[i].lane<Lane>() * rhs[i].lane<Lane>();
    };
    return result;
  }
}

TEST_CASE("simd_dot4(span<simd<f32>,4>,span<simd<f32>,4>)", "[algorithm][math]") {
  // Arrange

  const auto lhs = std::array<simd<f32>,4>{{
    simd<f32>::set(1,2,3,4),
    simd<f32>::set(5,6,7,8),
    simd<f32>::set(9,8,7,6),
    simd<f32>::set(5,4,3,2)
  }};
  const auto rhs = std::array<simd<f32>,4>{{
    simd<f32>::set(9,8,7,6),
    simd<f32>::set(5,4,3,2),
    simd<f32>::set(1,2,3,4),
    simd<f32>::set(5,6,7,8)
  }};
  const auto expected = simd<f32>::set(
    unrolled_dot4<0>(lhs, rhs),
    unrolled_dot4<1>(lhs, rhs),
    unrolled_dot4<2>(lhs, rhs),
    unrolled_dot4<3>(lhs, rhs)
  );

  // Act
  const auto result = simd_dot4(lhs, rhs);

  // Assert
  SECTION("Result vector contains dot product of the 4 pairs of inputs") {
    SECTION("First lane contains dot result of first lane of input vectors") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>()));
    }
    SECTION("Second lane contains dot result of second lane of input vectors") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>()));
    }
    SECTION("Third lane contains dot result of third lane of input vectors") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>()));
    }
    SECTION("Fourth lane contains dot result of fourth lane of input vectors") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>()));
    }
  }
}

namespace {
  template <std::size_t Lane>
  auto strided_dot(span<const f32,16> lhs, span<const f32,16> rhs)
    noexcept -> f32
  {
    auto result = f32{0};
    for (auto i = std::size_t{0u}; i < 4u; ++i) {
      const auto index = i * 4u + Lane;
      result += lhs[index] * rhs[index];
    };
    return result;
  }
}

TEST_CASE("simd_dot4(span<f32,16>,span<f32,16>)", "[algorithm][math]") {
  // Arrange

  const auto lhs = simd_aligned_array<f32,16>{{
    1,2,3,4,
    5,6,7,8,
    9,8,7,6,
    5,4,3,2,
  }};
  const auto rhs = simd_aligned_array<f32,16>{{
    9,8,7,6,
    5,4,3,2,
    1,2,3,4,
    5,6,7,8,
  }};
  const auto expected = simd<f32>::set(
    strided_dot<0>(lhs, rhs),
    strided_dot<1>(lhs, rhs),
    strided_dot<2>(lhs, rhs),
    strided_dot<3>(lhs, rhs)
  );

  // Act
  const auto result = simd_dot4(lhs, rhs);

  // Assert
  SECTION("Result vector contains dot product of the 4 pairs of inputs") {
    SECTION("First lane contains dot result of first lane of input vectors") {
      REQUIRE(almost_equal(result.lane<0>(),expected.lane<0>()));
    }
    SECTION("Second lane contains dot result of second lane of input vectors") {
      REQUIRE(almost_equal(result.lane<1>(),expected.lane<1>()));
    }
    SECTION("Third lane contains dot result of third lane of input vectors") {
      REQUIRE(almost_equal(result.lane<2>(),expected.lane<2>()));
    }
    SECTION("Fourth lane contains dot result of fourth lane of input vectors") {
      REQUIRE(almost_equal(result.lane<3>(),expected.lane<3>()));
    }
  }
}
} // namespace alloy::core::test
