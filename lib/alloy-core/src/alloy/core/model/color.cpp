#include "alloy/core/model/color.hpp"

#include "alloy/core/intrinsics.hpp"

//==============================================================================
// class : color
//==============================================================================

//------------------------------------------------------------------------------
// Static Member Functions
//------------------------------------------------------------------------------

alloy::core::color alloy::core::color::from_rgba32( rgba32 c )
  noexcept
{
  static constexpr auto radix = real{255};

  const auto hex = static_cast<std::uint32_t>(c);
  const auto r = static_cast<real>((hex & 0xFF000000) >> 24) / radix;
  const auto g = static_cast<real>((hex & 0x00FF0000) >> 16) / radix;
  const auto b = static_cast<real>((hex & 0x0000FF00) >>  8) / radix;
  const auto a = static_cast<real>((hex & 0x000000FF)      ) / radix;

  return {r,g,b,a};
}

alloy::core::color alloy::core::color::from_argb32( argb32 c )
  noexcept
{
  static constexpr auto radix = real{255};

  const auto hex = static_cast<std::uint32_t>(c);
  const auto a = static_cast<real>((hex & 0xFF000000) >> 24) / radix;
  const auto r = static_cast<real>((hex & 0x00FF0000) >> 16) / radix;
  const auto g = static_cast<real>((hex & 0x0000FF00) >>  8) / radix;
  const auto b = static_cast<real>((hex & 0x000000FF)      ) / radix;

  return {r,g,b,a};
}

alloy::core::color alloy::core::color::from_abgr32( abgr32 c )
  noexcept
{
  static constexpr auto radix = real{255};

  const auto hex = static_cast<std::uint32_t>(c);
  const auto a = static_cast<real>((hex & 0xFF000000) >> 24) / radix;
  const auto b = static_cast<real>((hex & 0x00FF0000) >> 16) / radix;
  const auto g = static_cast<real>((hex & 0x0000FF00) >>  8) / radix;
  const auto r = static_cast<real>((hex & 0x000000FF)      ) / radix;

  return {r,g,b,a};
}

alloy::core::color alloy::core::color::from_rgba64( rgba64 c )
  noexcept
{
  static constexpr auto radix = real{65535};

  const auto hex = static_cast<std::uint64_t>(c);
  const auto r = static_cast<real>((hex & 0xFFFF000000000000) >> 48) / radix;
  const auto g = static_cast<real>((hex & 0x0000FFFF00000000) >> 32) / radix;
  const auto b = static_cast<real>((hex & 0x00000000FFFF0000) >> 16) / radix;
  const auto a = static_cast<real>((hex & 0x000000000000FFFF)      ) / radix;

  return {r,g,b,a};
}

alloy::core::color alloy::core::color::from_argb64(argb64 c)
  noexcept
{
  static constexpr auto radix = real{65535};

  const auto hex = static_cast<std::uint64_t>(c);
  const auto a = static_cast<real>((hex & 0xFFFF000000000000) >> 48) / radix;
  const auto r = static_cast<real>((hex & 0x0000FFFF00000000) >> 32) / radix;
  const auto g = static_cast<real>((hex & 0x00000000FFFF0000) >> 16) / radix;
  const auto b = static_cast<real>((hex & 0x000000000000FFFF)      ) / radix;

  return {r,g,b,a};
}

alloy::core::color alloy::core::color::from_abgr64(abgr64 c)
  noexcept
{
  static constexpr auto radix = real{65535};

  const auto hex = static_cast<std::uint64_t>(c);
  const auto a = static_cast<real>((hex & 0xFFFF000000000000) >> 48) / radix;
  const auto b = static_cast<real>((hex & 0x0000FFFF00000000) >> 32) / radix;
  const auto g = static_cast<real>((hex & 0x00000000FFFF0000) >> 16) / radix;
  const auto r = static_cast<real>((hex & 0x000000000000FFFF)      ) / radix;

  return {r,g,b,a};
}

//------------------------------------------------------------------------------

alloy::core::color alloy::core::color::from_hsb(hsb c)
  noexcept
{
  const auto hue        = mod(c.hue, real{1});
  const auto saturation = saturate(c.saturation);
  const auto brightness = saturate(c.brightness);

  // 0 brightness means: must be black
  if (almost_equal(brightness, real{0})) {
    return {real{0},real{0},real{0}};
  }

  // 0 saturation means: must be gray
  if (almost_equal(saturation, real{0})) {
    return {brightness, brightness, brightness};
  }

  static constexpr auto total_domains  = 6;
  static constexpr auto red_domain     = 0;
  static constexpr auto yellow_domain  = 1;
  static constexpr auto green_domain   = 2;
  static constexpr auto cyan_domain    = 3;
  static constexpr auto blue_domain    = 4;
  static constexpr auto magenta_domain = 5;

  // wrap around, and allow mathematical errors
  auto hue_domain  = hue * real{total_domains};
  if (hue_domain >= real{total_domains}) {
    hue_domain = real{0};
  }

  const auto domain = static_cast<unsigned short>(hue_domain);
  const auto f1 = brightness * (1 - saturation);
  const auto f2 = brightness * (1 - saturation * (hue_domain - domain));
  const auto f3 = brightness * (1 - saturation * (1 - (hue_domain - domain)));

  switch (domain) {
    case red_domain: {
      return {brightness, f3, f1};
    }

    case yellow_domain: {
      return {f2, brightness, f1};
    }

    case green_domain: {
      return {f1, brightness, f3};
    }

    case cyan_domain: {
      return {f1, f2, brightness};
    }

    case blue_domain: {
      return {f3, f1, brightness};
    }

    case magenta_domain: {
      return {brightness, f1, f2};
    }

    default:
      ALLOY_UNREACHABLE();
  }
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

void alloy::core::color::invert()
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = real{1} - m_data[i];
  }
}

void alloy::core::color::contrast()
  noexcept
{
  static constexpr auto half = real{1} / real{2};

  for (auto i = 0; i < 3; ++i) {
    m_data[i] = mod(m_data[i] + half, real{1});
  }
}

void alloy::core::color::darken( real percent )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] * (real{1} - percent);
  }
}

void alloy::core::color::brighten( real percent )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] + (real{1} - m_data[i]) * percent;
  }
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

alloy::core::rgba32 alloy::core::color::to_rgba32()
  const noexcept
{
  const auto br = static_cast<std::uint32_t>(r() * 255);
  const auto bg = static_cast<std::uint32_t>(g() * 255);
  const auto bb = static_cast<std::uint32_t>(b() * 255);
  const auto ba = static_cast<std::uint32_t>(a() * 255);

  return static_cast<rgba32>((br << 24) | (bg << 16) | (bb << 8) | (ba));
}

alloy::core::argb32 alloy::core::color::to_argb32()
  const noexcept
{
  const auto br = static_cast<std::uint32_t>(r() * 255);
  const auto bg = static_cast<std::uint32_t>(g() * 255);
  const auto bb = static_cast<std::uint32_t>(b() * 255);
  const auto ba = static_cast<std::uint32_t>(a() * 255);

  return static_cast<argb32>((ba << 24) | (br << 16) | (bg << 8) | (bb));
}

alloy::core::abgr32 alloy::core::color::to_abgr32()
  const noexcept
{
  const auto br = static_cast<std::uint32_t>(r() * 255);
  const auto bg = static_cast<std::uint32_t>(g() * 255);
  const auto bb = static_cast<std::uint32_t>(b() * 255);
  const auto ba = static_cast<std::uint32_t>(a() * 255);

  return static_cast<abgr32>((ba << 24) | (bb << 16) | (bg << 8) | (br));
}

alloy::core::rgba64 alloy::core::color::to_rgba64()
  const noexcept
{
  const auto br = static_cast<std::uint64_t>(r() * 65535);
  const auto bg = static_cast<std::uint64_t>(g() * 65535);
  const auto bb = static_cast<std::uint64_t>(b() * 65535);
  const auto ba = static_cast<std::uint64_t>(a() * 65535);

  return static_cast<rgba64>((br << 48) | (bg << 32) | (bb << 16) | (ba));
}

alloy::core::argb64 alloy::core::color::to_argb64()
  const noexcept
{
  const auto br = static_cast<std::uint64_t>(r() * 65535);
  const auto bg = static_cast<std::uint64_t>(g() * 65535);
  const auto bb = static_cast<std::uint64_t>(b() * 65535);
  const auto ba = static_cast<std::uint64_t>(a() * 65535);

  return static_cast<argb64>((ba << 48) | (br << 32) | (bg << 16) | (bb));
}

alloy::core::abgr64 alloy::core::color::to_abgr64()
  const noexcept
{
  const auto br = static_cast<std::uint64_t>(r() * 65535);
  const auto bg = static_cast<std::uint64_t>(g() * 65535);
  const auto bb = static_cast<std::uint64_t>(b() * 65535);
  const auto ba = static_cast<std::uint64_t>(a() * 65535);

  return static_cast<abgr64>((ba << 48) | (bb << 32) | (bg << 16) | (br));
}

//------------------------------------------------------------------------------

alloy::core::hsb alloy::core::color::to_hsb()
  const noexcept
{
  const auto min_value = std::min(r(), std::min(g(), b()));
  const auto max_value = std::max(r(), std::max(g(), b()));
  const auto delta = max_value - min_value;

  const auto brightness = max_value;

  // If the difference is nearly 0, must be gray
  if (almost_equal(delta, real{0})) {
    return hsb{ real{0}, real{0}, brightness };
  }

  const auto saturation = delta / max_value;
  const auto half_delta = delta / real{2};

  const auto delta_r = (((max_value - r()) / real{6}) + half_delta) / delta;
  const auto delta_g = (((max_value - g()) / real{6}) + half_delta) / delta;
  const auto delta_b = (((max_value - b()) / real{6}) + half_delta) / delta;

  static constexpr auto one_third = real{1} / real{3};
  static constexpr auto two_third = real{2} / real{3};

  auto hue = real{0};
  if (almost_equal(r(), max_value)) {
    hue = delta_b - delta_g;
  } else if (almost_equal(g(), max_value)) {
    hue = one_third + delta_r - delta_b;
  } else if (almost_equal(b(), max_value)) {
    hue = two_third + delta_g - delta_r;
  }

  if (hue < real{0}) {
    hue += real{1};
  } else if (hue > real{1}) {
    hue -= real{1};
  }

  return hsb{hue, saturation, brightness};
}