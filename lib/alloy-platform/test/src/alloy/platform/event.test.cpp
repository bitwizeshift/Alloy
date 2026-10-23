// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/platform/event.hpp>

#include <alloy/platform/events/window_events.hpp>

#include <catch2/catch_test_macros.hpp>

#include <exception> // std::exception_ptr, std::current_exception
#include <utility>

namespace {
struct move_event{

  bool* to_write;

  move_event(bool* p)
    : to_write{p}{}

  move_event(move_event&& other)
    : to_write{other.to_write}
  {
    *to_write = true;
  }

  move_event(const move_event& other) = default;
};

struct copy_event{

  bool* to_write;

  copy_event(bool* p)
    : to_write{p}{}

  copy_event(const copy_event& other)
    : to_write{other.to_write}
  {
    *to_write = true;
  }

  copy_event(copy_event&& other) = default;
};

struct destructor_event{
  bool* to_write;

  destructor_event(bool* p)
    : to_write{p}{}
  destructor_event(const destructor_event&) = default;
  destructor_event(destructor_event&&) = default;

  ~destructor_event() { *to_write = true; }
};

}

ALLOY_DEFINE_USER_EVENT(copy_event, 1);
ALLOY_DEFINE_USER_EVENT(move_event, 2);
ALLOY_DEFINE_USER_EVENT(destructor_event, 3);

namespace alloy::platform::test {
namespace {

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

TEST_CASE("event::make_event(Args&&...args)", "[factory]")
{
  // Act
  const auto sut = event::make_event<window_close_event>(nullptr);

  // Assert
  SECTION("Constructs event with ID of underlying event")
  {
    REQUIRE(sut.id() == event::id_of<window_close_event>());
  }

  SECTION("Constructs event with priority of underlying event")
  {
    REQUIRE(sut.priority() == event::priority_of<window_close_event>());
  }
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("event::event()", "[ctor]")
{
  // Act
  const auto sut = event{};

  // Assert
  SECTION("Constructs event with 0 ID")
  {
    REQUIRE(sut.id() == event::id_type{0u});
  }

  SECTION("Constructs event with no priority")
  {
    REQUIRE(sut.priority() == event_priority::none);
  }
}


TEST_CASE("event::event(Event&& event)", "[ctor]")
{
  // Arrange
  const auto expected = window_close_event{nullptr};

  // Act
  const auto sut = event{expected};

  // Assert
  SECTION("Constructs event with ID of underlying event")
  {
    REQUIRE(sut.id() == event::id_of<window_close_event>());
  }

  SECTION("Constructs event with priority of underlying event")
  {
    REQUIRE(sut.priority() == event::priority_of<window_close_event>());
  }
}


TEST_CASE("event::event(event&& event)", "[ctor]")
{
  SECTION("Moves the source event")
  {
    // Arrange
    auto source = event::make_event<window_close_event>(nullptr);
    const auto expected = source;

    // Act
    const auto sut = std::move(source);

    // Assert
    SECTION("Has same underlying event id")
    {
      REQUIRE(sut.id() == expected.id());
    }

    SECTION("Has same underlying priority")
    {
      REQUIRE(sut.priority() == expected.priority());
    }
  }

  SECTION("Calls move constructor on underlying event")
  {
    // Arrange
    auto result = false;
    auto to_move = event::make_event<move_event>(&result);

    // Act
    const auto sut = std::move(to_move);

    // Assert
    REQUIRE(result);
  }
}


TEST_CASE("event::event(const event& event)", "[ctor]")
{
  SECTION("Copies the source event")
  {
    // Arrange
    const auto source = event::make_event<window_close_event>(nullptr);

    // Act
    const auto sut = source;

    // Assert
    SECTION("Has same underlying event id")
    {
      REQUIRE(sut.id() == source.id());
    }

    SECTION("Has same underlying priority")
    {
      REQUIRE(sut.priority() == source.priority());
    }
  }

  SECTION("Calls copy constructor on underlying event")
  {
    // Arrange
    auto result = false;
    const auto to_copy = event::make_event<copy_event>(&result);

    // Act
    const auto sut = to_copy;

    // Assert
    REQUIRE(result);
  }
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("event::operator=(event&& event)", "[assign]")
{
  SECTION("Moves the source event")
  {
    // Arrange
    auto sut = event{};
    auto source = event::make_event<window_close_event>(nullptr);
    const auto expected = source;

    // Act
    sut = std::move(source);

    // Assert
    SECTION("Has same underlying event id")
    {
      REQUIRE(sut.id() == expected.id());
    }

    SECTION("Has same underlying priority")
    {
      REQUIRE(sut.priority() == expected.priority());
    }
  }

  SECTION("Calls move constructor on underlying event")
  {
    // Arrange
    auto result = false;
    auto to_move = event::make_event<move_event>(&result);
    auto sut = event{};

    // Act
    sut = std::move(to_move);

    // Assert
    REQUIRE(result);
  }

  SECTION("Destructs existing event first")
  {
    // Arrange
    auto result = false;

    auto sut = event::make_event<destructor_event>(&result);
    auto other = event{};

    // Act
    sut = std::move(other);

    // Assert
    REQUIRE(result);
  }
}


TEST_CASE("event::operator=(const event& event)", "[assign]")
{
  SECTION("Copies the source event")
  {
    // Arrange
    auto sut = event{};
    const auto source = event::make_event<window_close_event>(nullptr);

    // Act
    sut = source;

    // Assert
    SECTION("Has same underlying event id")
    {
      REQUIRE(sut.id() == source.id());
    }

    SECTION("Has same underlying priority")
    {
      REQUIRE(sut.priority() == source.priority());
    }
  }

  SECTION("Calls copy constructor on underlying event")
  {
    // Arrange
    auto result = false;
    const auto to_copy = event::make_event<copy_event>(&result);
    auto sut = event{};

    // Act
    sut = to_copy;

    // Assert
    REQUIRE(result);
  }

  SECTION("Destructs existing event first")
  {
    // Arrange
    auto result = false;

    auto sut = event::make_event<destructor_event>(&result);
    auto other = event{};

    // Act
    sut = other;

    // Assert
    REQUIRE(result);
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("event::is<T>()", "[observer]")
{
  SECTION("Event contains a valid event")
  {
    // Arrange
    const auto sut = event::make_event<window_close_event>(nullptr);

    SECTION("Returns true for T = current event")
    {
      // Act
      const auto result = sut.is<window_close_event>();

      // Assert
      REQUIRE(result);
    }

    SECTION("Returns false if T is not the current event")
    {
      // Act
      const auto result = !sut.is<copy_event>();

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Event does not contain an event")
  {
    // Arrange
    const auto sut = event{};

    SECTION("Returns false if T is not the current event")
    {
      // Act
      const auto result = !sut.is<window_close_event>();

      // Assert
      REQUIRE(result);
    }
  }
}


TEST_CASE("event::as<T>()", "[observer]")
{
  SECTION("Event contains a valid event")
  {
    // Arrange
    const auto sut = event::make_event<window_close_event>(nullptr);

    SECTION("Returns underlying event on success")
    {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        sut.as<window_close_event>();
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
  }
}


TEST_CASE("event::try_as<T>()", "[observer]")
{
  SECTION("Event contains a valid event")
  {
    // Arrange
    const auto sut = event::make_event<window_close_event>(nullptr);

    SECTION("Returns non-null on success")
    {
      // Act
      const auto result = sut.try_as<window_close_event>();

      // Assert
      REQUIRE(result != nullptr);
    }

    SECTION("Returns null on failure")
    {
      // Act
      const auto result = sut.try_as<copy_event>();

      // Assert
      REQUIRE(result == nullptr);
    }
  }
  SECTION("Event does not contain valid event")
  {
    // Arrange
    const auto sut = event{};

    SECTION("Returns null on failure")
    {
      // Act
      const auto result = sut.try_as<copy_event>();

      // Assert
      REQUIRE(result == nullptr);
    }
  }
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

TEST_CASE("event::reset()", "[modifiers]")
{
  SECTION("Event contains a value")
  {
    // Arrange
    auto sut = event::make_event<window_close_event>(nullptr);
    const auto old_id = sut.id();

    // Act
    sut.reset();

    // Assert
    SECTION("Priority is none")
    {
      REQUIRE(sut.priority() == event_priority::none);
    }
    SECTION("ID changes from old underlying id")
    {
      REQUIRE(old_id != sut.id());
    }
  }

  SECTION("Event does not contain a value")
  {
    // Arrange
    auto sut = event{};
    const auto old_id = sut.id();

    // Act
    sut.reset();

    // Assert
    SECTION("Priority is none")
    {
      REQUIRE(sut.priority() == event_priority::none);
    }
    SECTION("ID changes from old underlying id")
    {
      REQUIRE(old_id == sut.id());
    }
  }
}

} // namespace <anonymous>
} // namespace alloy::platform::test
