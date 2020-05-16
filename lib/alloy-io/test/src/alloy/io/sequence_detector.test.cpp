#include <alloy/io/sequence_detector.hpp>
#include <alloy/core/containers/vector.hpp>

#include <catch2/catch.hpp>

namespace alloy::io::test {

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

TEST_CASE("sequence_test<T>::test(const value_type&)", "[modifiers]") {
  const auto values = core::vector<int>{1,2,3,4};
  auto sut = sequence_detector{values.copy()};

  SECTION("Sequence in unstarted state") {
    SECTION("Testing next value") {
      SECTION("Changes to started state") {
        // arrange / act
        sut.test(values[0]);

        // assert
        REQUIRE(sut.started());
      }

      SECTION("Returns true") {
        // act / assert
        REQUIRE(sut.test(values[0]));
      }
    }

    SECTION("Testing non-next value") {
      SECTION("Does not change to started state") {
        // arrange / act
        sut.test(42);

        // assert
        REQUIRE(!sut.started());
      }

      SECTION("Returns false") {
        // act / assert
        REQUIRE(!sut.test(42));
      }
    }
  }

  SECTION("Sequence in started state") {
    // arrange
    sut.test(values[0]);

    SECTION("Testing next value") {
      // act
      const auto started_before = sut.started();
      const auto result = sut.test(values[1]);

      SECTION("Returns true") {
        // assert
        REQUIRE(result == true);
      }

      SECTION("Does not change the state of 'started'") {
        // assert
        REQUIRE(sut.started() == started_before);
      }
    }

    SECTION("Testing non-next value") {
      // act
      const auto started_before = sut.started();
      const auto result = sut.test(values[0]);

      SECTION("Returns false") {
        // assert
        REQUIRE(result == false);
      }

      SECTION("Does not change the state of 'started'") {
        // assert
        REQUIRE(sut.started() != started_before);
      }
    }
  }

  SECTION("Sequence in completed state") {
    // arrange
    for (const auto& v : values) {
      sut.test(v);
    }

    SECTION("Testing any value") {
      // act
      const auto result = sut.test(1);

      SECTION("Return false") {
        // assert
        REQUIRE(result == false);
      }
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("sequence_test<T>::reset()", "[modifiers]") {
  const auto values = core::vector<int>{1,2,3,4};
  auto sut = sequence_detector{values.copy()};

  SECTION("Sequence is not started") {
    SECTION("Reset does not change started state") {
      // arrange
      const auto before = sut.started();

      // act
      sut.reset();

      // assert
      REQUIRE(before == sut.started());
    }
  }

  SECTION("Sequence is started") {
    SECTION("Reset changes to not started") {
      // arrange
      sut.test(values[0]);
      const auto before = sut.started();

      // act
      sut.reset();

      // assert
      REQUIRE(before != sut.started());
      REQUIRE(!sut.started());
    }
  }

  SECTION("Sequence is completed") {
    // arrange
    for (const auto& v : values) {
      sut.test(v);
    }

    SECTION("Reset changes to not started") {
      // arrange
      const auto before = sut.started();

      // act
      sut.reset();

      // assert
      REQUIRE(before != sut.started());
      REQUIRE(!sut.started());
    }

    SECTION("Reset sets completed state back") {
      // arrange
      const auto before = sut.completed();

      // act
      sut.reset();

      // assert
      REQUIRE(before != sut.completed());
      REQUIRE(!sut.completed());
    }
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("sequence_test<T>::started()", "[observers]") {
  // values
  const auto values = core::vector<int>{1,2,3,4};
  auto sut = sequence_detector{values.copy()};

  SECTION("Sequence is not started") {
    SECTION("started() returns false") {
      // act / assert
      REQUIRE(!sut.started());
    }
  }

  SECTION("Sequence is started") {
    SECTION("started() returns true") {
      // arrange
      sut.test(values[0]);

      // act / assert
      REQUIRE(sut.started());
    }
  }

  SECTION("Sequence is completed") {
    SECTION("started() returns true") {
      // arrange
      for (const auto& v : values) {
        sut.test(v);
      }

      // act / assert
      REQUIRE(sut.started());
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("sequence_test<T>::completed()", "[observers]") {
  // values
  const auto values = core::vector<int>{1,2,3,4};
  auto sut = sequence_detector{values.copy()};

  SECTION("Sequence is not started") {
    SECTION("completed() returns false") {
      // act / assert
      REQUIRE(!sut.completed());
    }
  }

  SECTION("Sequence is started, but incomplete") {
    SECTION("completed() returns false") {
      // arrange
      sut.test(values[0]);

      // act / assert
      REQUIRE(!sut.completed());
    }
  }

  SECTION("Sequence is completed") {
    SECTION("completed() returns true") {
      // arrange
      for (const auto& v : values) {
        sut.test(v);
      }

      // act / assert
      REQUIRE(sut.completed());
    }
  }
}

} // namespace alloy::io::test
