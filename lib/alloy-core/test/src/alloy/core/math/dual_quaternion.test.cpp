// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/dual_quaternion.hpp>

#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/dual_quaternion_constants.hpp>
#include <alloy/core/math/interpolation.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/matrix/matrix4.hpp>
#include <alloy/core/math/quaternion.hpp>
#include <alloy/core/math/quaternion_constants.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/math/vector/vector4.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("dual_quaternion::dual_quaternion()", "[ctor]")
{
  // Act
  const auto sut = dual_quaternion{};

  // Assert
  SECTION("Default dual quaternion is the identity")
  {
    REQUIRE( sut == dual_quaternion_constants::identity );
  }
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

TEST_CASE("dual_quaternion::translation()", "[observers]")
{
  // Arrange
  const auto translation = vector3{1, 2, 3};
  const auto sut = dual_quaternion::from_rotation_translation(
    quaternion::from_angle_axis(radian{0.5f}, vector3{1, 1, 0}),
    translation
  );

  // Act
  const auto result = sut.translation();

  // Assert
  SECTION("Recovers the translation that it was made from")
  {
    REQUIRE( almost_equal(result, translation) );
  }
}

TEST_CASE("dual_quaternion::transform_matrix()", "[observers]")
{
  // Arrange
  const auto sut = dual_quaternion::from_rotation_translation(
    quaternion::from_angle_axis(radian{0.5f}, vector3{1, 1, 0}),
    vector3{1, 2, 3}
  );
  const auto point = vector3{4, 5, 6};
  const auto transformed = sut.transform_point(point);
  const auto expected = vector4{
    transformed.x(),
    transformed.y(),
    transformed.z(),
    1
  };

  // Act
  const auto result = sut.transform_matrix() * vector4{
    point.x(),
    point.y(),
    point.z(),
    1
  };

  // Assert
  SECTION("Transforms points the same as transform_point")
  {
    REQUIRE( almost_equal(result, expected) );
  }
}

TEST_CASE("dual_quaternion::inverse()", "[observers]")
{
  SECTION("Product with a non-unit dual quaternion is the identity")
  {
    // Arrange
    const auto sut = dual_quaternion{
      quaternion{1, 2, 3, 4},
      quaternion{5, 6, 7, 8}
    };

    // Act
    const auto result = sut * sut.inverse();

    // Assert
    REQUIRE( almost_equal(result, dual_quaternion_constants::identity) );
  }

  SECTION("Inverse is also the left inverse")
  {
    // Arrange
    const auto sut = dual_quaternion{
      quaternion{1, 2, 3, 4},
      quaternion{5, 6, 7, 8}
    };

    // Act
    const auto result = sut.inverse() * sut;

    // Assert
    REQUIRE( almost_equal(result, dual_quaternion_constants::identity) );
  }

  SECTION("Zero real part inverts to zero")
  {
    // Arrange
    const auto sut = dual_quaternion{
      quaternion_constants::zero,
      quaternion{5, 6, 7, 8}
    };

    // Act
    const auto result = sut.inverse();

    // Assert
    REQUIRE( result == dual_quaternion_constants::zero );
  }
}

TEST_CASE("dual_quaternion::conjugated()", "[observers]")
{
  // Arrange
  const auto sut = dual_quaternion{
    quaternion{1, 2, 3, 4},
    quaternion{5, 6, 7, 8}
  };
  const auto expected = dual_quaternion{
    quaternion{1, -2, -3, -4},
    quaternion{5, -6, -7, -8}
  };

  // Act
  const auto result = sut.conjugated();

  // Assert
  SECTION("Negates the imaginary parts of both parts")
  {
    REQUIRE( result == expected );
  }
}

TEST_CASE("dual_quaternion::conjugated() of a unit dual quaternion", "[observers]")
{
  // Arrange
  const auto sut = dual_quaternion::from_rotation_translation(
    quaternion::from_angle_axis(radian{0.5f}, vector3{1, 1, 0}),
    vector3{1, 2, 3}
  );
  const auto point = vector3{4, 5, 6};

  // Act
  const auto result = sut.conjugated().transform_point(
    sut.transform_point(point)
  );

  // Assert
  SECTION("Undoes the transformation")
  {
    REQUIRE( almost_equal(result, point) );
  }
}

//------------------------------------------------------------------------------
// Transformations
//------------------------------------------------------------------------------

TEST_CASE("dual_quaternion::transform_point( const vector3& )", "[transformations]")
{
  // Arrange
  const auto sut = dual_quaternion::from_rotation_translation(
    quaternion::from_angle_axis(
      radian_constants::quarter_revolution,
      vector3{0, 0, 1}
    ),
    vector3{1, 2, 3}
  );

  // Act
  const auto result = sut.transform_point(vector3{1, 0, 0});

  // Assert
  SECTION("Rotates the point, and then translates it")
  {
    REQUIRE( almost_equal(result, vector3{1, 3, 3}) );
  }
}

TEST_CASE("dual_quaternion::transform_vector( const vector3& )", "[transformations]")
{
  // Arrange
  const auto sut = dual_quaternion::from_rotation_translation(
    quaternion::from_angle_axis(
      radian_constants::quarter_revolution,
      vector3{0, 0, 1}
    ),
    vector3{1, 2, 3}
  );

  // Act
  const auto result = sut.transform_vector(vector3{1, 0, 0});

  // Assert
  SECTION("Rotates the vector without translating it")
  {
    REQUIRE( almost_equal(result, vector3{0, 1, 0}) );
  }
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

TEST_CASE("dual_quaternion::normalize()", "[modifiers]")
{
  // Arrange
  const auto unit = dual_quaternion::from_rotation_translation(
    quaternion::from_angle_axis(radian{0.5f}, vector3{1, 1, 0}),
    vector3{1, 2, 3}
  );

  SECTION("Scaled dual quaternion transforms the same as the original")
  {
    // Arrange
    auto sut = unit * real{3};
    const auto point = vector3{4, 5, 6};

    const auto expected = unit.transform_point(point);

    // Act
    sut.normalize();
    const auto result = sut.transform_point(point);

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Real part becomes unit length")
  {
    // Arrange
    auto sut = unit * real{3};

    // Act
    sut.normalize();
    const auto result = sut.real().magnitude();

    // Assert
    REQUIRE( almost_equal(result, real{1}) );
  }

  SECTION("Dual part becomes orthogonal to the real part")
  {
    // Arrange
    auto sut = dual_quaternion{unit.real(), unit.dual() + unit.real()};

    // Act
    sut.normalize();
    const auto result = sut.real().dot(sut.dual());

    // Assert
    REQUIRE( almost_equal(result, real{0}) );
  }

  SECTION("Zero real part is left unchanged")
  {
    // Arrange
    const auto expected = dual_quaternion{
      quaternion_constants::zero,
      quaternion{1, 2, 3, 4}
    };
    auto sut = expected;

    // Act
    sut.normalize();

    // Assert
    REQUIRE( sut == expected );
  }
}

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

TEST_CASE("dual_quaternion::from_rotation( const quaternion& )", "[factory]")
{
  // Arrange
  const auto rotation = quaternion::from_angle_axis(
    radian{0.5f},
    vector3{1, 1, 0}
  );
  const auto point = vector3{4, 5, 6};
  const auto expected = rotation * point;

  // Act
  const auto sut = dual_quaternion::from_rotation(rotation);
  const auto result = sut.transform_point(point);

  // Assert
  SECTION("Rotates points about the origin")
  {
    REQUIRE( almost_equal(result, expected) );
  }
}

TEST_CASE("dual_quaternion::from_translation( const vector3& )", "[factory]")
{
  // Arrange
  const auto translation = vector3{1, 2, 3};
  const auto point = vector3{4, 5, 6};
  const auto expected = point + translation;

  // Act
  const auto sut = dual_quaternion::from_translation(translation);
  const auto result = sut.transform_point(point);

  // Assert
  SECTION("Translates points")
  {
    REQUIRE( almost_equal(result, expected) );
  }
}

TEST_CASE("dual_quaternion::from_rotation_translation( const quaternion&, const vector3& )", "[factory]")
{
  // Arrange
  const auto rotation = quaternion::from_angle_axis(
    radian{0.5f},
    vector3{1, 1, 0}
  );
  const auto translation = vector3{1, 2, 3};
  const auto expected = dual_quaternion::from_translation(translation)
                      * dual_quaternion::from_rotation(rotation);

  // Act
  const auto result = dual_quaternion::from_rotation_translation(
    rotation,
    translation
  );

  // Assert
  SECTION("Composes the translation after the rotation")
  {
    REQUIRE( almost_equal(result, expected) );
  }
}

TEST_CASE("dual_quaternion::from_transform_matrix( const matrix4& )", "[factory]")
{
  // Arrange
  const auto expected = dual_quaternion::from_rotation_translation(
    quaternion::from_angle_axis(radian{0.5f}, vector3{1, 1, 0}),
    vector3{1, 2, 3}
  );

  // Act
  const auto result = dual_quaternion::from_transform_matrix(
    expected.transform_matrix()
  );

  // Assert
  SECTION("Round-trips a transformation matrix")
  {
    REQUIRE( almost_equal(result, expected) );
  }
}

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

TEST_CASE("operator+( const dual_quaternion&, const dual_quaternion& )", "[arithmetic]")
{
  // Arrange
  const auto lhs = dual_quaternion{
    quaternion{1, 2, 3, 4},
    quaternion{5, 6, 7, 8}
  };
  const auto rhs = dual_quaternion{
    quaternion{8, 7, 6, 5},
    quaternion{4, 3, 2, 1}
  };
  const auto expected = dual_quaternion{
    quaternion{9, 9, 9, 9},
    quaternion{9, 9, 9, 9}
  };

  // Act
  const auto result = lhs + rhs;

  // Assert
  SECTION("Adds both parts component-wise")
  {
    REQUIRE( result == expected );
  }
}

TEST_CASE("operator*( const dual_quaternion&, const dual_quaternion& )", "[arithmetic]")
{
  SECTION("Identity leaves the other operand unchanged")
  {
    // Arrange
    const auto sut = dual_quaternion{
      quaternion{1, 2, 3, 4},
      quaternion{5, 6, 7, 8}
    };

    // Act
    const auto result = dual_quaternion_constants::identity * sut;

    // Assert
    REQUIRE( result == sut );
  }

  SECTION("Square of the dual unit is zero")
  {
    // Arrange
    const auto sut = dual_quaternion{
      quaternion_constants::zero,
      quaternion_constants::identity
    };

    // Act
    const auto result = sut * sut;

    // Assert
    REQUIRE( result == dual_quaternion_constants::zero );
  }

  SECTION("Product transforms by the right operand first")
  {
    // Arrange
    const auto lhs = dual_quaternion::from_rotation_translation(
      quaternion::from_angle_axis(radian{0.5f}, vector3{1, 1, 0}),
      vector3{1, 2, 3}
    );
    const auto rhs = dual_quaternion::from_rotation_translation(
      quaternion::from_angle_axis(radian{1.5f}, vector3{0, 1, 1}),
      vector3{-3, 0, 2}
    );
    const auto point = vector3{4, 5, 6};
    const auto expected = lhs.transform_point(rhs.transform_point(point));

    // Act
    const auto result = (lhs * rhs).transform_point(point);

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Dual unit scales the real part into the dual part")
  {
    // Arrange
    const auto epsilon = dual_quaternion{
      quaternion_constants::zero,
      quaternion_constants::identity
    };
    const auto sut = dual_quaternion{
      quaternion{1, 2, 3, 4},
      quaternion{5, 6, 7, 8}
    };
    const auto expected = dual_quaternion{
      quaternion_constants::zero,
      quaternion{1, 2, 3, 4}
    };

    // Act
    const auto result = epsilon * sut;

    // Assert
    REQUIRE( result == expected );
  }
}

TEST_CASE("operator/( const dual_quaternion&, const dual_quaternion& )", "[arithmetic]")
{
  // Arrange
  const auto lhs = dual_quaternion{
    quaternion{1, 2, 3, 4},
    quaternion{5, 6, 7, 8}
  };
  const auto rhs = dual_quaternion{
    quaternion{4, 3, 2, 1},
    quaternion{1, 0, 1, 0}
  };

  // Act
  const auto result = (lhs / rhs) * rhs;

  // Assert
  SECTION("Division is undone by multiplying with the divisor")
  {
    REQUIRE( almost_equal(result, lhs, real{1e-4f}) );
  }
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

TEST_CASE("almost_equal( const dual_quaternion&, const dual_quaternion&, real )", "[comparison]")
{
  // Arrange
  const auto sut = dual_quaternion{
    quaternion{1, 2, 3, 4},
    quaternion{5, 6, 7, 8}
  };

  SECTION("Differences within the tolerance are equal")
  {
    // Arrange
    const auto other = dual_quaternion{
      quaternion{1, 2, 3, 4},
      quaternion{5, 6, 7, real{8.01f}}
    };

    // Act
    const auto result = almost_equal(sut, other, real{0.1f});

    // Assert
    REQUIRE( result );
  }

  SECTION("Differences in the dual part beyond the tolerance are unequal")
  {
    // Arrange
    const auto other = dual_quaternion{
      quaternion{1, 2, 3, 4},
      quaternion{5, 6, 7, 9}
    };

    // Act
    const auto result = almost_equal(sut, other, real{0.1f});

    // Assert
    REQUIRE_FALSE( result );
  }
}

//------------------------------------------------------------------------------
// Interpolation
//------------------------------------------------------------------------------

TEST_CASE("sclerp( const dual_quaternion&, const dual_quaternion&, real )", "[interpolation]")
{
  // Arrange
  const auto start = dual_quaternion::from_rotation_translation(
    quaternion::from_angle_axis(radian{0.5f}, vector3{1, 1, 0}),
    vector3{1, 2, 3}
  );
  const auto end = dual_quaternion::from_rotation_translation(
    quaternion::from_angle_axis(radian{2.0f}, vector3{0, 1, 1}),
    vector3{-3, 0, 2}
  );

  SECTION("Screws along the axis at a constant velocity")
  {
    // Arrange
    const auto screw = dual_quaternion::from_rotation_translation(
      quaternion::from_angle_axis(
        radian_constants::quarter_revolution,
        vector3{0, 0, 1}
      ),
      vector3{0, 0, 2}
    );
    const auto expected = dual_quaternion::from_rotation_translation(
      quaternion::from_angle_axis(
        radian_constants::quarter_revolution * real{0.25},
        vector3{0, 0, 1}
      ),
      vector3{0, 0, real{0.5}}
    );

    // Act
    const auto result = sclerp(
      dual_quaternion_constants::identity,
      screw,
      real{0.25}
    );

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Midpoint splits the motion into two equal steps")
  {
    // Arrange
    const auto mid = sclerp(start, end, real{0.5});

    // Act
    const auto result = mid * (start.conjugated() * mid);

    // Assert
    REQUIRE( almost_equal(result, end) );
  }

  SECTION("Takes the shortest path when the end is negated")
  {
    // Arrange
    const auto expected = sclerp(start, end, real{0.5});

    // Act
    const auto result = sclerp(start, -end, real{0.5});

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Interpolates the translation between equal rotations")
  {
    // Arrange
    const auto expected = dual_quaternion::from_translation(vector3{1, 0, 0});

    // Act
    const auto result = sclerp(
      dual_quaternion_constants::identity,
      dual_quaternion::from_translation(vector3{2, 0, 0}),
      real{0.5}
    );

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Returns the start at 0")
  {
    // Act
    const auto result = sclerp(start, end, real{0});

    // Assert
    REQUIRE( almost_equal(result, start) );
  }

  SECTION("Returns the end at 1")
  {
    // Act
    const auto result = sclerp(start, end, real{1});

    // Assert
    REQUIRE( almost_equal(result, end) );
  }
}

TEST_CASE("nlerp( const dual_quaternion&, const dual_quaternion&, real )", "[interpolation]")
{
  SECTION("Interpolates the translation between equal rotations")
  {
    // Arrange
    const auto expected = dual_quaternion::from_translation(vector3{1, 0, 0});

    // Act
    const auto result = nlerp(
      dual_quaternion_constants::identity,
      dual_quaternion::from_translation(vector3{2, 0, 0}),
      real{0.5}
    );

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Midpoint is the half rotation")
  {
    // Arrange
    const auto end = dual_quaternion::from_rotation(
      quaternion::from_angle_axis(
        radian_constants::quarter_revolution,
        vector3{0, 0, 1}
      )
    );
    const auto expected = dual_quaternion::from_rotation(
      quaternion::from_angle_axis(
        radian_constants::quarter_revolution * real{0.5},
        vector3{0, 0, 1}
      )
    );

    // Act
    const auto result = nlerp(
      dual_quaternion_constants::identity,
      end,
      real{0.5}
    );

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Takes the shortest path when the end is negated")
  {
    // Arrange
    const auto start = dual_quaternion::from_rotation_translation(
      quaternion::from_angle_axis(radian{0.5f}, vector3{1, 1, 0}),
      vector3{1, 2, 3}
    );
    const auto end = dual_quaternion::from_rotation_translation(
      quaternion::from_angle_axis(radian{2.0f}, vector3{0, 1, 1}),
      vector3{-3, 0, 2}
    );
    const auto expected = nlerp(start, end, real{0.5});

    // Act
    const auto result = nlerp(start, -end, real{0.5});

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }
}

TEST_CASE("interpolator<dual_quaternion>", "[interpolation]")
{
  // Arrange
  const auto start = dual_quaternion_constants::identity;
  const auto end = dual_quaternion::from_rotation_translation(
    quaternion::from_angle_axis(
      radian_constants::half_revolution * real{0.9},
      vector3{1, 0, 0}
    ),
    vector3{1, 2, 3}
  );

  SECTION("Interpolated real part is unit length")
  {
    // Act
    const auto sut = interpolation::linear(start, end, real{0.5});
    const auto result = sut.real().magnitude();

    // Assert
    REQUIRE( almost_equal(result, real{1}) );
  }

  SECTION("Interpolated parts are orthogonal")
  {
    // Act
    const auto sut = interpolation::linear(start, end, real{0.5});
    const auto result = sut.real().dot(sut.dual());

    // Assert
    REQUIRE( almost_equal(result, real{0}) );
  }
}

} // namespace alloy::core::test
