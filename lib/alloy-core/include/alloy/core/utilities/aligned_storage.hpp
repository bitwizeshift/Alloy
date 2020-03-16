/*****************************************************************************
 * \file aligned_storage.hpp
 *
 * \brief This header defines the aligned_storage utility that is part of
 *        the C++ standard.
 *****************************************************************************/

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
#ifndef ALLOY_CORE_UTILITIES_ALIGNED_STORAGE_HPP
#define ALLOY_CORE_UTILITIES_ALIGNED_STORAGE_HPP

#include <cstddef>

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An simple utility for providing a contiguous sequence of bytes
  ///        aligned to an 'Align' byte boundary.
  ///
  /// Unlike std::aligned_storage, this type doesn't require the _t accessor
  /// or ::type subtype. The aligned_storage struct is already a proper
  /// alignment for having objects directly constructed into it.
  ///
  /// \note This type exists thanks to MSVC's terrible implementation of the
  ///       standard library. Their std::aligned_storage does not honor
  ///       alignments above alignof(max_align_t) unless a specific symbol is
  ///       defined prior to including it. This is extremely painful when
  ///       used in headers, since ALL inclusions of type_traits must have this
  ///       defined first -- either requiring the consumers to provide this as
  ///       a '-D' on command-line (which is a huge requirement), or to only
  ///       ensure that <type_traits> is only ever included *after* the defined
  ///       symbol, even with all transitive inclusions -- which is just painful
  ///       Its easier to work around this by just defining the type in this
  ///       library.
  ///
  /// \tparam Size the size of the buffer
  /// \tparam Align the alignment boundary
  //////////////////////////////////////////////////////////////////////////////
  template <std::size_t Size, std::size_t Align = alignof(std::max_align_t)>
  struct aligned_storage
  {
    alignas(Align) unsigned char data[Size];
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_UTILITIES_ALIGNED_STORAGE_HPP */
