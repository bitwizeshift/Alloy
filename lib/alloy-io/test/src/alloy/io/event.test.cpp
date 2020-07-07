#include <alloy/io/event.hpp>
#include <alloy/io/events/window_events.hpp>

#include <catch2/catch.hpp>

namespace {
struct move_event{

  bool* to_write;

  move_event(bool* p) : to_write{p}{}

  move_event(move_event&& other)
    : to_write{other.to_write}
  {
    *to_write = true;
  }

  move_event(const move_event& other) = default;
};

struct copy_event{

  bool* to_write;

  copy_event(bool* p) : to_write{p}{}

  copy_event(const copy_event& other)
    : to_write{other.to_write}
  {
    *to_write = true;
  }

  copy_event(copy_event&& other) = default;
};

struct destructor_event{
  bool* to_write;

  destructor_event(bool* p) : to_write{p}{}
  destructor_event(const destructor_event&) = default;
  destructor_event(destructor_event&&) = default;

  ~destructor_event() { *to_write = true; }
};

}

ALLOY_DEFINE_USER_EVENT(copy_event, 1);
ALLOY_DEFINE_USER_EVENT(move_event, 2);
ALLOY_DEFINE_USER_EVENT(destructor_event, 3);

namespace alloy::io::test {
namespace {

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

TEST_CASE("event::make_event(Args&&...args)", "[factory]")
{
  const auto sut = event::make_event<window_close_event>(nullptr);

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
  const auto sut = event{};

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
  const auto expected = window_close_event{nullptr};
  const auto sut = event{expected};

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
    auto source = event::make_event<window_close_event>(nullptr);
    const auto expected = source;

    const auto sut = std::move(source);

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
    auto result = false;
    auto to_move = event::make_event<move_event>(&result);

    const auto sut = std::move(to_move);

    REQUIRE(result);
  }
}


TEST_CASE("event::event(const event& event)", "[ctor]")
{
  SECTION("Copies the source event")
  {
    const auto source = event::make_event<window_close_event>(nullptr);

    const auto sut = source;

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
    auto result = false;
    const auto to_copy = event::make_event<copy_event>(&result);

    const auto sut = to_copy;

    REQUIRE(result);
  }
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("event::operator=(event&& event)", "[ctor]")
{
  SECTION("Moves the source event")
  {
    auto sut = event{};
    auto source = event::make_event<window_close_event>(nullptr);
    const auto expected = source;

    sut = std::move(source);

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
    auto result = false;
    auto to_move = event::make_event<move_event>(&result);
    auto sut = event{};

    sut = std::move(to_move);

    REQUIRE(result);
  }

  SECTION("Destructs existing event first")
  {
    auto result = false;
    auto sut = event::make_event<destructor_event>(&result);
    auto other = event{};

    sut = std::move(other);

    REQUIRE(result);
  }
}


TEST_CASE("event::operator=(const event& event)", "[ctor]")
{
  SECTION("Copies the source event")
  {
    auto sut = event{};
    const auto source = event::make_event<window_close_event>(nullptr);

    sut = source;

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
    auto result = false;
    const auto to_copy = event::make_event<copy_event>(&result);
    auto sut = event{};

    sut = to_copy;
    REQUIRE(result);
  }

  SECTION("Destructs existing event first")
  {
    auto result = false;
    auto sut = event::make_event<destructor_event>(&result);
    auto other = event{};

    sut = other;

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
    const auto sut = event::make_event<window_close_event>(nullptr);

    SECTION("Returns true for T = current event")
    {
      REQUIRE(sut.is<window_close_event>());
    }

    SECTION("Returns false if T is not the current event")
    {
      REQUIRE(!sut.is<copy_event>());
    }
  }
  SECTION("Event does not contain an event")
  {
    const auto sut = event{};

    SECTION("Returns false if T is not the current event")
    {
      REQUIRE(!sut.is<window_close_event>());
    }
  }
}


TEST_CASE("event::as<T>()", "[observer]")
{
  SECTION("Event contains a valid event")
  {
    const auto sut = event::make_event<window_close_event>(nullptr);

    SECTION("Returns underlying event on success")
    {
      REQUIRE_NOTHROW(sut.as<window_close_event>());
    }
  }
}


TEST_CASE("event::try_as<T>()", "[observer]")
{
  SECTION("Event contains a valid event")
  {
    const auto sut = event::make_event<window_close_event>(nullptr);

    SECTION("Returns non-null on success")
    {
      REQUIRE(sut.try_as<window_close_event>() != nullptr);
    }

    SECTION("Returns null on failure")
    {
      REQUIRE(sut.try_as<copy_event>() == nullptr);
    }
  }
  SECTION("Event does not contain valid event")
  {
    const auto sut = event{};

    SECTION("Returns null on failure")
    {
      REQUIRE(sut.try_as<copy_event>() == nullptr);
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
    auto sut = event::make_event<window_close_event>(nullptr);
    const auto old_id = sut.id();

    sut.reset();

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
    auto sut = event{};
    const auto old_id = sut.id();

    sut.reset();

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
} // namespace alloy::io::test