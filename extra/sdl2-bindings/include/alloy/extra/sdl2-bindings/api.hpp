/*****************************************************************************
 * \file api.hpp
 *
 * \brief This header contains the import/export API macro guards for the
 *        Alloy::Extra::SDL2Bindings library
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
#ifndef ALLOY_EXTRA_SDL2_BINDINGS_API_HPP
#define ALLOY_EXTRA_SDL2_BINDINGS_API_HPP

#ifdef ALLOY_EXTRA_SDL2_API_EXPORT
# error "ALLOY_EXTRA_SDL2_API_EXPORT defined before inclusion of the config header"
#endif
#ifdef ALLOY_EXTRA_SDL2_API_IMPORT
# error "ALLOY_EXTRA_SDL2_API_IMPORT defined before inclusion of the config header"
#endif
#ifdef ALLOY_EXTRA_SDL2_API
# error "ALLOY_EXTRA_SDL2_API defined before inclusion of the config header"
#endif

#if defined(ALLOY_EXTRA_SDL2_EXPORT) && defined(ALLOY_EXTRA_SDL2_STATIC)
# error "ALLOY_EXTRA_SDL2_EXPORT and ALLOY_EXTRA_SDL2_STATIC cannot both be set"
#endif

//! \def ALLOY_EXTRA_SDL2_API_EXPORT
//!
//! \brief Portable symbol for exporting a symbol during build/link.

//! \def ALLOY_EXTRA_SDL2_API_IMPORT
//!
//! \brief Portable symbol for importing a symbol during consumption.

//! \def ALLOY_EXTRA_SDL2_API
//!
//! \brief Portable export/import macros used for building/consuming the
//!        library respectively.

#if defined(ALLOY_EXTRA_SDL2_STATIC)
# define ALLOY_EXTRA_SDL2_API_EXPORT
# define ALLOY_EXTRA_SDL2_API_IMPORT
#else
# if defined(SDL2) || defined(_SDL2)
#   define ALLOY_EXTRA_SDL2_API_EXPORT __declspec(dllexport)
#   define ALLOY_EXTRA_SDL2_API_IMPORT __declspec(dllimport)
#   ifdef _MSC_VER
#     pragma warning(disable: 4251)
#   endif
# elif defined(__GNUC__)
#   define ALLOY_EXTRA_SDL2_API_EXPORT __attribute__ ((__visibility__ ("default")))
#   define ALLOY_EXTRA_SDL2_API_IMPORT __attribute__ ((__visibility__ ("default")))
# endif
#endif

#if defined(ALLOY_EXTRA_SDL2_EXPORT)
# define ALLOY_EXTRA_SDL2_API ALLOY_EXTRA_SDL2_API_EXPORT
#else
# define ALLOY_EXTRA_SDL2_API ALLOY_EXTRA_SDL2_API_IMPORT
#endif

#endif /* ALLOY_EXTRA_SDL2_BINDINGS_API_HPP */
