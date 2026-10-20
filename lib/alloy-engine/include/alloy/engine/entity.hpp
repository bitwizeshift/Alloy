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

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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

  bool operator==(const entity& lhs, const entity& rhs) noexcept;
  bool operator!=(const entity& lhs, const entity& rhs) noexcept;
  bool operator<(const entity& lhs, const entity& rhs) noexcept;
  bool operator>(const entity& lhs, const entity& rhs) noexcept;
  bool operator<=(const entity& lhs, const entity& rhs) noexcept;
  bool operator>=(const entity& lhs, const entity& rhs) noexcept;

} // namespace alloy::engine


//==============================================================================
// definitions : struct : entity
//==============================================================================

//------------------------------------------------------------------------------
// Comparison Operators
//------------------------------------------------------------------------------

inline bool alloy::engine::operator==(const entity& lhs,
                                      const entity& rhs)
  noexcept
{
  return lhs.index == rhs.index;
}

inline bool alloy::engine::operator!=(const entity& lhs,
                                      const entity& rhs)
  noexcept
{
  return !(lhs == rhs);
}

inline bool alloy::engine::operator<(const entity& lhs,
                                     const entity& rhs)
  noexcept
{
  return lhs.index < rhs.index;
}

inline bool alloy::engine::operator>(const entity& lhs,
                                     const entity& rhs)
  noexcept
{
  return rhs < lhs;
}

inline bool alloy::engine::operator<=(const entity& lhs,
                                      const entity& rhs)
  noexcept
{
  return !(rhs < lhs);
}

inline bool alloy::engine::operator>=(const entity& lhs,
                                      const entity& rhs)
  noexcept
{
  return !(lhs < rhs);
}

namespace std {
  template <>
  struct hash<alloy::engine::entity>
  {
    std::size_t operator()(const alloy::engine::entity& e) const noexcept
    {
      return static_cast<std::size_t>(e.index);
    }
  };
} // namespace std

#endif /* ALLOY_ENGINE_ENTITY_HPP */