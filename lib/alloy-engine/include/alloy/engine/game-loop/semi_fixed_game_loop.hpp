// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file semi_fixed_game_loop.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_ENGINE_GAME_LOOP_SEMI_FIXED_GAME_LOOP_HPP
#define ALLOY_ENGINE_GAME_LOOP_SEMI_FIXED_GAME_LOOP_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/engine/frame_clock.hpp"

#include <atomic>    // std::atomic
#include <algorithm> // std::min

namespace alloy::engine {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An implementation of a semi-fixed game loop
  ///
  /// This implementation uses the CRTP in order to call back to the derived
  /// type and avoid unnecessary virtual calls.
  ///
  /// The `Derived` type has some explicit criteria. Given:
  ///
  /// * `Derived& d`,
  /// * `const Derived& cd`, and
  /// * `Clock::duration t`
  ///
  /// the following must be satisfied:
  ///
  /// * `cd.frame_duration()` returns the desired time-step duration
  /// * `cd.max_frame_duration()` returns the max supported time-step duration
  /// * `d.update_input()` plumbs for input
  /// * `d.integrate(t, t)` updates the game state (first `t` is the elapsed
  ///                       game time, the second `t` is the time step)
  /// * `d.render()` renders the game state
  ///
  /// For more details on this approach, see
  /// https://gafferongames.com/post/fix_your_timestep/
  ///
  /// \tparam Derived The derived class that follows the CRTP
  /// \tparam Clock the underlying clock to use for timing
  //////////////////////////////////////////////////////////////////////////////
  template <typename Derived, typename Clock = frame_clock>
  class semi_fixed_game_loop
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using clock_type = Clock;
    using duration   = typename Clock::duration;
    using time_point = typename Clock::time_point;

    //--------------------------------------------------------------------------
    // Protected Constructors / Assignment
    //--------------------------------------------------------------------------
  protected:

    semi_fixed_game_loop() noexcept;
    semi_fixed_game_loop(const semi_fixed_game_loop&) = delete;
    semi_fixed_game_loop(semi_fixed_game_loop&&) = delete;

    auto operator=(const semi_fixed_game_loop&)
      -> semi_fixed_game_loop& = delete;
    auto operator=(semi_fixed_game_loop&&) -> semi_fixed_game_loop& = delete;

    //--------------------------------------------------------------------------
    // Execution
    //--------------------------------------------------------------------------
  public:

    /// \brief Starts this game loop
    ///
    /// This function will not return until `stop` is called
    auto run() -> void;

    /// \brief Stops this game loop
    auto stop() noexcept -> void;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    std::atomic<bool> m_is_running;
  };

} // namespace alloy::engine

//==============================================================================
// class : semi_fixed_game_loop
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename Derived, typename Clock>
inline
alloy::engine::semi_fixed_game_loop<Derived,Clock>::semi_fixed_game_loop()
  noexcept
  : m_is_running{false}
{
}

//------------------------------------------------------------------------------
// Execution
//------------------------------------------------------------------------------

template <typename Derived, typename Clock>
inline
auto alloy::engine::semi_fixed_game_loop<Derived,Clock>::run() -> void
{
  m_is_running.store(true);

  static constexpr auto zero_time = duration{0};

  auto& self = static_cast<Derived&>(*this);

  const auto dt = self.frame_duration();
  const auto max_time = self.max_frame_duration();
  auto elapsed = zero_time;

  auto previous_time = Clock::now();

  while (m_is_running.load()) {
    self.update_input();

    const auto current_time = Clock::now();
    auto frame_time = (current_time - previous_time);
    previous_time = current_time;

    if (frame_time > max_time) {
      frame_time = max_time;
    }

    while (frame_time > zero_time) {
      const auto delta = ((frame_time < dt) ? frame_time : dt);

      self.integrate(elapsed, delta);
      frame_time -= delta;
      elapsed += delta;
    }

    self.render();
  }
}

template <typename Derived, typename Clock>
inline
auto alloy::engine::semi_fixed_game_loop<Derived,Clock>::stop() noexcept -> void
{
  m_is_running.store(false);
}

#endif /* ALLOY_ENGINE_GAME_LOOP_SEMI_FIXED_GAME_LOOP_HPP */
