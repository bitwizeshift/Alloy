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

#include "alloy/core/saturated.hpp"

#include <catch2/catch.hpp>

namespace alloy::core::test {

//=============================================================================
// class : saturated
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

TEST_CASE("saturated<Float>::make(Float)", "[factory]") {
  SECTION("Input is above 1.0") {
    const auto sut = saturated<float>::make(1.5f);

    SECTION("Value is clamped to 1.0") {
      REQUIRE(sut.value() == 1.0f);
    }
  }

  SECTION("Input is below 0.0") {
    const auto sut = saturated<float>::make(-1.0f);

    SECTION("Value is clamped to 0.0") {
      REQUIRE(sut.value() == 0.0f);
    }
  }

  SECTION("Input is between 0.0 and 1.0") {
    const auto value = 0.5f;
    const auto sut = saturated<float>::make(value);

    SECTION("Value is passed through") {
      REQUIRE(sut.value() == value);
    }
  }
}

TEST_CASE("saturated<Float>::make_unchecked(Float)", "[factory]") {
  const auto value = 0.5f;
  const auto sut = saturated<float>::make_unchecked(value);

  SECTION("Value is passed through (assumed valid)") {
    REQUIRE(sut.value() == value);
  }
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("saturated<Float>::saturated()", "[ctor]") {
  SECTION("Constructor is trivial") {
    STATIC_REQUIRE(std::is_trivially_constructible_v<saturated<float>>);
  }
  SECTION("Constructs a default-initialized underlying float") {
    const auto sut = saturated<float>{};

    REQUIRE(sut.value() == 0.0f);
  }
}

TEST_CASE("saturated<Float>::saturated(const saturated&)", "[ctor]") {
  SECTION("Constructor is trivial") {
    STATIC_REQUIRE(std::is_trivially_copy_constructible_v<saturated<float>>);
  }
  SECTION("Copies underlying float") {
    const auto sut = saturated<float>::make(0.5);
    const auto result = sut;

    REQUIRE(sut == result);
  }
}

TEST_CASE("saturated<Float>::saturated(const saturated<UFloat>&)", "[ctor]") {
  SECTION("Copies underlying float") {
    const auto to_copy = saturated<double>::make_unchecked(0.5);
    const auto sut = saturated<float>{to_copy};

    REQUIRE(sut == static_cast<float>(to_copy.value()));
  }
}

//-----------------------------------------------------------------------------
// Assignment
//-----------------------------------------------------------------------------

TEST_CASE("saturated<Float>::operator=(const saturated&)", "[assign]") {
  SECTION("Assignment is trivial") {
    STATIC_REQUIRE(std::is_trivially_copy_assignable_v<saturated<float>>);
  }
  SECTION("Assigns the underlying value") {
    const auto copy = saturated<float>::make(0.5);
    auto sut = saturated<float>{};
    compiler::unused(sut);

    sut = copy;

    REQUIRE(sut == copy);
  }
}

//-----------------------------------------------------------------------------
// Arithmetic Operators
//-----------------------------------------------------------------------------

TEST_CASE("saturated<Float>::operator-()", "[arithmetic]") {
  SECTION("Always saturates to 0.0") {
    const auto sut = -saturated<float>::make(0.5f);

    REQUIRE(sut == 0.0f);
  }
}

TEST_CASE("saturated<Float>::operator+()", "[arithmetic]") {
  SECTION("Returns the input") {
    const auto copy = saturated<float>::make(0.5f);
    const auto sut  = +copy;

    REQUIRE(sut == copy);
  }
}

TEST_CASE("saturated<Float>::operator+=(saturated<Float>)", "[arithmetic]") {
  SECTION("Sum exceeds 1.0") {
    SECTION("Saturates value to 1.0") {
      auto sut = saturated<float>::make(0.9f);
      sut += saturated<float>::make_unchecked(0.5f);

      REQUIRE(sut == 1.0f);
    }
  }
  SECTION("Sum does not exceed 1.0") {
    SECTION("Returns sum") {
      const auto value = 0.25f;
      const auto expected = value + value;

      auto sut = saturated<float>::make(value);
      sut += saturated<float>::make_unchecked(value);

      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("saturated<Float>::operator+=(element_type)", "[arithmetic]") {
  SECTION("Sum exceeds 1.0") {
    SECTION("Saturates value to 1.0") {
      auto sut = saturated<float>::make(0.9f);
      sut += 0.5f;

      REQUIRE(sut == 1.0f);
    }
  }
  SECTION("Sum does not exceed 1.0") {
    SECTION("Returns sum") {
      const auto value = 0.25f;
      const auto expected = value + value;

      auto sut = saturated<float>::make(value);
      sut += value;

      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("saturated<Float>::operator-=(saturated<Float>)", "[arithmetic]") {
  SECTION("Difference precedes 0.0") {
    SECTION("Saturates value to 0.0") {
      auto sut = saturated<float>::make(0.1f);
      sut -= saturated<float>::make_unchecked(0.9f);

      REQUIRE(sut == 0.0f);
    }
  }
  SECTION("Difference does not precede 0.0") {
    SECTION("Returns difference") {
      const auto value = 0.25f;
      const auto expected = value - 0.1f;

      auto sut = saturated<float>::make(value);
      sut -= saturated<float>::make_unchecked(0.1f);

      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("saturated<Float>::operator-=(element_type)", "[arithmetic]") {
  SECTION("Difference precedes 0.0") {
    SECTION("Saturates value to 0.0") {
      auto sut = saturated<float>::make(0.1f);
      sut -= 0.9f;

      REQUIRE(sut == 0.0f);
    }
  }
  SECTION("Difference does not precede 0.0") {
    SECTION("Returns difference") {
      const auto value = 0.25f;
      const auto expected = value - 0.1f;

      auto sut = saturated<float>::make(value);
      sut -= 0.1f;

      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("saturated<Float>::operator*=(saturated<Float>)", "[arithmetic]") {
  SECTION("Returns product") {
    const auto value = 0.25f;
    const auto expected = value * value;

    auto sut = saturated<float>::make(value);
    sut *= saturated<float>::make(value);

    REQUIRE(sut == expected);
  }
}

TEST_CASE("saturated<Float>::operator*=(element_type)", "[arithmetic]") {
  SECTION("Product is between 0.0 and 1.0") {
    SECTION("Returns product") {
      const auto value = 0.25f;
      const auto expected = value * 1.5f;

      auto sut = saturated<float>::make(value);
      sut *= 1.5f;

      REQUIRE(sut == expected);
    }
  }
  SECTION("Product exceeds 1.0") {
    SECTION("Returns 1.0") {
      auto sut = saturated<float>::make(0.5f);
      sut *= 100.0f;

      REQUIRE(sut == 1.0f);
    }
  }
  SECTION("Product precedes 0.0") {
    SECTION("Returns 0.0") {
      auto sut = saturated<float>::make(0.5f);
      sut *= -100.0f;

      REQUIRE(sut == 0.0f);
    }
  }
}

TEST_CASE("saturated<Float>::operator/=(saturated<Float>)", "[arithmetic]") {
  SECTION("Quotient exceeds 1.0") {
    SECTION("Saturates value to 1.0") {
      auto sut = saturated<float>::make(0.5f);
      sut /= saturated<float>::make(0.1f);

      REQUIRE(sut == 1.0f);
    }
  }
  SECTION("Quotient does not exceed 1.0") {
    SECTION("Returns quotient") {
      const auto value = 0.25f;
      const auto expected = value / 0.9f;

      auto sut = saturated<float>::make(value);
      sut /= saturated<float>::make(0.9f);

      REQUIRE(sut == expected);
    }
  }
}

TEST_CASE("saturated<Float>::operator/=(element_type)", "[arithmetic]") {
  SECTION("Quotient exceeds 1.0") {
    SECTION("Saturates value to 1.0") {
      auto sut = saturated<float>::make(0.5f);
      sut /= 0.1f;

      REQUIRE(sut == 1.0f);
    }
  }
  SECTION("Quotient precedes 0.0") {
    SECTION("Saturates value to 0.0") {
      auto sut = saturated<float>::make(0.5f);
      sut /= -1.0f;

      REQUIRE(sut == 0.0f);
    }
  }
  SECTION("Quotient does not exceed 1.0") {
    SECTION("Returns quotient") {
      const auto value = 0.25f;
      const auto expected = value / 0.9f;

      auto sut = saturated<float>::make(value);
      sut /= 0.9f;

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
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = lhs;

      REQUIRE(lhs == rhs);
    }
  }
  SECTION("Values are not equal") {
    SECTION("Returns false") {
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      REQUIRE_FALSE(lhs == rhs);
    }
  }
}

TEST_CASE("operator!=(saturated<Float>, saturated<Float>)", "[comparison]") {
  SECTION("Values are equal") {
    SECTION("Returns false") {
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = lhs;

      REQUIRE_FALSE(lhs != rhs);
    }
  }
  SECTION("Values are not equal") {
    SECTION("Returns true") {
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      REQUIRE(lhs != rhs);
    }
  }
}

TEST_CASE("operator<(saturated<Float>,saturated<Float>)", "[comparison]") {
  SECTION("lhs is less than rhs") {
    SECTION("Returns true") {
      const auto lhs = saturated<float>::make(0.0f);
      const auto rhs = saturated<float>::make(0.5f);

      REQUIRE(lhs < rhs);
    }
  }
  SECTION("rhs is less than lhs") {
    SECTION("Returns true") {
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      REQUIRE_FALSE(lhs < rhs);
    }
  }
}

TEST_CASE("operator>(saturated<Float>,saturated<Float>)", "[comparison]") {
  SECTION("lhs is less than rhs") {
    SECTION("Returns false") {
      const auto lhs = saturated<float>::make(0.0f);
      const auto rhs = saturated<float>::make(0.5f);

      REQUIRE_FALSE(lhs > rhs);
    }
  }
  SECTION("rhs is less than lhs") {
    SECTION("Returns true") {
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      REQUIRE(lhs > rhs);
    }
  }
}

TEST_CASE("operator<=(saturated<Float>,saturated<Float>)", "[comparison]") {
  SECTION("lhs is less than rhs") {
    SECTION("Returns true") {
      const auto lhs = saturated<float>::make(0.0f);
      const auto rhs = saturated<float>::make(0.5f);

      REQUIRE(lhs <=rhs);
    }
  }
  SECTION("rhs is less than lhs") {
    SECTION("Returns true") {
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      REQUIRE_FALSE(lhs <= rhs);
    }
  }
}

TEST_CASE("operator>=(saturated<Float>,saturated<Float>)", "[comparison]") {
  SECTION("lhs is less than rhs") {
    SECTION("Returns false") {
      const auto lhs = saturated<float>::make(0.0f);
      const auto rhs = saturated<float>::make(0.5f);

      REQUIRE_FALSE(lhs >= rhs);
    }
  }
  SECTION("rhs is less than lhs") {
    SECTION("Returns true") {
      const auto lhs = saturated<float>::make(0.5f);
      const auto rhs = saturated<float>::make(0.0f);

      REQUIRE(lhs >= rhs);
    }
  }
}

} // namespace alloy::core::test
