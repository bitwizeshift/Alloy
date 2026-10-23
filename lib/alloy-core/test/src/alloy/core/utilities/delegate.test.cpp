// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/utilities/delegate.hpp>

#include <alloy/core/config.hpp>

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

namespace alloy::core::test {
namespace {

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto set_bool(bool& x) -> bool { return x = true; }

TEST_CASE("delegate::make<Fn>()")
{
  SECTION("Can bind exact function")
  {
    // Act
    const auto sut = delegate<bool(bool&)>::make<&set_bool>();

    // Assert
    SECTION("Creates bound delegate")
    {
      REQUIRE(static_cast<bool>(sut));
    }
    SECTION("Will call bound function")
    {
      auto output = false;
      sut(output);

      REQUIRE(output);
    }
  }

  SECTION("Can bind similar function")
  {
    // Act
    const auto sut = delegate<void(bool&)>::make<&set_bool>();

    // Assert
    SECTION("Creates bound delegate")
    {
      REQUIRE(static_cast<bool>(sut));
    }
    SECTION("Will call bound function")
    {
      auto output = false;
      sut(output);

      REQUIRE(output);
    }
  }
}

struct setter
{
  bool x;

  auto set_bool() -> bool
  {
    return x = true;
  }

  auto set_other_bool(bool& b) const -> bool
  {
    return b = true;
  }
};

TEST_CASE("delegate::make<MemberFn>(C*)")
{
  // Arrange
  auto to_bind = setter{false};

  SECTION("Can bind exact function")
  {
    // Act
    const auto sut = delegate<bool()>::make<&setter::set_bool>(&to_bind);

    // Assert
    SECTION("Creates bound delegate")
    {
      REQUIRE(static_cast<bool>(sut));
    }
    SECTION("Will call bound function")
    {
      sut();

      REQUIRE(to_bind.x);
    }
  }

  SECTION("Can bind similar function")
  {
    // Act
    const auto sut = delegate<void()>::make<&setter::set_bool>(&to_bind);

    // Assert
    SECTION("Creates bound delegate")
    {
      REQUIRE(static_cast<bool>(sut));
    }
    SECTION("Will call bound function")
    {
      sut();

      REQUIRE(to_bind.x);
    }
  }
}

TEST_CASE("delegate::make<MemberFn>(const C*)")
{
  // Arrange
  const auto to_bind = setter{false};

  SECTION("Can bind exact function")
  {
    // Act
    const auto sut = delegate<bool(bool&)>::make<&setter::set_other_bool>(&to_bind);

    // Assert
    SECTION("Creates bound delegate")
    {
      REQUIRE(static_cast<bool>(sut));
    }
    SECTION("Will call bound function")
    {
      auto output = false;
      sut(output);

      REQUIRE(output);
    }
  }

  SECTION("Can bind similar function")
  {
    // Act
    const auto sut = delegate<void(bool&)>::make<&setter::set_other_bool>(&to_bind);

    // Assert
    SECTION("Creates bound delegate")
    {
      REQUIRE(static_cast<bool>(sut));
    }
    SECTION("Will call bound function")
    {
      auto output = false;
      sut(output);

      REQUIRE(output);
    }
  }
}

struct bool_setting_functor
{
  bool x;
  auto operator()() -> bool { return x = true; }
};

struct any_setter_functor
{
  template <typename T>
  auto operator()(T& x, const T& y) -> T { return x = y; }
};

TEST_CASE("delegate::make(const Callable*)")
{
  SECTION("Can bind non-template functor")
  {
    // Arrange
    auto to_bind = bool_setting_functor{false};

    SECTION("Can bind exact function")
    {
      // Act
      const auto sut = delegate<bool()>::make(&to_bind);

      // Assert
      SECTION("Creates bound delegate")
      {
        REQUIRE(static_cast<bool>(sut));
      }
      SECTION("Will call bound function")
      {
        sut();

        REQUIRE(to_bind.x);
      }
    }

    SECTION("Can bind similar function")
    {
      // Act
      const auto sut = delegate<void()>::make(&to_bind);

      // Assert
      SECTION("Creates bound delegate")
      {
        REQUIRE(static_cast<bool>(sut));
      }
      SECTION("Will call bound function")
      {
        sut();

        REQUIRE(to_bind.x);
      }
    }
  }

  SECTION("Can bind template functor")
  {
    // Arrange
    auto to_bind = any_setter_functor{};

    SECTION("Can bind exact function")
    {
      // Act
      const auto sut = delegate<bool(bool&, const bool&)>::make(&to_bind);

      // Assert
      SECTION("Creates bound delegate")
      {
        REQUIRE(static_cast<bool>(sut));
      }
      SECTION("Will call bound function")
      {
        auto output = false;
        sut(output, true);

        REQUIRE(output);
      }
    }

    SECTION("Can bind similar function")
    {
      // Act
      const auto sut = delegate<void(bool&, bool)>::make(&to_bind);

      // Assert
      SECTION("Creates bound delegate")
      {
        REQUIRE(static_cast<bool>(sut));
      }
      SECTION("Will call bound function")
      {
        auto output = false;
        sut(output, true);

        REQUIRE(output);
      }
    }
  }

  SECTION("Can bind lambda")
  {
    // Arrange
    const auto to_bind = [](bool& out, bool value) {
      return out = value;
    };

    SECTION("Can bind exact function")
    {
      // Act
      const auto sut = delegate<bool(bool&, const bool&)>::make(&to_bind);

      // Assert
      SECTION("Creates bound delegate")
      {
        REQUIRE(static_cast<bool>(sut));
      }
      SECTION("Will call bound function")
      {
        auto output = false;
        sut(output, true);

        REQUIRE(output);
      }
    }

    SECTION("Can bind similar function")
    {
      // Act
      const auto sut = delegate<void(bool&, bool)>::make(&to_bind);

      // Assert
      SECTION("Creates bound delegate")
      {
        REQUIRE(static_cast<bool>(sut));
      }
      SECTION("Will call bound function")
      {
        auto output = false;
        sut(output, true);

        REQUIRE(output);
      }
    }
  }

  SECTION("Can bind auto-capture lambda")
  {
    // Arrange
    const auto to_bind = [](auto& out, auto value) {
      return out = value;
    };

    SECTION("Can bind exact function")
    {
      // Act
      const auto sut = delegate<bool(bool&, const bool&)>::make(&to_bind);

      // Assert
      SECTION("Creates bound delegate")
      {
        REQUIRE(static_cast<bool>(sut));
      }
      SECTION("Will call bound function")
      {
        auto output = false;
        sut(output, true);

        REQUIRE(output);
      }
    }

    SECTION("Can bind similar function")
    {
      // Act
      const auto sut = delegate<void(bool&, bool)>::make(&to_bind);

      // Assert
      SECTION("Creates bound delegate")
      {
        REQUIRE(static_cast<bool>(sut));
      }
      SECTION("Will call bound function")
      {
        auto output = false;
        sut(output, true);

        REQUIRE(output);
      }
    }
  }
}

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("delegate::delegate()")
{
  // Act
  const auto sut = delegate<void()>{};

  // Assert
  SECTION("Creates unbound delegate")
  {
    REQUIRE(!static_cast<bool>(sut));
  }
}

//------------------------------------------------------------------------------

// Not testing 'bind', since this just delegates to 'make'

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

TEST_CASE("delegate::reset()")
{
  SECTION("Delegate has a value")
  {
    // Arrange
    const auto to_bind = []{};
    auto sut = delegate<void()>::make(&to_bind);
    const auto state_before = static_cast<bool>(sut);

    // Act
    sut.reset();

    SECTION("State changes")
    {
      const auto state_after = static_cast<bool>(sut);

      // Assert
      REQUIRE(state_before != state_after);
    }

    // Assert
    SECTION("Delegate no longer contains state")
    {
      REQUIRE(!static_cast<bool>(sut));
    }
  }

  SECTION("Delegate does not have a value")
  {
    // Arrange
    auto sut = delegate<void()>{};
    const auto state_before = static_cast<bool>(sut);

    // Act
    sut.reset();

    // Assert
    SECTION("Delegate does not contain state")
    {
      REQUIRE(!static_cast<bool>(sut));
    }

    SECTION("State remains unchanged")
    {
      const auto state_after = static_cast<bool>(sut);

      REQUIRE(state_before == state_after);
    }
  }
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

TEST_CASE("delegate::operator(Args...)")
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  SECTION("Delegate is unbound")
  {
    // Arrange
    const auto sut = delegate<void()>{};
    SECTION("Throws bad_delegate_call exception")
    {
      // Arrange
      auto threw_expected = false;

      // Act
      try {
        sut();
      } catch (const bad_delegate_call&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
#endif

  SECTION("Delegate is bound")
  {
    // Arrange
    const auto to_bind = [](auto ptr) {
      return ptr != nullptr;
    };

    auto sut = delegate<bool(std::unique_ptr<int>)>::make(&to_bind);

    SECTION("Delegate calls bound function with prvalue")
    {
      // Act
      const auto result = sut(std::make_unique<int>(5));

      // Assert
      REQUIRE(result);
    }

    SECTION("Delegate calls bound function with rvlaue")
    {
      auto input = std::make_unique<int>(5);

      // Act
      const auto result = sut(std::move(input));

      // Assert
      REQUIRE(result);
    }

  }
}

} // namespace anonymous
} // namespace alloy::core::test
