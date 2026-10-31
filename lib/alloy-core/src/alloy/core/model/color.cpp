// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/model/color.hpp"

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/math.hpp"

#include <algorithm> // std::min, std::max
#include <cmath>     // std::pow

namespace {

  // The piecewise sRGB transfer functions from IEC 61966-2-1, which are
  // linear near black and a power curve elsewhere.

  auto srgb_to_linear(float c) noexcept -> float
  {
    if (c <= 0.04045f) {
      return c / 12.92f;
    }
    return std::pow((c + 0.055f) / 1.055f, 2.4f);
  }

  auto linear_to_srgb(float c) noexcept -> float
  {
    if (c <= 0.0031308f) {
      return c * 12.92f;
    }
    return 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
  }

} // namespace

//==============================================================================
// class : color
//==============================================================================

//------------------------------------------------------------------------------

auto alloy::core::color::from_hsb(hsb c) noexcept -> color
{
  const auto hue        = mod(c.hue, 1.0f);
  const auto saturation = saturate(c.saturation);
  const auto brightness = saturate(c.brightness);

  // 0 brightness means: must be black
  if (almost_equal(brightness, 0.0f)) {
    return {0.0f,0.0f,0.0f};
  }

  // 0 saturation means: must be gray
  if (almost_equal(saturation, 0.0f)) {
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
  auto hue_domain  = hue * float{total_domains};
  if (hue_domain >= float{total_domains}) {
    hue_domain = 0.0f;
  }

  const auto domain = static_cast<unsigned short>(hue_domain);
  const auto domain_offset = hue_domain - static_cast<float>(domain);

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

auto alloy::core::color::invert() noexcept -> void
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = 1.0f - m_data[i];
  }
}

auto alloy::core::color::contrast() noexcept -> void
{
  static constexpr auto half = 1.0f / 2.0f;

  for (auto i = 0; i < 3; ++i) {
    m_data[i] = mod(m_data[i] + half, 1.0f);
  }
}

auto alloy::core::color::darken(float percent) noexcept -> void
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] * (1.0f - percent);
  }
}

auto alloy::core::color::brighten(float percent) noexcept -> void
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] + (1.0f - m_data[i]) * percent;
  }
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::core::color::to_hsb() const noexcept -> hsb
{
  const auto min_value = std::min(r(), std::min(g(), b()));
  const auto max_value = std::max(r(), std::max(g(), b()));
  const auto delta     = max_value - min_value;

  const auto brightness = max_value;

  // If the difference is nearly 0, must be gray
  if (almost_equal(delta, 0.0f)) {
    return hsb{ 0.0f, 0.0f, brightness };
  }

  const auto saturation = delta / max_value;
  const auto half_delta = delta / 2.0f;

  const auto delta_r = (((max_value - r()) / 6.0f) + half_delta) / delta;
  const auto delta_g = (((max_value - g()) / 6.0f) + half_delta) / delta;
  const auto delta_b = (((max_value - b()) / 6.0f) + half_delta) / delta;

  static constexpr auto one_third = 1.0f / 3.0f;
  static constexpr auto two_third = 2.0f / 3.0f;

  auto hue = 0.0f;
  if (almost_equal(r(), max_value)) {
    hue = delta_b - delta_g;
  } else if (almost_equal(g(), max_value)) {
    hue = one_third + delta_r - delta_b;
  } else if (almost_equal(b(), max_value)) {
    hue = two_third + delta_g - delta_r;
  }

  if (hue < 0.0f) {
    hue += 1.0f;
  } else if (hue > 1.0f) {
    hue -= 1.0f;
  }

  return hsb{hue, saturation, brightness};
}

auto alloy::core::color::to_linear() const noexcept -> color
{
  return color{
    srgb_to_linear(r()),
    srgb_to_linear(g()),
    srgb_to_linear(b()),
    a()
  };
}

auto alloy::core::color::to_srgb() const noexcept -> color
{
  return color{
    linear_to_srgb(r()),
    linear_to_srgb(g()),
    linear_to_srgb(b()),
    a()
  };
}
