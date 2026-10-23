// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>     // std::reference_wrapper

namespace alloy::core {
namespace test {
namespace {

struct not_nothrow_default_constructible
{
  not_nothrow_default_constructible() noexcept(false){}
};

struct not_trivial_default_constructible
{
  not_trivial_default_constructible(){}
};

struct not_default_constructible
{
  not_default_constructible() = delete;
};

struct not_copy_or_moveable{
  not_copy_or_moveable() = default;
  not_copy_or_moveable(const not_copy_or_moveable&) = delete;
  not_copy_or_moveable(not_copy_or_moveable&&) = delete;
};

struct default_construct_test
{
  static constexpr auto default_value = 42;

  default_construct_test()
    : value{default_value}{}

  int value;
};
constexpr decltype(default_construct_test::default_value) default_construct_test::default_value;

template <typename T>
struct explicit_type : public T
{
  template <typename...Args,
            typename = typename std::enable_if<std::is_constructible<T,Args...>::value>::type>
  explicit explicit_type(Args&&...args)
    : T(std::forward<Args>(args)...)
  {
  }

  explicit explicit_type() = default;
  explicit explicit_type(const explicit_type&) = default;
  explicit explicit_type(explicit_type&&) = default;

  auto operator=(const explicit_type&) -> explicit_type& = default;
  auto operator=(explicit_type&&) -> explicit_type& = default;
};

template <typename T>
struct move_only : public T
{
  using T::T;

  move_only() = default;

  move_only(T&& other)
    : T(std::move(other)){}
  move_only(move_only&&) = default;
  move_only(const move_only&) = delete;

  auto operator=(move_only&&) -> move_only& = default;
  auto operator=(const move_only&) -> move_only& = delete;
};

} // namespace <anonymous>

//=============================================================================
// class : failure<E>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("failure<E>::failure()", "[ctor]") {
  SECTION("T is default constructible") {
    SECTION("Unexpected is also default constructible") {
      // Arrange
      using sut_type = failure<int>;

      // Act
      constexpr auto property_holds =
        std::is_default_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("T is nothrow default constructible") {
      SECTION("Unexpected propagates noexcept status") {
        // Arrange
        using sut_type = failure<int>;

        // Act
        constexpr auto property_holds =
          std::is_nothrow_default_constructible<sut_type>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }
    }

    SECTION("T is not nothrow default constructible") {
      SECTION("Unexpected propagates noexcept status") {
        // Arrange
        using sut_type = failure<not_nothrow_default_constructible>;

        // Act
        constexpr auto property_holds =
          std::is_nothrow_default_constructible<sut_type>::value;

        // Assert
        STATIC_REQUIRE_FALSE(property_holds);
      }
    }

    SECTION("T is trivial default constructible") {
      SECTION("Unexpected propagates trivial constructor") {
        // Arrange
        using sut_type = failure<int>;

        // Act
        constexpr auto property_holds =
          std::is_trivially_default_constructible<sut_type>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }
    }

    SECTION("T is not trivial default constructible") {
      SECTION("Unexpected propagates trivial constructor") {
        // Arrange
        using sut_type = failure<not_trivial_default_constructible>;

        SECTION("Retains nontrivial default construction") {
          // Act
          constexpr auto property_holds =
            std::is_trivially_default_constructible<sut_type>::value;

          // Assert
          STATIC_REQUIRE_FALSE(property_holds);
        }
        SECTION("Keeps default construction available") {
          // Act
          constexpr auto property_holds =
            std::is_default_constructible<sut_type>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }
      }
    }

    SECTION("Default constructs underlying type") {
      // Act
      auto sut = failure<default_construct_test>{};

      // Assert
      REQUIRE(sut.error().value == default_construct_test::default_value);
    }
  }

  SECTION("T is not default constructible") {
    SECTION("Unexpected is also not default constructible") {
      // Arrange
      using sut_type = failure<not_default_constructible>;

      // Act
      constexpr auto property_holds =
        std::is_default_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("failure<E>::failure(const element_type&)", "[ctor]") {
  SECTION("T is copy-constructible") {
    SECTION("Unexpected is also copy-constructible") {
      // Arrange
      using underlying = int;
      using sut_type = failure<underlying>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,const underlying&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
  SECTION("T is not copy-constructible") {
    SECTION("Unexpected is also not copy-constructible") {
      // Arrange
      using underlying = not_copy_or_moveable;
      using sut_type = failure<underlying>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,const underlying&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("Copies the contents of the other failure") {
    // Act
    const auto sut = failure<int>{42};

    const auto copy = sut;

    // Assert
    REQUIRE(copy == sut);
  }
}

TEST_CASE("failure<E>::failure(element_type&&)", "[ctor]") {
  SECTION("T is move-constructible") {
    SECTION("Unexpected is also move-constructible") {
      // Arrange
      using underlying = std::string;
      using sut_type = failure<underlying>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,underlying&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
  SECTION("T is not move-constructible") {
    SECTION("Unexpected is also not move-constructible") {
      // Arrange
      using underlying = not_copy_or_moveable;
      using sut_type = failure<underlying>;
      // Note: If a type is copy-constructible but is not move-constructible,
      //       the copy-constructor will be enabled -- but we have no way of
      //       properly detecting this statically

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,underlying&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("failure<E>::failure(in_place_t, Args&&...)", "[ctor]") {
  SECTION("Constructs E with args") {
    // Arrange
    const auto string = "Hello World";
    const auto expected = std::string{string, 5};

    // Act
    const auto sut = failure<std::string>{std::in_place, string, 5};

    // Assert
    REQUIRE(sut.error() == expected);
  }
}

TEST_CASE("failure<E>::failure(in_place_t, std::initializer_list<U>, Args&&...)", "[ctor]") {
  SECTION("Constructs E with args") {
    // Arrange
    const auto expected = std::string{
      {'H','e','l','l','o'},
      std::allocator<char>{}
    };

    // Act
    const auto sut = failure<std::string>{
      std::in_place,
      {'H','e','l','l','o'},
      std::allocator<char>{}
    };

    // Assert
    REQUIRE(sut.error() == expected);
  }
}

TEST_CASE("failure<E>::failure(U&&)", "[ctor]") {
  SECTION("U is convertible to E") {
    SECTION("U is convertible to failure<E>") {
      // Arrange
      using source_type = const char*;
      using sut_type = failure<std::string>;

      // Act
      constexpr auto property_holds =
        std::is_convertible<const source_type&,sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("failure<E> is constructible from U") {
      // Arrange
      using source_type = const char*;
      using sut_type = failure<std::string>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,const source_type&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Constructs the underlying type") {
      // Arrange
      const auto underlying = "Hello World";
      const std::string expected = underlying;

      // Act
      const failure<std::string> sut = underlying;

      // Assert
      REQUIRE(sut.error() == expected);
    }
  }
  SECTION("U is not convertible to E") {
    SECTION("U is not convertible to failure<E>") {
      // Arrange
      using source_type = int;
      using sut_type = failure<std::string>;

      // Act
      constexpr auto property_holds =
        std::is_convertible<const source_type&,sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("failure<E>::failure(U&&) (explicit)", "[ctor]") {
  SECTION("E is constructible from U") {
    SECTION("failure<E> is constructible from U") {
      // Arrange
      using source_type = const char*;
      using sut_type = failure<explicit_type<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,const source_type&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("U is not convertible to failure<E>") {
      // Arrange
      using source_type = const char*;
      using sut_type = failure<explicit_type<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_convertible<const source_type&,sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
    SECTION("Constructs the underlying type") {
      // Arrange
      const auto underlying = "Hello World";
      const explicit_type<std::string> expected{underlying};

      // Act
      const auto sut = failure<explicit_type<std::string>>{underlying};

      // Assert
      REQUIRE(sut.error() == expected);
    }
  }
  SECTION("E is not constructible from U") {
    SECTION("failure<E> is not constructible from U") {
      // Arrange
      using source_type = int;
      using sut_type = failure<explicit_type<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,const source_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("failure<E>::failure(const failure<E2>&) ", "[ctor]") {
  SECTION("E2 is copy-convertible to E") {
    SECTION("failure<E> is copy-convertible from failure<E2>") {
      // Arrange
      using source_type = failure<const char*>;
      using sut_type = failure<std::string>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,const source_type&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Copy-converts the underlying value") {
      // Arrange
      const auto source = fail("Hello World");

      // Act
      const auto sut = failure<std::string>{source};

      // Assert
      REQUIRE(sut == source);
    }
  }
  SECTION("E2 is not copy-convertible to E") {
    SECTION("failure<E> is not copy-convertible from failure<E2>") {
      // Arrange
      using source_type = failure<const char*>;
      using sut_type = failure<int>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,const source_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("failure<E>::failure(failure<E2>&&)", "[ctor]") {
  SECTION("E2 is move-convertible to E") {
    SECTION("failure<E> is move-convertible from failure<E2>") {
      // Arrange
      using source_type = failure<std::string>;
      using sut_type = failure<move_only<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,source_type&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Move-converts the underlying value") {
      // Arrange
      auto source = fail(std::string{"Hello World"});
      const auto expected = source;

      // Act
      const auto sut = failure<move_only<std::string>>{std::move(source)};

      // Assert
      REQUIRE(sut == expected);
    }
  }
  SECTION("E2 is not move-convertible to E") {
    SECTION("failure<E> is not move-convertible from failure<E2>") {
      // Arrange
      using source_type = failure<int>;
      using sut_type = failure<move_only<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,source_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("failure<E>::operator=(E2&&) ", "[assign]") {
  SECTION("E2 is assignable to E") {
    SECTION("E2 is assignable to failure<E>") {
      // Arrange
      using source_type = const char*;
      using sut_type = failure<std::string>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,source_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Assigns the underlying value") {
      // Arrange
      const auto expected = "Goodbye world";
      auto sut = fail<std::string>("Hello World");

      // Act
      sut = expected;

      // Assert
      REQUIRE(sut.error() == expected);
    }
  }
  SECTION("E2 is not assignable to E") {
    SECTION("E2 is not assignable to failure<E>") {
      // Arrange
      using source_type = const char*;
      using sut_type = failure<int>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,source_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("failure<E>::operator=(const failure<E2>&) ", "[assign]") {
  SECTION("const E2 is assignable to E") {
    SECTION("const failure<E2> is assignable to failure<E>") {
      // Arrange
      using source_type = failure<const char*>;
      using sut_type = failure<std::string>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const source_type&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Copy-converts the underlying value") {
      // Arrange
      const auto expected = fail("Goodbye World");
      auto sut = fail<std::string>("Hello world");

      // Act
      sut = expected;

      // Assert
      REQUIRE(sut == expected);
    }
  }
  SECTION("const E2 is not assignable to E") {
    SECTION("const failure<E2> is not assignable to failure<E>") {
      // Arrange
      using source_type = failure<const char*>;
      using sut_type = failure<int>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const source_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("failure<E>::operator=(failure<E2>&&)", "[assign]") {
  SECTION("E2&& is assignable to E") {
    SECTION("failure<E2>&& is assignable to failure<E>") {
      // Arrange
      using source_type = failure<std::string>;
      using sut_type = failure<move_only<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,source_type&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Assigns the underlying value") {
      // Arrange
      auto source = fail(std::string{"Goodbe World"});
      const auto expected = source;

      auto sut = failure<move_only<std::string>>{"Hello World"};

      // Act
      sut = std::move(source);

      // Assert
      REQUIRE(sut == expected);
    }
  }
  SECTION("E2&& is not assignable to E") {
    SECTION("failure<E2>&& is not assignable to failure<E>") {
      // Arrange
      using source_type = failure<int>;
      using sut_type = failure<move_only<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,source_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("failure<E>::error() &", "[accessor]") {
  // Arrange
  const auto expected = 42;
  auto sut = failure<int>(expected);

  SECTION("Returns a reference to the internal value") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(sut.error()),int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Is equal to the input value") {
    // Act
    const auto result = sut.error();

    // Assert
    REQUIRE(result == expected);
  }
}

TEST_CASE("failure<E>::error() &&", "[accessor]") {
  // Arrange
  const auto expected = 42;
  auto sut = failure<int>(expected);

  SECTION("Returns a reference to the internal value") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(std::move(sut).error()),int&&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Is equal to the input value") {
    // clang-tidy incorrectly flags this as "use-after-move" because of the
    // above test. These cases are independently executed, and thus this
    // diagnostic is incorrect.
    //
    // Act
    // NOLINTNEXTLINE(bugprone-use-after-move)
    const auto result = std::move(sut).error();

    // Assert
    REQUIRE(result == expected);
  }
}

TEST_CASE("failure<E>::error() const &", "[accessor]") {
  // Arrange
  const auto expected = 42;
  const auto sut = failure<int>(expected);

  SECTION("Returns a reference to the internal value") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(sut.error()),const int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Is equal to the input value") {
    // Act
    const auto result = sut.error();

    // Assert
    REQUIRE(result == expected);
  }
}


TEST_CASE("failure<E>::error() const &&", "[accessor]") {
  // Arrange
  const auto expected = 42;
  const auto sut = failure<int>(expected);

  SECTION("Returns a reference to the internal value") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(std::move(sut).error()),const int&&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Is equal to the input value") {
    // clang-tidy incorrectly flags this as "use-after-move" because of the
    // above test. These cases are independently executed, and thus this
    // diagnostic is incorrect.
    //
    // Act
    // NOLINTNEXTLINE(bugprone-use-after-move)
    const auto result = std::move(sut).error();

    // Assert
    REQUIRE(result == expected);
  }
}

//=============================================================================
// class : failure<E&>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("failure<E&>::failure()", "[ctor]") {
  SECTION("Is not defined for references") {
    // Act
    constexpr auto property_holds =
      std::is_default_constructible<failure<int&>>::value;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

TEST_CASE("failure<E&>::failure(const element_type&)", "[ctor]") {
  SECTION("Is not defined for references") {
    // Act
    constexpr auto property_holds =
      std::is_constructible<failure<int&>,const int&>::value;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

TEST_CASE("failure<E&>::failure(element_type&&)", "[ctor]") {
  SECTION("Is not defined for references") {
    // Act
    constexpr auto property_holds =
      std::is_constructible<failure<int&>,int&&>::value;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

TEST_CASE("failure<E&>::failure(element_type&)", "[ctor]") {
  SECTION("Is defined for references") {
    // Act
    constexpr auto property_holds =
      std::is_constructible<failure<int&>,int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Binds a reference to the failure value") {
    // Arrange
    auto value = 42;

    // Act
    auto sut = failure<int&>(value);

    // Assert
    SECTION("References underlying reference") {
      REQUIRE(&value == &sut.error());
    }
  }
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("failure<E&>::error() &", "[accessor]") {
  // Arrange
  auto expected = 42;
  auto sut = failure<int&>(expected);

  SECTION("Returns a reference to the internal value") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(sut.error()),int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Is equal to the input value") {
    // Act
    const auto result = &sut.error();

    // Assert
    REQUIRE(result == &expected);
  }
}

TEST_CASE("failure<E&>::error() &&", "[accessor]") {
  // Arrange
  auto expected = 42;
  auto sut = failure<int&>(expected);

  SECTION("Returns an lvalue reference to the internal value") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(std::move(sut).error()),int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Is equal to the input value") {
    // Act
    int& x = std::move(sut).error();

    // Assert
    REQUIRE(&x == &expected);
  }
}

TEST_CASE("failure<E&>::error() const &", "[accessor]") {
  // Arrange
  auto expected = 42;
  const auto sut = failure<int&>(expected);

  SECTION("Returns an lvalue reference to the internal value") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(sut.error()),int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Is equal to the input value") {
    // Act
    const auto result = &sut.error();

    // Assert
    REQUIRE(result == &expected);
  }
}

TEST_CASE("failure<E&>::error() const &&", "[accessor]") {
  // Arrange
  auto expected = 42;
  const auto sut = failure<int&>(expected);

  SECTION("Returns an lvalue reference to the internal value") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(std::move(sut).error()),int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Is equal to the input value") {
    // Act
    int& x = std::move(sut).error();

    // Assert
    REQUIRE(&x == &expected);
  }
}

//=============================================================================
// non-member functions : class : failure<E&>
//=============================================================================

//-----------------------------------------------------------------------------
// Comparison
//-----------------------------------------------------------------------------

TEST_CASE("operator==(const failure<E1>&, const failure<E2>&)", "[compare]") {
  // Arrange
  const auto value = 42l;
  const auto lhs = failure<int>(value);
  const auto rhs = fail(std::ref(value));

  SECTION("Uses underlying comparison operator") {
    // Act
    const auto result = lhs == rhs;

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("operator!=(const failure<E1>&, const failure<E2>&)", "[compare]") {
  // Arrange
  const auto value = 42l;
  const auto lhs = failure<int>(0);
  const auto rhs = fail(std::ref(value));

  SECTION("Uses underlying comparison operator") {
    // Act
    const auto result = lhs != rhs;

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("operator>=(const failure<E1>&, const failure<E2>&)", "[compare]") {
  // Arrange
  const auto value = 42l;
  const auto lhs = failure<int>(100);
  const auto rhs = fail(std::ref(value));

  SECTION("Uses underlying comparison operator") {
    // Act
    const auto result = lhs >= rhs;

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("operator<=(const failure<E1>&, const failure<E2>&)", "[compare]") {
  // Arrange
  const auto value = 42l;
  const auto lhs = failure<int>(0);
  const auto rhs = fail(std::ref(value));

  SECTION("Uses underlying comparison operator") {
    // Act
    const auto result = lhs <= rhs;

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("operator<(const failure<E1>&, const failure<E2>&)", "[compare]") {
  // Arrange
  const auto value = 42l;
  const auto lhs = failure<int>(0);
  const auto rhs = fail(std::ref(value));

  SECTION("Uses underlying comparison operator") {
    // Act
    const auto result = lhs < rhs;

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("operator>(const failure<E1>&, const failure<E2>&)", "[compare]") {
  // Arrange
  const auto value = 42l;
  const auto lhs = failure<int>(100);
  const auto rhs = fail(std::ref(value));

  SECTION("Uses underlying comparison operator") {
    // Act
    const auto result = lhs > rhs;

    // Assert
    REQUIRE(result);
  }
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

TEST_CASE("fail(E&& e)", "[utilities]") {
  // Arrange
  using error_type = std::string;
  const auto input = error_type{"Hello world"};

  // Act
  auto sut = fail(input);

  SECTION("Deduces a decayed input") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(sut),failure<error_type>>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }

  // Assert
  SECTION("Contains same value as input") {
    REQUIRE(sut.error() == input);
  }
}

TEST_CASE("fail<E>(Args&&...)", "[utilities]") {
  SECTION("Invokes E's constructor") {
    // Arrange
    const auto input = "hello world";
    const auto size = 5;
    const auto expected = std::string{input, size};

    // Act
    const auto sut = fail<std::string>(input, size);

    // Assert
    REQUIRE(sut.error() == expected);
  }
}

TEST_CASE("fail<E>(std::initializer_list<U>, Args&&...)", "[utilities]") {
  SECTION("Invokes E's constructor") {
    // Arrange
    const auto expected = std::string{
      {'H','e','l','l','o'},
      std::allocator<char>{}
    };

    // Act
    const auto sut = fail<std::string>(
      {'H','e','l','l','o'},
      std::allocator<char>{}
    );

    // Assert
    REQUIRE(sut.error() == expected);
  }
}

TEST_CASE("fail(std::reference_wrapper<E>)", "[utilities]") {
  // Arrange
  using error_type = std::string;

  auto error = error_type{};

  // Act
  auto sut = fail(std::ref(error));

  SECTION("Deduces failure<E&>") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(sut),failure<error_type&>>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }

  // Assert
  SECTION("Refers to input") {
    REQUIRE(&sut.error() == &error);
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("swap(failure<E>&, failure<E>&)", "[utilities]") {
  SECTION("Swaps the underlying contents") {
    // Arrange
    const auto left_input = std::string{"Hello"};
    const auto right_input = std::string{"Goodbye"};

    auto lhs = fail(left_input);
    auto rhs = fail(right_input);

    // Act
    swap(lhs, rhs);

    // Assert
    SECTION("lhs contains old value of rhs") {
      REQUIRE(lhs.error() == right_input);
    }
    SECTION("rhs contains old value of lhs") {
      REQUIRE(rhs.error() == left_input);
    }
  }
}

} // namespace test
} // namespace alloy::core
