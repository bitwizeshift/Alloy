// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/simd.hpp>

#include <alloy/core/math/math.hpp>

#include <alloy/core/types.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <span>
#include <type_traits>

namespace alloy::core::test {

//-----------------------------------------------------------------------------
// Factories
//-----------------------------------------------------------------------------

TEMPLATE_TEST_CASE("simd<T>::load(std::span<f32,4>)", "[factory]", f32, u32, s32) {
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

TEMPLATE_TEST_CASE("simd<T>::load_unaligned(std::span<f32,4>)", "[factory]", f32, u32, s32) {
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
// Storing
//-----------------------------------------------------------------------------

TEMPLATE_TEST_CASE("simd<T>::store(std::span<T,4>)", "[storing]", f32, u32, s32) {
  // Arrange
  const auto sut = simd<TestType>::set(1, 2, 3, 4);
  auto out = simd_aligned_array<TestType, 4>{};

  // Act
  sut.store(out);

  // Assert
  SECTION("Stores the lanes in order") {
    REQUIRE(out[0] == TestType(1));
    REQUIRE(out[1] == TestType(2));
    REQUIRE(out[2] == TestType(3));
    REQUIRE(out[3] == TestType(4));
  }
}

TEMPLATE_TEST_CASE("simd<T>::store_unaligned(std::span<T,4>)", "[storing]", f32, u32, s32) {
  // Arrange
  const auto sut = simd<TestType>::set(1, 2, 3, 4);
  auto storage = simd_aligned_array<TestType, 5>{};
  const auto out = std::span<TestType, 4>{storage.data() + 1, 4u};

  // Act
  sut.store_unaligned(out);

  // Assert
  SECTION("Stores the lanes in order at an unaligned address") {
    REQUIRE(storage[1] == TestType(1));
    REQUIRE(storage[2] == TestType(2));
    REQUIRE(storage[3] == TestType(3));
    REQUIRE(storage[4] == TestType(4));
  }
}

//-----------------------------------------------------------------------------
// Casting
//-----------------------------------------------------------------------------

TEST_CASE("simd_cast<To>(simd<From>)", "[casting]") {
  SECTION("Reinterprets f32 bits as u32") {
    // Arrange
    const auto sut = simd<f32>::spread_scalar(1.0f);

    // Act
    const auto result = simd_cast<u32>(sut);

    // Assert
    REQUIRE(result.lane<0>() == u32{0x3f800000});
  }
  SECTION("Reinterprets s32 bits as u32") {
    // Arrange
    const auto sut = simd<s32>::set(-1, 0, 1, -2);

    // Act
    const auto result = simd_cast<u32>(sut);

    // Assert
    REQUIRE(result.lane<0>() == u32{0xffffffff});
    REQUIRE(result.lane<3>() == u32{0xfffffffe});
  }
  SECTION("Round-trips through another type") {
    // Arrange
    const auto sut = simd<f32>::set(1.5f, -2.0f, 0.25f, 8.0f);

    // Act
    const auto result = simd_cast<f32>(simd_cast<s32>(sut));

    // Assert
    REQUIRE(result.lane<0>() == 1.5f);
    REQUIRE(result.lane<1>() == -2.0f);
    REQUIRE(result.lane<2>() == 0.25f);
    REQUIRE(result.lane<3>() == 8.0f);
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

TEST_CASE("simd_multiply(simd<s32>,simd<s32>) with signs and overflow", "[arithmetic]") {
  // Arrange
  const auto lhs = simd<s32>::set(-2, 3, -4, 0x10000);
  const auto rhs = simd<s32>::set(3, -5, -6, 0x10000);

  // Act
  const auto result = simd_multiply(lhs, rhs);

  // Assert
  SECTION("Every lane is the low 32 bits of its product") {
    REQUIRE(result.lane<0>() == -6);
    REQUIRE(result.lane<1>() == -15);
    REQUIRE(result.lane<2>() == 24);
    REQUIRE(result.lane<3>() == 0);
  }
}

TEST_CASE("simd_multiply(simd<u32>,simd<u32>) with large values", "[arithmetic]") {
  // Arrange
  const auto lhs = simd<u32>::set(0xffffffffu, 2u, 0x80000000u, 70000u);
  const auto rhs = simd<u32>::set(2u, 0x7fffffffu, 3u, 70000u);

  // Act
  const auto result = simd_multiply(lhs, rhs);

  // Assert
  SECTION("Every lane is the low 32 bits of its product") {
    REQUIRE(result.lane<0>() == 0xfffffffeu);
    REQUIRE(result.lane<1>() == 0xfffffffeu);
    REQUIRE(result.lane<2>() == 0x80000000u);
    REQUIRE(result.lane<3>() == 0x24101100u);
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

TEST_CASE("simd_fuse_add_multiply(simd<f32>,simd<f32>,simd<f32>)", "[arithmetic]") {
  // Arrange
  const auto add = simd<f32>::set(1, 2, 3, 4);
  const auto lhs = simd<f32>::set(2, 3, 4, 5);
  const auto rhs = simd<f32>::set(-1, 0.5f, 2, 10);

  // Act
  const auto result = simd_fuse_add_multiply(add, lhs, rhs);

  // Assert
  SECTION("Adds the product to the first argument") {
    REQUIRE(result.lane<0>() == -1.0f);
    REQUIRE(result.lane<1>() == 3.5f);
    REQUIRE(result.lane<2>() == 11.0f);
    REQUIRE(result.lane<3>() == 54.0f);
  }
}

TEST_CASE("simd_fuse_add_multiply_scalar(simd<f32>,simd<f32>,f32)", "[arithmetic]") {
  // Arrange
  const auto add = simd<f32>::set(1, 2, 3, 4);
  const auto lhs = simd<f32>::set(2, 3, 4, 5);

  // Act
  const auto result = simd_fuse_add_multiply_scalar(add, lhs, 2.0f);

  // Assert
  SECTION("Adds the scaled vector to the first argument") {
    REQUIRE(result.lane<0>() == 5.0f);
    REQUIRE(result.lane<3>() == 14.0f);
  }
}

//-----------------------------------------------------------------------------
// Comparisons
//-----------------------------------------------------------------------------

namespace {
  // The lanes compare as equal, less, greater, and less, using values that
  // differ in sign for signed types, and in the high bit for unsigned types.
  template <typename T>
  auto comparison_lhs() -> simd<T> {
    if constexpr (std::is_same_v<T, u32>) {
      return simd<T>::set(5u, 1u, 0x80000000u, 7u);
    } else {
      return simd<T>::set(T(5), T(-3), T(2), T(-8));
    }
  }

  template <typename T>
  auto comparison_rhs() -> simd<T> {
    if constexpr (std::is_same_v<T, u32>) {
      return simd<T>::set(5u, 0x80000000u, 1u, 0xffffffffu);
    } else {
      return simd<T>::set(T(5), T(2), T(-3), T(-1));
    }
  }

  auto mask(bool value) -> u32 {
    return value ? u32{0xffffffff} : u32{0};
  }
} // namespace

TEMPLATE_TEST_CASE("simd_equal(simd<T>,simd<T>)", "[comparison]", f32, s32, u32) {
  // Arrange
  const auto lhs = comparison_lhs<TestType>();
  const auto rhs = comparison_rhs<TestType>();

  // Act
  const auto result = simd_equal(lhs, rhs);

  // Assert
  SECTION("First lane is the comparison of equal values") {
    REQUIRE(result.template lane<0>() == mask(true));
  }
  SECTION("Second lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<1>() == mask(false));
  }
  SECTION("Third lane is the comparison of a greater value") {
    REQUIRE(result.template lane<2>() == mask(false));
  }
  SECTION("Fourth lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<3>() == mask(false));
  }
}

TEMPLATE_TEST_CASE("simd_not_equal(simd<T>,simd<T>)", "[comparison]", f32, s32, u32) {
  // Arrange
  const auto lhs = comparison_lhs<TestType>();
  const auto rhs = comparison_rhs<TestType>();

  // Act
  const auto result = simd_not_equal(lhs, rhs);

  // Assert
  SECTION("First lane is the comparison of equal values") {
    REQUIRE(result.template lane<0>() == mask(false));
  }
  SECTION("Second lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<1>() == mask(true));
  }
  SECTION("Third lane is the comparison of a greater value") {
    REQUIRE(result.template lane<2>() == mask(true));
  }
  SECTION("Fourth lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<3>() == mask(true));
  }
}

TEMPLATE_TEST_CASE("simd_less(simd<T>,simd<T>)", "[comparison]", f32, s32, u32) {
  // Arrange
  const auto lhs = comparison_lhs<TestType>();
  const auto rhs = comparison_rhs<TestType>();

  // Act
  const auto result = simd_less(lhs, rhs);

  // Assert
  SECTION("First lane is the comparison of equal values") {
    REQUIRE(result.template lane<0>() == mask(false));
  }
  SECTION("Second lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<1>() == mask(true));
  }
  SECTION("Third lane is the comparison of a greater value") {
    REQUIRE(result.template lane<2>() == mask(false));
  }
  SECTION("Fourth lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<3>() == mask(true));
  }
}

TEMPLATE_TEST_CASE("simd_greater(simd<T>,simd<T>)", "[comparison]", f32, s32, u32) {
  // Arrange
  const auto lhs = comparison_lhs<TestType>();
  const auto rhs = comparison_rhs<TestType>();

  // Act
  const auto result = simd_greater(lhs, rhs);

  // Assert
  SECTION("First lane is the comparison of equal values") {
    REQUIRE(result.template lane<0>() == mask(false));
  }
  SECTION("Second lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<1>() == mask(false));
  }
  SECTION("Third lane is the comparison of a greater value") {
    REQUIRE(result.template lane<2>() == mask(true));
  }
  SECTION("Fourth lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<3>() == mask(false));
  }
}

TEMPLATE_TEST_CASE("simd_less_equal(simd<T>,simd<T>)", "[comparison]", f32, s32, u32) {
  // Arrange
  const auto lhs = comparison_lhs<TestType>();
  const auto rhs = comparison_rhs<TestType>();

  // Act
  const auto result = simd_less_equal(lhs, rhs);

  // Assert
  SECTION("First lane is the comparison of equal values") {
    REQUIRE(result.template lane<0>() == mask(true));
  }
  SECTION("Second lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<1>() == mask(true));
  }
  SECTION("Third lane is the comparison of a greater value") {
    REQUIRE(result.template lane<2>() == mask(false));
  }
  SECTION("Fourth lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<3>() == mask(true));
  }
}

TEMPLATE_TEST_CASE("simd_greater_equal(simd<T>,simd<T>)", "[comparison]", f32, s32, u32) {
  // Arrange
  const auto lhs = comparison_lhs<TestType>();
  const auto rhs = comparison_rhs<TestType>();

  // Act
  const auto result = simd_greater_equal(lhs, rhs);

  // Assert
  SECTION("First lane is the comparison of equal values") {
    REQUIRE(result.template lane<0>() == mask(true));
  }
  SECTION("Second lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<1>() == mask(false));
  }
  SECTION("Third lane is the comparison of a greater value") {
    REQUIRE(result.template lane<2>() == mask(true));
  }
  SECTION("Fourth lane is the comparison of a lesser value") {
    REQUIRE(result.template lane<3>() == mask(false));
  }
}

TEST_CASE("simd_almost_equal(simd<f32>,simd<f32>,f32)", "[comparison]") {
  // Arrange
  const auto lhs = simd<f32>::set(1.0f, 1.0f, -1.0f, 2.0f);
  const auto rhs = simd<f32>::set(1.05f, 1.5f, -1.05f, -2.0f);

  // Act
  const auto result = simd_almost_equal(lhs, rhs, 0.1f);

  // Assert
  SECTION("Lanes within tolerance are equal") {
    REQUIRE(result.lane<0>() == mask(true));
    REQUIRE(result.lane<2>() == mask(true));
  }
  SECTION("Lanes outside tolerance are not equal") {
    REQUIRE(result.lane<1>() == mask(false));
    REQUIRE(result.lane<3>() == mask(false));
  }
}

//-----------------------------------------------------------------------------
// Math and Algorithms
//-----------------------------------------------------------------------------

TEST_CASE("simd_fast_reciprocal(simd<f32>)", "[algorithm][math]") {
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
  const auto result = simd_fast_reciprocal(sut);

  // Assert
  SECTION("Result is an approximation of the reciprocal") {
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

TEST_CASE("simd_reciprocal(simd<f32>)", "[algorithm][math]") {
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
  const auto result = simd_reciprocal(sut);

  // Assert
  SECTION("Result is an approximation of the reciprocal") {
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
  SECTION("Result is an approximation of the reciprocal") {
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
  SECTION("Result is an approximation of the reciprocal") {
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

TEST_CASE("simd_sqrt(simd<f32>) of zero", "[algorithm][math]") {
  // Arrange
  const auto sut = simd<f32>::spread_scalar(0.0f);

  // Act
  const auto result = simd_sqrt(sut);

  // Assert
  SECTION("Square root of 0 is 0") {
    REQUIRE(result.lane<0>() == 0.0f);
  }
}

TEST_CASE("simd_fast_reciprocal_sqrt(simd<f32>)", "[algorithm][math]") {
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
  const auto result = simd_fast_reciprocal_sqrt(sut);

  // Assert
  SECTION("Result is an approximation of the reciprocal") {
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

TEST_CASE("simd_reciprocal_sqrt(simd<f32>)", "[algorithm][math]") {
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
  const auto result = simd_reciprocal_sqrt(sut);

  // Assert
  SECTION("Result is an approximation of the reciprocal") {
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
  auto unrolled_dot4(
    std::span<const simd<f32>,4> lhs,
    std::span<const simd<f32>,4> rhs
  ) noexcept -> f32
  {
    auto result = f32{0};
    for (auto i = 0u; i < 4u; ++i) {
      result += lhs[i].lane<Lane>() * rhs[i].lane<Lane>();
    };
    return result;
  }
}

TEST_CASE("simd_dot4(std::span<simd<f32>,4>,std::span<simd<f32>,4>)", "[algorithm][math]") {
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
  auto strided_dot(std::span<const f32,16> lhs, std::span<const f32,16> rhs)
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

TEST_CASE("simd_dot4(std::span<f32,16>,std::span<f32,16>)", "[algorithm][math]") {
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
//-----------------------------------------------------------------------------
// Batched Dot Products
//-----------------------------------------------------------------------------

namespace {
  // Builds 'N' floats of 4-component vectors in groups of 4, laid out as
  // [x0 x1 x2 x3 y0 y1 y2 y3 z0 z1 z2 z3 w0 w1 w2 w3] per group.
  template <std::size_t N>
  auto soa_values(f32 seed) -> simd_aligned_array<f32, N> {
    auto result = simd_aligned_array<f32, N>{};
    for (auto i = 0u; i < N; ++i) {
      result[i] = seed + static_cast<f32>(i % 7) - static_cast<f32>(i % 3);
    }
    return result;
  }

  // Computes the dot product of vector 'lane' of group 'group'
  template <std::size_t N>
  auto soa_dot(
    const simd_aligned_array<f32, N>& lhs,
    const simd_aligned_array<f32, N>& rhs,
    std::size_t group,
    std::size_t lane
  ) -> f32 {
    auto sum = f32{0};
    for (auto component = std::size_t{0}; component < 4u; ++component) {
      const auto index = (16u * group) + (4u * component) + lane;
      sum += lhs[index] * rhs[index];
    }
    return sum;
  }

  template <std::size_t N>
  auto to_simd(const simd_aligned_array<f32, N>& values)
    -> std::array<simd<f32>, (N / 4)> {
    auto result = std::array<simd<f32>, (N / 4)>{};
    for (auto i = 0u; i < (N / 4); ++i) {
      result[i] = simd<f32>::load(
        std::span<const f32, 4>{values.data() + (4u * i), 4u}
      );
    }
    return result;
  }

  // Computes the dot product of every vector, in the order that the batched
  // dot products produce them: group by group, and lane by lane in a group.
  template <std::size_t N>
  auto soa_dots(
    const simd_aligned_array<f32, N>& lhs,
    const simd_aligned_array<f32, N>& rhs
  ) -> std::array<f32, (N / 4)> {
    auto result = std::array<f32, (N / 4)>{};
    for (auto group = std::size_t{0}; group < (N / 16); ++group) {
      for (auto lane = std::size_t{0}; lane < 4u; ++lane) {
        result[(4u * group) + lane] = soa_dot(lhs, rhs, group, lane);
      }
    }
    return result;
  }

  // Reads the lanes of every simd value, in order
  template <std::size_t M>
  auto to_lanes(const std::array<simd<f32>, M>& values)
    -> std::array<f32, (M * 4)> {
    auto result = std::array<f32, (M * 4)>{};
    for (auto i = std::size_t{0}; i < M; ++i) {
      result[(4u * i) + 0u] = values[i].template lane<0>();
      result[(4u * i) + 1u] = values[i].template lane<1>();
      result[(4u * i) + 2u] = values[i].template lane<2>();
      result[(4u * i) + 3u] = values[i].template lane<3>();
    }
    return result;
  }
} // namespace

TEMPLATE_TEST_CASE_SIG("simd_dot<N>(std::span<const f32,N>,std::span<const f32,N>)", "[algorithm][math]", ((std::size_t N), N), 16, 32) {
  // Arrange
  const auto lhs = soa_values<N>(1.0f);
  const auto rhs = soa_values<N>(-2.0f);
  const auto expected = soa_dots(lhs, rhs);

  // Act
  const auto result = simd_dot<N>(
    std::span<const f32, N>{lhs},
    std::span<const f32, N>{rhs}
  );

  // Assert
  SECTION("Each group holds the dot product of its vectors") {
    REQUIRE(std::ranges::equal(to_lanes(result), expected));
  }
}

TEMPLATE_TEST_CASE_SIG("simd_dot<N>(std::span<const simd<f32>,N>,std::span<const simd<f32>,N>)", "[algorithm][math]", ((std::size_t N), N), 4, 8) {
  // Arrange
  const auto lhs_values = soa_values<N * 4>(1.0f);
  const auto rhs_values = soa_values<N * 4>(-2.0f);
  const auto lhs = to_simd(lhs_values);
  const auto rhs = to_simd(rhs_values);
  const auto expected = soa_dots(lhs_values, rhs_values);

  // Act
  const auto result = simd_dot<N>(
    std::span<const simd<f32>, N>{lhs},
    std::span<const simd<f32>, N>{rhs}
  );

  // Assert
  SECTION("Each group holds the dot product of its vectors") {
    REQUIRE(std::ranges::equal(to_lanes(result), expected));
  }
}

TEMPLATE_TEST_CASE_SIG("simd_dot4_to<N>(std::span<const f32,N>,std::span<const f32,N>,std::span<f32,N/4>)", "[algorithm][math]", ((std::size_t N), N), 16, 32) {
  // Arrange
  const auto lhs = soa_values<N>(1.0f);
  const auto rhs = soa_values<N>(-2.0f);
  const auto expected = soa_dots(lhs, rhs);
  auto out = simd_aligned_array<f32, (N / 4)>{};

  // Act
  simd_dot4_to<N>(
    std::span<const f32, N>{lhs},
    std::span<const f32, N>{rhs},
    std::span<f32, (N / 4)>{out}
  );

  // Assert
  SECTION("Each group stores the dot products of its vectors in order") {
    REQUIRE(std::ranges::equal(out, expected));
  }
}

TEMPLATE_TEST_CASE_SIG("simd_dot4_to<N>(std::span<const f32,N>,std::span<const f32,N>,std::span<simd<f32>,N/16>)", "[algorithm][math]", ((std::size_t N), N), 16, 32) {
  // Arrange
  const auto lhs = soa_values<N>(1.0f);
  const auto rhs = soa_values<N>(-2.0f);
  const auto expected = soa_dots(lhs, rhs);
  auto out = std::array<simd<f32>, (N / 16)>{};

  // Act
  simd_dot4_to<N>(
    std::span<const f32, N>{lhs},
    std::span<const f32, N>{rhs},
    std::span<simd<f32>, (N / 16)>{out}
  );

  // Assert
  SECTION("Each group holds the dot product of its vectors") {
    REQUIRE(std::ranges::equal(to_lanes(out), expected));
  }
}

TEMPLATE_TEST_CASE_SIG("simd_dot4_to<N>(std::span<const simd<f32>,N>,std::span<const simd<f32>,N>,std::span<simd<f32>,N/4>)", "[algorithm][math]", ((std::size_t N), N), 4, 8) {
  // Arrange
  const auto lhs_values = soa_values<N * 4>(1.0f);
  const auto rhs_values = soa_values<N * 4>(-2.0f);
  const auto lhs = to_simd(lhs_values);
  const auto rhs = to_simd(rhs_values);
  const auto expected = soa_dots(lhs_values, rhs_values);
  auto out = std::array<simd<f32>, (N / 4)>{};

  // Act
  simd_dot4_to<N>(
    std::span<const simd<f32>, N>{lhs},
    std::span<const simd<f32>, N>{rhs},
    std::span<simd<f32>, (N / 4)>{out}
  );

  // Assert
  SECTION("Each group holds the dot product of its vectors") {
    REQUIRE(std::ranges::equal(to_lanes(out), expected));
  }
}

} // namespace alloy::core::test
