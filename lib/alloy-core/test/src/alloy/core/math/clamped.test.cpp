/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "alloy/core/math/clamped.hpp"
#include "alloy/core/math/saturated.hpp"

#include <catch2/catch.hpp>

namespace alloy::core::test {

//=============================================================================
// class : clamped
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

TEST_CASE("clamped<Float>::make(Float)", "[factory]") {
  SECTION("Input exceeds 1.0") {
    const auto sut = clamped<float>::make(2.0f);

    SECTION("Results in overflow error") {
      REQUIRE(sut == fail(clamped<float>::error::overflow));
    }
  }
  SECTION("Input precedes 0.0") {
    const auto sut = clamped<float>::make(2.0f);

    SECTION("Results in underflow error") {
      REQUIRE(sut == fail(clamped<float>::error::overflow));
    }
  }
  SECTION("Input is between 1.0 and 0.0") {
    const auto expected = 0.5f;
    const auto sut = clamped<float>::make(expected);

    SECTION("Results in a clamped containing the input") {
      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("clamped<Float>::make_unchecked(Float)", "[factory]") {
  const auto expected = 0.5f;
  const auto sut = clamped<float>::make(expected);

  SECTION("Results in a clamped containing the input") {
    REQUIRE(sut == expected);
  }
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("clamped<Float>::clamped()", "[ctor]") {
  SECTION("Constructor is trivial") {
    STATIC_REQUIRE(std::is_trivially_constructible_v<clamped<float>>);
  }
  SECTION("Constructs a default-initialized underlying float") {
    const auto sut = clamped<float>{};

    REQUIRE(sut.value() == 0.0f);
  }
}

TEST_CASE("clamped<Float>::clamped(const clamped&)", "[ctor]") {
  SECTION("Constructor is trivial") {
    STATIC_REQUIRE(std::is_trivially_copy_constructible_v<clamped<float>>);
  }
  SECTION("Copies underlying float") {
    const auto sut = clamped<float>::make_unchecked(0.5);
    const auto result = sut;

    REQUIRE(sut == result);
  }
}

TEST_CASE("clamped<Float>::clamped(const saturated<UFloat>&)", "[ctor]") {
  SECTION("Copies underlying float") {
    const auto to_copy = saturated<float>::make_unchecked(0.5);
    const auto sut = clamped<float>{to_copy};

    REQUIRE(sut == to_copy.value());
  }
}

TEST_CASE("clamped<Float>::clamped(const clamped<UFloat>&)", "[ctor]") {
  SECTION("Copies underlying float") {
    const auto to_copy = clamped<double>::make_unchecked(0.5);
    const auto sut = clamped<float>{to_copy};

    REQUIRE(sut == static_cast<float>(to_copy.value()));
  }
}

//-----------------------------------------------------------------------------
// Assignment
//-----------------------------------------------------------------------------

TEST_CASE("clamped<Float>::operator=(const clamped&)", "[assign]") {
  SECTION("Assignment is trivial") {
    STATIC_REQUIRE(std::is_trivially_copy_assignable_v<clamped<float>>);
  }
  SECTION("Assigns the underlying value") {
    const auto copy = clamped<float>::make_unchecked(0.5);
    auto sut = clamped<float>{};
    compiler::unused(sut);

    sut = copy;

    REQUIRE(sut == copy);
  }
}
} // namespace alloy::core::test
