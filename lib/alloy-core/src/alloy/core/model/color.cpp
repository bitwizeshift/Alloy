#include "alloy/core/model/color.hpp"

#include "alloy/core/intrinsics.hpp"
#include <algorithm> // std::min, std::max

//==============================================================================
// class : color
//==============================================================================

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
  const auto domain_offset = hue_domain - static_cast<real>(domain);

  const auto f1 = brightness * (1 - saturation);
  const auto f2 = brightness * (1 - saturation * domain_offset);
  const auto f3 = brightness * (1 - saturation * (1 - domain_offset));

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
