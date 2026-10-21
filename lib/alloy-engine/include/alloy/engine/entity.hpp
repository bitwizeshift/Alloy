// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file entity.hpp
 *
 * \todo
 *****************************************************************************/

#ifndef ALLOY_ENGINE_ENTITY_HPP
#define ALLOY_ENGINE_ENTITY_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/engine/config.hpp"

#include <cstddef> // std::size_T
#include <functional> // std::hash

namespace alloy::engine {

  //============================================================================
  // struct : entity
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An entity in the entity-component-system
  //////////////////////////////////////////////////////////////////////////////
  struct entity
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------

    using id_type = ALLOY_ENGINE_ENTITY_TYPE;

    //--------------------------------------------------------------------------
    // Members
    //--------------------------------------------------------------------------

    id_type index;
  };

  //============================================================================
  // functions : struct : entity
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparison Operators
  //----------------------------------------------------------------------------

  auto operator==(const entity& lhs, const entity& rhs) noexcept -> bool;
  auto operator!=(const entity& lhs, const entity& rhs) noexcept -> bool;
  auto operator<(const entity& lhs, const entity& rhs) noexcept -> bool;
  auto operator>(const entity& lhs, const entity& rhs) noexcept -> bool;
  auto operator<=(const entity& lhs, const entity& rhs) noexcept -> bool;
  auto operator>=(const entity& lhs, const entity& rhs) noexcept -> bool;

} // namespace alloy::engine


//==============================================================================
// definitions : struct : entity
//==============================================================================

//------------------------------------------------------------------------------
// Comparison Operators
//------------------------------------------------------------------------------

inline
auto alloy::engine::operator==(const entity& lhs, const entity& rhs)
  noexcept -> bool
{
  return lhs.index == rhs.index;
}

inline
auto alloy::engine::operator!=(const entity& lhs, const entity& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

inline
auto alloy::engine::operator<(const entity& lhs, const entity& rhs)
  noexcept -> bool
{
  return lhs.index < rhs.index;
}

inline
auto alloy::engine::operator>(const entity& lhs, const entity& rhs)
  noexcept -> bool
{
  return rhs < lhs;
}

inline
auto alloy::engine::operator<=(const entity& lhs, const entity& rhs)
  noexcept -> bool
{
  return !(rhs < lhs);
}

inline
auto alloy::engine::operator>=(const entity& lhs, const entity& rhs)
  noexcept -> bool
{
  return !(lhs < rhs);
}

namespace std {
  template <>
  struct hash<alloy::engine::entity>
  {
    auto operator()(const alloy::engine::entity& e)
      const noexcept -> std::size_t
    {
      return static_cast<std::size_t>(e.index);
    }
  };
} // namespace std

#endif /* ALLOY_ENGINE_ENTITY_HPP */
