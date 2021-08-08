/*****************************************************************************
 * \file types.hpp
 * 
 * \brief This header defines the base types used in Alloy
 *****************************************************************************/

/*
 The MIT License (MIT)
 Copyright (c) 2021 Matthew Rodusek. All rights reserved.
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the \Software\), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED \AS IS\ WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/
#ifndef ALLOY_CORE_TYPES_HPP
#define ALLOY_CORE_TYPES_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <cstdint>     // std::uint8_t, etc

namespace alloy::core {

  //----------------------------------------------------------------------------
  // Fixed Sizes
  //----------------------------------------------------------------------------

  using u8  = std::uint8_t;
  using s8  = std::int8_t;
  using u16 = std::uint16_t;
  using s16 = std::int16_t;
  using u32 = std::uint32_t;
  using s32 = std::int32_t;
  using u64 = std::uint64_t;
  using s64 = std::int64_t;

} // namespace alloy::core

#endif /* ALLOY_CORE_TYPES_HPP */
