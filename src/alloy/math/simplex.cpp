#include "alloy/math/simplex.hpp"

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

  const auto g_sqrt3 = alloy::math::sqrt( 3.0 );
  const auto g_sqrt5 = alloy::math::sqrt( 5.0 );

  const auto g_skew
    = alloy::core::real{0.5} * (g_sqrt3 - alloy::core::real{1});
  const auto g_unskew
    = (alloy::core::real{3.0} - g_sqrt5) / alloy::core::real{6.0};

  constexpr alloy::core::real dot2( const int* a,
                                    alloy::core::real x,
                                    alloy::core::real y )
    noexcept
  {
    return a[0]*x + a[1]*y;
  }

  constexpr alloy::core::real dot3( const int* a,
                                    alloy::core::real x,
                                    alloy::core::real y,
                                    alloy::core::real z )
    noexcept
  {
    return a[0]*x + a[1]*y + a[2]*z;
  }

  constexpr int floor_f_to_i( alloy::core::real x )
    noexcept
  {
    return x > 0 ? static_cast<int>(x) : static_cast<int>(x) - 1;
  }
} // anonymous namespace

//------------------------------------------------------------------------------
// Raw Noise
//------------------------------------------------------------------------------

alloy::core::real
  alloy::math::simplex::raw_noise( core::real x, core::real y )
  noexcept
{
  // Noise contributions from the three corners
  core::real n0, n1, n2;

  // Hairy factor for 2D
  auto s = (x + y) * g_skew;
  auto i = floor_f_to_i( x + s );
  auto j = floor_f_to_i( y + s );

  auto t = (i + j) * g_unskew;

  // Unskew the cell origin back to (x,y) space
  const auto x_unskew = i - t;
  const auto y_unskew = j - t;

  // The x,y distances from the cell origin
  auto x0 = x - x_unskew;
  auto y0 = y - y_unskew;

  // For the 2D case, the simplex shape is an equilateral triangle.
  // Determine which simplex we are in.

  int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords

  if(x0>y0){
    i1 = 1;
    j1 = 0;
  }else{ // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    i1 = 0;
    j1 = 1;
  }      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

  // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
  // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
  // c = (3-sqrt(3))/6
  auto x1 = x0 - i1 + g_unskew; // Offsets for middle corner in (x,y) unskewed coords
  auto y1 = y0 - j1 + g_unskew;
  auto x2 = x0 - 1.0 + 2.0 * g_unskew; // Offsets for last corner in (x,y) unskewed coords
  auto y2 = y0 - 1.0 + 2.0 * g_unskew;

  // Work out the hashed gradient indices of the three simplex corners
  auto ii = i & 255;
  auto jj = j & 255;
  auto gi0 = g_permutation_table[ ii + g_permutation_table[jj] ] % 12;
  auto gi1 = g_permutation_table[ ii + i1 + g_permutation_table[jj + j1] ] % 12;
  auto gi2 = g_permutation_table[ ii + 1  + g_permutation_table[jj + 1] ] % 12;

  // Calculate the contribution from the three corners
  auto t0 = 0.5 - (x0*x0) - (y0*y0);

  if(t0<0){
    n0 = 0.0;
  }else{
    t0 *= t0;
    n0 = t0 * t0 * ::dot2( g_grad[gi0], x0, y0 );
  }

  auto t1 = 0.5 - x1*x1-y1*y1;
  if(t1<0){
    n1 = 0.0;
  } else {
    t1 *= t1;
    n1 = t1 * t1 * ::dot2( g_grad[gi1], x1, y1 );
  }

  auto t2 = 0.5 - x2*x2-y2*y2;
  if(t2<0){
    n2 = 0.0;
  } else {
    t2 *= t2;
    n2 = t2 * t2 * ::dot2( g_grad[gi2], x2, y2 );
  }

  // Add contributions from each corner to get the final noise value.
  // The result is scaled to return values in the interval [-1,1].
  return 70.0 * (n0 + n1 + n2);
}

alloy::core::real
  alloy::math::simplex::raw_noise( core::real x, core::real y, core::real z )
  noexcept
{
  // TODO(matthew rodusek) 2017-04-22: Implement
  return x+y+z;
}

//------------------------------------------------------------------------------
// Octave Noises
//------------------------------------------------------------------------------

alloy::core::real
  alloy::math::simplex::octave_noise( core::real octaves,
                                      core::real persistence,
                                      core::real scale,
                                      core::real x,
                                      core::real y )
  noexcept
{

    auto frequency = scale;
    auto total     = core::real(0.0);
    auto amplitude = core::real(1.0);

    auto max_amplitude = core::real(0);

    for( auto i = 0; i < int(octaves); ++i ) {
      total += raw_noise( x * frequency, y * frequency ) * amplitude;

      max_amplitude += amplitude;
      frequency *= 2;
      amplitude *= persistence;
    }

    return total / max_amplitude;
}

alloy::core::real
  alloy::math::simplex::octave_noise( core::real octaves,
                                      core::real persistence,
                                      core::real scale,
                                      core::real x,
                                      core::real y,
                                      core::real z )
  noexcept
{

    auto frequency = scale;
    auto total     = core::real(0.0);
    auto amplitude = core::real(1.0);

    auto max_amplitude = core::real(0);

    for( int i = 0; i < int(octaves); ++i ) {
      total += raw_noise( x*frequency, y*frequency, z*frequency ) * amplitude;

      max_amplitude += amplitude;
      frequency *= 2;
      amplitude *= persistence;
    }

    return total / max_amplitude;
}