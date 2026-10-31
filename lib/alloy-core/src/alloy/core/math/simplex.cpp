// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/math/simplex.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/math/math.hpp"

namespace alloy::core {
  namespace {

    // The gradients are the midpoints of the vertices of a cube.
    constexpr int g_grad[12][3] = {
      {1,1,0}, {-1,1,0}, {1,-1,0}, {-1,-1,0},
      {1,0,1}, {-1,0,1}, {1,0,-1}, {-1,0,-1},
      {0,1,1}, {0,-1,1}, {0,1,-1}, {0,-1,-1}
    };

    // Permutation table. The same list is repeated twice.
    constexpr int g_permutation_table[512] = {
      151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
      8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
      35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
      134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
      55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
      18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
      250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
      189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
      172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
      228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
      107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
      138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,

      151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
      8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
      35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
      134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
      55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
      18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
      250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
      189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
      172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
      228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
      107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
      138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };

    constexpr
    auto dot2(const int* a, float x, float y) noexcept -> float
    {
      return static_cast<float>(a[0]) * x +
             static_cast<float>(a[1]) * y;
    }

    constexpr
    auto dot3(const int* a, float x, float y, float z) noexcept -> float
    {
      return static_cast<float>(a[0]) * x +
             static_cast<float>(a[1]) * y +
             static_cast<float>(a[2]) * z;
    }

    constexpr
    auto corner3(int gi, float x, float y, float z) noexcept -> float
    {
      auto t = 0.6f - (x*x) - (y*y) - (z*z);
      if (t < 0.0f) {
        return 0.0f;
      }
      t *= t;
      return t * t * dot3(g_grad[gi], x, y, z);
    }

    constexpr
    auto floor_f_to_i(float x) noexcept -> int
    {
      const auto truncated = static_cast<int>(x);

      return (x < static_cast<float>(truncated)) ? truncated - 1 : truncated;
    }
  } // anonymous namespace
} // namespace alloy::core

//------------------------------------------------------------------------------
// Raw Noise
//------------------------------------------------------------------------------

auto alloy::core::simplex::raw_noise(float x, float y) noexcept -> float
{
  static const auto s_sqrt3 = static_cast<float>(sqrt( 3.0 ));
  static const auto s_skew  = 0.5f * (s_sqrt3 - 1.0f);

  static const auto s_unskew = (3.0f - s_sqrt3) / 6.0f;

  // Noise contributions from the three corners
  auto n0 = float{};
  auto n1 = float{};
  auto n2 = float{};

  // Hairy factor for 2D
  auto s = (x + y) * s_skew;
  auto i = floor_f_to_i( x + s );
  auto j = floor_f_to_i( y + s );

  auto t = static_cast<float>(i + j) * s_unskew;

  // Unskew the cell origin back to (x,y) space
  const auto x_unskew = static_cast<float>(i) - t;
  const auto y_unskew = static_cast<float>(j) - t;

  // The x,y distances from the cell origin
  auto x0 = x - x_unskew;
  auto y0 = y - y_unskew;

  // For the 2D case, the simplex shape is an equilateral triangle.
  // Determine which simplex we are in.

  // Offsets for second (middle) corner of simplex in (i,j) coords
  auto i1 = int{};
  auto j1 = int{};

  if (x0 > y0) {
    i1 = 1;
    j1 = 0;
  } else { // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    i1 = 0;
    j1 = 1;
  }      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

  // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
  // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
  // c = (3-sqrt(3))/6

  // Offsets for middle corner in (x,y) unskewed coords
  auto x1 = x0 - static_cast<float>(i1) + s_unskew;
  auto y1 = y0 - static_cast<float>(j1) + s_unskew;

  // Offsets for last corner in (x,y) unskewed coords
  auto x2 = x0 - 1.0f + 2.0f * s_unskew;
  auto y2 = y0 - 1.0f + 2.0f * s_unskew;

  // Work out the hashed gradient indices of the three simplex corners
  auto ii = i & 255;
  auto jj = j & 255;
  auto gi0 = g_permutation_table[ ii + g_permutation_table[jj] ] % 12;
  auto gi1 = g_permutation_table[ ii + i1 + g_permutation_table[jj + j1] ] % 12;
  auto gi2 = g_permutation_table[ ii + 1  + g_permutation_table[jj + 1] ] % 12;

  // Calculate the contribution from the three corners
  auto t0 = 0.5f - (x0*x0) - (y0*y0);

  if (t0 < 0) {
    n0 = 0.0f;
  } else {
    t0 *= t0;
    n0 = t0 * t0 * dot2( g_grad[gi0], x0, y0 );
  }

  auto t1 = 0.5f - x1*x1-y1*y1;
  if (t1 < 0) {
    n1 = 0.0f;
  } else {
    t1 *= t1;
    n1 = static_cast<float>(t1 * t1) * dot2( g_grad[gi1], x1, y1 );
  }

  auto t2 = 0.5f - x2*x2-y2*y2;
  if (t2 < 0){
    n2 = 0.0f;
  } else {
    t2 *= t2;
    n2 = static_cast<float>(t2 * t2) * dot2( g_grad[gi2], x2, y2 );
  }

  // Add contributions from each corner to get the final noise value.
  // The result is scaled to return values in the interval [-1,1].
  const auto result = 70.0f * static_cast<float>(n0 + n1 + n2);

  return static_cast<float>(result);
}

auto alloy::core::simplex::raw_noise(float x, float y, float z)
  noexcept -> float
{
  static constexpr auto s_skew = 1.0f / 3.0f;
  static constexpr auto s_unskew = 1.0f / 6.0f;

  // Skew the input space to determine which simplex cell we are in
  const auto s = (x + y + z) * s_skew;
  const auto i = floor_f_to_i(x + s);
  const auto j = floor_f_to_i(y + s);
  const auto k = floor_f_to_i(z + s);

  const auto t = static_cast<float>(i + j + k) * s_unskew;

  // The x,y,z distances from the unskewed cell origin
  const auto x0 = x - (static_cast<float>(i) - t);
  const auto y0 = y - (static_cast<float>(j) - t);
  const auto z0 = z - (static_cast<float>(k) - t);

  // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
  // The order of the offsets determines which of the six simplices of the
  // cell we are in, and so the (i,j,k) offsets of its second and third
  // corners.
  auto i1 = int{}; auto j1 = int{}; auto k1 = int{};
  auto i2 = int{}; auto j2 = int{}; auto k2 = int{};

  if (x0 >= y0) {
    if (y0 >= z0) {        // X Y Z order
      i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
    } else if (x0 >= z0) { // X Z Y order
      i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1;
    } else {               // Z X Y order
      i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1;
    }
  } else {
    if (y0 < z0) {         // Z Y X order
      i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1;
    } else if (x0 < z0) {  // Y Z X order
      i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1;
    } else {               // Y X Z order
      i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
    }
  }

  // A step of 1 in (i,j,k) is a step of (1 - c) in that axis and -c in the
  // others in (x,y,z), where c = 1/6
  const auto x1 = x0 - static_cast<float>(i1) + s_unskew;
  const auto y1 = y0 - static_cast<float>(j1) + s_unskew;
  const auto z1 = z0 - static_cast<float>(k1) + s_unskew;

  const auto x2 = x0 - static_cast<float>(i2) + 2.0f * s_unskew;
  const auto y2 = y0 - static_cast<float>(j2) + 2.0f * s_unskew;
  const auto z2 = z0 - static_cast<float>(k2) + 2.0f * s_unskew;

  const auto x3 = x0 - 1.0f + 3.0f * s_unskew;
  const auto y3 = y0 - 1.0f + 3.0f * s_unskew;
  const auto z3 = z0 - 1.0f + 3.0f * s_unskew;

  // Work out the hashed gradient indices of the four simplex corners
  const auto ii = i & 255;
  const auto jj = j & 255;
  const auto kk = k & 255;
  const auto* const p = g_permutation_table;
  const auto gi0 = p[ii + p[jj + p[kk]]] % 12;
  const auto gi1 = p[ii + i1 + p[jj + j1 + p[kk + k1]]] % 12;
  const auto gi2 = p[ii + i2 + p[jj + j2 + p[kk + k2]]] % 12;
  const auto gi3 = p[ii + 1 + p[jj + 1 + p[kk + 1]]] % 12;

  // Add contributions from each corner to get the final noise value.
  // The result is scaled to stay just inside [-1,1]
  return 32.0f * (
    corner3(gi0, x0, y0, z0) +
    corner3(gi1, x1, y1, z1) +
    corner3(gi2, x2, y2, z2) +
    corner3(gi3, x3, y3, z3)
  );
}

//------------------------------------------------------------------------------
// Octave Noises
//------------------------------------------------------------------------------

auto alloy::core::simplex::octave_noise(
  float octaves,
  float persistence,
  float scale,
  float x,
  float y
) noexcept -> float
{
    ALLOY_ASSERT(octaves >= 1.0f);

    auto frequency = scale;
    auto total     = 0.0f;
    auto amplitude = 1.0f;

    auto max_amplitude = 0.0f;

    for( auto i = 0; i < int(octaves); ++i ) {
      total += raw_noise( x * frequency, y * frequency ) * amplitude;

      max_amplitude += amplitude;
      frequency *= 2;
      amplitude *= persistence;
    }

    return static_cast<float>(total / max_amplitude);
}

auto alloy::core::simplex::octave_noise(
  float octaves,
  float persistence,
  float scale,
  float x,
  float y,
  float z
) noexcept -> float
{
    ALLOY_ASSERT(octaves >= 1.0f);

    auto frequency = scale;
    auto total     = 0.0f;
    auto amplitude = 1.0f;

    auto max_amplitude = 0.0f;

    for( int i = 0; i < int(octaves); ++i ) {
      total += raw_noise( x*frequency, y*frequency, z*frequency ) * amplitude;

      max_amplitude += amplitude;
      frequency *= 2;
      amplitude *= persistence;
    }

    return static_cast<float>(total / max_amplitude);
}
