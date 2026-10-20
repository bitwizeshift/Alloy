// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file mixins.hpp
 *
 * \brief This header defines various mixin utilities
 *****************************************************************************/

#ifndef ALLOY_CORE_UTILITIES_MIXINS_HPP
#define ALLOY_CORE_UTILITIES_MIXINS_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <cstddef>
#include <new>

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A mixin type for conditionally enabling move construction and
  ///        assignment if the condition is met.
  ///
  /// \tparam Condition the condition to check. Move is enabled if `true`.
  //////////////////////////////////////////////////////////////////////////////
  template <bool Condition>
  struct enable_move_if
  {
    enable_move_if() = default;
    enable_move_if(const enable_move_if&) = default;
    enable_move_if(enable_move_if&&) = delete;

    auto operator=(const enable_move_if&) -> enable_move_if& = default;
    auto operator=(enable_move_if&&) -> void = delete;
  };

  template <>
  struct enable_move_if<true>
  {
    enable_move_if() = default;
    enable_move_if(const enable_move_if&) = default;
    enable_move_if(enable_move_if&&) = default;

    auto operator=(const enable_move_if&) -> enable_move_if& = default;
    auto operator=(enable_move_if&&) -> enable_move_if& = default;
  };

  /// \brief A mixing to make the derived type immovable
  using immovable = enable_move_if<false>;

  //----------------------------------------------------------------------------

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A mixin type for conditionally enabling copy construction and
  ///        assignment if the condition is met.
  ///
  /// \tparam Condition the condition to check. Copy is enabled if `true`.
  //////////////////////////////////////////////////////////////////////////////
  template <bool Condition>
  struct enable_copy_if
  {
    enable_copy_if() = default;
    enable_copy_if(const enable_copy_if&) = delete;
    enable_copy_if(enable_copy_if&&) = default;

    auto operator=(const enable_copy_if&) -> void = delete;
    auto operator=(enable_copy_if&&) -> enable_copy_if& = default;
  };

  template <>
  struct enable_copy_if<true>
  {
    enable_copy_if() = default;
    enable_copy_if(const enable_copy_if&) = delete;
    enable_copy_if(enable_copy_if&&) = default;

    auto operator=(const enable_copy_if&) -> enable_copy_if& = default;
    auto operator=(enable_copy_if&&) -> enable_copy_if& = default;
  };

  /// \brief A mixin to make the derived type uncopyable
  using uncopyable = enable_copy_if<false>;

  //----------------------------------------------------------------------------

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A mixin type for disabling the destructor
  //////////////////////////////////////////////////////////////////////////////
  struct indestructible
  {
    ~indestructible() = delete;
  };

  //----------------------------------------------------------------------------

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A mixin type for preventing objects from being allocated on the
  ///        heap.
  //////////////////////////////////////////////////////////////////////////////
  struct unheapable
  {
    auto operator new(std::size_t) -> void* = delete;
    auto operator new[](std::size_t) -> void* = delete;
    auto operator new(std::size_t, std::align_val_t) -> void* = delete;
    auto operator new[](std::size_t, std::align_val_t) -> void* = delete;

    template <typename...Args>
    auto operator new(std::size_t,
                      Args&&...) -> void* = delete;
    template <typename...Args>
    auto operator new[](std::size_t,
                        Args&&...) -> void* = delete;
    template <typename...Args>
    auto operator new(std::size_t,
                      std::align_val_t,
                      Args&&...) -> void* = delete;
    template <typename...Args>
    auto operator new[](std::size_t,
                        std::align_val_t,
                        Args&&...) -> void* = delete;

    auto operator delete(void*) -> void = delete;
    auto operator delete[](void*) -> void = delete;
    auto operator delete(void*,
                         std::align_val_t) -> void = delete;
    auto operator delete[](void*,
                           std::align_val_t) -> void = delete;
    auto operator delete(void*,
                         std::size_t) -> void = delete;
    auto operator delete[](void*,
                           std::size_t) -> void = delete;
    auto operator delete(void*,
                         std::size_t,
                         std::align_val_t) -> void = delete;
    auto operator delete[](void*,
                           std::size_t,
                           std::align_val_t) -> void = delete;
    template <typename...Args>
    auto operator delete(void*,Args&&...) -> void = delete;
    template <typename...Args>
    auto operator delete[](void*,Args&&...) -> void = delete;

  };

  //----------------------------------------------------------------------------

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A mixin type for preventing a class from being instantiable.
  ///
  /// This should be used with pure static-class types to ensure that the
  /// object cannot be constructed in any other way.
  //////////////////////////////////////////////////////////////////////////////
  struct uninstantiable
    : unheapable, indestructible, uncopyable, immovable
  {
    uninstantiable() = delete;
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_UTILITIES_MIXINS_HPP */
