/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include "alloy/core/utilities/int_utilities.hpp"
#include "alloy/core/types.hpp"

#include <catch2/catch.hpp>

#include <vector>
#include <iterator>
#include <array>

namespace alloy::core::test {

namespace {
enum class integer32 : u32 {};
enum class integer16 : u16 {};
}

TEST_CASE("int_utilities::make<u32>(...)") {
  SECTION("Result type is an enum") {
    SECTION("Concatenates values") {
      REQUIRE(int_utilities::make<integer32>(u32{0x12345678}) == integer32{0x12345678});
    }
  }
  SECTION("Input contains enum") {
    SECTION("Concatenates values") {
      REQUIRE(int_utilities::make<u16>(std::byte{0x12}, std::byte{0x34}) == u16{0x1234});
    }
  }
  SECTION("Input and result are numeric") {
    SECTION("Concatenates values") {
      REQUIRE(int_utilities::make<u32>(u8{0x12},u16{0x3456},u8{0x78}) == 0x12345678);
    }
  }
  SECTION("Input is signed") {
    SECTION("Casts to unsigned") {
      REQUIRE(int_utilities::make<u32>(42) == 42u);
    }
  }
}

TEST_CASE("int_utilities::extract<std::size_t,To>(const Int&)") {
  SECTION("Output is enum type") {
    SECTION("Input is enum") {
      const auto input = integer32{0x12345678};

      SECTION("First output is correct") {
        const auto expected = integer16{0x1234};
        const auto v = int_utilities::extract<0,integer16>(input);

        REQUIRE(v == expected);
      }
      SECTION("Second output is correct") {
        const auto expected = integer16{0x5678};
        const auto v = int_utilities::extract<1,integer16>(input);

        REQUIRE(v == expected);
      }
    }
    SECTION("input is numeric") {
      const auto input = u32{0x12345678};

      SECTION("First output is correct") {
        const auto expected = integer16{0x1234};
        const auto v = int_utilities::extract<0,integer16>(input);

        REQUIRE(v == expected);
      }
      SECTION("Second output is correct") {
        const auto expected = integer16{0x5678};
        const auto v = int_utilities::extract<1,integer16>(input);

        REQUIRE(v == expected);
      }
    }
  }
  SECTION("Output is numeric") {
    SECTION("Input is numeric") {
      const auto input = u32{0x12345678};

      SECTION("First output is correct") {
        const auto expected = u16{0x1234};
        const auto v = int_utilities::extract<0,u16>(input);

        REQUIRE(v == expected);
      }
      SECTION("Second output is correct") {
        const auto expected = u16{0x5678};
        const auto v = int_utilities::extract<1,u16>(input);

        REQUIRE(v == expected);
      }
    }
    SECTION("Input is enum") {
      const auto input = integer32{0x12345678};

      SECTION("First output is correct") {
        const auto expected = u16{0x1234};
        const auto v = int_utilities::extract<0,u16>(input);

        REQUIRE(v == expected);
      }
      SECTION("Second output is correct") {
        const auto expected = u16{0x5678};
        const auto v = int_utilities::extract<1,u16>(input);

        REQUIRE(v == expected);
      }
    }
  }
}

TEST_CASE("int_utilities::split<To>(const Int&)") {
  SECTION("Output is enum type") {
    SECTION("Input is enum") {
      auto [a,b,c,d] = int_utilities::split<std::byte>(integer32{0x12345678});

      SECTION("Output is ordered correctly") {
        REQUIRE(a == std::byte{0x12});
        REQUIRE(b == std::byte{0x34});
        REQUIRE(c == std::byte{0x56});
        REQUIRE(d == std::byte{0x78});
      }
    }
    SECTION("Input is numeric") {
      auto [a,b,c,d] = int_utilities::split<std::byte>(u32{0x12345678});

      SECTION("Output is ordered correctly") {
        REQUIRE(a == std::byte{0x12});
        REQUIRE(b == std::byte{0x34});
        REQUIRE(c == std::byte{0x56});
        REQUIRE(d == std::byte{0x78});
      }
    }
  }
  SECTION("Output is numeric") {
    SECTION("Input is numeric") {
      auto [a,b,c,d] = int_utilities::split(u32{0x12345678});

      SECTION("Output is ordered correctly") {
        REQUIRE(a == u8{0x12});
        REQUIRE(b == u8{0x34});
        REQUIRE(c == u8{0x56});
        REQUIRE(d == u8{0x78});
      }
    }
    SECTION("Input is enum") {
      auto [a,b,c,d] = int_utilities::split(integer32{0x12345678});

      SECTION("Output is ordered correctly") {
        REQUIRE(a == u8{0x12});
        REQUIRE(b == u8{0x34});
        REQUIRE(c == u8{0x56});
        REQUIRE(d == u8{0x78});
      }
    }
  }
}

TEST_CASE("int_utilities::split_into<To>(OutputIt, const Int&)") {
  SECTION("Output is enum type") {
    const auto expected = std::vector<std::byte>{{
      std::byte{0x12},std::byte{0x34},std::byte{0x56},std::byte{0x78}
    }};
    auto result = std::vector<std::byte>{};
    SECTION("Input is enum type") {
      int_utilities::split_into<std::byte>(std::back_inserter(result), integer32{0x12345678});

      SECTION("Output is ordered correctly") {
        REQUIRE(result == expected);
      }
    }
    SECTION("Input is numeric") {
      int_utilities::split_into<std::byte>(std::back_inserter(result), u32{0x12345678});

      SECTION("Output is ordered correctly") {
        REQUIRE(result == expected);
      }
    }
  }
  SECTION("Output is numeric") {
    const auto expected = std::vector<u8>{{
      u8{0x12},u8{0x34},u8{0x56},u8{0x78}
    }};
    auto result = std::vector<u8>{};
    SECTION("Input is enum type") {
      int_utilities::split_into(std::back_inserter(result), integer32{0x12345678});

      SECTION("Output is ordered correctly") {
        REQUIRE(result == expected);
      }
    }
    SECTION("Input is numeric") {
      int_utilities::split_into(std::back_inserter(result), u32{0x12345678});

      SECTION("Output is ordered correctly") {
        REQUIRE(result == expected);
      }
    }
  }
}

} // namespace alloy::core::test
