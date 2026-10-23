// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>

#include <exception>    // std::exception_ptr, std::current_exception
#include <functional>
#include <ios>          // std::ios_errc
#include <memory>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

#if defined(_MSC_VER)
# pragma warning(push)
# pragma warning(disable: 4244) // Disable warnings about implicit conversions
#endif

namespace alloy::core {
namespace test {
namespace {

struct not_copy_or_moveable{
  not_copy_or_moveable() = default;
  not_copy_or_moveable(const not_copy_or_moveable&) = delete;
  not_copy_or_moveable(not_copy_or_moveable&&) = delete;

  auto operator=(const not_copy_or_moveable&) -> not_copy_or_moveable& = delete;
  auto operator=(not_copy_or_moveable&&) -> not_copy_or_moveable& = delete;
};

struct not_default_constructible{
  not_default_constructible() = delete;
};

template <typename T>
struct move_only : public T
{
  using T::T;

  move_only() = default;

  move_only(T&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
    : T(std::move(other))
  {
  }
  move_only(move_only&&) = default;
  move_only(const move_only&) = delete;

  auto operator=(move_only&&) -> move_only& = default;
  auto operator=(const move_only&) -> move_only& = delete;
};

// This deliberately claims 'noexcept' regardless of T, so that types like
// std::string can be used where 'result' requires nothrow construction or
// assignment.
// NOLINTBEGIN(bugprone-exception-escape)
template <typename T>
struct nonthrowing : public T
{
  nonthrowing() noexcept
    : T(){}

  nonthrowing(const T& other) noexcept
    : T(other){}
  nonthrowing(T&& other) noexcept
    : T(std::move(other)){}
  nonthrowing(const nonthrowing& other) noexcept
    : T(other){}
  nonthrowing(nonthrowing&& other) noexcept
    : T(std::move(other)){}

  template <typename...Args,
            typename = typename std::enable_if<std::is_constructible<T,Args...>::value>::type>
  nonthrowing(Args&&...args)
    : T(std::forward<Args>(args)...){}

  auto operator=(nonthrowing&& other) noexcept -> nonthrowing&
  {
    T::operator=(std::move(other));
    return (*this);
  }

  auto operator=(const nonthrowing& other) noexcept -> nonthrowing&
  {
    T::operator=(other);
    return (*this);
  }
  template <typename U,
            typename = typename std::enable_if<std::is_nothrow_assignable<T,U>::value>::type>
  auto operator=(U&& other) noexcept -> nonthrowing&
  {
    T::operator=(std::forward<U>(other));
    return (*this);
  }
};
// NOLINTEND(bugprone-exception-escape)

template <typename T>
struct throwing : public T
{
  using T::T;

  throwing() noexcept(false)
    : T(){}

  throwing(const T& other) noexcept(false)
    : T(other){}
  throwing(T&& other) noexcept(false)
    : T(std::move(other)){}
  throwing(throwing&& other) noexcept(false)
    : T(std::move(other)) {}
  throwing(const throwing& other) noexcept(false)
    : T(other){}

  using T::operator=;
  auto operator=(throwing&& other) noexcept(false) -> throwing&
  {
    T::operator=(std::move(other));
    return (*this);
  }

  auto operator=(const throwing& other) noexcept(false) -> throwing&
  {
    T::operator=(other);
    return (*this);
  }
};

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

struct report_destructor
{
  report_destructor()
    : output{nullptr}{}
  report_destructor(bool* b)
    : output{b}{}
  report_destructor(const report_destructor&) = default;
  report_destructor(report_destructor&&) = default;

  auto operator=(const report_destructor&) -> report_destructor& = default;
  auto operator=(report_destructor&&) -> report_destructor& = default;

  ~report_destructor()
  {
    *output = true;
  }
  bool* output;
};

struct base
{
  virtual ~base() = default;
  virtual auto get_value() const noexcept -> int { return 42; }
};

struct derived : public base
{
  derived(int v)
    : value{v}{}

  auto get_value() const noexcept -> int override { return value; }
  int value;
};

template <typename T>
auto suppress_unused(const T&) -> void {}

} // namespace <anonymous>

//=============================================================================
// class : result<T, E>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("result<T,E>::result()", "[ctor]") {
  SECTION("T is default-constructible") {
    SECTION("Result is default-constructible") {
      // Arrange
      using sut_type = result<int,int>;

      // Act
      constexpr auto property_holds =
        std::is_default_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    // Note: 'E' must be default-constructible for use in 'error()', but
    //       otherwise does not contribute to this.
    SECTION("Default constructs the underlying T") {
      // Arrange
      using value_type = std::string;
      using sut_type = result<value_type,int>;

      // Act
      auto sut = sut_type{};

      // Assert
      REQUIRE(*sut == value_type());
    }
    SECTION("Invokes T's default-construction") {
      // TODO: Use a static mock
    }
  }

  SECTION("T is not default-constructible") {
    SECTION("Result is not default-constructible") {
      // Arrange
      using sut_type = result<not_default_constructible,int>;

      // Act
      constexpr auto property_holds =
        std::is_default_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("result<T,E>::result(const result&)", "[ctor]") {

  // Constructible:

  SECTION("T and E are both trivially copy-constructible") {
    // Arrange
    using sut_type = result<int,int>;

    SECTION("Result is trivially copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Copy source contained a value") {
      const auto value = 42;
      const sut_type source{value};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains a value") {
        REQUIRE(sut.has_value());
      }
      SECTION("Copy contains a value equal to the source") {
        REQUIRE(*sut == value);
      }
    }

    SECTION("Copy source contained an error") {
      const auto error = fail(42);
      const sut_type source{error};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Copy contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  } // T and E are both trivially copy-constructible

  SECTION("T is copy-constructible, but not trivial") {
    // Arrange
    using sut_type = result<std::string,int>;

    SECTION("Result is not trivially copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }

    SECTION("Result is copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Copy source contained a value") {
      const auto value = "hello world";
      const sut_type source{value};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains a value") {
        REQUIRE(sut.has_value());
      }
      SECTION("Copy contains a value equal to the source") {
        REQUIRE(*sut == value);
      }
    }

    SECTION("Copy source contained an error") {
      const auto error = fail(42);
      const sut_type source{error};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Copy contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  } // T is copy-constructible, but not trivial

  SECTION("E is copy-constructible, but not trivial") {
    // Arrange
    using sut_type = result<int,std::string>;

    SECTION("Result is not trivially copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }

    SECTION("Result is copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Copy source contained a value") {
      const auto value = 42;
      const sut_type source{value};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains a value") {
        REQUIRE(sut.has_value());
      }
      SECTION("Copy contains a value equal to the source") {
        REQUIRE(*sut == value);
      }
    }

    SECTION("Copy source contained an error") {
      const auto error = fail("Hello world");
      const sut_type source{error};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Copy contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  }

  SECTION("T and E are both copy-constructible, but not trivial") {
    // Arrange
    using sut_type = result<std::string,std::string>;

    SECTION("Result is not trivially copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }

    SECTION("Result is copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Copy source contained a value") {
      const auto value = "Hello world";
      const sut_type source{value};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains a value") {
        REQUIRE(sut.has_value());
      }
      SECTION("Copy contains a value equal to the source") {
        REQUIRE(*sut == value);
      }
    }

    SECTION("Copy source contained an error") {
      const auto error = fail("Goodbye world");
      const sut_type source{error};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Copy contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  }

  // Not constructible:

  SECTION("T is not copy-constructible") {
    // Arrange
    using sut_type = result<not_copy_or_moveable,int>;

    SECTION("Result is not copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is not copy-constructible") {
    // Arrange
    using sut_type = result<int,not_copy_or_moveable>;

    SECTION("Result is not copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T and E are not copy-constructible") {
    // Arrange
    using sut_type = result<not_copy_or_moveable,not_copy_or_moveable>;

    SECTION("Result is not copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("result<T,E>::result(result&&)", "[ctor]") {

  // Constructible:

  SECTION("T and E are both trivially move-constructible") {
    // Arrange
    using sut_type = result<int,int>;

    SECTION("Result is trivially move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Move source contained a value") {
      const auto value = 42;
      sut_type source{value};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains a value") {
        REQUIRE(sut.has_value());
      }
      SECTION("New result contains a value equal to the source") {
        REQUIRE(*sut == value);
      }
    }

    SECTION("Move source contained an error") {
      const auto error = fail(42);
      sut_type source{error};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("New result contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  } // T and E are both trivially move-constructible

  SECTION("T is move-constructible, but not trivial") {
    // Arrange
    using sut_type = result<move_only<std::string>,int>;

    SECTION("Result is not trivially move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }

    SECTION("Result is move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Move source contained a value") {
      const auto value = "hello world";
      sut_type source{value};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains a value") {
        REQUIRE(sut.has_value());
      }
      SECTION("New result contains a value equal to the source") {
        REQUIRE(*sut == value);
      }
    }

    SECTION("Move source contained an error") {
      const auto error = fail(42);
      sut_type source{error};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("New result contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  } // T is move-constructible, but not trivial

  SECTION("E is move-constructible, but not trivial") {
    // Arrange
    using sut_type = result<int,move_only<std::string>>;

    SECTION("Result is not trivially move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }

    SECTION("Result is move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Move source contained a value") {
      const auto value = 42;
      sut_type source{value};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains a value") {
        REQUIRE(sut.has_value());
      }
      SECTION("New result contains a value equal to the source") {
        REQUIRE(*sut == value);
      }
    }

    SECTION("Move source contained an error") {
      const auto error = fail("Hello world");
      sut_type source{error};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("New result contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  }

  SECTION("T and E are both move-constructible, but not trivial") {
    // Arrange
    using sut_type = result<move_only<std::string>,move_only<std::string>>;

    SECTION("Result is not trivially move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }

    SECTION("Result is move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Move source contained a value") {
      const auto value = "Hello world";
      sut_type source{value};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains a value") {
        REQUIRE(sut.has_value());
      }
      SECTION("New result contains a value equal to the source") {
        REQUIRE(*sut == value);
      }
    }

    SECTION("Move source contained an error") {
      const auto error = fail("Goodbye world");
      sut_type source{error};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("New result contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  }

  // Not constructible:

  SECTION("T is not move-constructible") {
    // Arrange
    using sut_type = result<not_copy_or_moveable,int>;

    SECTION("Result is not move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is not move-constructible") {
    // Arrange
    using sut_type = result<int,not_copy_or_moveable>;

    SECTION("Result is not move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T and E are not move-constructible") {
    // Arrange
    using sut_type = result<not_copy_or_moveable,not_copy_or_moveable>;

    SECTION("Result is not move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("result<T,E>::result(const result<T2,E2>&)", "[ctor]") {
  // Constructible:

  SECTION("T and E are both constructible from T2 and E2") {
    // Arrange
    using copy_type = result<const char*, const char*>;
    using sut_type = result<std::string,std::string>;

    SECTION("Result is constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Result is implicit constructible") {
      // Act
      constexpr auto property_holds =
        std::is_convertible<const copy_type&,sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Copy source contained a value") {
      const auto value = "Hello world";
      const copy_type source{value};

      // Act
      const sut_type sut = source;

      // Assert
      SECTION("Copy contains a value") {
        REQUIRE(sut.has_value());
      }
      SECTION("Copy contains a value equal to the source") {
        REQUIRE(*sut == value);
      }
    }

    SECTION("Copy source contained an error") {
      const auto error = fail("Goodbye world");
      const copy_type source{error};

      // Act
      const sut_type sut = source;

      // Assert
      SECTION("Copy contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Copy contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  }

  // Not Constructible:

  SECTION("T is not constructible from T2") {
    // Arrange
    using copy_type = result<std::string,int>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is not constructible from E2") {
    // Arrange
    using copy_type = result<int,std::string>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T and E are not constructible from T2 and E2") {
    // Arrange
    using copy_type = result<std::string,std::string>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("result<T,E>::result(const result<T2,E2>&) (explicit)", "[ctor]") {

  // Constructible:

  SECTION("T and E are both constructible from T2 and E2") {
    SECTION("T is explicit constructible from T2") {
      // Arrange
      using copy_type = result<std::string, const char*>;
      using sut_type = result<explicit_type<std::string>,std::string>;

      SECTION("Result is constructible") {
        // Act
        constexpr auto property_holds =
          std::is_constructible<sut_type,const copy_type&>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }

      SECTION("Result is not implicit constructible") {
        // Act
        constexpr auto property_holds =
          std::is_convertible<const copy_type&,sut_type>::value;

        // Assert
        STATIC_REQUIRE_FALSE(property_holds);
      }

      SECTION("Copy source contained a value") {
        const auto value = "Hello world";
        const copy_type source{value};

        // Act
        const sut_type sut{source};

        // Assert
        SECTION("Copy contains a value") {
          REQUIRE(sut.has_value());
        }
        SECTION("Copy contains a value equal to the source") {
          REQUIRE(*sut == value);
        }
      }

      SECTION("Copy source contained an error") {
        const auto error = fail("Goodbye world");
        const copy_type source{error};

        // Act
        const sut_type sut{source};

        // Assert
        SECTION("Copy contains an error") {
          REQUIRE(sut.has_error());
        }
        SECTION("Copy contains an error equal to the source") {
          REQUIRE(sut == error);
        }
      }
    }

    SECTION("E is explicit constructible from E2") {
      // Arrange
      using copy_type = result<const char*,std::string>;
      using sut_type = result<std::string,explicit_type<std::string>>;

      SECTION("Result is constructible") {
        // Act
        constexpr auto property_holds =
          std::is_constructible<sut_type,const copy_type&>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }

      SECTION("Result is not implicit constructible") {
        // Act
        constexpr auto property_holds =
          std::is_convertible<const copy_type&,sut_type>::value;

        // Assert
        STATIC_REQUIRE_FALSE(property_holds);
      }

      SECTION("Copy source contained a value") {
        const auto value = "Hello world";
        const copy_type source{value};

        // Act
        const sut_type sut{source};

        // Assert
        SECTION("Copy contains a value") {
          REQUIRE(sut.has_value());
        }
        SECTION("Copy contains a value equal to the source") {
          REQUIRE(*sut == value);
        }
      }

      SECTION("Copy source contained an error") {
        const auto error = fail("Goodbye world");
        const copy_type source{error};

        // Act
        const sut_type sut{source};

        // Assert
        SECTION("Copy contains an error") {
          REQUIRE(sut.has_error());
        }
        SECTION("Copy contains an error equal to the source") {
          REQUIRE(sut == error);
        }
      }
    }
    SECTION("T and E are explicit constructible from T2 and E2") {
      // Arrange
      using copy_type = result<std::string, std::string>;
      using sut_type = result<explicit_type<std::string>,explicit_type<std::string>>;

      SECTION("Result is constructible") {
        // Act
        constexpr auto property_holds =
          std::is_constructible<sut_type,const copy_type&>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }

      SECTION("Result is not implicit constructible") {
        // Act
        constexpr auto property_holds =
          std::is_convertible<const copy_type&,sut_type>::value;

        // Assert
        STATIC_REQUIRE_FALSE(property_holds);
      }

      SECTION("Copy source contained a value") {
        const auto value = "Hello world";
        const copy_type source{value};

        // Act
        const sut_type sut{source};

        // Assert
        SECTION("Copy contains a value") {
          REQUIRE(sut.has_value());
        }
        SECTION("Copy contains a value equal to the source") {
          REQUIRE(*sut == value);
        }
      }

      SECTION("Copy source contained an error") {
        const auto error = fail("Goodbye world");
        const copy_type source{error};

        // Act
        const sut_type sut{source};

        // Assert
        SECTION("Copy contains an error") {
          REQUIRE(sut.has_error());
        }
        SECTION("Copy contains an error equal to the source") {
          REQUIRE(sut == error);
        }
      }
    }
  }

  // Not Constructible:

  SECTION("T is not constructible from T2") {
    // Arrange
    using copy_type = result<std::string,int>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is not constructible from E2") {
    // Arrange
    using copy_type = result<int,std::string>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T and E are not constructible from T2 and E2") {
    // Arrange
    using copy_type = result<std::string,std::string>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("result<T,E>::result(result<T2,E2>&&)", "[ctor]") {

  // Constructible Case:

  SECTION("T and E are both constructible from T2 and E2") {
    // Arrange
    using copy_type = result<std::string,std::string>;
    using sut_type = result<move_only<std::string>,move_only<std::string>>;

    SECTION("Result is constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Result is implicit constructible") {
      // Act
      constexpr auto property_holds =
        std::is_convertible<copy_type&&,sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Copy source contained a value") {
      const auto value = "Hello world";
      copy_type source{value};

      // Act
      const sut_type sut = std::move(source);

      // Assert
      SECTION("Copy contains a value") {
        REQUIRE(sut.has_value());
      }
      SECTION("Copy contains a value equal to the source") {
        REQUIRE(*sut == value);
      }
    }

    SECTION("Copy source contained an error") {
      const auto error = fail("Goodbye world");
      copy_type source{error};

      // Act
      const sut_type sut = std::move(source);

      // Assert
      SECTION("Copy contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Copy contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  }

  // Not Constructible:

  SECTION("T is not constructible from T2") {
    // Arrange
    using copy_type = result<std::string,int>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is not constructible from E2") {
    // Arrange
    using copy_type = result<int,std::string>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T and E are not constructible from T2 and E2") {
    // Arrange
    using copy_type = result<std::string,std::string>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("result<T,E>::result(result<T2,E2>&&) (explicit)", "[ctor]") {

  // Constructors:

  SECTION("T and E are both constructible from T2 and E2") {
    SECTION("T is explicit constructible from T2") {
      // Arrange
      using copy_type = result<std::string, const char*>;
      using sut_type = result<explicit_type<move_only<std::string>>,std::string>;

      SECTION("Result is constructible") {
        // Act
        constexpr auto property_holds =
          std::is_constructible<sut_type,copy_type&&>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }

      SECTION("Result is not implicit constructible") {
        // Act
        constexpr auto property_holds =
          std::is_convertible<copy_type&&,sut_type>::value;

        // Assert
        STATIC_REQUIRE_FALSE(property_holds);
      }

      SECTION("Copy source contained a value") {
        const auto value = "Hello world";
        copy_type source{value};

        // Act
        const sut_type sut{std::move(source)};

        // Assert
        SECTION("Copy contains a value") {
          REQUIRE(sut.has_value());
        }
        SECTION("Copy contains a value equal to the source") {
          REQUIRE(*sut == value);
        }
      }

      SECTION("Copy source contained an error") {
        const auto error = fail("Goodbye world");
        copy_type source{error};

        // Act
        const sut_type sut{std::move(source)};

        // Assert
        SECTION("Copy contains an error") {
          REQUIRE(sut.has_error());
        }
        SECTION("Copy contains an error equal to the source") {
          REQUIRE(sut == error);
        }
      }
    }

    SECTION("E is explicit constructible from E2") {
      // Arrange
      using copy_type = result<const char*,std::string>;
      using sut_type = result<std::string,explicit_type<move_only<std::string>>>;

      SECTION("Result is constructible") {
        // Act
        constexpr auto property_holds =
          std::is_constructible<sut_type,copy_type&&>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }

      SECTION("Result is not implicit constructible") {
        // Act
        constexpr auto property_holds =
          std::is_convertible<copy_type&&,sut_type>::value;

        // Assert
        STATIC_REQUIRE_FALSE(property_holds);
      }

      SECTION("Copy source contained a value") {
        const auto value = "Hello world";
        copy_type source{value};

        // Act
        const sut_type sut{std::move(source)};

        // Assert
        SECTION("Copy contains a value") {
          REQUIRE(sut.has_value());
        }
        SECTION("Copy contains a value equal to the source") {
          REQUIRE(*sut == value);
        }
      }

      SECTION("Copy source contained an error") {
        const auto error = fail("Goodbye world");
        copy_type source{error};

        // Act
        const sut_type sut{std::move(source)};

        // Assert
        SECTION("Copy contains an error") {
          REQUIRE(sut.has_error());
        }
        SECTION("Copy contains an error equal to the source") {
          REQUIRE(sut == error);
        }
      }
    }
    SECTION("T and E are explicit constructible from T2 and E2") {
      // Arrange
      using copy_type = result<std::string, std::string>;
      using sut_type = result<
        explicit_type<move_only<std::string>>,
        explicit_type<move_only<std::string>>
      >;

      SECTION("Result is constructible") {
        // Act
        constexpr auto property_holds =
          std::is_constructible<sut_type,copy_type&&>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }

      SECTION("Result is not implicit constructible") {
        // Act
        constexpr auto property_holds =
          std::is_convertible<copy_type&&,sut_type>::value;

        // Assert
        STATIC_REQUIRE_FALSE(property_holds);
      }

      SECTION("Copy source contained a value") {
        const auto value = "Hello world";
        copy_type source{value};

        // Act
        const sut_type sut{std::move(source)};

        // Assert
        SECTION("Copy contains a value") {
          REQUIRE(sut.has_value());
        }
        SECTION("Copy contains a value equal to the source") {
          REQUIRE(*sut == value);
        }
      }

      SECTION("Copy source contained an error") {
        const auto error = fail("Goodbye world");
        copy_type source{error};

        // Act
        const sut_type sut{std::move(source)};

        // Assert
        SECTION("Copy contains an error") {
          REQUIRE(sut.has_error());
        }
        SECTION("Copy contains an error equal to the source") {
          REQUIRE(sut == error);
        }
      }
    }
  }

  // Not Constructible:

  SECTION("T is not constructible from T2") {
    // Arrange
    using copy_type = result<std::string,int>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is not constructible from E2") {
    // Arrange
    using copy_type = result<int,std::string>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T and E are not constructible from T2 and E2") {
    // Arrange
    using copy_type = result<std::string,std::string>;
    using sut_type = result<int,int>;

    SECTION("Result is not constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("result<T,E>::result(in_place_t, Args&&...)", "[ctor]") {
  // Arrange
  using sut_type = result<std::string,int>;

  const auto input = "hello world";
  const auto size = 5;
  const auto value = std::string{input, size};

  // Act
  const sut_type sut(std::in_place, input, size);

  // Assert
  SECTION("Contains a value") {
    REQUIRE(sut.has_value());
  }

  SECTION("Contains an error constructed from a copy of E2") {
    REQUIRE(sut == value);
  }
}

TEST_CASE("result<T,E>::result(in_place_t, std::initializer_list<U>, Args&&...)", "[ctor]") {
  // Arrange
  using sut_type = result<std::string,int>;

  const auto value = std::string{
    {'h','e','l','l','o'}, std::allocator<char>{}
  };

  // Act
  const sut_type sut(std::in_place, {'h','e','l','l','o'}, std::allocator<char>{});

  // Assert
  SECTION("Contains a value") {
    REQUIRE(sut.has_value());
  }

  SECTION("Contains an error constructed from a copy of E2") {
    REQUIRE(sut == value);
  }
}

TEST_CASE("result<T,E>::result(in_place_error_t, Args&&...)", "[ctor]") {
  // Arrange
  using sut_type = result<int,std::string>;

  const auto input = "hello world";
  const auto size = 5;
  const auto result = std::string{input, size};

  // Act
  const sut_type sut(in_place_error, input, size);

  // Assert
  SECTION("Contains an error") {
    REQUIRE(sut.has_error());
  }

  SECTION("Contains an error constructed from a copy of E2") {
    REQUIRE(sut == fail(std::ref(result)));
  }
}

TEST_CASE("result<T,E>::result(in_place_error_t, std::initializer_list<U>, Args&&...)", "[ctor]") {
  // Arrange
  using sut_type = result<int,std::string>;

  const auto value = std::string{
    {'h','e','l','l','o'}, std::allocator<char>{}
  };

  // Act
  const sut_type sut(in_place_error, {'h','e','l','l','o'}, std::allocator<char>{});

  // Assert
  SECTION("Contains an error") {
    REQUIRE(sut.has_error());
  }

  SECTION("Contains an error constructed from a copy of E2") {
    REQUIRE(sut == fail(std::ref(value)));
  }
}

TEST_CASE("result<T,E>::result(const failure<E2>&)", "[ctor]") {
  // Arrange
  using sut_type = result<int,std::string>;
  const auto source = fail<std::string>("hello world");

  // Act
  const sut_type sut{source};

  // Assert
  SECTION("Contains an error") {
    REQUIRE(sut.has_error());
  }

  SECTION("Contains an error constructed from a copy of E2") {
    REQUIRE(sut == source);
  }
}

TEST_CASE("result<T,E>::result(failure<E2>&&)", "[ctor]") {
  // Arrange
  using sut_type = result<int,move_only<std::string>>;
  auto source = fail<std::string>("hello world");
  const auto copy = source;

  // Act
  const sut_type sut{std::move(source)};

  // Assert
  SECTION("Contains an error") {
    REQUIRE(sut.has_error());
  }

  SECTION("Contains an error constructed from E2") {
    REQUIRE(sut == copy);
  }
}

TEST_CASE("result<T,E>::result(U&&)", "[ctor]") {
  // Arrange
  using sut_type = result<std::string,int>;
  auto source = "Hello world";
  const auto copy = std::string{source};

  // Act
  const sut_type sut = source;

  // Assert
  SECTION("Contains a value") {
    REQUIRE(sut.has_value());
  }

  SECTION("Contains a value constructed from U") {
    REQUIRE(sut == copy);
  }

  SECTION("Is disabled for explicit constructors") {
    // Act
    constexpr auto property_holds =
      std::is_convertible<const std::string&,explicit_type<std::string>>::value;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

TEST_CASE("result<T,E>::result(U&&) (explicit)", "[ctor]") {
  // Arrange
  using sut_type = result<explicit_type<std::string>,int>;
  auto source = "Hello world";
  const auto copy = std::string{source};

  // Act
  const sut_type sut{source};

  // Assert
  SECTION("Contains a value") {
    REQUIRE(sut.has_value());
  }

  SECTION("Contains a value constructed from U") {
    REQUIRE(sut == copy);
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<T,E>::~result()", "[dtor]") {
  SECTION("T and E are trivially destructible") {
    // Arrange
    using sut_type = result<int, int>;

    SECTION("result's destructor is trivial") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_destructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
  SECTION("T is not trivially destructible") {
    // Arrange
    using sut_type = result<report_destructor, int>;

    auto invoked = false;

    // Act
    {
      sut_type sut{std::in_place, &invoked};
      (void) sut;
    }

    SECTION("result's destructor is not trivial") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_destructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }

    // Assert
    SECTION("Invokes T's underlying destructor") {
      REQUIRE(invoked);
    }
  }
  SECTION("E is not trivially destructible") {
    // Arrange
    using sut_type = result<int, report_destructor>;

    auto invoked = false;

    // Act
    {
      sut_type sut{in_place_error, &invoked};
      (void) sut;
    }

    SECTION("result's destructor is not trivial") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_destructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }

    // Assert
    SECTION("Invokes E's underlying destructor") {
      REQUIRE(invoked);
    }
  }
  SECTION("T and E are not trivially destructible") {
    // Arrange
    using sut_type = result<report_destructor, report_destructor>;

    SECTION("T is active") {
      SECTION("Invokes T's underlying destructor") {
        auto invoked = false;

        // Act
        {
          sut_type sut{std::in_place, &invoked};
          (void) sut;
        }

        // Assert
        REQUIRE(invoked);
      }
    }
    SECTION("E is active") {
      SECTION("Invokes E's underlying destructor") {
        auto invoked = false;

        // Act
        {
          sut_type sut{in_place_error, &invoked};
          (void) sut;
        }

        // Assert
        REQUIRE(invoked);
      }
    }

    SECTION("result's destructor is not trivial") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_destructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<T,E>::operator=(const result&)", "[assign]") {
  SECTION("T is not nothrow copy constructible") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using sut_type = result<std::string,int>;

      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not nothrow copy constructible") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using sut_type = result<int,std::string>;

      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("T and E are not nothrow copy constructible") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using sut_type = result<std::string,std::string>;

      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T is not copy-assignable") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using sut_type = result<not_copy_or_moveable,int>;

      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not copy-assignable") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using sut_type = result<int,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("T and E are not copy-assignable") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using sut_type = result<not_copy_or_moveable,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T and E are nothrow copy-constructible") {
    SECTION("Result is copy-assignable") {
      // Arrange
      using sut_type = result<int,std::error_code>;

      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("result contains a value") {
      SECTION("'other' contains a value") {
        // Arrange
        using sut_type = result<int,std::error_code>;

        const auto value = 42;
        const sut_type copy{value};
        sut_type sut{};

        // Act
        sut = copy;
        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_copy_assignable<sut_type>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("active element is value") {
          REQUIRE(sut.has_value());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
      SECTION("'other' contains an error") {
        // Arrange
        using sut_type = result<report_destructor,const char*>;

        const auto value = fail("42");
        auto is_invoked = false;
        const sut_type copy{value};
        sut_type sut{&is_invoked};

        // Act
        sut = copy;
        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_copy_assignable<sut_type>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("Calls 'T's destructor first") {
          REQUIRE(is_invoked);
        }
        SECTION("active element is error") {
          REQUIRE(sut.has_error());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
    }
    SECTION("result contains an error") {
      SECTION("'other' contains a value") {
        // Arrange
        using sut_type = result<int,report_destructor>;

        const auto value = 42;
        auto is_invoked = false;
        const sut_type copy{value};
        sut_type sut{in_place_error, &is_invoked};

        // Act
        sut = copy;

        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_copy_assignable<sut_type>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("Calls 'T's destructor first") {
          REQUIRE(is_invoked);
        }
        SECTION("active element is value") {
          REQUIRE(sut.has_value());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
      SECTION("'other' contains an error") {
        // Arrange
        using sut_type = result<int,int>;

        const auto value = fail(42);
        const sut_type copy{value};
        sut_type sut{fail(0)};

        // Act
        sut = copy;
        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_copy_assignable<sut_type>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("active element is error") {
          REQUIRE(sut.has_error());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
    }
  }

  SECTION("T and E are trivial copy-constructible and copy-assignable") {
    // Arrange
    using sut_type = result<int, int>;

    SECTION("Result is copy-assignable") {
      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Result is trivial copy-assignable") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
}

TEST_CASE("result<T,E>::operator=(result&&)", "[assign]") {
  SECTION("T is not nothrow move constructible") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using sut_type = result<throwing<std::string>,int>;

      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not nothrow move constructible") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using sut_type = result<int,throwing<std::string>>;

      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("T and E are not nothrow move constructible") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using sut_type = result<throwing<std::string>,throwing<std::string>>;

      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T is not move-assignable") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using sut_type = result<not_copy_or_moveable,int>;

      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not move-assignable") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using sut_type = result<int,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("T and E are not move-assignable") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using sut_type = result<not_copy_or_moveable,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T and E are nothrow copy-constructible") {
    SECTION("Result is move-assignable") {
      // Arrange
      using sut_type = result<int,std::error_code>;

      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("result contains a value") {
      SECTION("'other' contains a value") {
        // Arrange
        using sut_type = result<move_only<std::string>,std::error_code>;

        const auto value = "Hello world";
        sut_type original{value};
        sut_type sut{};

        // Act
        sut = std::move(original);
        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_move_assignable<sut_type>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("active element is value") {
          REQUIRE(sut.has_value());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
      SECTION("'other' contains an error") {
        // Arrange
        using sut_type = result<move_only<report_destructor>,const char*>;

        const auto value = fail("42");
        auto is_invoked = false;
        sut_type original{value};
        sut_type sut{&is_invoked};

        // Act
        sut = std::move(original);
        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_move_assignable<sut_type>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("Calls 'T's destructor first") {
          REQUIRE(is_invoked);
        }
        SECTION("active element is error") {
          REQUIRE(sut.has_error());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
    }
    SECTION("result contains an error") {
      SECTION("'other' contains a value") {
        // Arrange
        using sut_type = result<int,move_only<report_destructor>>;

        const auto value = 42;
        auto is_invoked = false;
        sut_type original{value};
        sut_type sut{in_place_error, &is_invoked};

        // Act
        sut = std::move(original);

        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_move_assignable<sut_type>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("Calls 'T's destructor first") {
          REQUIRE(is_invoked);
        }
        SECTION("active element is value") {
          REQUIRE(sut.has_value());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
      SECTION("'other' contains an error") {
        // Arrange
        using sut_type = result<int,move_only<std::string>>;

        const auto value = fail("hello world");
        sut_type copy{value};
        sut_type sut{fail("goodbye world")};

        // Act
        sut = std::move(copy);
        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_move_assignable<sut_type>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("active element is error") {
          REQUIRE(sut.has_error());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
    }
  }

  SECTION("T and E are trivial copy-constructible and copy-assignable") {
    // Arrange
    using sut_type = result<int, int>;

    SECTION("Result is move-assignable") {
      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Result is trivial move-assignable") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
}

TEST_CASE("result<T,E>::operator=(const result<T2,E2>&)", "[assign]") {
SECTION("T is not nothrow constructible from T2") {
    SECTION("Result is not assignable") {
      // Arrange
      using copy_type = result<std::string, long>;
      using sut_type = result<throwing<std::string>,int>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not nothrow constructible from E2") {
    SECTION("Result is not assignable") {
      // Arrange
      using copy_type = result<int,std::string>;
      using sut_type = result<int,throwing<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("T and E are not nothrow move constructible") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = result<std::string,std::string>;
      using sut_type = result<throwing<std::string>,throwing<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T is not move-assignable") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = result<not_copy_or_moveable,long>;
      using sut_type = result<not_copy_or_moveable,int>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not move-assignable") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = result<long,not_copy_or_moveable>;
      using sut_type = result<int,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("T and E are not move-assignable") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = result<not_copy_or_moveable,not_copy_or_moveable>;
      using sut_type = result<not_copy_or_moveable,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T and E are nothrow copy-constructible") {
    SECTION("Result is move-assignable") {
      // Arrange
      using copy_type = result<long,std::io_errc>;
      using sut_type = result<int,move_only<std::error_code>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("result contains a value") {
      SECTION("'other' contains a value") {
        // Arrange
        using copy_type = result<const char*,std::error_code>;
        using sut_type = result<nonthrowing<std::string>,std::error_code>;

        const auto value = "Hello world";
        copy_type original{value};
        sut_type sut{};

        // Act
        sut = original;

        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_assignable<sut_type,const copy_type&>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("active element is value") {
          REQUIRE(sut.has_value());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
      SECTION("'other' contains an error") {
        // Arrange
        using copy_type = result<report_destructor,const char*>;
        using sut_type = result<report_destructor,const char*>;

        const auto value = fail("42");
        auto is_invoked = false;
        copy_type original{value};
        sut_type sut{&is_invoked};

        // Act
        sut = original;

        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_assignable<sut_type,copy_type&&>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("Calls 'T's destructor first") {
          REQUIRE(is_invoked);
        }
        SECTION("active element is error") {
          REQUIRE(sut.has_error());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
    }
    SECTION("result contains an error") {
      SECTION("'other' contains a value") {
        // Arrange
        using copy_type = result<int,report_destructor>;
        using sut_type = result<int,report_destructor>;

        const auto value = 42;
        auto is_invoked = false;
        copy_type original{value};
        sut_type sut{in_place_error, &is_invoked};

        // Act
        sut = original;

        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_assignable<sut_type,const copy_type&>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("Calls 'T's destructor first") {
          REQUIRE(is_invoked);
        }
        SECTION("active element is value") {
          REQUIRE(sut.has_value());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
      SECTION("'other' contains an error") {
        // Arrange
        using copy_type = result<int,const char*>;
        using sut_type = result<int,nonthrowing<std::string>>;

        const auto value = fail("hello world");
        copy_type copy{value};
        sut_type sut{fail("goodbye world")};

        // Act
        sut = copy;

        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_assignable<sut_type,const copy_type&>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("active element is error") {
          REQUIRE(sut.has_error());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
    }
  }
}

TEST_CASE("result<T,E>::operator=(result<T2,E2>&&)", "[assign]") {
  SECTION("T is not nothrow constructible from T2") {
    SECTION("Result is not assignable") {
      // Arrange
      using copy_type = result<std::string, long>;
      using sut_type = result<throwing<std::string>,int>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not nothrow constructible from E2") {
    SECTION("Result is not assignable") {
      // Arrange
      using copy_type = result<int,std::string>;
      using sut_type = result<int,throwing<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("T and E are not nothrow move constructible") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = result<std::string,std::string>;
      using sut_type = result<throwing<std::string>,throwing<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T is not move-assignable") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = result<not_copy_or_moveable,long>;
      using sut_type = result<not_copy_or_moveable,int>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not move-assignable") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = result<long,not_copy_or_moveable>;
      using sut_type = result<int,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("T and E are not move-assignable") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = result<not_copy_or_moveable,not_copy_or_moveable>;
      using sut_type = result<not_copy_or_moveable,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("T and E are nothrow copy-constructible") {
    SECTION("Result is move-assignable") {
      // Arrange
      using copy_type = result<long,std::io_errc>;
      using sut_type = result<int,move_only<std::error_code>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("result contains a value") {
      SECTION("'other' contains a value") {
        // Arrange
        using copy_type = result<std::string,std::error_code>;
        using sut_type = result<move_only<std::string>,std::error_code>;

        const auto value = "Hello world";
        copy_type original{value};
        sut_type sut{};

        // Act
        sut = std::move(original);
        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_assignable<sut_type,copy_type&&>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("active element is value") {
          REQUIRE(sut.has_value());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
      SECTION("'other' contains an error") {
        // Arrange
        using copy_type = result<report_destructor,const char*>;
        using sut_type = result<move_only<report_destructor>,const char*>;

        const auto value = fail("42");
        auto is_invoked = false;
        copy_type original{value};
        sut_type sut{&is_invoked};

        // Act
        sut = std::move(original);

        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_assignable<sut_type,copy_type&&>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("Calls 'T's destructor first") {
          REQUIRE(is_invoked);
        }
        SECTION("active element is error") {
          REQUIRE(sut.has_error());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
    }
    SECTION("result contains an error") {
      SECTION("'other' contains a value") {
        // Arrange
        using copy_type = result<int,report_destructor>;
        using sut_type = result<int,move_only<report_destructor>>;

        const auto value = 42;
        auto is_invoked = false;
        copy_type original{value};
        sut_type sut{in_place_error, &is_invoked};

        // Act
        sut = std::move(original);

        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_assignable<sut_type,copy_type&&>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("Calls 'T's destructor first") {
          REQUIRE(is_invoked);
        }
        SECTION("active element is value") {
          REQUIRE(sut.has_value());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
      SECTION("'other' contains an error") {
        // Arrange
        using copy_type = result<int,std::string>;
        using sut_type = result<int,move_only<std::string>>;

        const auto value = fail("hello world");
        copy_type copy{value};
        sut_type sut{fail("goodbye world")};

        // Act
        sut = std::move(copy);

        SECTION("Result can be assigned") {
          // Act
          constexpr auto property_holds =
            std::is_assignable<sut_type,copy_type&&>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }

        // Assert
        SECTION("active element is error") {
          REQUIRE(sut.has_error());
        }
        SECTION("error is copied") {
          REQUIRE(sut == value);
        }
      }
    }
  }
}

TEST_CASE("result<T,E>::operator=(U&&)", "[assign]") {
  SECTION("T is not nothrow constructible from U") {
    SECTION("result is not assignable from U") {
      // Act
      constexpr auto property_holds =
        std::is_assignable<result<throwing<std::string>,std::error_code>,const char*>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("T is constructible from U, and nothrow move constructible") {
    SECTION("result is assignable from U") {
      // This works by creating an intermediate `result` object which then
      // is moved through non-throwing move-assignment

      // Act
      constexpr auto property_holds =
        std::is_assignable<result<std::string,std::error_code>,const char*>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
  SECTION("T is not assignable or constructible from U") {
    SECTION("result is not assignable from U") {
      // Act
      constexpr auto property_holds =
        std::is_assignable<result<int,std::error_code>,const char*>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("result contains a value") {
    // Arrange
    using sut_type = result<int,std::error_code>;

    const auto value = 42ll;
    sut_type sut{};

    // Act
    sut = value;

    SECTION("Result can be assigned") {
      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,decltype(value)>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    // Assert
    SECTION("active element is value") {
      REQUIRE(sut.has_value());
    }
    SECTION("error is copied") {
      REQUIRE(sut == value);
    }
  }
  SECTION("result contains an error") {
    // Arrange
    using sut_type = result<int, report_destructor>;

    const auto value = 42ll;
    auto is_invoked = false;
    sut_type sut{in_place_error, &is_invoked};

    // Act
    sut = value;
    SECTION("Result can be assigned") {
      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,decltype(value)>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    // Assert
    SECTION("Calls 'T's destructor first") {
      REQUIRE(is_invoked);
    }
    SECTION("active element is value") {
      REQUIRE(sut.has_value());
    }
    SECTION("error is copied") {
      REQUIRE(sut == value);
    }
  }
}

TEST_CASE("result<T,E>::operator=(const failure<E2>&)", "[assign]") {
  SECTION("E cant be constructed or assigned from E2") {
    SECTION("Result cannot be assigned from E2") {
      // Arrange
      using copy_type = failure<not_copy_or_moveable>;
      using sut_type  = result<int,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not nothrow constructible from E2") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using copy_type = failure<const char*>;
      using sut_type = result<int,throwing<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION(
    "E is not nothrow move constructible from E2, but is nothrow-move "
    "constructible"
  ) {
    SECTION("Result is move-assignable") {
      // Arrange
      using copy_type = failure<const char*>;
      using sut_type = result<int,std::string>;

      // This works because it generates an intermediate 'result<int, E>' in
      // between. The result is then move-constructed instead -- which is
      // non-throwing.

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("E can be constructed and assigned from E2") {
    SECTION("active element is T") {
      // Arrange
      using copy_type = failure<long>;
      using sut_type = result<report_destructor, int>;

      const auto value = copy_type{42};

      auto is_invoked = false;
      sut_type sut{&is_invoked};

      // Act
      sut = value;

      SECTION("Result can be assigned") {
        // Act
        constexpr auto property_holds =
          std::is_assignable<sut_type,const copy_type&>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }

      // Assert
      SECTION("Calls 'T's destructor first") {
        REQUIRE(is_invoked);
      }
      SECTION("active element is error") {
        REQUIRE(sut.has_error());
      }
      SECTION("error is copied") {
        REQUIRE(sut == value);
      }
    }

    SECTION("active element is E") {
      // Arrange
      using copy_type = failure<int>;
      using sut_type = result<int, long>;

      const auto value = copy_type{42};
      sut_type sut{copy_type{0}};

      // Act
      sut = value;

      SECTION("Result can be assigned") {
        // Act
        constexpr auto property_holds =
          std::is_assignable<sut_type,const copy_type&>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }

      // Assert
      SECTION("active element is error") {
        REQUIRE(sut.has_error());
      }
      SECTION("error is copied") {
        REQUIRE(sut == value);
      }
    }
  }
}

TEST_CASE("result<T,E>::operator=(failure<E2>&&)", "[assign]") {
  SECTION("E cant be constructed or assigned from E2") {
    SECTION("Result cannot be assigned from E2") {
      // Arrange
      using copy_type = failure<not_copy_or_moveable>;
      using sut_type  = result<int,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not nothrow move constructible from E2") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = failure<const char*>;
      using sut_type = result<int,throwing<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION(
    "E is not nothrow move constructible from E2, but is nothrow-move "
    "constructible"
  ) {
    SECTION("Result is move-assignable") {
      // Arrange
      using copy_type = failure<const char*>;
      using sut_type = result<int,std::string>;

      // This works because it generates an intermediate 'result<int, E>' in
      // between. The result is then move-constructed instead -- which is
      // non-throwing.

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("E can be constructed and assigned from E2") {
    SECTION("active element is T") {
      // Arrange
      using copy_type = failure<std::string>;
      using sut_type = result<report_destructor, move_only<std::string>>;

      auto value = copy_type{"hello world"};
      const auto copy = value;

      auto is_invoked = false;
      sut_type sut{&is_invoked};

      // Act
      sut = std::move(value);

      SECTION("Result can be assigned") {
        // Act
        constexpr auto property_holds =
          std::is_assignable<sut_type,copy_type&&>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }

      // Assert
      SECTION("Calls 'T's destructor first") {
        REQUIRE(is_invoked);
      }
      SECTION("active element is error") {
        REQUIRE(sut.has_error());
      }
      SECTION("error is copied") {
        REQUIRE(sut == copy);
      }
    }

    SECTION("active element is E") {
      // Arrange
      using copy_type = failure<std::string>;
      using sut_type = result<int, move_only<std::string>>;

      auto value = copy_type{"goodbye world"};
      const auto copy = value;
      sut_type sut{copy_type{"hello world"}};

      // Act
      sut = std::move(value);

      SECTION("Result can be assigned") {
        // Act
        constexpr auto property_holds =
          std::is_assignable<sut_type,copy_type&&>::value;

        // Assert
        STATIC_REQUIRE(property_holds);
      }

      // Assert
      SECTION("active element is error") {
        REQUIRE(sut.has_error());
      }
      SECTION("error is copied") {
        REQUIRE(sut == copy);
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("result<T,E>::operator->()", "[observers]") {
  // Arrange
  auto sut = result<int,int>{42};

  SECTION("Returns pointer to internal structure") {
    // Act
    const auto* pointer = sut.operator->();

    // Assert
    REQUIRE(pointer == &*sut);
  }

  SECTION("Returns a mutable pointer") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(sut.operator->()),int*>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
}

TEST_CASE("result<T,E>::operator->() const", "[observers]") {
  // Arrange
  const auto sut = result<int,int>{42};

  SECTION("Returns pointer to internal structure") {
    // Act
    const auto* pointer = sut.operator->();

    // Assert
    REQUIRE(pointer == &*sut);
  }

  SECTION("Returns a const pointer") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(sut.operator->()),const int*>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<T,E>::operator*() &", "[observers]") {
  // Arrange
  auto sut = result<int,int>{42};

  SECTION("Returns mutable lvalue reference") {
    // Act
    constexpr auto property_holds = std::is_same<decltype(*sut),int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Returns reference to internal structure") {
    // Act
    auto& x = sut.operator*();

    // Assert
    REQUIRE(x == 42);
  }
}

TEST_CASE("result<T,E>::operator*() const &", "[observers]") {
  // Arrange
  const auto sut = result<int,int>{42};

  SECTION("Returns const lvalue reference") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(*sut),const int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Returns reference to internal structure") {
    // Act
    auto& x = sut.operator*();

    // Assert
    REQUIRE(x == 42);
  }
}

TEST_CASE("result<T,E>::operator*() &&", "[observers]") {
  // Arrange
  auto sut = result<int,int>{42};

  SECTION("Returns mutable rvalue reference") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(*std::move(sut)),int&&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Returns reference to internal structure") {
    // Act
    auto&& x = std::move(sut).operator*();

    // Assert
    REQUIRE(x == 42);
  }
}

TEST_CASE("result<T,E>::operator*() const &&", "[observers]") {
  // Arrange
  const auto sut = result<int,int>{42};

  SECTION("Returns const rvalue reference") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(*std::move(sut)),const int&&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Returns reference to internal structure") {
    // Act
    auto&& x = std::move(sut).operator*();

    // Assert
    REQUIRE(x == 42);
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<T,E>::operator bool()", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Returns true") {
      // Arrange
      auto sut = result<int, int>{};

      // Act
      const auto actual = static_cast<bool>(sut);

      // Assert
      REQUIRE(actual);
    }
  }
  SECTION("result does not contain a value") {
    SECTION("Returns false") {
      // Arrange
      auto sut = result<int, int>{fail(42)};

      // Act
      const auto actual = static_cast<bool>(sut);

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
}

TEST_CASE("result<T,E>::has_value()", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Returns true") {
      // Arrange
      auto sut = result<int, int>{};

      // Act
      const auto actual = sut.has_value();

      // Assert
      REQUIRE(actual);
    }
  }
  SECTION("result does not contain a value") {
    SECTION("Returns false") {
      // Arrange
      auto sut = result<int, int>{fail(42)};

      // Act
      const auto actual = sut.has_value();

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
}

TEST_CASE("result<T,E>::has_error()", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Returns false") {
      // Arrange
      auto sut = result<int, int>{};

      // Act
      const auto actual = sut.has_error();

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
  SECTION("result does not contain a value") {
    SECTION("Returns true") {
      // Arrange
      auto sut = result<int, int>{fail(42)};

      // Act
      const auto actual = sut.has_error();

      // Assert
      REQUIRE(actual);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<T,E>::value() &", "[observers]") {
  SECTION("result contains a value") {
    // Arrange
    auto sut = result<int,std::error_code>{42};
    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        suppress_unused(sut.value());
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
    SECTION("Returns mutable lvalue reference to internal storage") {
      // Act
      constexpr auto property_holds =
        std::is_same<decltype(sut.value()),int&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
  SECTION("result contains an error") {
    SECTION("throws bad_result_access") {
      // Arrange
      auto sut = result<int,int>{
        fail(42)
      };

      // Arrange
      auto threw_expected = false;

      // Act
      try {
        suppress_unused(sut.value());
      } catch (const bad_result_access<int>&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
}

TEST_CASE("result<T,E>::value() const &", "[observers]") {
  SECTION("result contains a value") {
    // Arrange
    const auto sut = result<int,std::error_code>{42};
    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        suppress_unused(sut.value());
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
    SECTION("Returns const lvalue reference to internal storage") {
      // Act
      constexpr auto property_holds =
        std::is_same<decltype(sut.value()),const int&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
  SECTION("result contains an error") {
    SECTION("throws bad_result_access") {
      // Arrange
      const auto sut = result<int,int>{
        fail(42)
      };

      // Arrange
      auto threw_expected = false;

      // Act
      try {
        suppress_unused(sut.value());
      } catch (const bad_result_access<int>&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
}

TEST_CASE("result<T,E>::value() &&", "[observers]") {
  SECTION("result contains a value") {
    // Arrange
    auto sut = result<int,std::error_code>{42};
    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        suppress_unused(std::move(sut).value());
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
    SECTION("Returns mutable rvalue reference to internal storage") {
      // Act
      constexpr auto property_holds =
        std::is_same<decltype(std::move(sut).value()),int&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
  SECTION("result contains an error") {
    SECTION("throws bad_result_access") {
      // Arrange
      auto sut = result<int,move_only<std::string>>{
        fail("hello world")
      };

      // Arrange
      auto threw_expected = false;

      // Act
      try {
        suppress_unused(std::move(sut).value());
      } catch (const bad_result_access<move_only<std::string>>&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
}

TEST_CASE("result<T,E>::value() const &&", "[observers]") {
  SECTION("result contains a value") {
    // Arrange
    const auto sut = result<int,std::error_code>{42};
    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        suppress_unused(std::move(sut).value());
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
    SECTION("Returns const rvalue reference to internal storage") {
      // Act
      constexpr auto property_holds =
        std::is_same<decltype(std::move(sut).value()),const int&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
  SECTION("result contains an error") {
    SECTION("throws bad_result_access") {
      // Arrange
      const auto sut = result<int,int>{
        fail(42)
      };

      // Arrange
      auto threw_expected = false;

      // Act
      try {
        suppress_unused(std::move(sut).value());
      } catch (const bad_result_access<int>&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
}

//-----------------------------------------------------------------------------


TEST_CASE("result<T,E>::error() const &", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Returns default-constructed error") {
      // Arrange
      auto sut = result<int, int>{};

      // Act
      const auto output = sut.error();

      // Assert
      REQUIRE(output == int{});
    }
  }
  SECTION("result contains an error") {
    SECTION("Returns a copy of the exception") {
      // Arrange
      const auto value = 42;
      auto sut = result<int, int>{fail(value)};

      // Act
      const auto output = sut.error();

      // Assert
      REQUIRE(output == value);
    }
  }
}

TEST_CASE("result<T,E>::error() &&", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Returns default-constructed error") {
      // Arrange
      auto sut = result<int, move_only<std::error_code>>{};

      // Act
      const auto output = std::move(sut).error();

      // Assert
      REQUIRE(output == std::error_code{});
    }
  }
  SECTION("result contains an error") {
    SECTION("Returns a copy of the exception") {
      // Arrange
      const auto value = std::io_errc::stream;
      auto sut = result<int, move_only<std::error_code>>{fail(value)};

      // Act
      const auto output = std::move(sut).error();

      // Assert
      REQUIRE(output == value);
    }
  }
}

TEST_CASE("result<T,E>::expect(String&&) const &", "[observers]") {
  SECTION("result contains value") {
    // Arrange
    const auto sut = result<int,int>{42};

    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        sut.expect("test");
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
  }
  SECTION("result contains error") {
    SECTION("throws exception") {
      // Arrange
      const auto sut = result<int,int>{fail(42)};

      // Arrange
      auto threw_expected = false;

      // Act
      try {
        sut.expect("test");
      } catch (const bad_result_access<int>&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
}

TEST_CASE("result<T,E>::expect(String&&) &&", "[observers]") {
  SECTION("result contains value") {
    // Arrange
    auto sut = result<int,move_only<std::string>>{42};

    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        std::move(sut).expect("test");
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
  }
  SECTION("result contains error") {
    SECTION("throws exception") {
      // Arrange
      auto sut = result<int,move_only<std::string>>{fail("hello world")};

      // Arrange
      auto threw_expected = false;

      // Act
      try {
        std::move(sut).expect("test");
      } catch (const bad_result_access<move_only<std::string>>&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
}

//-----------------------------------------------------------------------------
// Monadic Functionalities
//-----------------------------------------------------------------------------

TEST_CASE("result<T,E>::value_or(U&&) const &", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Returns result's value") {
      // Arrange
      const auto input = 0;
      auto sut = result<int, std::error_code>{42};

      // Act
      const auto output = sut.value_or(input);

      // Assert
      REQUIRE(output == *sut);
    }
  }
  SECTION("result contains an error") {
    SECTION("Returns the input") {
      // Arrange
      const auto input = 42;
      auto sut = result<int, std::error_code>{
        fail(std::io_errc::stream)
      };

      // Act
      const auto output = sut.value_or(input);

      // Assert
      REQUIRE(output == input);
    }
  }
}

TEST_CASE("result<T,E>::value_or(U&&) &&", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Returns the input") {
      // Arrange
      auto input = "Hello world";
      auto sut = result<move_only<std::string>, int>{input};

      // Act
      const auto output = std::move(sut).value_or("other");

      // Assert
      REQUIRE(output == input);
    }
  }
  SECTION("result contains an error") {
    SECTION("Returns the error") {
      // Arrange
      auto input = "Hello world";
      auto sut = result<move_only<std::string>, int>{
        fail(42)
      };

      // Act
      const auto output = std::move(sut).value_or(input);

      // Assert
      REQUIRE(output == input);
    }
  }
}

TEST_CASE("result<T,E>::error_or(U&&) const &", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Returns the input") {
      // Arrange
      const auto input = std::error_code{};
      auto sut = result<int, std::error_code>{42};

      // Act
      const auto output = sut.error_or(input);

      // Assert
      REQUIRE(output == input);
    }
  }
  SECTION("result contains an error") {
    SECTION("Returns the error") {
      // Arrange
      const auto input = std::error_code{std::io_errc::stream};
      auto sut = result<int, std::error_code>{
        fail(input)
      };

      // Act
      const auto output = sut.error_or(input);

      // Assert
      REQUIRE(output == input);
    }
  }
}

TEST_CASE("result<T,E>::error_or(U&&) &&", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Returns the input") {
      // Arrange
      auto input = std::error_code{};
      const auto copy = input;
      auto sut = result<int, move_only<std::error_code>>{42};

      // Act
      const auto output = std::move(sut).error_or(std::move(input));

      // Assert
      REQUIRE(output == copy);
    }
  }
  SECTION("result contains an error") {
    SECTION("Returns the error") {
      // Arrange
      auto input = std::error_code{std::io_errc::stream};
      const auto copy = input;
      auto sut = result<int, move_only<std::error_code>>{
        fail(input)
      };

      // Act
      const auto output = std::move(sut).error_or(std::move(input));

      // Assert
      REQUIRE(output == copy);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<T,E>::and_then(U&&) const", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Maps the input") {
      // Arrange
      const auto input = 42;
      auto sut = result<int, std::error_code>{};

      // Act
      const auto output = sut.and_then(input);

      // Assert
      REQUIRE(output == input);
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto input = 42;
      const auto error = fail(std::io_errc::stream);
      auto sut = result<int, std::error_code>{error};

      // Act
      const auto output = sut.and_then(input);

      // Assert
      REQUIRE(output == error);
    }
  }
}

TEST_CASE("result<T,E>::flat_map(Fn&&) const &", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Maps the input") {
      // Arrange
      const auto value = 42;
      const auto sut = result<int,std::error_code>{value};

      // Act
      const auto output = sut.flat_map([](int x){
        return result<std::string,std::error_code>{std::to_string(x)};
      });

      // Assert
      REQUIRE(output == std::to_string(value));
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail(std::io_errc::stream);
      const auto sut = result<int,std::error_code>{error};

      // Act
      const auto output = sut.flat_map([](int x){
        return result<std::string,std::error_code>{std::to_string(x)};
      });

      // Assert
      REQUIRE(output == error);
    }
  }
}

TEST_CASE("result<T,E>::flat_map(Fn&&) &&", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Maps the input") {
      // Arrange
      const auto value = 42;
      auto sut = result<int,move_only<std::error_code>>{value};

      // Act
      const auto output = std::move(sut).flat_map([](int x){
        return result<std::string,std::error_code>{std::to_string(x)};
      });

      // Assert
      REQUIRE(output == std::to_string(value));
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail(std::io_errc::stream);
      auto sut = result<int,move_only<std::error_code>>{error};

      // Act
      const auto output = std::move(sut).flat_map([](int x){
        return result<std::string,std::error_code>{std::to_string(x)};
      });

      // Assert
      REQUIRE(output == error);
    }
  }
}

TEST_CASE("result<T,E>::map(Fn&&) const &", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Function returns non-void") {
      SECTION("Maps the input") {
        // Arrange
        const auto value = 42;
        auto sut = result<int,std::io_errc>{value};

        // Act
        const auto output = sut.map([](int x){
          return std::to_string(x);
        });

        // Assert
        REQUIRE(output == std::to_string(value));
      }
    }
    SECTION("Function returns void") {
      SECTION("Maps input to void") {
        // Arrange
        const auto value = 42;
        auto sut = result<int,std::io_errc>{value};

        // Act
        const auto output = sut.map([](int){});

        // Assert
        SECTION("Result has value") {
          REQUIRE(output.has_value());
        }
        SECTION("Result is result<void,E>") {
          // Act
          constexpr auto property_holds =
            std::is_same<decltype(output),const result<void,std::io_errc>>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }
      }
    }
  }
  SECTION("result contains an error") {
    SECTION("Function returns non-void") {
      SECTION("Maps the error") {
        // Arrange
        const auto error = fail(std::io_errc::stream);
        auto sut = result<int,std::io_errc>{error};

        // Act
        const auto output = sut.map([](int x){
          return std::to_string(x);
        });

        // Assert
        REQUIRE(output == error);
      }
    }
    SECTION("Function returns void") {
      SECTION("Maps input to void") {
        // Arrange
        const auto error = fail(std::io_errc::stream);
        auto sut = result<int,std::io_errc>{error};

        // Act
        const auto output = sut.map([](int) -> void{});

        // Assert
        SECTION("Result contains error") {
          REQUIRE(output == error);
        }
        SECTION("Result is result<void,E>") {
          // Act
          constexpr auto property_holds =
            std::is_same<decltype(output),const result<void,std::io_errc>>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }
      }
    }
  }
}

TEST_CASE("result<T,E>::map(Fn&&) &&", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Function returns non-void") {
      SECTION("Maps the input") {
        // Arrange
        const auto value = 42;
        auto sut = result<int,move_only<std::error_code>>{value};

        // Act
        const auto output = std::move(sut).map([](int x){
          return std::to_string(x);
        });

        // Assert
        REQUIRE(output == std::to_string(value));
      }
    }
    SECTION("Function returns void") {
      SECTION("Maps input to void") {
        // Arrange
        const auto value = 42;
        auto sut = result<int,move_only<std::error_code>>{value};

        // Act
        const auto output = std::move(sut).map([](int){});

        // Assert
        SECTION("Result has value") {
          REQUIRE(output.has_value());
        }
        SECTION("Result is result<void,E>") {
          // Act
          constexpr auto property_holds =
            std::is_same<decltype(output),const result<void,move_only<std::error_code>>>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }
      }
    }
  }
  SECTION("result contains an error") {
    SECTION("Function returns non-void") {
      SECTION("Maps the error") {
        // Arrange
        const auto error = fail(std::io_errc::stream);
        auto sut = result<int,move_only<std::error_code>>{error};

        // Act
        const auto output = std::move(sut).map([&](int x){
          return std::to_string(x);
        });

        // Assert
        REQUIRE(output == error);
      }
    }
    SECTION("Function returns void") {
      SECTION("Maps input to void") {
        // Arrange
        const auto error = fail(std::io_errc::stream);
        auto sut = result<int,move_only<std::error_code>>{error};

        // Act
        const auto output = std::move(sut).map([](int) -> void{});

        // Assert
        SECTION("Result contains the error") {
          REQUIRE(output == error);
        }
        SECTION("Result is result<void,E>") {
          // Act
          constexpr auto property_holds =
            std::is_same<decltype(output),const result<void,move_only<std::error_code>>>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }
      }
    }
  }
}

TEST_CASE("result<T,E>::map_error(Fn&&) const &", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Maps the input") {
      // Arrange
      const auto value = 42;
      auto sut = result<int,std::io_errc>{value};

      // Act
      const auto output = sut.map_error([](std::io_errc e){
        return std::error_code{e};
      });

      // Assert
      REQUIRE(output == value);
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail(std::io_errc::stream);
      auto sut = result<int,std::io_errc>{error};

      // Act
      const auto output = sut.map_error([](std::io_errc e){
        return std::error_code{e};
      });

      // Assert
      REQUIRE(output == error);
    }
  }
}

TEST_CASE("result<T,E>::map_error(Fn&&) &&", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Maps the input") {
      // Arrange
      const auto value = "hello world";
      auto sut = result<move_only<std::string>,std::io_errc>{value};

      // Act
      const auto output = std::move(sut).map_error([](std::io_errc e){
        return std::error_code{e};
      });

      // Assert
      REQUIRE(output == value);
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail(std::io_errc::stream);
      auto sut = result<move_only<std::string>,std::io_errc>{error};

      // Act
      const auto output = std::move(sut).map_error([](std::io_errc e){
        return std::error_code{e};
      });

      // Assert
      REQUIRE(output == error);
    }
  }
}

TEST_CASE("result<T,E>::flat_map_error(Fn&&) const &", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Forwards underlying value") {
      // Arrange
      const auto value = 42;
      const auto sut = result<int,int>{value};

      // Act
      const auto output = sut.flat_map_error([&](int x){
        return result<long, int>{x};
      });

      // Assert
      REQUIRE(output == value);
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail(42);
      const auto sut = result<int, long>{error};

      // Act
      const auto output = sut.flat_map_error([&](long x){
        return result<int, short>{x};
      });

      // Assert
      REQUIRE(output == error.error());
    }
  }
}

TEST_CASE("result<T,E>::flat_map_error(Fn&&) &&", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Default-initializes T") {
      // Arrange
      const auto value = "hello world";
      auto sut = result<move_only<std::string>,move_only<std::string>>{value};

      // Act
      const auto output = std::move(sut).flat_map_error([](move_only<std::string>&& x){
        return result<std::string, std::string>{in_place_error, std::move(x)};
      });

      // Assert
      REQUIRE(output == value);
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail("Hello world");
      auto sut = result<long,move_only<std::string>>{error};

      // Act
      const auto output = std::move(sut).flat_map_error([](move_only<std::string>&& x){
        return result<int, std::string>{in_place_error, std::move(x)};
      });

      // Assert
      REQUIRE(output == error);
    }
  }
}

//=============================================================================
// class : result<T, E>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("result<T&,E>::result()", "[ctor]") {
  SECTION("Result containing a reference is not default-constructible") {
    // Arrange
    using sut_type = result<int&,int>;

    // Act
    constexpr auto property_holds =
      std::is_default_constructible<sut_type>::value;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

TEST_CASE("result<T&,E>::result(const result&)", "[ctor]") {
  // Arrange
  using sut_type = result<int&,int>;

  SECTION("Result containing a reference is copy-constructible") {
    // Act
    constexpr auto property_holds = std::is_copy_constructible<sut_type>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }

  SECTION("Constructs an result that references the source value") {
    auto value = int{42};

    const auto source = sut_type{value};

    // Act
    const auto sut = source;

    // Assert
    SECTION("Refers to underlying input") {
      REQUIRE(&value == &(*sut));
    }
  }
}

TEST_CASE("result<T&,E>::result(result&&)", "[ctor]") {
  // Arrange
  using sut_type = result<int&,move_only<std::string>>;

  SECTION("Result containing a reference is copy-constructible") {
    // Act
    constexpr auto property_holds = std::is_move_constructible<sut_type>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }

  SECTION("Constructs an result that references the source value") {
    auto value = int{42};

    auto source = sut_type{value};

    // Act
    const auto sut = std::move(source);

    // Assert
    SECTION("Refers to underlying input") {
      REQUIRE(&value == &(*sut));
    }
  }
}

TEST_CASE("result<T&,E>::result(const result<T2,E2>&)", "[ctor]") {
  SECTION("other's type holds T by-value") {
    SECTION("Constructor is disabled") {
      // Arrange
      using from_type = result<int,std::error_code>;
      using sut_type = result<int&, std::error_code>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, const from_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("other's type holds T by reference") {
    // Arrange
    using from_type = result<derived&,int>;
    using sut_type = result<base&,int>;

    SECTION("Result containing a reference is copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, const from_type&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Constructs an result that references the source value") {
      const auto input = 10;
      auto value = derived{input};

      const auto source = from_type{value};

      // Act
      const auto sut    = sut_type{source};

      // Assert
      SECTION("Refers to underlying input") {
        REQUIRE(&value == &(*sut));
      }
      SECTION("Reference works with hierarchies") {
        REQUIRE(sut->get_value() == input);
      }
    }
  }
}

TEST_CASE("result<T&,E>::result(result<T2,E2>&&)", "[ctor]") {

  SECTION("other's type holds T by-value") {
    SECTION("Constructor is disabled") {
      // Arrange
      using from_type = result<int,move_only<std::string>>;
      using sut_type = result<int&,move_only<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, from_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("other's type holds T by reference") {
    // Arrange
    using from_type = result<derived&,move_only<std::string>>;
    using sut_type = result<base&,move_only<std::string>>;

    SECTION("Result containing a reference is copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type, from_type&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Constructs an result that references the source value") {
      const auto input = 10;
      auto value = derived{input};

      auto source = from_type{value};

      // Act
      const auto sut = sut_type{std::move(source)};

      // Assert
      SECTION("Refers to underlying input") {
        REQUIRE(&value == &(*sut));
      }
      SECTION("Reference works with hierarchies") {
        REQUIRE(sut->get_value() == input);
      }
    }
  }
}

TEST_CASE("result<T&,E>::result(in_place_t, Args&&...)", "[ctor]") {
  // Arrange
  using sut_type = result<base&,int>;

  auto value = derived{42};

  // Act
  const sut_type sut{std::in_place, value};

  // Assert
  SECTION("Contains a value") {
    REQUIRE(sut.has_value());
  }

  SECTION("References old input") {
    REQUIRE(&value == &(*sut));
  }
}

TEST_CASE("result<T&,E>::result(const failure<E2>&)", "[ctor]") {
  // Arrange
  using sut_type = result<int&,std::string>;
  const auto source = fail<std::string>("hello world");

  // Act
  const sut_type sut{source};

  // Assert
  SECTION("Contains an error") {
    REQUIRE(sut.has_error());
  }

  SECTION("Contains an error constructed from a copy of E2") {
    REQUIRE(sut == source);
  }
}

TEST_CASE("result<T&,E>::result(failure<E2>&&)", "[ctor]") {
  // Arrange
  using sut_type = result<int&,move_only<std::string>>;
  auto source = fail<std::string>("hello world");
  const auto copy = source;

  // Act
  const sut_type sut{std::move(source)};

  // Assert
  SECTION("Contains an error") {
    REQUIRE(sut.has_error());
  }

  SECTION("Contains an error constructed from E2") {
    REQUIRE(sut == copy);
  }
}

TEST_CASE("result<T&,E>::result(U&&)", "[ctor]") {
  SECTION("Input is lvalue") {
    // Arrange
    using sut_type = result<base&,int>;

    auto value = derived{42};

    // Act
    const sut_type sut{value};

    // Assert
    SECTION("Constructor is enabled") {
      REQUIRE(std::is_constructible<sut_type,derived&>::value);
    }

    SECTION("Contains a value") {
      REQUIRE(sut.has_value());
    }

    SECTION("References old input") {
      REQUIRE(&value == &(*sut));
    }
  }
  SECTION("Input is rvalue") {
    // Arrange
    using sut_type = result<base&,int>;

    SECTION("Constructor is disabled") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,derived&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("Input is value") {
    // Arrange
    using sut_type = result<base&,int>;

    SECTION("Constructor is disabled") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<sut_type,derived>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<T&,E>::operator=(const result&)", "[assign]") {
  SECTION("result contains a value") {
    SECTION("other contains a value") {
      // Arrange
      auto value = derived{42};
      auto next = derived{0};

      const auto source = result<base&,std::error_code>{next};
      auto sut = result<base&,std::error_code>{value};

      // Act
      sut = source;

      // Assert
      SECTION("Underlying reference is rebound") {
        REQUIRE(&next == &(*sut));
      }
    }
  }
  SECTION("result contains an error") {
    SECTION("other contains a value") {
      // Arrange
      auto next = derived{0};

      const auto source = result<base&,int>{next};
      auto sut = result<base&,int>{fail(42)};

      // Act
      sut = source;

      // Assert
      SECTION("Binds the new reference") {
        REQUIRE(&next == &(*sut));
      }
    }
  }
}

TEST_CASE("result<T&,E>::operator=(result&&)", "[assign]") {
  SECTION("result contains a value") {
    SECTION("other contains a value") {
      // Arrange
      auto value = derived{42};
      auto next = derived{0};

      auto source = result<base&,move_only<std::string>>{next};
      auto sut = result<base&,move_only<std::string>>{value};

      // Act
      sut = std::move(source);

      // Assert
      SECTION("Underlying reference is rebound") {
        REQUIRE(&next == &(*sut));
      }
    }
  }
  SECTION("result contains an error") {
    SECTION("other contains a value") {
      // Arrange
      auto next = derived{0};

      auto source = result<base&,move_only<std::string>>{next};
      auto sut = result<base&,move_only<std::string>>{fail("foo")};

      // Act
      sut = std::move(source);

      // Assert
      SECTION("Binds the new reference") {
        REQUIRE(&next == &(*sut));
      }
    }
  }
}

TEST_CASE("result<T&,E>::operator=(const result<T2,E2>&)", "[assign]") {
  SECTION("other's type holds T by-value") {
    SECTION("Assignment is disabled") {
      // Arrange
      using source_type = result<int,std::error_code>;
      using sut_type = result<int&,std::error_code>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, const source_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("result contains a value") {
    SECTION("other contains a value") {
      // Arrange
      auto value = derived{42};
      auto next = derived{0};

      const auto source = result<derived&,std::error_code>{next};
      auto sut = result<base&,std::error_code>{value};

      // Act
      sut = source;

      // Assert
      SECTION("Underlying reference is rebound") {
        REQUIRE(&next == &(*sut));
      }
    }
  }
  SECTION("result contains an error") {
    SECTION("other contains a value") {
      // Arrange
      auto next = derived{0};

      const auto source = result<derived&,int>{next};
      auto sut = result<base&,int>{fail(42)};

      // Act
      sut = source;

      // Assert
      SECTION("Binds the new reference") {
        REQUIRE(&next == &(*sut));
      }
    }
  }
}

TEST_CASE("result<T&,E>::operator=(result<T2,E2>&&)", "[assign]") {
  SECTION("other's type holds T by-value") {
    SECTION("Assignment is disabled") {
      // Arrange
      using source_type = result<int,move_only<std::string>>;
      using sut_type = result<int&,move_only<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type, source_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("result contains a value") {
    SECTION("other contains a value") {
      // Arrange
      auto value = derived{42};
      auto next = derived{0};

      auto source = result<derived&,move_only<std::string>>{next};
      auto sut = result<base&,move_only<std::string>>{value};

      // Act
      sut = std::move(source);

      // Assert
      SECTION("Underlying reference is rebound") {
        REQUIRE(&next == &(*sut));
      }
    }
  }
  SECTION("result contains an error") {
    SECTION("other contains a value") {
      // Arrange
      auto next = derived{0};

      auto source = result<derived&,move_only<std::string>>{next};
      auto sut = result<base&,move_only<std::string>>{fail("foo")};

      // Act
      sut = std::move(source);

      // Assert
      SECTION("Binds the new reference") {
        REQUIRE(&next == &(*sut));
      }
    }
  }
}

TEST_CASE("result<T&,E>::operator=(U&&)", "[assign]") {
  SECTION("Source contains a value") {
    // Arrange
    auto value = int{42};
    auto next = int{0};

    auto sut = result<int&,std::error_code>{value};

    // Act
    sut = next;

    // Assert
    SECTION("Rebinds the referred-to value") {
      REQUIRE(&next == &(*sut));
    }
  }
  SECTION("Source contains an error") {
    // Arrange
    auto next = int{0};

    auto sut = result<int&,int>{
      fail(42)
    };

    // Act
    sut = next;

    // Assert
    SECTION("Result contains a value") {
      REQUIRE(sut.has_value());
    }
    SECTION("Binds to the new reference") {
      REQUIRE(&next == &(*sut));
    }
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("result<T&,E>::operator*() &", "[observers]") {
  // Arrange
  auto value = int{42};
  auto sut = result<int&,int>{value};

  SECTION("Returns mutable lvalue reference") {
    // Act
    constexpr auto property_holds = std::is_same<decltype(*sut),int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Returns reference to referred value") {
    // Act
    const auto result = &(*sut);

    // Assert
    REQUIRE(result == &value);
  }
}

TEST_CASE("result<T&,E>::operator*() const &", "[observers]") {
  // Arrange
  auto value = int{42};
  const auto sut = result<int&,int>{value};

  SECTION("Returns lvalue reference that does not propagate constness") {
    // Act
    constexpr auto property_holds = std::is_same<decltype(*sut),int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Returns reference to referred value") {
    // Act
    const auto result = &(*sut);

    // Assert
    REQUIRE(result == &value);
  }
}

TEST_CASE("result<T&,E>::operator*() &&", "[observers]") {
  // Arrange
  auto value = int{42};
  auto sut = result<int&,int>{value};

  SECTION("Returns mutable lvalue reference") {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(*std::move(sut)),int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Returns reference to referred value") {
    // Act
    int& x = (*std::move(sut));

    // Assert
    REQUIRE(&x == &value);
  }
}

TEST_CASE("result<T&,E>::operator*() const &&", "[observers]") {
  // Arrange
  auto value = int{42};
  const auto sut = result<int&,int>{value};

  SECTION(
    "Returns mutable Lvalue reference that does not propagate constness"
  ) {
    // Act
    constexpr auto property_holds =
      std::is_same<decltype(*std::move(sut)),int&>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Returns reference to referred value") {
    // Act
    const int& x = (*std::move(sut));

    // Assert
    REQUIRE(&x == &value);
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<T&,E>::value() &", "[observers]") {
  SECTION("result contains a value") {
    // Arrange
    auto value = int{42};
    auto sut = result<int&,int>{value};

    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        suppress_unused(sut.value());
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
    SECTION("Returns mutable lvalue reference to internal storage") {
      // Act
      constexpr auto property_holds =
        std::is_same<decltype(sut.value()),int&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Refers to referenced value") {
      // Act
      int& x = sut.value();

      // Assert
      REQUIRE(&value == &x);
    }
  }
}

TEST_CASE("result<T&,E>::value() const &", "[observers]") {
  SECTION("result contains a value") {
    // Arrange
    auto value = int{42};
    const auto sut = result<int&,int>{value};

    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        suppress_unused(sut.value());
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
    SECTION("Returns mutable lvalue that does not propagate constness") {
      // Act
      constexpr auto property_holds =
        std::is_same<decltype(sut.value()),int&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Refers to referenced value") {
      // Act
      int& x = sut.value();

      // Assert
      REQUIRE(&value == &x);
    }
  }
}

TEST_CASE("result<T&,E>::value() &&", "[observers]") {
  SECTION("result contains a value") {
    // Arrange
    auto value = int{42};
    auto sut = result<int&,int>{value};

    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        suppress_unused(std::move(sut).value());
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
    SECTION("Returns mutable lvalue") {
      // Act
      constexpr auto property_holds =
        std::is_same<decltype(std::move(sut).value()),int&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Refers to referenced value") {
      // clang-tidy incorrectly deduces this as a use-after-move due to the
      // std::move in the section above. These cases are never invoked together,
      // and thus this use-after-move can never happen.
      //
      // Act
      // NOLINTNEXTLINE(bugprone-use-after-move)
      int& x = std::move(sut).value();

      // Assert
      REQUIRE(&value == &x);
    }
  }
}

TEST_CASE("result<T&,E>::value() const &&", "[observers]") {
  SECTION("result contains a value") {
    // Arrange
    auto value = int{42};
    const auto sut = result<int&,int>{value};

    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        suppress_unused(std::move(sut).value());
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
    SECTION("Returns mutable lvalue that does not propagate constness") {
      // Act
      constexpr auto property_holds =
        std::is_same<decltype(std::move(sut).value()),int&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Refers to referenced value") {
      // clang-tidy incorrectly deduces this as a use-after-move due to the
      // std::move in the section above. These cases are never invoked together,
      // and thus this use-after-move can never happen.
      //
      // Act
      // NOLINTNEXTLINE(bugprone-use-after-move)
      int& x = std::move(sut).value();

      // Assert
      REQUIRE(&value == &x);
    }
  }
}

//=============================================================================
// class : result<void, E>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("result<void,E>::result()", "[ctor]") {
  SECTION("Result is default-constructible") {
    // Arrange
    using sut_type = result<void,int>;

    // Act
    constexpr auto property_holds =
      std::is_default_constructible<sut_type>::value;

    // Assert
    STATIC_REQUIRE(property_holds);
  }

  SECTION("Constructs an result with a 'value' state") {
    // Arrange
    using sut_type = result<void,int>;

    // Act
    auto sut = sut_type{};

    // Assert
    REQUIRE(sut.has_value());
  }
}

TEST_CASE("result<void,E>::result(const result&)", "[ctor]") {

  // Constructible:

  SECTION("E is trivially copy-constructible") {
    // Arrange
    using sut_type = result<void,int>;

    SECTION("Result is trivially copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Copy source contained a value") {
      const sut_type source{};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains a value") {
        REQUIRE(sut.has_value());
      }
    }

    SECTION("Copy source contained an error") {
      const auto error = fail(42);
      const sut_type source{error};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Copy contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  } // T is trivially copy-constructible

  SECTION("E is copy-constructible, but not trivial") {
    // Arrange
    using sut_type = result<void,std::string>;

    SECTION("Result is not trivially copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }

    SECTION("Result is copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Copy source contained a value") {
      const sut_type source{};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains a value") {
        REQUIRE(sut.has_value());
      }
    }

    SECTION("Copy source contained an error") {
      const auto error = fail("Hello world");
      const sut_type source{error};

      // Act
      const auto sut = source;

      // Assert
      SECTION("Copy contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Copy contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  }

  // Not constructible:

  SECTION("E is not copy-constructible") {
    // Arrange
    using sut_type = result<void,not_copy_or_moveable>;

    SECTION("Result is not copy-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_copy_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("result<void,E>::result(result&&)", "[ctor]") {

  // Constructible:

  SECTION("E is trivially move-constructible") {
    // Arrange
    using sut_type = result<void,int>;

    SECTION("Result is trivially move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Move source contained a value") {
      sut_type source{};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains a value") {
        REQUIRE(sut.has_value());
      }
    }

    SECTION("Move source contained an error") {
      const auto error = fail(42);
      sut_type source{error};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("New result contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  } // E is trivially move-constructible

  SECTION("E is move-constructible, but not trivial") {
    // Arrange
    using sut_type = result<void,move_only<std::string>>;

    SECTION("Result is not trivially move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }

    SECTION("Result is move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }

    SECTION("Move source contained a value") {
      sut_type source{};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains a value") {
        REQUIRE(sut.has_value());
      }
    }

    SECTION("Move source contained an error") {
      const auto error = fail("Hello world");
      sut_type source{error};

      // Act
      const auto sut = std::move(source);

      // Assert
      SECTION("New result contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("New result contains an error equal to the source") {
        REQUIRE(sut == error);
      }
    }
  }

  // Not constructible:

  SECTION("E is not move-constructible") {
    // Arrange
    using sut_type = result<void,not_copy_or_moveable>;

    SECTION("Result is not move-constructible") {
      // Act
      constexpr auto property_holds =
        std::is_move_constructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("result<void,E>::result(const result<U,E2>&)", "[ctor]") {
  SECTION("E2 is convertible to E") {
    SECTION("Constructor is enabled") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<result<void,int>,const result<void,long>&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Other contains value") {
      // Arrange
      const auto other = result<int,long>{42};

      // Act
      const auto sut = result<void,int>{other};

      // Assert
      SECTION("Constructed result is in a value state") {
        REQUIRE(sut.has_value());
      }
    }

    SECTION("Other contains error") {
      // Arrange
      const auto other = result<int,long>{
        fail(42)
      };

      // Act
      const auto sut = result<void,int>{other};

      // Assert
      SECTION("Constructed result is in an error state") {
        REQUIRE(sut.has_error());
      }
      SECTION("Contained error is converted from other") {
        REQUIRE(sut.error() == other.error());
      }
    }
  }

  SECTION("E2 is not convertible to E") {
    SECTION("Constructor is disabled") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<result<void,int>,const result<void,std::string>&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("result<void,E>::result(result<U,E2>&&)", "[ctor]") {
  SECTION("E2 is convertible to E") {
    SECTION("Constructor is enabled") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<result<void,int>,result<void,long>&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Other contains value") {
      // Arrange
      auto other = result<int,long>{42};

      // Act
      auto sut = result<void,int>{std::move(other)};

      // Assert
      SECTION("Constructed result is in a value state") {
        REQUIRE(sut.has_value());
      }
    }

    SECTION("Other contains error") {
      // Arrange
      auto error = fail("Hello world");
      auto other = result<int,move_only<std::string>>{error};

      // Act
      auto sut = result<void,std::string>{
        std::move(other)
      };

      // Assert
      SECTION("Constructed result is in an error state") {
        REQUIRE(sut.has_error());
      }
      SECTION("Contained error is converted from other") {
        REQUIRE(sut == error);
      }
    }
  }

  SECTION("E2 is not convertible to E") {
    SECTION("Constructor is disabled") {
      // Act
      constexpr auto property_holds =
        std::is_constructible<result<void,int>,result<void,std::string>&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
}

TEST_CASE("result<void,E>::result(in_place_t)", "[ctor]") {
  // Arrange
  using sut_type = result<void,int>;

  // Act
  const sut_type sut(std::in_place);

  // Assert
  SECTION("Contains a value") {
    REQUIRE(sut.has_value());
  }
}

TEST_CASE("result<void,E>::result(in_place_error_t, Args&&...)", "[ctor]") {
  // Arrange
  using sut_type = result<void,std::string>;

  const auto input = "hello world";
  const auto size = 5;
  const auto result = std::string{input, size};

  // Act
  const sut_type sut(in_place_error, input, size);

  // Assert
  SECTION("Contains an error") {
    REQUIRE(sut.has_error());
  }

  SECTION("Contains an error constructed from a copy of E2") {
    REQUIRE(sut == fail(std::ref(result)));
  }
}

TEST_CASE("result<void,E>::result(in_place_error_t, std::initializer_list<U>, Args&&...)", "[ctor]") {
  // Arrange
  using sut_type = result<void,std::string>;

  const auto result = std::string{
    {'h','e','l','l','o'}, std::allocator<char>{}
  };

  // Act
  const sut_type sut(in_place_error, {'h','e','l','l','o'}, std::allocator<char>{});

  // Assert
  SECTION("Contains an error") {
    REQUIRE(sut.has_error());
  }

  SECTION("Contains an error constructed from a copy of E2") {
    REQUIRE(sut == fail(std::ref(result)));
  }
}

TEST_CASE("result<void,E>::result(const failure<E2>&)", "[ctor]") {
  // Arrange
  using sut_type = result<void,std::string>;
  const auto source = fail<std::string>("hello world");

  // Act
  const sut_type sut{source};

  // Assert
  SECTION("Contains an error") {
    REQUIRE(sut.has_error());
  }

  SECTION("Contains an error constructed from a copy of E2") {
    REQUIRE(sut == source);
  }
}

TEST_CASE("result<void,E>::result(failure<E2>&&)", "[ctor]") {
  // Arrange
  using sut_type = result<void,move_only<std::string>>;
  auto source = fail<std::string>("hello world");
  const auto copy = source;

  // Act
  const sut_type sut{std::move(source)};

  // Assert
  SECTION("Contains an error") {
    REQUIRE(sut.has_error());
  }

  SECTION("Contains an error constructed from E2") {
    REQUIRE(sut == copy);
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<void,E>::~result()", "[dtor]") {
  SECTION("E is trivially destructible") {
    // Arrange
    using sut_type = result<void, int>;

    SECTION("result's destructor is trivial") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_destructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("E is not trivially destructible") {
    // Arrange
    using sut_type = result<void, report_destructor>;

    auto invoked = false;

    // Act
    {
      sut_type sut{in_place_error, &invoked};
      (void) sut;
    }

    SECTION("result's destructor is not trivial") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_destructible<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }

    // Assert
    SECTION("Invokes E's underlying destructor") {
      REQUIRE(invoked);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<void,E>::operator=(const result& other)", "[assign]") {
  SECTION("E is not nothrow copy constructible") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using sut_type = result<void,std::string>;

      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is not copy-assignable") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using sut_type = result<void,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is nothrow copy-constructible") {
    // Arrange
    using sut_type = result<void,std::error_code>;

    SECTION("Result is copy-assignable") {
      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Copies the state of 'other'") {
      SECTION("Original contains value") {
        const auto output = fail(std::io_errc::stream);
        const sut_type copy{output};
        sut_type sut{};

        // Act
        sut = copy;

        // Assert
        SECTION("Result changes to error type") {
          REQUIRE(sut.has_error());
        }
        SECTION("Result constructs error") {
          REQUIRE(copy == sut);
        }
      }
      SECTION("Original contains error") {
        const auto output = fail(std::io_errc::stream);
        const sut_type copy{output};
        // Start from an error other than 'stream' so the assignment is
        // observable. 'io_errc' is a scoped enum, so zero is a valid value
        // even though no enumerator names it.
        // NOLINTNEXTLINE(bugprone-invalid-enum-default-initialization)
        sut_type sut{fail(std::io_errc{})};

        // Act
        sut = copy;

        // Assert
        SECTION("Result has error type") {
          REQUIRE(sut.has_error());
        }
        SECTION("Result assigns error") {
          REQUIRE(copy == sut);
        }
      }
    }
  }

  SECTION("E is trivial copy-constructible and copy-assignable") {
    // Arrange
    using sut_type = result<void, int>;

    SECTION("Result is copy-assignable") {
      // Act
      constexpr auto property_holds = std::is_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Result is trivial copy-assignable") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_copy_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
}

TEST_CASE("result<void,E>::operator=(result&& other)", "[assign]") {
  SECTION("E is not nothrow move constructible") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using sut_type = result<void,throwing<std::string>>;

      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is not move-assignable") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using sut_type = result<void,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is nothrow move-constructible") {
    // Arrange
    using sut_type = result<void,move_only<std::error_code>>;

    SECTION("Result is move-assignable") {
      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Copies the state of 'other'") {
      SECTION("Original contains value") {
        const auto output = fail(std::io_errc::stream);
        sut_type other{output};
        sut_type sut{};

        // Act
        sut = std::move(other);

        // Assert
        SECTION("Result changes to error type") {
          REQUIRE(sut.has_error());
        }
        SECTION("Result constructs error") {
          REQUIRE(output == sut);
        }
      }
      SECTION("Original contains error") {
        const auto output = fail(std::io_errc::stream);
        sut_type other{output};
        // Start from an error other than 'stream' so the assignment is
        // observable. 'io_errc' is a scoped enum, so zero is a valid value
        // even though no enumerator names it.
        // NOLINTNEXTLINE(bugprone-invalid-enum-default-initialization)
        sut_type sut{fail(std::io_errc{})};

        // Act
        sut = std::move(other);

        // Assert
        SECTION("Result has error type") {
          REQUIRE(sut.has_error());
        }
        SECTION("Result assigns error") {
          REQUIRE(output == sut);
        }
      }
    }
  }

  SECTION("E is trivial move-constructible and move-assignable") {
    // Arrange
    using sut_type = result<void, int>;

    SECTION("Result is move-assignable") {
      // Act
      constexpr auto property_holds = std::is_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Result is trivial move-assignable") {
      // Act
      constexpr auto property_holds =
        std::is_trivially_move_assignable<sut_type>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
}

TEST_CASE("result<void,E>::operator=(const result<void,E2>&)", "[assign]") {
  SECTION("E is not nothrow copy constructible") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using copy_type = result<void,const char*>;
      using sut_type = result<void,std::string>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is not copy-assignable") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using copy_type = result<void,const char*>;
      using sut_type = result<void,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is nothrow copy-constructible") {
    // Arrange
    using copy_type = result<void,std::io_errc>;
    using sut_type = result<void,std::error_code>;

    SECTION("Result is copy-assignable") {
      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Other contains value") {
      SECTION("Sut contains value") {
        const copy_type other{};
        sut_type sut{};

        // Act
        sut = other;

        // Assert
        SECTION("Result contains value") {
          REQUIRE(sut.has_value());
        }
      }
      SECTION("Sut contains error") {
        const copy_type other{};
        sut_type sut{
          fail(std::io_errc::stream)
        };

        // Act
        sut = other;

        // Assert
        SECTION("Result contains value") {
          REQUIRE(sut.has_value());
        }
      }
    }
    SECTION("Other contains error") {
      SECTION("Sut contains value") {
        const auto output = fail(std::io_errc::stream);
        const copy_type other{output};
        sut_type sut{};

        // Act
        sut = other;

        // Assert
        SECTION("Result changes to error type") {
          REQUIRE(sut.has_error());
        }
        SECTION("Result constructs error") {
          REQUIRE(output == sut);
        }
      }
      SECTION("Sut contains error") {
        const auto output = fail(std::io_errc::stream);
        const copy_type other{output};
        // Start from an error other than 'stream' so the assignment is
        // observable. 'io_errc' is a scoped enum, so zero is a valid value
        // even though no enumerator names it.
        // NOLINTNEXTLINE(bugprone-invalid-enum-default-initialization)
        sut_type sut{fail(std::io_errc{})};

        // Act
        sut = other;

        // Assert
        SECTION("Result has error type") {
          REQUIRE(sut.has_error());
        }
        SECTION("Result assigns error") {
          REQUIRE(output == sut);
        }
      }
    }
  }
}

TEST_CASE("result<void,E>::operator=(result<void,E2>&&)", "[assign]") {
  SECTION("E is not nothrow move constructible") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = result<void,const char*>;
      using sut_type = result<void,std::string>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is not move-assignable") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = result<void,const char*>;
      using sut_type = result<void,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("E is nothrow move-constructible") {
    // Arrange
    using copy_type = result<void,std::io_errc>;
    using sut_type = result<void,move_only<std::error_code>>;

    SECTION("Result is move-assignable") {
      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Copies the state of 'other'") {
      SECTION("Original contains value") {
        const auto output = fail(std::io_errc::stream);
        copy_type other{output};
        sut_type sut{};

        // Act
        sut = std::move(other);

        // Assert
        SECTION("Result changes to error type") {
          REQUIRE(sut.has_error());
        }
        SECTION("Result constructs error") {
          REQUIRE(output == sut);
        }
      }
      SECTION("Original contains error") {
        const auto output = fail(std::io_errc::stream);
        sut_type other{output};
        // Start from an error other than 'stream' so the assignment is
        // observable. 'io_errc' is a scoped enum, so zero is a valid value
        // even though no enumerator names it.
        // NOLINTNEXTLINE(bugprone-invalid-enum-default-initialization)
        sut_type sut{fail(std::io_errc{})};

        // Act
        sut = std::move(other);

        // Assert
        SECTION("Result has error type") {
          REQUIRE(sut.has_error());
        }
        SECTION("Result assigns error") {
          REQUIRE(output == sut);
        }
      }
    }
  }
}

TEST_CASE("result<void,E>::operator=(const failure<E2>&)", "[assign]") {
  SECTION("E cant be constructed or assigned from E2") {
    SECTION("Result cannot be assigned from E2") {
      // Arrange
      using copy_type = failure<not_copy_or_moveable>;
      using sut_type  = result<void,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not nothrow copy constructible from E2") {
    SECTION("Result is not copy-assignable") {
      // Arrange
      using copy_type = failure<const char*>;
      using sut_type = result<void,throwing<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION(
    "E is not nothrow copy constructible from E2, but is nothrow-move "
    "constructible"
  ) {
    SECTION("Result is copy-assignable") {
      // Arrange
      using copy_type = failure<const char*>;
      using sut_type = result<void,std::string>;

      // This works because it generates an intermediate 'result<void, E>' in
      // between. The result is then move-assigned instead

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("E can be constructed and assigned from E2") {
    // Arrange
    using copy_type = failure<long>;
    using sut_type = result<void, int>;

    SECTION("Sut contains value") {
      const auto original = copy_type{42};
      sut_type sut{};

      // Act
      sut = original;

      // Assert
      SECTION("Contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Error is changed to input") {
        REQUIRE(sut == original);
      }
    }
    SECTION("Sut contains error") {
      const auto original = copy_type{42};
      sut_type sut{fail(0)};

      // Act
      sut = original;

      // Assert
      SECTION("Contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Error is changed to input") {
        REQUIRE(sut == original);
      }
    }
    SECTION("Result can be assigned") {
      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,const copy_type&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
}

TEST_CASE("result<void,E>::operator=(failure<E2>&&)", "[assign]") {
  SECTION("E cant be constructed or assigned from E2") {
    SECTION("Result cannot be assigned from E2") {
      // Arrange
      using copy_type = failure<not_copy_or_moveable>;
      using sut_type  = result<void,not_copy_or_moveable>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }
  SECTION("E is not nothrow move constructible from E2") {
    SECTION("Result is not move-assignable") {
      // Arrange
      using copy_type = failure<const char*>;
      using sut_type = result<void,throwing<std::string>>;

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION(
    "E is not nothrow move constructible from E2, but is nothrow-move "
    "constructible"
  ) {
    SECTION("Result is move-assignable") {
      // Arrange
      using copy_type = failure<const char*>;
      using sut_type = result<void,std::string>;

      // This works because it generates an intermediate 'result<void, E>' in
      // between. The result is then move-assigned instead

      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("E can be constructed and assigned from E2") {
    // Arrange
    using copy_type = failure<std::string>;
    using sut_type = result<void, move_only<std::string>>;

    SECTION("Sut contains value") {
      auto other = copy_type{"42"};
      const auto original = other;
      sut_type sut{};

      // Act
      sut = std::move(other);

      // Assert
      SECTION("Contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Error is changed to input") {
        REQUIRE(sut == original);
      }
    }

    SECTION("Sut contains error") {
      auto other = copy_type{"42"};
      const auto original = other;
      sut_type sut{fail("")};

      // Act
      sut = std::move(other);

      // Assert
      SECTION("Contains an error") {
        REQUIRE(sut.has_error());
      }
      SECTION("Error is changed to input") {
        REQUIRE(sut == original);
      }
    }

    SECTION("Result can be assigned") {
      // Act
      constexpr auto property_holds =
        std::is_assignable<sut_type,copy_type&&>::value;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("result<void,E>::operator bool()", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Returns true") {
      // Arrange
      auto sut = result<void, int>{};

      // Act
      const auto actual = static_cast<bool>(sut);

      // Assert
      REQUIRE(actual);
    }
  }
  SECTION("result does not contain a value") {
    SECTION("Returns false") {
      // Arrange
      auto sut = result<void, int>{fail(42)};

      // Act
      const auto actual = static_cast<bool>(sut);

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
}

TEST_CASE("result<void,E>::has_value()", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Returns true") {
      // Arrange
      auto sut = result<void, int>{};

      // Act
      const auto actual = sut.has_value();

      // Assert
      REQUIRE(actual);
    }
  }
  SECTION("result does not contain a value") {
    SECTION("Returns false") {
      // Arrange
      auto sut = result<void, int>{fail(42)};

      // Act
      const auto actual = sut.has_value();

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
}

TEST_CASE("result<void,E>::has_error()", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Returns false") {
      // Arrange
      auto sut = result<void, int>{};

      // Act
      const auto actual = sut.has_error();

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
  SECTION("result does not contain a value") {
    SECTION("Returns true") {
      // Arrange
      auto sut = result<void, int>{fail(42)};

      // Act
      const auto actual = sut.has_error();

      // Assert
      REQUIRE(actual);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<void,E>::value() const &", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Does nothing") {
      // Arrange
      auto sut = result<void, int>{};

      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        sut.value();
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
  }
  SECTION("result contains an error") {
    SECTION("Throws bad_result_access") {
      // Arrange
      auto sut = result<void, int>{fail(42)};

      // Arrange
      auto threw_expected = false;

      // Act
      try {
        sut.value();
      } catch (const bad_result_access<int>&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
}

TEST_CASE("result<void,E>::value() &&", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Does nothing") {
      // Arrange
      auto sut = result<void, move_only<std::string>>{};

      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        std::move(sut).value();
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
  }
  SECTION("result contains an error") {
    SECTION("Throws bad_result_access") {
      // Arrange
      auto sut = result<void, move_only<std::string>>{fail("Hello world")};

      // Arrange
      auto threw_expected = false;

      // Act
      try {
        std::move(sut).value();
      } catch (const bad_result_access<move_only<std::string>>&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<void,E>::error() const &", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Returns default-constructed error") {
      // Arrange
      auto sut = result<void, int>{};

      // Act
      const auto output = sut.error();

      // Assert
      REQUIRE(output == int{});
    }
  }
  SECTION("result contains an error") {
    SECTION("Returns a copy of the exception") {
      // Arrange
      const auto value = 42;
      auto sut = result<void, int>{fail(value)};

      // Act
      const auto output = sut.error();

      // Assert
      REQUIRE(output == value);
    }
  }
}

TEST_CASE("result<void,E>::error() &&", "[observers]") {
  SECTION("result contains a value") {
    SECTION("Returns default-constructed error") {
      // Arrange
      auto sut = result<void, move_only<std::error_code>>{};

      // Act
      const auto output = std::move(sut).error();

      // Assert
      REQUIRE(output == std::error_code{});
    }
  }
  SECTION("result contains an error") {
    SECTION("Returns a copy of the exception") {
      // Arrange
      const auto value = std::io_errc::stream;
      auto sut = result<void, move_only<std::error_code>>{fail(value)};

      // Act
      const auto output = std::move(sut).error();

      // Assert
      REQUIRE(output == value);
    }
  }
}

TEST_CASE("result<void,E>::expect(String&&) const &", "[observers]") {
  SECTION("result contains value") {
    // Arrange
    const auto sut = result<void,int>{};

    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        sut.expect("test");
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
  }
  SECTION("result contains error") {
    SECTION("throws exception") {
      // Arrange
      const auto sut = result<void,int>{fail(42)};

      // Arrange
      auto threw_expected = false;

      // Act
      try {
        sut.expect("test");
      } catch (const bad_result_access<int>&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
}

TEST_CASE("result<void,E>::expect(String&&) &&", "[observers]") {
  SECTION("result contains value") {
    // Arrange
    auto sut = result<void,move_only<std::string>>{};

    SECTION("Does not throw exception") {
      // Arrange
      auto exception = std::exception_ptr{};

      // Act
      try {
        std::move(sut).expect("test");
      } catch (...) {
        exception = std::current_exception();
      }

      // Assert
      REQUIRE(exception == nullptr);
    }
  }
  SECTION("result contains error") {
    SECTION("throws exception") {
      // Arrange
      auto sut = result<void,move_only<std::string>>{fail("hello world")};

      // Arrange
      auto threw_expected = false;

      // Act
      try {
        std::move(sut).expect("test");
      } catch (const bad_result_access<move_only<std::string>>&) {
        threw_expected = true;
      }

      // Assert
      REQUIRE(threw_expected);
    }
  }
}

//-----------------------------------------------------------------------------
// Monadic Functionalities
//-----------------------------------------------------------------------------

TEST_CASE("result<void,E>::error_or(U&&) const &", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Returns the input") {
      // Arrange
      const auto input = std::error_code{};
      auto sut = result<void, std::error_code>{};

      // Act
      const auto output = sut.error_or(input);

      // Assert
      REQUIRE(output == input);
    }
  }
  SECTION("result contains an error") {
    SECTION("Returns the error") {
      // Arrange
      const auto input = std::error_code{std::io_errc::stream};
      auto sut = result<void, std::error_code>{
        fail(input)
      };

      // Act
      const auto output = sut.error_or(input);

      // Assert
      REQUIRE(output == input);
    }
  }
}

TEST_CASE("result<void,E>::error_or(U&&) &&", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Returns the input") {
      // Arrange
      auto input = std::error_code{};
      const auto copy = input;
      auto sut = result<void, move_only<std::error_code>>{};

      // Act
      const auto output = std::move(sut).error_or(std::move(input));

      // Assert
      REQUIRE(output == copy);
    }
  }
  SECTION("result contains an error") {
    SECTION("Returns the error") {
      // Arrange
      auto input = std::error_code{std::io_errc::stream};
      const auto copy = input;
      auto sut = result<void, move_only<std::error_code>>{
        fail(input)
      };

      // Act
      const auto output = std::move(sut).error_or(std::move(input));

      // Assert
      REQUIRE(output == copy);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("result<void,E>::and_then(U&&) const", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Maps the input") {
      // Arrange
      const auto input = 42;
      auto sut = result<void, std::error_code>{};

      // Act
      const auto output = sut.and_then(input);

      // Assert
      REQUIRE(output == input);
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto input = 42;
      const auto error = fail(std::io_errc::stream);
      auto sut = result<void, std::error_code>{error};

      // Act
      const auto output = sut.and_then(input);

      // Assert
      REQUIRE(output == error);
    }
  }
}

TEST_CASE("result<void,E>::flat_map(Fn&&) const &", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Maps the input") {
      // Arrange
      const auto value = 42;
      const auto sut = result<void,std::error_code>{};

      // Act
      const auto output = sut.flat_map([&]{
        return result<int,std::error_code>{value};
      });

      // Assert
      REQUIRE(output == value);
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail(std::io_errc::stream);
      const auto value = 42;
      const auto sut = result<void,std::error_code>{error};

      // Act
      const auto output = sut.flat_map([&]{
        return result<int,std::error_code>{value};
      });

      // Assert
      REQUIRE(output == error);
    }
  }
}

TEST_CASE("result<void,E>::flat_map(Fn&&) &&", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Maps the input") {
      // Arrange
      const auto value = 42;
      auto sut = result<void,move_only<std::error_code>>{};

      // Act
      const auto output = std::move(sut).flat_map([&]{
        return result<int,std::error_code>{value};
      });

      // Assert
      REQUIRE(output == value);
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail(std::io_errc::stream);
      const auto value = 42;
      auto sut = result<void,move_only<std::error_code>>{error};

      // Act
      const auto output = std::move(sut).flat_map([&]{
        return result<int,std::error_code>{value};
      });

      // Assert
      REQUIRE(output == error);
    }
  }
}

TEST_CASE("result<void,E>::map(Fn&&) const &", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Function returns non-void") {
      SECTION("Maps the input") {
        // Arrange
        const auto value = 42;
        auto sut = result<void,std::io_errc>{};

        // Act
        const auto output = sut.map([&]{
          return value;
        });

        // Assert
        REQUIRE(output == value);
      }
    }
    SECTION("Function returns void") {
      SECTION("Maps input to void") {
        // Arrange
        auto sut = result<void,std::io_errc>{};

        // Act
        const auto output = sut.map([]{});

        // Assert
        SECTION("Result has value") {
          REQUIRE(output.has_value());
        }
        SECTION("Result is result<void,E>") {
          // Act
          constexpr auto property_holds =
            std::is_same<decltype(output),const result<void,std::io_errc>>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }
      }
    }
  }
  SECTION("result contains an error") {
    SECTION("Function returns non-void") {
      SECTION("Maps the error") {
        // Arrange
        const auto value = 42;
        const auto error = fail(std::io_errc::stream);
        auto sut = result<void,std::io_errc>{error};

        // Act
        const auto output = sut.map([&]{
          return value;
        });

        // Assert
        REQUIRE(output == error);
      }
    }
    SECTION("Function returns void") {
      SECTION("Maps input to void") {
        // Arrange
        const auto error = fail(std::io_errc::stream);
        auto sut = result<void,std::io_errc>{error};

        // Act
        const auto output = sut.map([]{});

        // Assert
        SECTION("Result contains the error") {
          REQUIRE(output == error);
        }
        SECTION("Result is result<void,E>") {
          // Act
          constexpr auto property_holds =
            std::is_same<decltype(output),const result<void,std::io_errc>>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }
      }
    }
  }
}

TEST_CASE("result<void,E>::map(Fn&&) &&", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Function returns non-void") {
      SECTION("Maps the input") {
        // Arrange
        const auto value = 42;
        auto sut = result<void,move_only<std::error_code>>{};

        // Act
        const auto output = std::move(sut).map([&]{
          return value;
        });

        // Assert
        REQUIRE(output == value);
      }
    }
    SECTION("Function returns void") {
      SECTION("Maps input to void") {
        // Arrange
        auto sut = result<void,move_only<std::error_code>>{};

        // Act
        const auto output = std::move(sut).map([]{});

        // Assert
        SECTION("Result has value") {
          REQUIRE(output.has_value());
        }
        SECTION("Result is result<void,E>") {
          // Act
          constexpr auto property_holds =
            std::is_same<decltype(output),const result<void,move_only<std::error_code>>>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }
      }
    }
  }
  SECTION("result contains an error") {
    SECTION("Function returns non-void") {
      SECTION("Maps the error") {
        // Arrange
        const auto value = 42;
        const auto error = fail(std::io_errc::stream);
        auto sut = result<void,move_only<std::error_code>>{error};

        // Act
        const auto output = std::move(sut).map([&]{
          return value;
        });

        // Assert
        REQUIRE(output == error);
      }
    }
    SECTION("Function returns void") {
      SECTION("Maps input to void") {
        // Arrange
        const auto error = fail(std::io_errc::stream);
        auto sut = result<void,move_only<std::error_code>>{error};

        // Act
        const auto output = std::move(sut).map([]{});

        // Assert
        SECTION("Result contains the error") {
          REQUIRE(output == error);
        }
        SECTION("Result is result<void,E>") {
          // Act
          constexpr auto property_holds =
            std::is_same<decltype(output),const result<void,move_only<std::error_code>>>::value;

          // Assert
          STATIC_REQUIRE(property_holds);
        }
      }
    }
  }
}

TEST_CASE("result<void,E>::map_error(Fn&&) const &", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Maps the input") {
      // Arrange
      auto sut = result<void,std::io_errc>{};

      // Act
      const auto output = sut.map_error([](std::io_errc e){
        return std::error_code{e};
      });

      // Assert
      REQUIRE(output.has_value());
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail(std::io_errc::stream);
      auto sut = result<void,std::io_errc>{error};

      // Act
      const auto output = sut.map_error([](std::io_errc e){
        return std::error_code{e};
      });

      // Assert
      REQUIRE(output == error);
    }
  }
}

TEST_CASE("result<void,E>::map_error(Fn&&) &&", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Maps the input") {
      // Arrange
      auto sut = result<void,move_only<std::error_code>>{};

      // Act
      const auto output = std::move(sut).map_error([](move_only<std::error_code> e){
        return std::error_code{e};
      });

      // Assert
      REQUIRE(output.has_value());
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail(std::io_errc::stream);
      auto sut = result<void,move_only<std::error_code>>{error};

      // Act
      const auto output = std::move(sut).map_error([](move_only<std::error_code> e){
        return std::error_code{e};
      });

      // Assert
      REQUIRE(output == error);
    }
  }
}

TEST_CASE("result<void,E>::flat_map_error(Fn&&) const &", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Default-initializes T") {
      // Arrange
      const auto sut = result<void,int>{};

      // Act
      const auto output = sut.flat_map_error([&](int x){
        return result<long, int>{x};
      });

      // Assert
      REQUIRE(output == long{});
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail(42);
      const auto sut = result<void, long>{error};

      // Act
      const auto output = sut.flat_map_error([&](long x){
        return result<int, short>{x};
      });

      // Assert
      REQUIRE(output == error.error());
    }
  }
}

TEST_CASE("result<void,E>::flat_map_error(Fn&&) &&", "[monadic]") {
  SECTION("result contains a value") {
    SECTION("Default-initializes T") {
      // Arrange
      auto sut = result<void,move_only<std::string>>{};

      // Act
      const auto output = std::move(sut).flat_map_error([](move_only<std::string>&& x){
        return result<int, std::string>{in_place_error, std::move(x)};
      });

      // Assert
      REQUIRE(output == int{});
    }
  }
  SECTION("result contains an error") {
    SECTION("Maps the error") {
      // Arrange
      const auto error = fail("Hello world");
      auto sut = result<void,move_only<std::string>>{error};

      // Act
      const auto output = std::move(sut).flat_map_error([](move_only<std::string>&& x){
        return result<int, std::string>{in_place_error, std::move(x)};
      });

      // Assert
      REQUIRE(output == error);
    }
  }
}

//=============================================================================
// non-member functions : class : result<void, E>
//=============================================================================

//-----------------------------------------------------------------------------
// Comparisons
//-----------------------------------------------------------------------------

TEST_CASE("operator==(const result<T1,E1>&, const result<T2,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    SECTION("Left equal to right") {
      // Arrange
      const auto lhs = result<int,int>{42};
      const auto rhs = result<long,long>{42};

      SECTION("returns true") {
        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Left not equal to right") {
      // Arrange
      const auto lhs = result<int,int>{42};
      const auto rhs = result<long,long>{0};

      SECTION("returns false") {
        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left equal to right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(42)
      };
      const auto rhs = result<long,long>{
        fail(42)
      };
      SECTION("returns true") {
        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Left not equal to right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(42)
      };
      const auto rhs = result<long,long>{
        fail(0)
      };
      SECTION("returns false") {
        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<int,int>{};
    const auto rhs = result<long,long>{
      fail(42)
    };
    SECTION("returns false") {
      // Act
      const auto result = lhs == rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<int,int>{
      fail(42)
    };
    const auto rhs = result<long,long>{};
    SECTION("returns false") {
      // Act
      const auto result = lhs == rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("operator!=(const result<T1,E1>&, const result<T2,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    SECTION("Left equal to right") {
      // Arrange
      const auto lhs = result<int,int>{42};
      const auto rhs = result<long,long>{42};

      SECTION("returns false") {
        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Left not equal to right") {
      // Arrange
      const auto lhs = result<int,int>{42};
      const auto rhs = result<long,long>{0};

      SECTION("returns true") {
        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left equal to right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(42)
      };
      const auto rhs = result<long,long>{
        fail(42)
      };
      SECTION("returns false") {
        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Left not equal to right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(42)
      };
      const auto rhs = result<long,long>{
        fail(0)
      };
      SECTION("returns true") {
        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<int,int>{};
    const auto rhs = result<long,long>{
      fail(42)
    };
    SECTION("returns true") {
      // Act
      const auto result = lhs != rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<int,int>{
      fail(42)
    };
    const auto rhs = result<long,long>{};
    SECTION("returns true") {
      // Act
      const auto result = lhs != rhs;

      // Assert
      REQUIRE(result);
    }
  }
}

TEST_CASE("operator>=(const result<T1,E1>&, const result<T2,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<int,int>{100};
      const auto rhs = result<long,long>{42};

      SECTION("returns true") {
        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<int,int>{0};
      const auto rhs = result<long,long>{42};

      SECTION("returns false") {
        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(100)
      };
      const auto rhs = result<long,long>{
        fail(42)
      };

      SECTION("returns true") {
        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(0)
      };
      const auto rhs = result<long,long>{
        fail(42)
      };

      SECTION("returns false") {
        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<int,int>{0};
    const auto rhs = result<long,long>{
      fail(42)
    };

    SECTION("returns true") {
      // Act
      const auto result = lhs >= rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<int,int>{
      fail(42)
    };
    const auto rhs = result<long,long>{0};

    SECTION("returns false") {
      // Act
      const auto result = lhs >= rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("operator<=(const result<T1,E1>&, const result<T2,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<int,int>{100};
      const auto rhs = result<long,long>{42};

      SECTION("returns false") {
        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<int,int>{0};
      const auto rhs = result<long,long>{42};

      SECTION("returns true") {
        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(100)
      };
      const auto rhs = result<long,long>{
        fail(42)
      };

      SECTION("returns false") {
        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(0)
      };
      const auto rhs = result<long,long>{
        fail(42)
      };

      SECTION("returns true") {
        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<int,int>{0};
    const auto rhs = result<long,long>{
      fail(42)
    };

    SECTION("returns false") {
      // Act
      const auto result = lhs <= rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<int,int>{
      fail(42)
    };
    const auto rhs = result<long,long>{0};

    SECTION("returns true") {
      // Act
      const auto result = lhs <= rhs;

      // Assert
      REQUIRE(result);
    }
  }
}

TEST_CASE("operator>(const result<T1,E1>&, const result<T2,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<int,int>{100};
      const auto rhs = result<long,long>{42};

      SECTION("returns true") {
        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<int,int>{0};
      const auto rhs = result<long,long>{42};

      SECTION("returns false") {
        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(100)
      };
      const auto rhs = result<long,long>{
        fail(42)
      };

      SECTION("returns true") {
        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(0)
      };
      const auto rhs = result<long,long>{
        fail(42)
      };

      SECTION("returns false") {
        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<int,int>{0};
    const auto rhs = result<long,long>{
      fail(42)
    };

    SECTION("returns true") {
      // Act
      const auto result = lhs > rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<int,int>{
      fail(42)
    };
    const auto rhs = result<long,long>{0};

    SECTION("returns false") {
      // Act
      const auto result = lhs > rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("operator<(const result<T1,E1>&, const result<T2,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<int,int>{100};
      const auto rhs = result<long,long>{42};

      SECTION("returns false") {
        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<int,int>{0};
      const auto rhs = result<long,long>{42};

      SECTION("returns true") {
        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(100)
      };
      const auto rhs = result<long,long>{
        fail(42)
      };

      SECTION("returns false") {
        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<int,int>{
        fail(0)
      };
      const auto rhs = result<long,long>{
        fail(42)
      };

      SECTION("returns true") {
        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<int,int>{0};
    const auto rhs = result<long,long>{
      fail(42)
    };

    SECTION("returns false") {
      // Act
      const auto result = lhs < rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<int,int>{
      fail(42)
    };
    const auto rhs = result<long,long>{0};

    SECTION("returns true") {
      // Act
      const auto result = lhs < rhs;

      // Assert
      REQUIRE(result);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("operator==(const result<void,E1>&, const result<void,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{};

    SECTION("returns true") {
      // Act
      const auto result = lhs == rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left equal to right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(42)
      };
      const auto rhs = result<void,long>{
        fail(42)
      };
      SECTION("returns true") {
        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Left not equal to right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(42)
      };
      const auto rhs = result<void,long>{
        fail(0)
      };
      SECTION("returns false") {
        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{
      fail(42)
    };
    SECTION("returns false") {
      // Act
      const auto result = lhs == rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<void,int>{
      fail(42)
    };
    const auto rhs = result<void,long>{};
    SECTION("returns false") {
      // Act
      const auto result = lhs == rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("operator!=(const result<void,E1>&, const result<void,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{};

    SECTION("returns false") {
      // Act
      const auto result = lhs != rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left equal to right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(42)
      };
      const auto rhs = result<void,long>{
        fail(42)
      };
      SECTION("returns false") {
        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Left not equal to right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(42)
      };
      const auto rhs = result<void,long>{
        fail(0)
      };
      SECTION("returns true") {
        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{
      fail(42)
    };
    SECTION("returns true") {
      // Act
      const auto result = lhs != rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<void,int>{
      fail(42)
    };
    const auto rhs = result<void,long>{};
    SECTION("returns true") {
      // Act
      const auto result = lhs != rhs;

      // Assert
      REQUIRE(result);
    }
  }
}

TEST_CASE("operator>=(const result<void,E1>&, const result<void,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{};

    SECTION("returns true") {
      // Act
      const auto result = lhs >= rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(100)
      };
      const auto rhs = result<void,long>{
        fail(42)
      };
      SECTION("returns true") {
        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(0)
      };
      const auto rhs = result<void,long>{
        fail(42)
      };
      SECTION("returns false") {
        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{
      fail(42)
    };
    SECTION("returns true") {
      // Act
      const auto result = lhs >= rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<void,int>{
      fail(42)
    };
    const auto rhs = result<void,long>{};
    SECTION("returns false") {
      // Act
      const auto result = lhs >= rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("operator<=(const result<void,E1>&, const result<void,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{};

    SECTION("returns true") {
      // Act
      const auto result = lhs <= rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(100)
      };
      const auto rhs = result<void,long>{
        fail(42)
      };
      SECTION("returns false") {
        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(0)
      };
      const auto rhs = result<void,long>{
        fail(42)
      };
      SECTION("returns true") {
        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{
      fail(42)
    };
    SECTION("returns false") {
      // Act
      const auto result = lhs <= rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<void,int>{
      fail(42)
    };
    const auto rhs = result<void,long>{};
    SECTION("returns true") {
      // Act
      const auto result = lhs <= rhs;

      // Assert
      REQUIRE(result);
    }
  }
}

TEST_CASE("operator>(const result<void,E1>&, const result<void,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{};

    SECTION("returns false") {
      // Act
      const auto result = lhs > rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(100)
      };
      const auto rhs = result<void,long>{
        fail(42)
      };
      SECTION("returns true") {
        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(0)
      };
      const auto rhs = result<void,long>{
        fail(42)
      };
      SECTION("returns false") {
        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{
      fail(42)
    };
    SECTION("returns true") {
      // Act
      const auto result = lhs > rhs;

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<void,int>{
      fail(42)
    };
    const auto rhs = result<void,long>{};
    SECTION("returns false") {
      // Act
      const auto result = lhs > rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("operator<(const result<void,E1>&, const result<void,E2>&)", "[compare]") {
  SECTION("Both left and right contain value") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{};

    SECTION("returns false") {
      // Act
      const auto result = lhs < rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Left and right contain errors") {
    SECTION("Left greater than right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(100)
      };
      const auto rhs = result<void,long>{
        fail(42)
      };
      SECTION("returns false") {
        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Left less than right") {
      // Arrange
      const auto lhs = result<void,int>{
        fail(0)
      };
      const auto rhs = result<void,long>{
        fail(42)
      };
      SECTION("returns true") {
        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("Left contains a value, right contains an error") {
    // Arrange
    const auto lhs = result<void,int>{};
    const auto rhs = result<void,long>{
      fail(42)
    };
    SECTION("returns false") {
      // Act
      const auto result = lhs < rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Left contains an error, right contains a value") {
    // Arrange
    const auto lhs = result<void,int>{
      fail(42)
    };
    const auto rhs = result<void,long>{};
    SECTION("returns true") {
      // Act
      const auto result = lhs < rhs;

      // Assert
      REQUIRE(result);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("operator==(const result<T1,E1>&, const U&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto lhs = result<int,std::string>{0};

    SECTION("value compares equal") {
      const auto rhs = 0l;

      SECTION("returns true") {
        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("value compares unequal") {
      const auto rhs = 42l;

      SECTION("returns false") {
        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("result contains an error") {
    // Arrange
    const auto lhs = result<int,std::string>{
      fail("0")
    };
    const auto rhs = 0l;

    SECTION("returns false") {
      // Act
      const auto result = lhs == rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("operator==(const U&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto rhs = result<int,std::string>{0};

    SECTION("value compares equal") {
      const auto lhs = 0l;

      SECTION("returns true") {
        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("value compares unequal") {
      const auto lhs = 42l;

      SECTION("returns false") {
        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("result contains an error") {
    // Arrange
    const auto rhs = result<int,std::string>{
      fail("0")
    };
    const auto lhs = 0l;

    SECTION("returns false") {
      // Act
      const auto result = lhs == rhs;

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("operator!=(const result<T1,E1>&, const U&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto lhs = result<int,std::string>{0};

    SECTION("value compares equal") {
      const auto rhs = 0l;

      SECTION("returns false") {
        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("value compares unequal") {
      const auto rhs = 42l;

      SECTION("returns true") {
        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("result contains an error") {
    // Arrange
    const auto lhs = result<int,std::string>{
      fail("0")
    };
    const auto rhs = 0l;

    SECTION("returns true") {
      // Act
      const auto result = lhs != rhs;

      // Assert
      REQUIRE(result);
    }
  }
}

TEST_CASE("operator!=(const U&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto rhs = result<int,std::string>{0};

    SECTION("value compares equal") {
      const auto lhs = 0l;

      SECTION("returns false") {
        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("value compares unequal") {
      const auto lhs = 42l;

      SECTION("returns true") {
        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("result contains an error") {
    // Arrange
    const auto rhs = result<int,std::string>{
      fail("0")
    };
    const auto lhs = 0l;

    SECTION("returns true") {
      // Act
      const auto result = lhs != rhs;

      // Assert
      REQUIRE(result);
    }
  }
}

TEST_CASE("operator>=(const result<T1,E1>&, const U&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto lhs = result<int,std::string>{5};
    SECTION("value is greater or equal to result") {
      SECTION("returns true") {
        const auto rhs = 0l;

        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("value is not greater or equal to result") {
      SECTION("returns false") {
        const auto rhs = 9l;

        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Result contains an error") {
    SECTION("returns true") {
      // Arrange
      const auto lhs = result<int,std::string>{fail("hello world")};
      const auto rhs = 0l;

      // Act
      const auto actual = lhs >= rhs;

      // Assert
      REQUIRE(actual);
    }
  }
}

TEST_CASE("operator>=(const U&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto rhs = result<int,std::string>{5};
    SECTION("value is greater or equal to result") {
      SECTION("returns true") {
        const auto lhs = 0l;

        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("value is not greater or equal to result") {
      SECTION("returns false") {
        const auto lhs = 9l;

        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("Result contains an error") {
    SECTION("returns true") {
      // Arrange
      const auto rhs = result<int,std::string>{fail("hello world")};
      const auto lhs = 0l;

      // Act
      const auto actual = lhs >= rhs;

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
}

TEST_CASE("operator<=(const result<T1,E1>&, const U&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto lhs = result<int,std::string>{5};
    SECTION("value is less or equal to result") {
      SECTION("returns true") {
        const auto rhs = 9l;

        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("value is not less or equal to result") {
      SECTION("returns false") {
        const auto rhs = 0l;

        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Result contains an error") {
    SECTION("returns false") {
      // Arrange
      const auto lhs = result<int,std::string>{fail("hello world")};
      const auto rhs = 9l;

      // Act
      const auto actual = lhs <= rhs;

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
}

TEST_CASE("operator<=(const U&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto rhs = result<int,std::string>{5};
    SECTION("value is less or equal to result") {
      SECTION("returns true") {
        const auto lhs = 0l;

        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("value is not less or equal to result") {
      SECTION("returns false") {
        const auto lhs = 9l;

        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Result contains an error") {
    SECTION("returns true") {
      // Arrange
      const auto rhs = result<int,std::string>{fail("hello world")};
      const auto lhs = 9l;

      // Act
      const auto actual = lhs <= rhs;

      // Assert
      REQUIRE(actual);
    }
  }
}

TEST_CASE("operator>(const result<T1,E1>&, const U&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto lhs = result<int,std::string>{5};
    SECTION("value is greater or equal to result") {
      SECTION("returns true") {
        const auto rhs = 0l;

        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("value is not greater or equal to result") {
      SECTION("returns false") {
        const auto rhs = 9l;

        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Result contains an error") {
    SECTION("returns false") {
      // Arrange
      const auto lhs = result<int,std::string>{fail("hello world")};
      const auto rhs = 0l;

      // Act
      const auto actual = lhs > rhs;

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
}

TEST_CASE("operator>(const U&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto rhs = result<int,std::string>{5};
    SECTION("value is greater or equal to result") {
      SECTION("returns false") {
        const auto lhs = 0l;

        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("value is not greater or equal to result") {
      SECTION("returns false") {
        const auto lhs = 9l;

        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
  SECTION("Result contains an error") {
    SECTION("returns true") {
      // Arrange
      const auto rhs = result<int,std::string>{fail("hello world")};
      const auto lhs = 0l;

      // Act
      const auto actual = lhs > rhs;

      // Assert
      REQUIRE(actual);
    }
  }
}

TEST_CASE("operator<(const result<T1,E1>&, const U&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto lhs = result<int,std::string>{5};
    SECTION("value is less or equal to result") {
      SECTION("returns true") {
        const auto rhs = 9l;

        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("value is not less or equal to result") {
      SECTION("returns false") {
        const auto rhs = 0l;

        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Result contains an error") {
    SECTION("returns false") {
      // Arrange
      const auto lhs = result<int,std::string>{fail("hello world")};
      const auto rhs = 9l;

      // Act
      const auto actual = lhs < rhs;

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
}

TEST_CASE("operator<(const U&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains a value") {
    // Arrange
    const auto rhs = result<int,std::string>{5};
    SECTION("value is less or equal to result") {
      SECTION("returns true") {
        const auto lhs = 0l;

        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("value is not less or equal to result") {
      SECTION("returns false") {
        const auto lhs = 9l;

        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
  SECTION("Result contains an error") {
    SECTION("returns false") {
      // Arrange
      const auto rhs = result<int,std::string>{fail("hello world")};
      const auto lhs = 9l;

      // Act
      const auto actual = lhs < rhs;

      // Assert
      REQUIRE(actual);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("operator==(const result<T1,E1>&, const failure<E>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns false") {
      // Arrange
      const auto lhs = result<int,std::string>{0};
      const auto rhs = fail("hello world");

      // Act
      const auto actual = lhs == rhs;

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto lhs = result<int,std::string>{
      fail("0")
    };
    SECTION("failure compares equal") {
      SECTION("Returns true") {
        const auto rhs = fail("0");

        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("failure compares unequal") {
      SECTION("Returns false") {
        const auto rhs = fail("hello");

        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

TEST_CASE("operator==(const failure<E>&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns false") {
      // Arrange
      const auto rhs = result<int,std::string>{0};
      const auto lhs = fail("hello world");

      // Act
      const auto actual = lhs == rhs;

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto rhs = result<int,std::string>{
      fail("0")
    };
    SECTION("failure compares equal") {
      SECTION("Returns true") {
        const auto lhs = fail("0");

        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("failure compares unequal") {
      SECTION("Returns false") {
        const auto lhs = fail("hello");

        // Act
        const auto result = lhs == rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

TEST_CASE("operator!=(const result<T1,E1>&, const failure<E>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = result<int,std::string>{0};
      const auto rhs = fail("hello world");

      // Act
      const auto actual = lhs != rhs;

      // Assert
      REQUIRE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto lhs = result<int,std::string>{
      fail("0")
    };
    SECTION("failure compares equal") {
      SECTION("Returns false") {
        const auto rhs = fail("0");

        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("failure compares unequal") {
      SECTION("Returns true") {
        const auto rhs = fail("hello");

        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
}

TEST_CASE("operator!=(const failure<E>&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns true") {
      // Arrange
      const auto rhs = result<int,std::string>{0};
      const auto lhs = fail("hello world");

      // Act
      const auto actual = lhs != rhs;

      // Assert
      REQUIRE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto rhs = result<int,std::string>{
      fail("0")
    };
    SECTION("failure compares equal") {
      SECTION("Returns false") {
        const auto lhs = fail("0");

        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
    SECTION("failure compares unequal") {
      SECTION("Returns true") {
        const auto lhs = fail("hello");

        // Act
        const auto result = lhs != rhs;

        // Assert
        REQUIRE(result);
      }
    }
  }
}

TEST_CASE("operator>=(const result<T1,E1>&, const failure<E>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns false") {
      // Arrange
      const auto lhs = result<int,std::string>{0};
      const auto rhs = fail("hello world");

      // Act
      const auto actual = lhs >= rhs;

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto lhs = result<int,std::string>{
      fail("5")
    };
    SECTION("result is greater or equal to failure") {
      SECTION("Returns true") {
        const auto rhs = fail("0");

        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("result is not greater or equal to failure") {
      SECTION("Returns false") {
        const auto rhs = fail("9");

        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

TEST_CASE("operator>=(const failure<E>&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns false") {
      // Arrange
      const auto lhs = fail("hello world");
      const auto rhs = result<int,std::string>{0};

      // Act
      const auto actual = lhs >= rhs;

      // Assert
      REQUIRE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto rhs = result<int,std::string>{
      fail("3")
    };
    SECTION("failure is greater or equal to result") {
      SECTION("Returns true") {
        const auto lhs = fail("5");

        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("failure is not greater or equal to result") {
      SECTION("Returns false") {
        const auto lhs = fail("0");

        // Act
        const auto result = lhs >= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

TEST_CASE("operator<=(const result<T1,E1>&, const failure<E>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = result<int,std::string>{0};
      const auto rhs = fail("hello world");

      // Act
      const auto actual = lhs <= rhs;

      // Assert
      REQUIRE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto lhs = result<int,std::string>{
      fail("5")
    };
    SECTION("result is less or equal to failure") {
      SECTION("Returns true") {
        const auto rhs = fail("9");

        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("result is not less or equal to failure") {
      SECTION("Returns false") {
        const auto rhs = fail("0");

        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

TEST_CASE("operator<=(const failure<E>&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = fail("hello world");
      const auto rhs = result<int,std::string>{0};

      // Act
      const auto actual = lhs <= rhs;

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto rhs = result<int,std::string>{
      fail("5")
    };
    SECTION("result is less or equal to failure") {
      SECTION("Returns true") {
        const auto lhs = fail("0");

        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("result is not less or equal to failure") {
      SECTION("Returns false") {
        const auto lhs = fail("9");

        // Act
        const auto result = lhs <= rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

TEST_CASE("operator>(const result<T1,E1>&, const failure<E>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns false") {
      // Arrange
      const auto lhs = result<int,std::string>{0};
      const auto rhs = fail("hello world");

      // Act
      const auto actual = lhs > rhs;

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto lhs = result<int,std::string>{
      fail("5")
    };
    SECTION("result is greater or equal to failure") {
      SECTION("Returns true") {
        const auto rhs = fail("0");

        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("result is not greater or equal to failure") {
      SECTION("Returns false") {
        const auto rhs = fail("9");

        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

TEST_CASE("operator>(const failure<E>&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns false") {
      // Arrange
      const auto lhs = fail("hello world");
      const auto rhs = result<int,std::string>{0};

      // Act
      const auto actual = lhs > rhs;

      // Assert
      REQUIRE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto rhs = result<int,std::string>{
      fail("3")
    };
    SECTION("failure is greater or equal to result") {
      SECTION("Returns true") {
        const auto lhs = fail("5");

        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("failure is not greater or equal to result") {
      SECTION("Returns false") {
        const auto lhs = fail("0");

        // Act
        const auto result = lhs > rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

TEST_CASE("operator<(const result<T1,E1>&, const failure<E>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = result<int,std::string>{0};
      const auto rhs = fail("hello world");

      // Act
      const auto actual = lhs < rhs;

      // Assert
      REQUIRE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto lhs = result<int,std::string>{
      fail("5")
    };
    SECTION("result is less or equal to failure") {
      SECTION("Returns true") {
        const auto rhs = fail("9");

        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("result is not less or equal to failure") {
      SECTION("Returns false") {
        const auto rhs = fail("0");

        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

TEST_CASE("operator<(const failure<E>&, const result<T1,E1>&)", "[compare]") {
  SECTION("result contains value") {
    SECTION("Returns true") {
      // Arrange
      const auto lhs = fail("hello world");
      const auto rhs = result<int,std::string>{0};

      // Act
      const auto actual = lhs < rhs;

      // Assert
      REQUIRE_FALSE(actual);
    }
  }
  SECTION("result contains error") {
    // Arrange
    const auto rhs = result<int,std::string>{
      fail("5")
    };
    SECTION("result is less or equal to failure") {
      SECTION("Returns true") {
        const auto lhs = fail("0");

        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("result is not less or equal to failure") {
      SECTION("Returns false") {
        const auto lhs = fail("9");

        // Act
        const auto result = lhs < rhs;

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

TEST_CASE("swap(result<T,E>&, result<T,E>&)", "[utility]") {
  SECTION("lhs and rhs contain a value") {
    // Arrange
    auto lhs_sut = result<int, int>{42};
    auto rhs_sut = result<int, int>{100};
    const auto lhs_old = lhs_sut;
    const auto rhs_old = rhs_sut;

    // Act
    swap(lhs_sut, rhs_sut);

    // Assert
    SECTION("lhs contains value") {
      REQUIRE(lhs_sut.has_value());
    }
    SECTION("rhs contains value") {
      REQUIRE(rhs_sut.has_value());
    }
    SECTION("lhs contains rhs's old value") {
      REQUIRE(lhs_sut == rhs_old);
    }
    SECTION("rhs contains lhs's old value") {
      REQUIRE(rhs_sut == lhs_old);
    }
  }
  SECTION("lhs and rhs contain an error") {
    // Arrange
    auto lhs_sut = result<int, int>{fail(42)};
    auto rhs_sut = result<int, int>{fail(100)};
    const auto lhs_old = lhs_sut;
    const auto rhs_old = rhs_sut;

    // Act
    swap(lhs_sut, rhs_sut);

    // Assert
    SECTION("lhs contains error") {
      REQUIRE(lhs_sut.has_error());
    }
    SECTION("rhs contains error") {
      REQUIRE(rhs_sut.has_error());
    }
    SECTION("lhs contains rhs's old value") {
      REQUIRE(lhs_sut == rhs_old);
    }
    SECTION("rhs contains lhs's old value") {
      REQUIRE(rhs_sut == lhs_old);
    }
  }
  SECTION("lhs contains a value, rhs contains an error") {
    // Arrange
    auto lhs_sut = result<int, int>{42};
    auto rhs_sut = result<int, int>{fail(42)};
    const auto lhs_old = lhs_sut;
    const auto rhs_old = rhs_sut;

    // Act
    swap(lhs_sut, rhs_sut);

    // Assert
    SECTION("lhs contains error") {
      REQUIRE(lhs_sut.has_error());
    }
    SECTION("rhs contains value") {
      REQUIRE(rhs_sut.has_value());
    }
    SECTION("lhs contains rhs's old value") {
      REQUIRE(lhs_sut == rhs_old);
    }
    SECTION("rhs contains lhs's old value") {
      REQUIRE(rhs_sut == lhs_old);
    }
  }
  SECTION("lhs contains an error, rhs contains a value") {
    // Arrange
    auto lhs_sut = result<int, int>{fail(42)};
    auto rhs_sut = result<int, int>{};
    const auto lhs_old = lhs_sut;
    const auto rhs_old = rhs_sut;

    // Act
    swap(lhs_sut, rhs_sut);

    // Assert
    SECTION("lhs contains value") {
      REQUIRE(lhs_sut.has_value());
    }
    SECTION("rhs contains error") {
      REQUIRE(rhs_sut.has_error());
    }
    SECTION("lhs contains rhs's old value") {
      REQUIRE(lhs_sut == rhs_old);
    }
    SECTION("rhs contains lhs's old value") {
      REQUIRE(rhs_sut == lhs_old);
    }
  }
}

TEST_CASE("swap(result<void,E>&, result<void,E>&)", "[utility]") {
  SECTION("lhs and rhs contain a value") {
    // Arrange
    auto lhs_sut = result<void, int>{};
    auto rhs_sut = result<void, int>{};
    const auto lhs_old = lhs_sut;
    const auto rhs_old = rhs_sut;

    // Act
    swap(lhs_sut, rhs_sut);

    // Assert
    SECTION("lhs contains value") {
      REQUIRE(lhs_sut.has_value());
    }
    SECTION("rhs contains value") {
      REQUIRE(rhs_sut.has_value());
    }
    SECTION("lhs is unchanged") {
      REQUIRE(lhs_sut == lhs_old);
    }
    SECTION("rhs is unchanged") {
      REQUIRE(rhs_sut == rhs_old);
    }
  }
  SECTION("lhs and rhs contain an error") {
    // Arrange
    auto lhs_sut = result<void, int>{fail(42)};
    auto rhs_sut = result<void, int>{fail(100)};
    const auto lhs_old = lhs_sut;
    const auto rhs_old = rhs_sut;

    // Act
    swap(lhs_sut, rhs_sut);

    // Assert
    SECTION("lhs contains error") {
      REQUIRE(lhs_sut.has_error());
    }
    SECTION("rhs contains error") {
      REQUIRE(rhs_sut.has_error());
    }
    SECTION("lhs contains rhs's old value") {
      REQUIRE(lhs_sut == rhs_old);
    }
    SECTION("rhs contains lhs's old value") {
      REQUIRE(rhs_sut == lhs_old);
    }
  }
  SECTION("lhs contains a value, rhs contains an error") {
    // Arrange
    auto lhs_sut = result<void, int>{};
    auto rhs_sut = result<void, int>{fail(42)};
    const auto lhs_old = lhs_sut;
    const auto rhs_old = rhs_sut;

    // Act
    swap(lhs_sut, rhs_sut);

    // Assert
    SECTION("lhs contains error") {
      REQUIRE(lhs_sut.has_error());
    }
    SECTION("rhs contains value") {
      REQUIRE(rhs_sut.has_value());
    }
    SECTION("lhs contains rhs's old value") {
      REQUIRE(lhs_sut == rhs_old);
    }
    SECTION("rhs contains lhs's old value") {
      REQUIRE(rhs_sut == lhs_old);
    }
  }
  SECTION("lhs contains an error, rhs contains a value") {
    // Arrange
    auto lhs_sut = result<void, int>{fail(42)};
    auto rhs_sut = result<void, int>{};
    const auto lhs_old = lhs_sut;
    const auto rhs_old = rhs_sut;

    // Act
    swap(lhs_sut, rhs_sut);

    // Assert
    SECTION("lhs contains value") {
      REQUIRE(lhs_sut.has_value());
    }
    SECTION("rhs contains error") {
      REQUIRE(rhs_sut.has_error());
    }
    SECTION("lhs contains rhs's old value") {
      REQUIRE(lhs_sut == rhs_old);
    }
    SECTION("rhs contains lhs's old value") {
      REQUIRE(rhs_sut == lhs_old);
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("std::hash<result<T,E>>::operator()", "[utility]") {
  SECTION("Value is active") {
    SECTION("Hashes value") {
      // Arrange
      const auto sut = result<int,int>{42};

      // Act
      const auto output = std::hash<result<int,int>>{}(sut);

      // Assert
      SUCCEED("Hash is " << output);
    }
  }
  SECTION("Error is active") {
    SECTION("Hashes error") {
      // Arrange
      const auto sut = result<int,int>{fail(42)};

      // Act
      const auto output = std::hash<result<int,int>>{}(sut);

      // Assert
      SUCCEED("Hash is " << output);
    }
  }

  SECTION("Hash of T and E are different for the same values for T and E") {
    // Arrange
    const auto value_sut = result<int,int>{42};
    const auto error_sut = result<int,int>{fail(42)};

    // Act
    const auto value_hash = std::hash<result<int,int>>{}(value_sut);
    const auto error_hash = std::hash<result<int,int>>{}(error_sut);

    // Assert
    REQUIRE(value_hash != error_hash);
  }
}

TEST_CASE("std::hash<result<void,E>>::operator()", "[utility]") {
  SECTION("Value is active") {
    SECTION("Produces '0' hash") {
      // Arrange
      const auto sut = result<void,int>{};

      // Act
      const auto output = std::hash<result<void,int>>{}(sut);

      // Assert
      REQUIRE(output == 0u);
    }
  }

  SECTION("Error is active") {
    SECTION("Hashes error") {
      // Arrange
      const auto sut = result<void,int>{fail(42)};

      // Act
      const auto output = std::hash<result<void,int>>{}(sut);

      // Assert
      SUCCEED("Hash is " << output);
    }
  }
}

} // namespace test
} // namespace cpp

#if defined(_MSC_VER)
# pragma warning(pop)
#endif
