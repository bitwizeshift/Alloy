/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include "alloy/core/containers/polymorphic_vector.hpp"

#include <catch2/catch.hpp>

namespace alloy::core {
namespace {

  struct interface {
    virtual ~interface() = 0;

    virtual auto get() const noexcept -> int = 0;

    virtual auto clone_with(allocator alloc) const -> interface* = 0;
  };
  interface::~interface() = default;

  struct object final : interface{
    explicit object(int a) : a{a}{}
    auto get() const noexcept -> int override { return a; }
    auto clone_with(allocator alloc) const -> interface* override { return alloc.make<object>(*this); }
    int a;
  };

  struct destructible final : interface{
    explicit destructible(bool* p) : m_destructor_called{p}{}
    ~destructible() { (*m_destructor_called) = true; }
    auto get() const noexcept -> int override { return 42; }
    auto clone_with(allocator alloc) const -> interface* override { return alloc.make<destructible>(*this); }

    bool* m_destructor_called;
  };

} // namespace <anonymous>

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

TEST_CASE("polymorphic_vector::polymorphic_vector()", "[ctor]") {
  const auto sut = polymorphic_vector<interface>{};

  SECTION("Has zero capacity") {
    REQUIRE(sut.capacity() == 0u);
  }
  SECTION("Has zero size") {
    REQUIRE(sut.size() == 0u);
    REQUIRE(sut.empty());
  }
}

TEST_CASE("polymorphic_vector::polymorphic_vector(size_type, const Derived&)", "[ctor]") {
  const auto copy = object{5};
  const auto size = 5u;

  const auto sut = polymorphic_vector<interface>{size, copy};

  SECTION("Has specified size") {
    REQUIRE(sut.size() == size);
    REQUIRE_FALSE(sut.empty());
  }
  SECTION("Contains N copies") {
    for (const auto& v : sut) {
      REQUIRE(v.get() == copy.get());
    }
  }
}

TEST_CASE("polymorphic_vector::polymorphic_vector(polymorphic_vector&&)", "[ctor]") {
  const auto copy = object{5};
  const auto size = 5u;

  auto original = polymorphic_vector<interface>{size, copy};
  const auto sut = std::move(original);

  SECTION("Has specified size") {
    REQUIRE(sut.size() == size);
    REQUIRE_FALSE(sut.empty());
  }
  SECTION("Contains N copies") {
    for (const auto& v : sut) {
      REQUIRE(v.get() == copy.get());
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("polymorphic_vector::operator=(polymorphic_vector&&)", "[assignment]") {
  const auto copy = object{5};
  const auto size = 5u;

  auto original = polymorphic_vector<interface>{size, copy};
  auto sut = polymorphic_vector<interface>{};

  sut = std::move(original);

  SECTION("Has specified size") {
    REQUIRE(sut.size() == size);
    REQUIRE_FALSE(sut.empty());
  }
  SECTION("Contains N copies") {
    for (const auto& v : sut) {
      REQUIRE(v.get() == copy.get());
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("polymorphic_vector::copy()", "[copying]") {
  const auto copy = object{5};
  const auto size = 5u;

  const auto original = polymorphic_vector<interface>{size, copy};
  const auto sut = original.copy();

  SECTION("Has specified size") {
    REQUIRE(sut.size() == size);
    REQUIRE_FALSE(sut.empty());
  }
  SECTION("Contains N copies") {
    const auto result = std::equal(
      original.begin(), original.end(),
      sut.begin(), sut.end(),
      [](const auto& lhs, const auto& rhs) -> bool { return lhs.get() == rhs.get(); }
    );
    REQUIRE(result);
  }
}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

TEST_CASE("polymorphic_vector::reserve(size_type)", "[capacity]") {
  const auto size = 5u;

  auto sut = polymorphic_vector<interface>{};

  sut.reserve(size);

  SECTION("Changes capacity") {
    REQUIRE(sut.capacity() >= size);
  }
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

TEST_CASE("polymorphic_vector::clear()", "[modifiers]") {
  bool destructor_called = false;

  auto sut = polymorphic_vector<interface>{};
  sut.emplace_back<destructible>(&destructor_called);
  const auto capacity_before = sut.capacity();

  sut.clear();

  SECTION("Does not change capacity") {
    REQUIRE(sut.capacity() == capacity_before);
  }
  SECTION("Sets size to zero") {
    REQUIRE(sut.size() == 0u);
  }
  SECTION("Size is empty") {
    REQUIRE(sut.empty());
  }
  SECTION("Calls destructor") {
    REQUIRE(destructor_called);
  }
}

TEST_CASE("polymorphic_vector::emplace_back<Derived>(args...)", "[modifiers]") {
  auto sut = polymorphic_vector<interface>{};
  const auto size_before = sut.size();

  const auto expected = object{13};

  sut.emplace_back<object>(expected);

  SECTION("Increases size by 1") {
    const auto expected_size = size_before + 1u;

    REQUIRE(expected_size == sut.size());
  }
  SECTION("Inserts object at the end") {
    REQUIRE(expected.get() == sut.back().get());
  }
  SECTION("Returns reference to constructed object") {
    const auto& ref = sut.emplace_back<object>(0xdead);

    REQUIRE(&ref == &sut.back());
  }
}

TEST_CASE("polymorphic_vector::pop_back()", "[modifiers]") {
  const auto size = 5u;
  bool destructor_called = false;
  auto sut = polymorphic_vector<interface>{size, object{42}};

  sut.emplace_back<destructible>(&destructor_called);
  const auto size_before = sut.size();
  const auto* back_before = &sut.back();

  sut.pop_back();

  SECTION("Decrements size by 1") {
    const auto expected = size_before - 1u;

    REQUIRE(expected == sut.size());
  }
  SECTION("Calls destructor") {
    REQUIRE(destructor_called);
  }
  SECTION("Removes object at the end") {
    REQUIRE(back_before != &sut.back());
  }
}
} // namespace alloy::core
