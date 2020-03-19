/*****************************************************************************
 * \file windows.hpp
 *
 * \brief An internal header used for including all required win32 types
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_EXTRA_WIN32_BINDINGS_WINDOWS_HPP
#define ALLOY_EXTRA_WIN32_BINDINGS_WINDOWS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef NOMINMAX
# define NOMINMAX 1
#endif
#ifndef VC_EXTRALEAN
# define VC_EXTRALEAN 1
#endif
#ifndef UNICODE
# define UNICODE 1
#endif

#if !defined(WINVER)
# define WINVER 0x0501
#endif

#if WINVER < 0x0501
# undef WINVER
# define WINVER 0x0501
#endif

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT 0x0501
#endif

#if _WIN32_WINNT < 0x0501
# undef _WIN32_WINNT
# define _WIN32_WINNT 0x0501
#endif

#define DIRECTINPUT_VERSION 0x0800

#define OEMRESOURCE

// IWYU pragma: begin_exports
#include <wctype.h>
#include <windows.h>
#include <windef.h>
#include <dinput.h>
#include <xinput.h>
#include <dbt.h>
// IWYU pragma: end_exports

#endif /* ALLOY_EXTRA_WIN32_BINDINGS_WINDOWS_HPP */