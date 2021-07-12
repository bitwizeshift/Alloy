#include "alloy/core/model/color.hpp"

#include "alloy/core/intrinsics.hpp"
#include <algorithm> // std::min, std::max

//==============================================================================
// class : color
//==============================================================================

//------------------------------------------------------------------------------
// Static Member Functions
//------------------------------------------------------------------------------

auto alloy::core::color::from_rgba8(rgba8 c)
  noexcept -> color
{
  const auto hex = static_cast<std::uint32_t>(c);

  const auto r = (hex & 0xFF000000u) >> 24u;
  const auto g = (hex & 0x00FF0000u) >> 16u;
  const auto b = (hex & 0x0000FF00u) >>  8u;
  const auto a = (hex & 0x000000FFu);

  return from_rgba8(r,g,b,a);
}

auto alloy::core::color::from_argb8(argb8 c)
  noexcept -> color
{
  const auto hex = static_cast<std::uint32_t>(c);

  const auto a = (hex & 0xFF000000u) >> 24u;
  const auto r = (hex & 0x00FF0000u) >> 16u;
  const auto g = (hex & 0x0000FF00u) >>  8u;
  const auto b = (hex & 0x000000FFu);

  return from_rgba8(r,g,b,a);
}

auto alloy::core::color::from_abgr8(abgr8 c)
  noexcept -> color
{
  const auto hex = static_cast<std::uint32_t>(c);

  const auto a = (hex & 0xFF000000u) >> 24u;
  const auto b = (hex & 0x00FF0000u) >> 16u;
  const auto g = (hex & 0x0000FF00u) >>  8u;
  const auto r = (hex & 0x000000FFu);

  return from_rgba8(r,g,b,a);
}

auto alloy::core::color::from_rgba16(rgba16 c)
  noexcept -> color
{
  const auto hex = static_cast<std::uint64_t>(c);

  const auto r = (hex & 0xFFFF000000000000u) >> 48u;
  const auto g = (hex & 0x0000FFFF00000000u) >> 32u;
  const auto b = (hex & 0x00000000FFFF0000u) >> 16u;
  const auto a = (hex & 0x000000000000FFFFu);

  return from_rgba16(r,g,b,a);
}

auto alloy::core::color::from_argb16(argb16 c)
  noexcept -> color
{
  const auto hex = static_cast<std::uint64_t>(c);

  const auto a = (hex & 0xFFFF000000000000u) >> 48u;
  const auto r = (hex & 0x0000FFFF00000000u) >> 32u;
  const auto g = (hex & 0x00000000FFFF0000u) >> 16u;
  const auto b = (hex & 0x000000000000FFFFu);

  return from_rgba16(r,g,b,a);
}

auto alloy::core::color::from_abgr16(abgr16 c)
  noexcept -> color
{
  const auto hex = static_cast<std::uint64_t>(c);

  const auto a = (hex & 0xFFFF000000000000u) >> 48u;
  const auto b = (hex & 0x0000FFFF00000000u) >> 32u;
  const auto g = (hex & 0x00000000FFFF0000u) >> 16u;
  const auto r = (hex & 0x000000000000FFFFu);

  return from_rgba16(r,g,b,a);
}

//------------------------------------------------------------------------------

auto alloy::core::color::from_hsb(hsb c)
  noexcept -> color
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
  const auto fdomain = static_cast<real>(domain);
  const auto f1 = brightness * (1 - saturation);
  const auto f2 = brightness * (1 - saturation * (hue_domain - fdomain));
  const auto f3 = brightness * (1 - saturation * (1 - (hue_domain - fdomain)));

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

auto alloy::core::color::invert()
  noexcept -> void
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = real{1} - m_data[i];
  }
}

auto alloy::core::color::contrast()
  noexcept -> void
{
  static constexpr auto half = real{1} / real{2};

  for (auto i = 0; i < 3; ++i) {
    m_data[i] = mod(m_data[i] + half, real{1});
  }
}

auto alloy::core::color::darken(real percent)
  noexcept -> void
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] * (real{1} - percent);
  }
}

auto alloy::core::color::brighten(real percent)
  noexcept -> void
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] + (real{1} - m_data[i]) * percent;
  }
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::core::color::to_rgba8()
  const noexcept -> rgba8
{
  const auto br = std::uint32_t{r8()};
  const auto bg = std::uint32_t{g8()};
  const auto bb = std::uint32_t{b8()};
  const auto ba = std::uint32_t{a8()};

  return static_cast<rgba8>((br << 24u) | (bg << 16u) | (bb << 8u) | (ba));
}

auto alloy::core::color::to_argb8()
  const noexcept-> argb8
{
  const auto br = std::uint32_t{r8()};
  const auto bg = std::uint32_t{g8()};
  const auto bb = std::uint32_t{b8()};
  const auto ba = std::uint32_t{a8()};

  return static_cast<argb8>((ba << 24u) | (br << 16u) | (bg << 8u) | (bb));
}

auto alloy::core::color::to_abgr8()
  const noexcept -> abgr8
{
  const auto br = std::uint32_t{r8()};
  const auto bg = std::uint32_t{g8()};
  const auto bb = std::uint32_t{b8()};
  const auto ba = std::uint32_t{a8()};

  return static_cast<abgr8>((ba << 24u) | (bb << 16u) | (bg << 8u) | (br));
}

auto alloy::core::color::to_rgba16()
  const noexcept -> rgba16
{
  const auto br = std::uint64_t{r16()};
  const auto bg = std::uint64_t{g16()};
  const auto bb = std::uint64_t{b16()};
  const auto ba = std::uint64_t{a16()};

  return static_cast<rgba16>((br << 48u) | (bg << 32u) | (bb << 16u) | (ba));
}

auto alloy::core::color::to_argb16()
  const noexcept -> argb16
{
  const auto br = std::uint64_t{r16()};
  const auto bg = std::uint64_t{g16()};
  const auto bb = std::uint64_t{b16()};
  const auto ba = std::uint64_t{a16()};

  return static_cast<argb16>((ba << 48u) | (br << 32u) | (bg << 16u) | (bb));
}

auto alloy::core::color::to_abgr16()
  const noexcept -> abgr16
{
  const auto br = std::uint64_t{r16()};
  const auto bg = std::uint64_t{g16()};
  const auto bb = std::uint64_t{b16()};
  const auto ba = std::uint64_t{a16()};

  return static_cast<abgr16>((ba << 48u) | (bb << 32u) | (bg << 16u) | (br));
}

//------------------------------------------------------------------------------

auto alloy::core::color::to_hsb()
  const noexcept -> hsb
{
  const auto min_value = std::min(r(), std::min(g(), b()));
  const auto max_value = std::max(r(), std::max(g(), b()));
  const auto delta     = max_value - min_value;

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
