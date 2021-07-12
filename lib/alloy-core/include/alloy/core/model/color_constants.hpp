////////////////////////////////////////////////////////////////////////////////
/// \file color_constants.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef ALLOY_CORE_MODEL_COLOR_CONSTANTS_HPP
#define ALLOY_CORE_MODEL_COLOR_CONSTANTS_HPP

#include "alloy/core/model/color.hpp"

namespace alloy::core {

  //===========================================================================
  // static class : color_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class for defining some of the basic / most common
  ///        named color objects, as 4-component real colors.
  ///
  /// The names of the colors are taken from the CSS specification:
  /// https://www.w3.org/TR/2011/REC-SVG11-20110816/types.html#ColorKeywords
  ///
  /// The idea to use CSS for these names has been adopted from Amazon's
  /// O3DE, which does the same.
  /////////////////////////////////////////////////////////////////////////////
  struct color_constants
  {
    color_constants() = delete;
    ~color_constants() = delete;

    //--------------------------------------------------------------------------
    // CSS1 Colors
    //--------------------------------------------------------------------------

    inline static constexpr auto white   = color::from_rgb8(255u, 255u, 255u);
    inline static constexpr auto silver  = color::from_rgb8(192u, 192u, 192u);
    inline static constexpr auto gray    = color::from_rgb8(128u, 128u, 128u);
    inline static constexpr auto black   = color::from_rgb8(0u,   0u,   0u);
    inline static constexpr auto red     = color::from_rgb8(255u, 0u,   0u);
    inline static constexpr auto maroon  = color::from_rgb8(128u, 0u,   0u);
    inline static constexpr auto lime    = color::from_rgb8(0u,   255u, 0u);
    inline static constexpr auto green   = color::from_rgb8(0u,   128u, 0u);
    inline static constexpr auto blue    = color::from_rgb8(0u,   0u,   255u);
    inline static constexpr auto navy    = color::from_rgb8(0u,   0u,   128u);
    inline static constexpr auto yellow  = color::from_rgb8(255u, 255u, 0u);
    inline static constexpr auto orange  = color::from_rgb8(255u, 165u, 0u);
    inline static constexpr auto olive   = color::from_rgb8(128u, 128u, 0u);
    inline static constexpr auto purple  = color::from_rgb8(128u, 0u,   128u);
    inline static constexpr auto fuchsia = color::from_rgb8(255u, 0u,   255u);
    inline static constexpr auto teal    = color::from_rgb8(0u,   128u, 128u);
    inline static constexpr auto aqua    = color::from_rgb8(0u,   255u, 255u);

    //--------------------------------------------------------------------------
    // CSS3 colors
    //--------------------------------------------------------------------------

    // Reds

    inline static constexpr auto indian_red   = color::from_rgb8(205u, 92u, 92u);
    inline static constexpr auto light_coral  = color::from_rgb8(240u, 128u, 128u);
    inline static constexpr auto salmon       = color::from_rgb8(250u, 128u, 114u);
    inline static constexpr auto dark_salmon  = color::from_rgb8(233u, 150u, 122u);
    inline static constexpr auto light_salmon = color::from_rgb8(255u, 160u, 122u);
    inline static constexpr auto crimson      = color::from_rgb8(220u, 20u, 60u);
    inline static constexpr auto fire_brick   = color::from_rgb8(178u, 34u, 34u);
    inline static constexpr auto dark_red     = color::from_rgb8(139u, 0u, 0u);

    // Pinks

    inline static constexpr auto pink              = color::from_rgb8(255u, 192u, 203u);
    inline static constexpr auto light_pink        = color::from_rgb8(255u, 182u, 193u);
    inline static constexpr auto hot_pink          = color::from_rgb8(255u, 105u, 180u);
    inline static constexpr auto deep_pink         = color::from_rgb8(255u, 20u, 147u);
    inline static constexpr auto medium_violet_red = color::from_rgb8(199u, 21u, 133u);
    inline static constexpr auto pale_violet_red   = color::from_rgb8(219u, 112u, 147u);

    // Oranges

    inline static constexpr auto coral       = color::from_rgb8(255u, 127u, 80u);
    inline static constexpr auto tomato      = color::from_rgb8(255u, 99u, 71u);
    inline static constexpr auto orange_red  = color::from_rgb8(255u, 69u, 0u);
    inline static constexpr auto dark_orange = color::from_rgb8(255u, 140u, 0u);

    // Yellows

    inline static constexpr auto gold                   = color::from_rgb8(255u, 215u, 0u);
    inline static constexpr auto light_yellow           = color::from_rgb8(255u, 255u, 224u);
    inline static constexpr auto lemon_chiffon          = color::from_rgb8(255u, 250u, 205u);
    inline static constexpr auto light_goldenrod_yellow = color::from_rgb8(250u, 250u, 210u);
    inline static constexpr auto papaya_whip            = color::from_rgb8(255u, 239u, 213u);
    inline static constexpr auto moccasin               = color::from_rgb8(255u, 228u, 181u);
    inline static constexpr auto peach_puff             = color::from_rgb8(255u, 218u, 185u);
    inline static constexpr auto pale_goldenrod         = color::from_rgb8(238u, 232u, 170u);
    inline static constexpr auto khaki                  = color::from_rgb8(240u, 230u, 140u);
    inline static constexpr auto dark_khaki             = color::from_rgb8(189u, 183u, 107u);

    // Purples

    inline static constexpr auto lavender          = color::from_rgb8(230u, 230u, 250u);
    inline static constexpr auto thistle           = color::from_rgb8(216u, 191u, 216u);
    inline static constexpr auto plum              = color::from_rgb8(221u, 160u, 221u);
    inline static constexpr auto violet            = color::from_rgb8(238u, 130u, 238u);
    inline static constexpr auto orchid            = color::from_rgb8(218u, 112u, 214u);
    inline static constexpr auto magenta           = color::from_rgb8(255u, 0u, 255u);
    inline static constexpr auto medium_orchid     = color::from_rgb8(186u, 85u, 211u);
    inline static constexpr auto medium_purple     = color::from_rgb8(147u, 112u, 219u);
    inline static constexpr auto blue_violet       = color::from_rgb8(138u, 43u, 226u);
    inline static constexpr auto dark_violet       = color::from_rgb8(148u, 0u, 211u);
    inline static constexpr auto dark_orchid       = color::from_rgb8(153u, 50u, 204u);
    inline static constexpr auto dark_magenta      = color::from_rgb8(139u, 0u, 139u);
    inline static constexpr auto rebecca_prple     = color::from_rgb8(102u, 51u, 153u);
    inline static constexpr auto indigo            = color::from_rgb8(75u, 0u, 130u);
    inline static constexpr auto medium_slate_blue = color::from_rgb8(123u, 104u, 238u);
    inline static constexpr auto slate_blue        = color::from_rgb8(106u, 90u, 205u);
    inline static constexpr auto dark_slate_blue   = color::from_rgb8(72u, 61u, 139u);

    // Greens

    inline static constexpr auto greenYellow         = color::from_rgb8(173u, 255u, 47u);
    inline static constexpr auto chartreuse          = color::from_rgb8(127u, 255u, 0u);
    inline static constexpr auto lawn_green          = color::from_rgb8(124u, 252u, 0u);
    inline static constexpr auto lime_green          = color::from_rgb8(50u, 205u, 50u);
    inline static constexpr auto pale_green          = color::from_rgb8(152u, 251u, 152u);
    inline static constexpr auto light_green         = color::from_rgb8(144u, 238u, 144u);
    inline static constexpr auto medium_spring_green = color::from_rgb8(0u, 250u, 154u);
    inline static constexpr auto spring_green        = color::from_rgb8(0u, 255u, 127u);
    inline static constexpr auto medium_sea_green    = color::from_rgb8(60u, 179u, 113u);
    inline static constexpr auto sea_green           = color::from_rgb8(46u, 139u, 87u);
    inline static constexpr auto forest_green        = color::from_rgb8(34u, 139u, 34u);
    inline static constexpr auto dark_green          = color::from_rgb8(0u, 100u, 0u);
    inline static constexpr auto yellow_green        = color::from_rgb8(154u, 205u, 50u);
    inline static constexpr auto olive_drab          = color::from_rgb8(107u, 142u, 35u);
    inline static constexpr auto dark_olive_green    = color::from_rgb8(85u, 107u, 47u);
    inline static constexpr auto medium_qquamarine   = color::from_rgb8(102u, 205u, 170u);
    inline static constexpr auto dark_sea_green      = color::from_rgb8(143u, 188u, 143u);
    inline static constexpr auto light_sea_green     = color::from_rgb8(32u, 178u, 170u);
    inline static constexpr auto dark_cyan           = color::from_rgb8(0u, 139u, 139u);

    // Blues

    inline static constexpr auto cyan             = color::from_rgb8(0u, 255u, 255u);
    inline static constexpr auto light_cyan       = color::from_rgb8(224u, 255u, 255u);
    inline static constexpr auto pale_turquoise   = color::from_rgb8(175u, 238u, 238u);
    inline static constexpr auto aquamarine       = color::from_rgb8(127u, 255u, 212u);
    inline static constexpr auto turquoise        = color::from_rgb8(64u, 224u, 208u);
    inline static constexpr auto medium_turquoise = color::from_rgb8(72u, 209u, 204u);
    inline static constexpr auto dark_turquoise   = color::from_rgb8(0u, 206u, 209u);
    inline static constexpr auto cadet_blue       = color::from_rgb8(95u, 158u, 160u);
    inline static constexpr auto steel_blue       = color::from_rgb8(70u, 130u, 180u);
    inline static constexpr auto light_steel_blue = color::from_rgb8(176u, 196u, 222u);
    inline static constexpr auto powder_blue      = color::from_rgb8(176u, 224u, 230u);
    inline static constexpr auto light_blue       = color::from_rgb8(173u, 216u, 230u);
    inline static constexpr auto sky_blue         = color::from_rgb8(135u, 206u, 235u);
    inline static constexpr auto light_sky_blue   = color::from_rgb8(135u, 206u, 250u);
    inline static constexpr auto deep_sky_blue    = color::from_rgb8(0u, 191u, 255u);
    inline static constexpr auto dodger_blue      = color::from_rgb8(30u, 144u, 255u);
    inline static constexpr auto cornflower_blue  = color::from_rgb8(100u, 149u, 237u);
    inline static constexpr auto royal_blue       = color::from_rgb8(65u, 105u, 225u);
    inline static constexpr auto medium_blue      = color::from_rgb8(0u, 0u, 205u);
    inline static constexpr auto dark_blue        = color::from_rgb8(0u, 0u, 139u);
    inline static constexpr auto midnight_blue    = color::from_rgb8(25u, 25u, 112u);

    // Browns

    inline static constexpr auto cornsilk        = color::from_rgb8(255u, 248u, 220u);
    inline static constexpr auto blanched_almond = color::from_rgb8(255u, 235u, 205u);
    inline static constexpr auto bisque          = color::from_rgb8(255u, 228u, 196u);
    inline static constexpr auto navajo_white    = color::from_rgb8(255u, 222u, 173u);
    inline static constexpr auto wheat           = color::from_rgb8(245u, 222u, 179u);
    inline static constexpr auto burly_wood      = color::from_rgb8(222u, 184u, 135u);
    inline static constexpr auto tan             = color::from_rgb8(210u, 180u, 140u);
    inline static constexpr auto rosy_brown      = color::from_rgb8(188u, 143u, 143u);
    inline static constexpr auto sandy_brown     = color::from_rgb8(244u, 164u, 96u);
    inline static constexpr auto goldenrod       = color::from_rgb8(218u, 165u, 32u);
    inline static constexpr auto dark_goldenrod  = color::from_rgb8(184u, 134u, 11u);
    inline static constexpr auto peru            = color::from_rgb8(205u, 133u, 63u);
    inline static constexpr auto chocolate       = color::from_rgb8(210u, 105u, 30u);
    inline static constexpr auto saddleBrown     = color::from_rgb8(139u, 69u, 19u);
    inline static constexpr auto sienna          = color::from_rgb8(160u, 82u, 45u);
    inline static constexpr auto brown           = color::from_rgb8(165u, 42u, 42u);

    // Whites

    inline static constexpr auto snow           = color::from_rgb8(255u, 250u, 250u);
    inline static constexpr auto honeydew       = color::from_rgb8(240u, 255u, 240u);
    inline static constexpr auto mint_cream     = color::from_rgb8(245u, 255u, 250u);
    inline static constexpr auto azure          = color::from_rgb8(240u, 255u, 255u);
    inline static constexpr auto alice_blue     = color::from_rgb8(240u, 248u, 255u);
    inline static constexpr auto ghost_white    = color::from_rgb8(248u, 248u, 255u);
    inline static constexpr auto white_smoke    = color::from_rgb8(245u, 245u, 245u);
    inline static constexpr auto seashell       = color::from_rgb8(255u, 245u, 238u);
    inline static constexpr auto beige          = color::from_rgb8(245u, 245u, 220u);
    inline static constexpr auto old_lace       = color::from_rgb8(253u, 245u, 230u);
    inline static constexpr auto floral_white   = color::from_rgb8(255u, 250u, 240u);
    inline static constexpr auto ivory          = color::from_rgb8(255u, 255u, 240u);
    inline static constexpr auto antique_white  = color::from_rgb8(250u, 235u, 215u);
    inline static constexpr auto linen          = color::from_rgb8(250u, 240u, 230u);
    inline static constexpr auto lavender_blush = color::from_rgb8(255u, 240u, 245u);
    inline static constexpr auto misty_rose     = color::from_rgb8(255u, 228u, 225u);

    // Grays

    inline static constexpr auto gainsboro        = color::from_rgb8(220u, 220u, 220u);
    inline static constexpr auto light_gray       = color::from_rgb8(211u, 211u, 211u);
    inline static constexpr auto dark_gray        = color::from_rgb8(169u, 169u, 169u);
    inline static constexpr auto dim_gray         = color::from_rgb8(105u, 105u, 105u);
    inline static constexpr auto light_slate_gray = color::from_rgb8(119u, 136u, 153u);
    inline static constexpr auto slate_gray       = color::from_rgb8(112u, 128u, 144u);
    inline static constexpr auto dark_slate_gray  = color::from_rgb8(47u, 79u, 79u);
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MODEL_COLOR_CONSTANTS_HPP */
