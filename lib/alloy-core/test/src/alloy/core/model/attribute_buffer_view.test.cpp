#include <alloy/core/model/attribute_buffer_view.hpp>

#include <catch2/catch.hpp>

#include <array>

namespace alloy::core::test {
namespace {

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("attribute_buffer_view::attribute_buffer_view()", "[ctor]")
{
  const auto sut = vertex_buffer_view{};

  SECTION("Constructs empty view") {
    SECTION("Data is null") {
      REQUIRE(sut.data() == nullptr);
    }

    SECTION("Size is 0") {
      REQUIRE(sut.size() == 0u);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("attribute_buffer_view::attribute_buffer_view(const real*, size_type)", "[ctor]")
{
  const auto data = std::array<real,6u> {
    1,2,3,
    4,5,6,
  };

  const auto sut = normal_buffer_view{data.data(), data.size()};

  SECTION("Constructs view of input sequence") {
    SECTION("Data points to viewed data") {
      REQUIRE(sut.data() == data.data());
    }

    SECTION("Size is data's size / sizeof(value_type)") {
      REQUIRE(sut.size() == 2u);
    }
  }
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

TEST_CASE("attribute_buffer_view::front()", "[element access]")
{
  const auto expected = vector3{1,2,3};
  const auto data = std::array<real,6u> {
    expected.x(), expected.y(), expected.z(),
    4,5,6,
  };

  const auto sut = normal_buffer_view{data.data(), data.size()};

  SECTION("Gets element at the front of the view") {
    REQUIRE(sut.front() == expected);
  }
}

TEST_CASE("attribute_buffer_view::back()", "[element access]")
{
  const auto expected = vector3{1,2,3};
  const auto data = std::array<real,6u> {
    4,5,6,
    expected.x(), expected.y(), expected.z(),
  };

  const auto sut = normal_buffer_view{data.data(), data.size()};

  SECTION("Gets element at the back of the view") {
    REQUIRE(sut.back() == expected);
  }
}

TEST_CASE("attribute_buffer_view::operator[](size_type)", "[element access]")
{
  const auto expected = vector3{1,2,3};
  const auto data = std::array<real,9u> {
    4,5,6,
    expected.x(), expected.y(), expected.z(),
    7,8,9
  };

  const auto sut = normal_buffer_view{data.data(), data.size()};

  SECTION("Gets the nth element") {
    REQUIRE(sut[1] == expected);
  }
}

TEST_CASE("attribute_buffer_view::at(size_type)", "[element access]")
{
  const auto expected = vector3{1,2,3};
  const auto data = std::array<real,9u> {
    4,5,6,
    expected.x(), expected.y(), expected.z(),
    7,8,9
  };

  const auto sut = normal_buffer_view{data.data(), data.size()};

#if ALLOY_CORE_EXCEPTIONS_ENABLED

  SECTION("Index is out of range") {
    SECTION("Throws except") {
      REQUIRE_THROWS_AS(sut.at(5), std::out_of_range);
    }
  }

#endif

  SECTION("Index is in range") {
    SECTION("Gets the nth element") {
      REQUIRE(sut.at(1) == expected);
    }
  }
}

TEST_CASE("attribute_buffer_view::data()", "[element access]")
{
  const auto data = std::array<real,3u> {
    4,5,6,
  };

  const auto sut = normal_buffer_view{data.data(), data.size()};

  SECTION("Gets the underlying data") {
    REQUIRE(sut.data() == data.data());
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("attribute_buffer_view::size()", "[observers]")
{
  const auto data = std::array<real,9u> {
    1, 2, 3,
    4, 5, 6,
    7, 8, 9,
  };

  const auto sut = normal_buffer_view{data.data(), data.size()};

  SECTION("Size is in terms of underyling T elements") {
    REQUIRE(sut.size() == 3u);
    REQUIRE(sut.size() == data.size() / (sizeof(vector3) / sizeof(real)));
  }
}

TEST_CASE("attribute_buffer_view::empty()", "[observers]")
{
  SECTION("View is empty") {
    SECTION("Returns true") {
      const auto sut = normal_buffer_view{};

      REQUIRE(sut.empty());
    }
  }

  SECTION("View is not empty") {
    SECTION("Returns false")
    {
      const auto data = std::array<real, 3u>{
        4, 5, 6,
      };

      const auto sut = normal_buffer_view{ data.data(), data.size() };

      REQUIRE_FALSE(sut.empty());
    }
  }
}


} // namespace <anonymous>
} // namespace alloy::core::test
