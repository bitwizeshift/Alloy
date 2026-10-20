// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file precision.hpp
 *
 * \brief This is a convenience header for including the various precision
 *        types supported by the alloy engine.
 *****************************************************************************/

#ifndef ALLOY_CORE_PRECISION_HPP
#define ALLOY_CORE_PRECISION_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/precision/fixed.hpp" // IWYU pragma: export
#include "alloy/core/precision/half.hpp" // IWYU pragma: export
#include "alloy/core/precision/real.hpp" // IWYU pragma: export

#endif /* ALLOY_CORE_PRECISION_HPP */