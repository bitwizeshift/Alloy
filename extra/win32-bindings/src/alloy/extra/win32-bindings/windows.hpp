// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file windows.hpp
 *
 * \brief An internal header used for including all required win32 types
 *****************************************************************************/

#ifndef ALLOY_EXTRA_WIN32_BINDINGS_WINDOWS_HPP
#define ALLOY_EXTRA_WIN32_BINDINGS_WINDOWS_HPP

#ifdef _MSC_VER
# pragma once
#endif

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
#include <Windows.h>
#include <windef.h>
#include <dinput.h>
#include <Xinput.h>
#include <Dbt.h>
// IWYU pragma: end_exports

#endif /* ALLOY_EXTRA_WIN32_BINDINGS_WINDOWS_HPP */
