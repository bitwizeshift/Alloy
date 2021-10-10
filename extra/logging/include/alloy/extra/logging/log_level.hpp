/*****************************************************************************
 * \file log_Level.hpp
 *
 * \brief This header defines the log-level enum used for logging specific
 *        levels of messages
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_EXTRA_LOGGING_LOG_LEVEL_HPP
#define ALLOY_EXTRA_LOGGING_LOG_LEVEL_HPP

#include "alloy/core/types.hpp"


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

#endif /* ALLOY_EXTRA_LOGGING_LOG_LEVEL_HPP */