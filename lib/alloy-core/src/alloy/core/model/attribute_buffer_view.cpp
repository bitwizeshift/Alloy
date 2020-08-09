////////////////////////////////////////////////////////////////////////////////
/// \file real_array_view.cpp
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

#include "alloy/core/model/attribute_buffer_view.hpp"

#include "alloy/core/intrinsics.hpp"

// GCC is complaining that 'ALLOY_CORE_API' is ignored in this circumstance,
// but clang accepts this without any issues.
// This is being ignored for now, since we don't build with -fvisibility=private
// yet.
ALLOY_COMPILER_GCC_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GCC_DIAGNOSTIC_IGNORE(-Wattributes)

template class ALLOY_CORE_API alloy::core::attribute_buffer_view<alloy::core::vertex>;
template class ALLOY_CORE_API alloy::core::attribute_buffer_view<alloy::core::color>;
template class ALLOY_CORE_API alloy::core::attribute_buffer_view<alloy::core::vector3>;

ALLOY_COMPILER_GCC_DIAGNOSTIC_POP()
