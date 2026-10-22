// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file frame_clock.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_ENGINE_FRAME_CLOCK_HPP
#define ALLOY_ENGINE_FRAME_CLOCK_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <cstddef>
#include <ratio>
#include "alloy/core/intrinsics.hpp"

#include <chrono> // std::chrono::...

namespace alloy::engine {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A steady clock used for frame-time calculations
  ///
  /// The `frame_clock` operates in terms of the system's `steady_clock`, but is
  /// instead written with a representation of `double` units -- to allow for
  /// better interoperability with integration logic.
  ///
  /// This makes computation of a frame computation trivial
  //////////////////////////////////////////////////////////////////////////////
  struct frame_clock
  {
    using underlying_clock = std::chrono::steady_clock;

    /// High-res time in units of seconds
    using duration   = std::chrono::duration<double>;
    using rep        = typename duration::rep;
    using period     = typename duration::period;
    using time_point = std::chrono::time_point<frame_clock, duration>;

    inline static constexpr bool is_steady = true;

    static auto now() noexcept -> time_point;
  };

  //============================================================================
  // static class : frame_clock_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of functional constants for representing frame times
  //////////////////////////////////////////////////////////////////////////////
  struct frame_clock_constants final
  {
    //--------------------------------------------------------------------------
    // Deleted Constructor / Destructor
    //--------------------------------------------------------------------------

    frame_clock_constants() = delete;
    ~frame_clock_constants() = delete;

    //--------------------------------------------------------------------------
    // Framerates
    //--------------------------------------------------------------------------

    /// \brief Creates a duration object with 1/N time representation (N hz)
    ///
    /// The returned object is in units of seconds
    ///
    /// \tparam N the number of hertz
    /// \param d the amount of time that passed
    /// \return the duration object
    template <std::size_t N>
    static constexpr auto hz(double d) noexcept -> frame_clock::duration;

    /// \{
    /// \brief Helper utility that create hertz objects with common periods
    ///
    /// \param d the amount of time that passed
    static constexpr auto hz30(double d) noexcept -> frame_clock::duration;
    static constexpr auto hz60(double d) noexcept -> frame_clock::duration;
    static constexpr auto hz100(double d) noexcept -> frame_clock::duration;
    static constexpr auto hz120(double d) noexcept -> frame_clock::duration;
    /// \}
  };

} // namespace alloy::engine

//==============================================================================
// class : frame_clock
//==============================================================================

ALLOY_FORCE_INLINE
auto alloy::engine::frame_clock::now() noexcept -> time_point
{
  // chrono will automatically perform the conversion on return
  using highres_duration = std::chrono::duration<double,underlying_clock::period>;
  using highres_time_point = std::chrono::time_point<frame_clock,highres_duration>;

  return highres_time_point{
    std::chrono::steady_clock::now().time_since_epoch()
  };
}

//==============================================================================
// static class : frame_clock_constants
//==============================================================================

template <std::size_t N>
ALLOY_FORCE_INLINE constexpr
auto alloy::engine::frame_clock_constants::hz(double d)
  noexcept -> frame_clock::duration
{
  using frame_duration = std::chrono::duration<double,std::ratio<1u,N>>;

  return frame_duration{d};
}

ALLOY_FORCE_INLINE constexpr
auto alloy::engine::frame_clock_constants::hz30(double d)
  noexcept -> frame_clock::duration
{
  return hz<30u>(d);
}

ALLOY_FORCE_INLINE constexpr
auto alloy::engine::frame_clock_constants::hz60(double d)
  noexcept -> frame_clock::duration
{
  return hz<60u>(d);
}

ALLOY_FORCE_INLINE constexpr
auto alloy::engine::frame_clock_constants::hz100(double d)
  noexcept -> frame_clock::duration
{
  return hz<100u>(d);
}

ALLOY_FORCE_INLINE constexpr
auto alloy::engine::frame_clock_constants::hz120(double d)
  noexcept -> frame_clock::duration
{
  return hz<120u>(d);
}

#endif /* ALLOY_ENGINE_FRAME_CLOCK_HPP */
