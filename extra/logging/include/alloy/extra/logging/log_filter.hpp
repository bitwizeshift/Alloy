/*****************************************************************************
 * \file log_filter.hpp
 *
 * \brief This header defines a filter for reducing log verbosity based on
 *        log levels
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
#ifndef ALLOY_EXTRA_LOGGING_LOG_FILTER_HPP
#define ALLOY_EXTRA_LOGGING_LOG_FILTER_HPP

#include "alloy/extra/logging/log_level.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/utilities/option_set.hpp"

namespace alloy::extra {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A filter for which log levels may be logged
  //////////////////////////////////////////////////////////////////////////////
  class log_filter
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Creates a log filter that only allows the specified \p levels
    ///
    /// \param levels the levels to create a log for
    /// \return the log_filter
    template <typename...LogLevels,
              typename = std::enable_if_t<(std::is_same_v<LogLevels,log_level> && ...)>>
    static constexpr auto only(LogLevels...levels) noexcept -> log_filter;

    /// \brief Creates a log filter that allows all logs
    ///
    /// \return the log_filter
    static constexpr auto all() noexcept -> log_filter;

    /// \brief Creates a lot filter that allows no logs
    ///
    /// \return the log_filter
    static constexpr auto none() noexcept -> log_filter;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    log_filter(const log_filter& other) = default;

    //--------------------------------------------------------------------------

    auto operator=(const log_filter& other) -> log_filter& = default;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Allows the specified log \p level in this filter
    ///
    /// \param level the level to allow
    /// \post `is_allowed(level)` returns `true`
    constexpr auto allow(log_level level) noexcept -> log_filter&;

    /// \brief Blocks the specified log \p level in this filter
    ///
    /// \param level the level to block
    /// \post `is_allowed(level)` returns `false`
    constexpr auto block(log_level level) noexcept -> log_filter&;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Queries whether the specified \p level is allowed
    ///
    /// \param level the level to query
    /// \return `true` if allowed
    [[nodiscard]]
    constexpr auto is_allowed(log_level level) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::option_set<log_level> m_allowed;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    explicit constexpr log_filter(core::option_set<log_level> allowed) noexcept;

  };

} // namespace alloy::extra

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

template <typename...LogLevels, typename>
inline constexpr
auto alloy::extra::log_filter::only(LogLevels...levels)
  noexcept -> log_filter
{
  return log_filter{
    core::option_set<log_level>{levels...}
  };
}

inline constexpr
auto alloy::extra::log_filter::all()
  noexcept -> log_filter
{
  return only(
    log_level::debug,
    log_level::info,
    log_level::warning,
    log_level::error,
    log_level::fatal
  );
}

inline constexpr
auto alloy::extra::log_filter::none()
  noexcept -> log_filter
{
  return only();
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline constexpr
auto alloy::extra::log_filter::allow(log_level level)
  noexcept -> log_filter&
{
  m_allowed.set(level);
  return (*this);
}

inline constexpr
auto alloy::extra::log_filter::block(log_level level)
  noexcept -> log_filter&
{
  m_allowed.set(level, false);
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr
auto alloy::extra::log_filter::is_allowed(log_level level)
  const noexcept -> bool
{
  return m_allowed.test(level);
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::extra::log_filter::log_filter(core::option_set<log_level> allowed)
  noexcept
  : m_allowed{allowed}
{

}

#endif /* ALLOY_EXTRA_LOGGING_LOG_FILTER_HPP */