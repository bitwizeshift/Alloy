// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file api.hpp
 *
 * \brief This header contains the import/export API macro guards for the
 *        Alloy::Core library
 *****************************************************************************/

#ifndef ALLOY_CORE_API_HPP
#define ALLOY_CORE_API_HPP

#ifdef ALLOY_CORE_API_EXPORT
# error "ALLOY_CORE_API_EXPORT defined before inclusion of the config header"
#endif
#ifdef ALLOY_CORE_API_IMPORT
# error "ALLOY_CORE_API_IMPORT defined before inclusion of the config header"
#endif
#ifdef ALLOY_CORE_API
# error "ALLOY_CORE_API defined before inclusion of the config header"
#endif

#if defined(ALLOY_CORE_EXPORT) && defined(ALLOY_CORE_STATIC)
# error "ALLOY_CORE_EXPORT and ALLOY_CORE_STATIC cannot both be set"
#endif

//! \def ALLOY_CORE_API_EXPORT
//!
//! \brief Portable symbol for exporting a symbol during build/link.

//! \def ALLOY_CORE_API_IMPORT
//!
//! \brief Portable symbol for importing a symbol during consumption.

//! \def ALLOY_CORE_API
//!
//! \brief Portable export/import macros used for building/consuming the
//!        library respectively.

#if defined(ALLOY_CORE_STATIC)
# define ALLOY_CORE_API_EXPORT
# define ALLOY_CORE_API_IMPORT
#else
# if defined(WIN32) || defined(_WIN32)
#   define ALLOY_CORE_API_EXPORT __declspec(dllexport)
#   define ALLOY_CORE_API_IMPORT __declspec(dllimport)
#   ifdef _MSC_VER
#     pragma warning(disable: 4251)
#   endif
# elif defined(__GNUC__)
#   define ALLOY_CORE_API_EXPORT __attribute__ ((__visibility__ ("default")))
#   define ALLOY_CORE_API_IMPORT __attribute__ ((__visibility__ ("default")))
# endif
#endif

#if defined(ALLOY_CORE_EXPORT)
# define ALLOY_CORE_API ALLOY_CORE_API_EXPORT
#else
# define ALLOY_CORE_API ALLOY_CORE_API_IMPORT
#endif

#endif /* ALLOY_CORE_API_HPP */