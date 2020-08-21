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
  /////////////////////////////////////////////////////////////////////////////
  struct color_constants
  {
    color_constants() = delete;
    ~color_constants() = delete;

    inline static constexpr auto black   = color{0.0, 0.0, 0.0};
    inline static constexpr auto red     = color{1.0, 0.0, 0.0};
    inline static constexpr auto green   = color{0.0, 1.0, 0.0};
    inline static constexpr auto blue    = color{0.0, 0.0, 1.0};
    inline static constexpr auto yellow  = color{1.0, 1.0, 0.0};
    inline static constexpr auto cyan    = color{0.0, 1.0, 1.0};
    inline static constexpr auto magenta = color{1.0, 0.0, 1.0};
    inline static constexpr auto white   = color{1.0, 1.0, 1.0};

    inline static constexpr auto gray         = color{0.5, 0.5, 0.5};
    inline static constexpr auto dark_red     = color{0.5, 0.0, 0.0};
    inline static constexpr auto dark_green   = color{0.0, 0.5, 0.0};
    inline static constexpr auto dark_blue    = color{0.0, 0.0, 0.5};
    inline static constexpr auto dark_yellow  = color{0.5, 0.5, 0.0};
    inline static constexpr auto dark_cyan    = color{0.0, 0.5, 0.5};
    inline static constexpr auto dark_magenta = color{0.5, 0.0, 0.5};
  };

  //===========================================================================
  // static class : rgba32_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class for defining some of the basic / most common
  ///        named color objects, as 32-bit rgba colors.
  /////////////////////////////////////////////////////////////////////////////
  struct rgba32_constants
  {
    inline static constexpr auto black   = rgba32{0x000000FF};
    inline static constexpr auto red     = rgba32{0xFF0000FF};
    inline static constexpr auto green   = rgba32{0x00FF00FF};
    inline static constexpr auto blue    = rgba32{0x0000FFFF};
    inline static constexpr auto yellow  = rgba32{0xFFFF00FF};
    inline static constexpr auto cyan    = rgba32{0x00FFFFFF};
    inline static constexpr auto magenta = rgba32{0xFF00FFFF};
    inline static constexpr auto white   = rgba32{0xFFFFFFFF};

    inline static constexpr auto gray         = rgba32{0x000000FF};
    inline static constexpr auto dark_red     = rgba32{0x800000FF};
    inline static constexpr auto dark_green   = rgba32{0x008000FF};
    inline static constexpr auto dark_blue    = rgba32{0x000080FF};
    inline static constexpr auto dark_yellow  = rgba32{0x808000FF};
    inline static constexpr auto dark_cyan    = rgba32{0x008080FF};
    inline static constexpr auto dark_magenta = rgba32{0x800080FF};
  };

  //===========================================================================
  // static class : argb32_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class for defining some of the basic / most common
  ///        named color objects, as 32-bit argb colors.
  /////////////////////////////////////////////////////////////////////////////
  struct argb32_constants
  {
    inline static constexpr auto black   = rgba32{0xFF000000};
    inline static constexpr auto red     = rgba32{0xFFFF0000};
    inline static constexpr auto green   = rgba32{0xFF00FF00};
    inline static constexpr auto blue    = rgba32{0xFF0000FF};
    inline static constexpr auto yellow  = rgba32{0xFFFFFF00};
    inline static constexpr auto cyan    = rgba32{0xFF00FFFF};
    inline static constexpr auto magenta = rgba32{0xFFFF00FF};
    inline static constexpr auto white   = rgba32{0xFFFFFFFF};

    inline static constexpr auto gray         = rgba32{0xFF000000};
    inline static constexpr auto dark_red     = rgba32{0xFF800000};
    inline static constexpr auto dark_green   = rgba32{0xFF008000};
    inline static constexpr auto dark_blue    = rgba32{0xFF000080};
    inline static constexpr auto dark_yellow  = rgba32{0xFF808000};
    inline static constexpr auto dark_cyan    = rgba32{0xFF008080};
    inline static constexpr auto dark_magenta = rgba32{0xFF800080};
  };

  //===========================================================================
  // static class : abgr32_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class for defining some of the basic / most common
  ///        named color objects, as 32-bit abgr colors.
  /////////////////////////////////////////////////////////////////////////////
  struct abgr32_constants
  {
    inline static constexpr auto black   = rgba32{0xFF000000};
    inline static constexpr auto red     = rgba32{0xFF0000FF};
    inline static constexpr auto green   = rgba32{0xFF00FF00};
    inline static constexpr auto blue    = rgba32{0xFFFF0000};
    inline static constexpr auto yellow  = rgba32{0xFF00FFFF};
    inline static constexpr auto cyan    = rgba32{0xFFFFFF00};
    inline static constexpr auto magenta = rgba32{0xFFFF00FF};
    inline static constexpr auto white   = rgba32{0xFFFFFFFF};

    inline static constexpr auto gray         = rgba32{0xFF000000};
    inline static constexpr auto dark_red     = rgba32{0xFF000080};
    inline static constexpr auto dark_green   = rgba32{0xFF008000};
    inline static constexpr auto dark_blue    = rgba32{0xFF800000};
    inline static constexpr auto dark_yellow  = rgba32{0xFF008080};
    inline static constexpr auto dark_cyan    = rgba32{0xFF808000};
    inline static constexpr auto dark_magenta = rgba32{0xFF800080};
  };

  //===========================================================================
  // static class : rgba64_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class for defining some of the basic / most common
  ///        named color objects, as 64-bit rgba colors.
  /////////////////////////////////////////////////////////////////////////////
  struct rgba64_constants
  {
    inline static constexpr auto black   = rgba64{0x000000000000FFFFULL};
    inline static constexpr auto red     = rgba64{0xFFFF00000000FFFFULL};
    inline static constexpr auto green   = rgba64{0x00FFFF000000FFFFULL};
    inline static constexpr auto blue    = rgba64{0x00000000FFFFFFFFULL};
    inline static constexpr auto yellow  = rgba64{0xFFFFFFFF0000FFFFULL};
    inline static constexpr auto cyan    = rgba64{0x0000FFFFFFFFFFFFULL};
    inline static constexpr auto magenta = rgba64{0xFFFF0000FFFFFFFFULL};
    inline static constexpr auto white   = rgba64{0xFFFFFFFFFFFFFFFFULL};

    inline static constexpr auto gray         = rgba64{0x000000000000FFFFULL};
    inline static constexpr auto dark_red     = rgba64{0x808000000000FFFFULL};
    inline static constexpr auto dark_green   = rgba64{0x000080800000FFFFULL};
    inline static constexpr auto dark_blue    = rgba64{0x000000008080FFFFULL};
    inline static constexpr auto dark_yellow  = rgba64{0x808080800000FFFFULL};
    inline static constexpr auto dark_cyan    = rgba64{0x000080808080FFFFULL};
    inline static constexpr auto dark_magenta = rgba64{0x808000008080FFFFULL};
  };

  //===========================================================================
  // static class : argb64_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class for defining some of the basic / most common
  ///        named color objects, as 64-bit argb colors.
  /////////////////////////////////////////////////////////////////////////////
  struct argb64_constants
  {
    inline static constexpr auto black   = rgba64{0xFFFF000000000000ULL};
    inline static constexpr auto red     = rgba64{0xFFFFFFFF00000000ULL};
    inline static constexpr auto green   = rgba64{0xFFFF00FFFF000000ULL};
    inline static constexpr auto blue    = rgba64{0xFFFF00000000FFFFULL};
    inline static constexpr auto yellow  = rgba64{0xFFFFFFFFFFFF0000ULL};
    inline static constexpr auto cyan    = rgba64{0xFFFF0000FFFFFFFFULL};
    inline static constexpr auto magenta = rgba64{0xFFFFFFFF0000FFFFULL};
    inline static constexpr auto white   = rgba64{0xFFFFFFFFFFFFFFFFULL};

    inline static constexpr auto gray         = rgba64{0xFFFF000000000000ULL};
    inline static constexpr auto dark_red     = rgba64{0xFFFF808000000000ULL};
    inline static constexpr auto dark_green   = rgba64{0xFFFF000080800000ULL};
    inline static constexpr auto dark_blue    = rgba64{0xFFFF000000008080ULL};
    inline static constexpr auto dark_yellow  = rgba64{0xFFFF808080800000ULL};
    inline static constexpr auto dark_cyan    = rgba64{0xFFFF000080808080ULL};
    inline static constexpr auto dark_magenta = rgba64{0xFFFF808000008080ULL};
  };

  //===========================================================================
  // static class : abgr64_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class for defining some of the basic / most common
  ///        named color objects, as 64-bit abgr colors.
  /////////////////////////////////////////////////////////////////////////////
  struct abgr64_constants
  {
    inline static constexpr auto black   = rgba64{0xFFFF000000000000ULL};
    inline static constexpr auto red     = rgba64{0xFFFF00000000FFFFULL};
    inline static constexpr auto green   = rgba64{0xFFFF0000FFFF0000ULL};
    inline static constexpr auto blue    = rgba64{0xFFFFFFFF00000000ULL};
    inline static constexpr auto yellow  = rgba64{0xFFFF0000FFFFFFFFULL};
    inline static constexpr auto cyan    = rgba64{0xFFFFFFFFFFFF0000ULL};
    inline static constexpr auto magenta = rgba64{0xFFFFFFFF0000FFFFULL};
    inline static constexpr auto white   = rgba64{0xFFFFFFFFFFFFFFFFULL};

    inline static constexpr auto gray         = rgba64{0xFFFF000000000000ULL};
    inline static constexpr auto dark_red     = rgba64{0xFFFF000000008080ULL};
    inline static constexpr auto dark_green   = rgba64{0xFFFF000080800000ULL};
    inline static constexpr auto dark_blue    = rgba64{0xFFFF808000000000ULL};
    inline static constexpr auto dark_yellow  = rgba64{0xFFFF000080808080ULL};
    inline static constexpr auto dark_cyan    = rgba64{0xFFFF808080800000ULL};
    inline static constexpr auto dark_magenta = rgba64{0xFFFF808000008080ULL};
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MODEL_COLOR_CONSTANTS_HPP */
