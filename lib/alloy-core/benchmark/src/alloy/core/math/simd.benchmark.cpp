/*
 Any copyright for benchmark files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/

#include "alloy/core/math/simd.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector.hpp"
#include "alloy/core/memory/allocator.hpp"

#include <array>
#include <catch2/catch.hpp>
#include <cstdlib>
#include <random>
#include <string>
#include <utility>

namespace alloy::core::benchmark {

///////////////////////////////////////////////////////////////////////////////
/// \brief A fixture for benchmarking the dot product operations in a consistent
///        way
///////////////////////////////////////////////////////////////////////////////
struct DotFixture {
  std::mt19937 engine{};
  std::uniform_real_distribution<f32> distribution{
    -100.0f,
    100.0f,
  };

  /// \brief A simple deleter that uses the core::allocator and deletes
  ///        aligned values using the default global allocator
  struct aligned_deleter {
    auto operator()(f32* p) -> void  {
      allocator{}.aligned_dispose_array<f32>(assume_not_null(p), m_size, simd_alignment);
    }
    std::size_t m_size;
  };

  using aligned_float_ptr = std::unique_ptr<float[],aligned_deleter>;

  /// \brief Generates a pair of inputs to perform the dot-product on
  ///
  /// \param size the number of elements to use for computing
  auto generate_input_pair(std::size_t size) -> std::array<aligned_float_ptr,2> {
    auto result = std::array<aligned_float_ptr,2>{{
      aligned_float_ptr{
        allocator{}.aligned_make_array<f32>(size, simd_alignment),
        {size}
      },
      aligned_float_ptr{
        allocator{}.aligned_make_array<f32>(size, simd_alignment),
        {size}
      },
    }};

    for (auto i = 0u; i < size; ++i) {
      result[0][i] = distribution(engine);
      result[1][i] = distribution(engine);
    }
    return result;
  }
};

namespace {
// A baseline "dot" product implementation written in terms of spans
auto dot(span<const f32,4> lhs, span<const f32,4> rhs) noexcept -> f32 {
  auto result = f32{0};

  for (auto i = 0; i < 4; ++i) {
    result += lhs[i] * rhs[i];
  }
  return result;
}
// A simplified operation for computing 4 dot products
auto dot4(span<const f32,16> lhs, span<const f32,16> rhs) noexcept -> std::array<f32,4> {
  return {
    dot(lhs.subspan<0,4>(), rhs.subspan<0,4>()),
    dot(lhs.subspan<4,4>(), rhs.subspan<4,4>()),
    dot(lhs.subspan<8,4>(), rhs.subspan<8,4>()),
    dot(lhs.subspan<12,4>(), rhs.subspan<12,4>()),
  };
}
}

TEST_CASE_METHOD(DotFixture, "Dot Product", "[benchmark][dot]") {

  constexpr auto components_per_vector = 4u;
  constexpr auto vectors_per_operation = 2u;
  const auto input_size = 4u;
  const auto vector_components = input_size * components_per_vector * vectors_per_operation;

  // Reset the seed per execution for reproducible benchmarks
  engine.seed(0xdeadbeef);
  auto result = generate_input_pair(vector_components);
  auto& lhs = result[0];
  auto& rhs = result[1];

  CHECK(vector_components % 16 == 0);

  BENCHMARK("Baseline 4 dot products") {
    return dot4(
      span<const f32,16>{lhs.get(), 16u},
      span<const f32,16>{rhs.get(), 16u}
    );
  };

  BENCHMARK("Simd dot4 with loading") {
    return simd_dot4(
      span<const f32,16>{lhs.get(), 16u},
      span<const f32,16>{rhs.get(), 16u}
    );
  };

  BENCHMARK_ADVANCED("Simd dot4 without loading")(Catch::Benchmark::Chronometer meter) {
    const simd<f32> lhs_simd[4] = {
      simd<f32>::load(span<const f32,4>{lhs.get() + 0u, 4u}),
      simd<f32>::load(span<const f32,4>{lhs.get() + 4u, 4u}),
      simd<f32>::load(span<const f32,4>{lhs.get() + 8u, 4u}),
      simd<f32>::load(span<const f32,4>{lhs.get() + 12u, 4u})
    };
    const simd<f32> rhs_simd[4] = {
      simd<f32>::load(span<const f32,4>{rhs.get() + 0u, 4u}),
      simd<f32>::load(span<const f32,4>{rhs.get() + 4u, 4u}),
      simd<f32>::load(span<const f32,4>{rhs.get() + 8u, 4u}),
      simd<f32>::load(span<const f32,4>{rhs.get() + 12u, 4u})
    };

    meter.measure([&]{
      return simd_dot4(lhs_simd, rhs_simd);
    });
  };
}


} // namespace alloy::core::benchmark
