/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

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

#include <alloy/core/utilities/delegate.hpp>

#include <memory>
#include <catch2/catch.hpp>

namespace alloy::core::test {
namespace {

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

bool set_bool(bool& x) { return x = true; }

TEST_CASE("delegate::make<Fn>()")
{
  SECTION("Can bind exact function")
  {
    const auto sut = delegate<bool(bool&)>::make<&set_bool>();

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
    const auto sut = delegate<void(bool&)>::make<&set_bool>();

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

  bool set_bool()
  {
    return x = true;
  }

  bool set_other_bool(bool& b) const
  {
    return b = true;
  }
};

TEST_CASE("delegate::make<MemberFn>(C*)")
{
  auto to_bind = setter{false};

  SECTION("Can bind exact function")
  {
    const auto sut = delegate<bool()>::make<&setter::set_bool>(&to_bind);

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
    const auto sut = delegate<void()>::make<&setter::set_bool>(&to_bind);

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
  const auto to_bind = setter{false};

  SECTION("Can bind exact function")
  {
    const auto sut = delegate<bool(bool&)>::make<&setter::set_other_bool>(&to_bind);

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
    const auto sut = delegate<void(bool&)>::make<&setter::set_other_bool>(&to_bind);

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
  bool operator()(){ return x = true; }
};

struct any_setter_functor
{
  template <typename T>
  T operator()(T& x, const T& y) { return x = y; }
};

TEST_CASE("delegate::make(const Callable*)")
{
  SECTION("Can bind non-template functor")
  {
    auto to_bind = bool_setting_functor{false};

    SECTION("Can bind exact function")
    {
      const auto sut = delegate<bool()>::make(&to_bind);

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
      const auto sut = delegate<void()>::make(&to_bind);

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
    auto to_bind = any_setter_functor{};

    SECTION("Can bind exact function")
    {
      const auto sut = delegate<bool(bool&, const bool&)>::make(&to_bind);

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
      const auto sut = delegate<void(bool&, bool)>::make(&to_bind);

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
    const auto to_bind = [](bool& out, bool value) {
      return out = value;
    };

    SECTION("Can bind exact function")
    {
      const auto sut = delegate<bool(bool&, const bool&)>::make(&to_bind);

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
      const auto sut = delegate<void(bool&, bool)>::make(&to_bind);

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
    const auto to_bind = [](auto& out, auto value) {
      return out = value;
    };

    SECTION("Can bind exact function")
    {
      const auto sut = delegate<bool(bool&, const bool&)>::make(&to_bind);

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
      const auto sut = delegate<void(bool&, bool)>::make(&to_bind);

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
  const auto sut = delegate<void()>{};
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
    const auto to_bind = []{};
    auto sut = delegate<void()>::make(&to_bind);
    const auto state_before = static_cast<bool>(sut);

    sut.reset();

    SECTION("State changes")
    {
      const auto state_after = static_cast<bool>(sut);

      REQUIRE(state_before != state_after);
    }

    SECTION("Delegate no longer contains state")
    {
      REQUIRE(!static_cast<bool>(sut));
    }
  }

  SECTION("Delegate does not have a value")
  {
    auto sut = delegate<void()>{};
    const auto state_before = static_cast<bool>(sut);

    sut.reset();

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
    const auto sut = delegate<void()>{};
    SECTION("Throws bad_delegate_call exception")
    {
      REQUIRE_THROWS_AS(sut(), bad_delegate_call);
    }
  }
#endif

  SECTION("Delegate is bound")
  {
    const auto to_bind = [](auto ptr) {
      return ptr != nullptr;
    };

    auto sut = delegate<bool(std::unique_ptr<int>)>::make(&to_bind);

    SECTION("Delegate calls bound function with prvalue")
    {
      REQUIRE(sut(std::make_unique<int>(5)));
    }

    SECTION("Delegate calls bound function with rvlaue")
    {
      auto input = std::make_unique<int>(5);
      REQUIRE(sut(std::move(input)));
    }

  }
}

} // namespace anonymous
} // namespace alloy::core::test