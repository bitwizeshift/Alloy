// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/****************************************************************************
 * \file matrix.hpp
 *
 * \brief This is a convenience header for including all of the various matrix
 *        subsystem types
 ****************************************************************************/

#ifndef ALLOY_CORE_MATH_MATRIX_HPP
#define ALLOY_CORE_MATH_MATRIX_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/matrix/matrix2.hpp" // IWYU pragma: export
#include "alloy/core/math/matrix/matrix3.hpp" // IWYU pragma: export
#include "alloy/core/math/matrix/matrix4.hpp" // IWYU pragma: export
#include "alloy/core/math/matrix/matrix_utilities.hpp" // IWYU pragma: export

#endif /* ALLOY_CORE_MATH_MATRIX_HPP */
