// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/****************************************************************************
 * \file angle.hpp
 *
 * \brief This is a convenience header for including all of the various angle
 *        subsystem types
 ****************************************************************************/

#ifndef ALLOY_CORE_MATH_ANGLE_HPP
#define ALLOY_CORE_MATH_ANGLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/math/angle/basic_angle.hpp" // IWYU pragma: export
#include "alloy/core/math/angle/radian.hpp" // IWYU pragma: export
#include "alloy/core/math/angle/degree.hpp" // IWYU pragma: export
#include "alloy/core/math/angle/gradian.hpp" // IWYU pragma: export

#endif /* ALLOY_CORE_MATH_ANGLE_HPP */