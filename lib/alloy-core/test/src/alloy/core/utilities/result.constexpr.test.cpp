/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include "alloy/core/utilities/result.hpp"

#include <catch2/catch.hpp>

namespace alloy::core {
namespace test {
namespace {

/// \brief A pointless utility that exists to force a move operation to occur
///        in a constexpr context
///
/// \param x the value to move
/// \return a new T object
template <typename T>
constexpr auto force_move(T&& x) -> T {
  return static_cast<T&&>(x);
}

struct constexpr_type
{
  constexpr constexpr_type() noexcept = default;
  constexpr constexpr_type(const constexpr_type&) noexcept = default;
  constexpr constexpr_type(constexpr_type&&) noexcept = default;
  constexpr constexpr_type(int x) noexcept : m_storage{x}{}

  auto operator=(const constexpr_type&) noexcept -> constexpr_type& = default;
  auto operator=(constexpr_type&&) noexcept -> constexpr_type& = default;
  auto operator=(int x) noexcept -> constexpr_type&
  {
    m_storage = x;
    return (*this);
  }
  int m_storage = 0;
};

inline constexpr
auto operator==(const constexpr_type& lhs, const constexpr_type& rhs)
  noexcept -> bool
{
  return lhs.m_storage == rhs.m_storage;
}

using literal_sut = result<constexpr_type, constexpr_type>;

static_assert(std::is_trivially_copyable<literal_sut>::value, "");
static_assert(std::is_trivially_destructible<literal_sut>::value, "");

} // namespace <anonymous>

//=============================================================================
// class : result<T,E>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("constexpr result<T,E>::result()", "[constexpr][ctor]") {
  constexpr literal_sut sut{};

  STATIC_REQUIRE(sut.has_value());
}

TEST_CASE("constexpr result<T,E>::result(const result&)", "[constexpr][ctor]") {
  constexpr literal_sut original{};
  constexpr literal_sut sut{original};

  STATIC_REQUIRE(sut.has_value());
}

TEST_CASE("constexpr result<T,E>::result(result&&)", "[constexpr][ctor]") {
  constexpr literal_sut sut{ force_move(literal_sut{}) };

  STATIC_REQUIRE(sut.has_value());
}

TEST_CASE("constexpr result<T,E>::result(in_place_t, Args&&...)", "[constexpr][ctor]") {
  constexpr literal_sut sut{std::in_place, 42};

  STATIC_REQUIRE(sut.has_value());
}

TEST_CASE("constexpr result<T,E>::result(in_place_error_t, Args&&...)", "[constexpr][ctor]") {
  constexpr literal_sut sut{in_place_error, 42};

  STATIC_REQUIRE(sut.has_error());
}

TEST_CASE("constexpr result<T,E>::result(U&&) (implicit)", "[constexpr][ctor]") {
  constexpr literal_sut sut = 42;

  STATIC_REQUIRE(sut.has_value());
}

TEST_CASE("constexpr result<T,E>::result(U&&) (explicit)", "[constexpr][ctor]") {
  constexpr literal_sut sut{42};

  STATIC_REQUIRE(sut.has_value());
}

TEST_CASE("constexpr result<T,E>::result(const unresult<U>&)", "[constexpr][ctor]") {
  constexpr failure<int> value{42};
  constexpr literal_sut sut{value};

  STATIC_REQUIRE(sut.has_error());
}

#if __cplusplus >= 201402L

// This constructor cannot be constexpr since C++11 does not allow non-const
// member functions to be constexpr, which disallows extracting an rvalue
// references from 'failure' in a constant expression. C++14 fixes this
TEST_CASE("constexpr result<T,E>::result(failure<U>&&)") {
  constexpr literal_sut sut{failure<int>{42}};

  STATIC_REQUIRE(sut.has_error());
}
#endif

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("constexpr result<T,E>::value() const &", "[constexpr][observer]") {
  constexpr auto value = 42;
  constexpr literal_sut sut{value};

  STATIC_REQUIRE(sut.value() == value);
}

TEST_CASE("constexpr result<T,E>::value() const &&", "[constexpr][observer]") {
  constexpr auto value = 42;
  constexpr literal_sut sut{value};

  STATIC_REQUIRE(static_cast<const literal_sut&&>(sut).value() == value);
}

TEST_CASE("constexpr result<T,E>::error() const &", "[constexpr][observer]") {
  constexpr auto value = 42;
  constexpr auto error = failure<int>{value};
  constexpr literal_sut sut{error};

  STATIC_REQUIRE(sut.error() == value);
}

//=============================================================================
// class : result<void,E>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("constexpr result<void,E>::result()", "[constexpr][ctor]") {
  constexpr result<void,int> sut{};

  STATIC_REQUIRE(sut.has_value());
}

TEST_CASE("constexpr result<void,E>::result(const result&)", "[constexpr][ctor]") {
  constexpr result<void,int> original{};
  constexpr result<void,int> sut{original};

  STATIC_REQUIRE(sut.has_value());
}

TEST_CASE("constexpr result<void,E>::result(result&&)", "[constexpr][ctor]") {
  constexpr result<void,int> sut{ force_move(result<void,int>{}) };

  STATIC_REQUIRE(sut.has_value());
}


TEST_CASE("constexpr result<void,E>::result(in_place_error_t, Args&&...)", "[constexpr][ctor]") {
  constexpr result<void,int> sut{in_place_error, 42};

  STATIC_REQUIRE(sut.has_error());
}

TEST_CASE("constexpr result<void,E>::result(const failure<U>&)", "[constexpr][ctor]") {
  constexpr failure<int> value{42};
  constexpr result<void,int> sut{value};

  STATIC_REQUIRE(sut.has_error());
}

#if __cplusplus >= 201402L

// This constructor cannot be constexpr since C++11 does not allow non-const
// member functions to be constexpr, which disallows extracting an rvalue
// references from 'failure' in a constant expression. C++14 fixes this
TEST_CASE("constexpr result<void,E>::result(failure<U>&&)") {
  constexpr result<void,int> sut{failure<int>{42}};

  STATIC_REQUIRE(sut.has_error());
}
#endif

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

#if __cplusplus >= 201402L

TEST_CASE("constexpr result<void,E>::value() const", "[constexpr][observer]") {
  constexpr result<void,int> sut{};

  STATIC_REQUIRE((sut.value(),true));
}

#endif // __cplusplus >= 201402L

TEST_CASE("constexpr result<void,E>::error() const &", "[constexpr][observer]") {
  constexpr auto value = 42;
  constexpr auto error = failure<int>{value};
  constexpr result<void,int> sut{error};

  STATIC_REQUIRE(sut.error() == value);
}

} // namespace test
} // namespace cpp
