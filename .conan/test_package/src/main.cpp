
#include <alloy/core/math/angle.hpp>
#include <alloy/core/math/quaternion.hpp>
#include <alloy/core/math/matrix.hpp>
#include <alloy/core/math/math_constants.hpp>

int main()
{
  using namespace alloy::core::casts;

  // const auto q = alloy::core::quaternion::from_angles(
  //   alloy::core::radian{ alloy::core::math_constants::pi() },
  //   alloy::core::radian{ alloy::core::math_constants::pi() / 2 },
  //   alloy::core::radian{ alloy::core::math_constants::pi() / 4 }
  // );

  // auto matrix = alloy::core::matrix3{};
  // q.extract_rotation_matrix(&matrix);

  // (void) matrix;
  return 0;
}