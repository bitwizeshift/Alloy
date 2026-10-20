// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file log_Level.hpp
 *
 * \brief This header defines the log-level enum used for logging specific
 *        levels of messages
 *****************************************************************************/

#ifndef ALLOY_EXTRA_LOGGING_LOG_LEVEL_HPP
#define ALLOY_EXTRA_LOGGING_LOG_LEVEL_HPP

#include "alloy/core/types.hpp"
#include "alloy/core/utilities/enum_traits.hpp"

#include <type_traits> // std::true_type

namespace alloy::extra {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The severity of a log statement
  //////////////////////////////////////////////////////////////////////////////
  enum class log_level : core::u8
  {
    debug,   ///< Indicates a message for developer debugging
    info,    ///< Indicates a control flow message (for general info)
    warning, ///< Indicates a recoverable bad state occurred
    error,   ///< Indicates a recoverable error has occurred
    fatal,   ///< Indicates an unrecoverable error has occurred. Should be
             ///< followed by some form of program termination
  };

} // namespace alloy::extra

template <>
struct alloy::core::enum_is_contiguous<alloy::extra::log_level>
  : std::true_type {};

#endif /* ALLOY_EXTRA_LOGGING_LOG_LEVEL_HPP */
