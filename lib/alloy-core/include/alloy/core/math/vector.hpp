// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/****************************************************************************
 * \file vector.hpp
 *
 * \brief This is a convenience header for including all of the various vector
 *        subsystem types
 ****************************************************************************/

#ifndef ALLOY_CORE_MATH_VECTOR_HPP
#define ALLOY_CORE_MATH_VECTOR_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/vector/vector2.hpp"           // IWYU pragma: export
#include "alloy/core/math/vector/vector2_constants.hpp" // IWYU pragma: export
#include "alloy/core/math/vector/vector3.hpp"           // IWYU pragma: export
#include "alloy/core/math/vector/vector3_constants.hpp" // IWYU pragma: export
#include "alloy/core/math/vector/vector4.hpp"           // IWYU pragma: export
#include "alloy/core/math/vector/vector4_constants.hpp" // IWYU pragma: export
#include "alloy/core/math/vector/vector_utilities.hpp"  // IWYU pragma: export

#endif /* ALLOY_CORE_MATH_VECTOR_HPP */
