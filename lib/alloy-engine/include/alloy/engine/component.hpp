// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file component.hpp
 *
 * \todo
 *****************************************************************************/

#ifndef ALLOY_ENGINE_COMPONENT_HPP
#define ALLOY_ENGINE_COMPONENT_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <cstddef> // std::size_t
#include <cstdint> // std::uint8_t
#include <functional> // std::hash

namespace alloy::engine {

  //============================================================================
  // struct : component
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A component in the entity-component-system
  //////////////////////////////////////////////////////////////////////////////
  struct component
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------

    using id_type = std::uint8_t;

    //--------------------------------------------------------------------------
    // Members
    //--------------------------------------------------------------------------

    id_type index;
  };

  //============================================================================
  // non-member functions : struct : component
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparison Operators
  //----------------------------------------------------------------------------

  bool operator==(const component& lhs, const component& rhs) noexcept;
  bool operator!=(const component& lhs, const component& rhs) noexcept;
  bool operator<(const component& lhs, const component& rhs) noexcept;
  bool operator>(const component& lhs, const component& rhs) noexcept;
  bool operator<=(const component& lhs, const component& rhs) noexcept;
  bool operator>=(const component& lhs, const component& rhs) noexcept;

} // namespace alloy::engine

//==============================================================================
// definitions : non-member functions : struct : component
//==============================================================================

//------------------------------------------------------------------------------
// Comparison Operators
//------------------------------------------------------------------------------

inline bool alloy::engine::operator==(const component& lhs,
                                      const component& rhs)
  noexcept
{
  return lhs.index == rhs.index;
}

inline bool alloy::engine::operator!=(const component& lhs,
                                      const component& rhs)
  noexcept
{
  return !(lhs == rhs);
}

inline bool alloy::engine::operator<(const component& lhs,
                                     const component& rhs)
  noexcept
{
  return lhs.index < rhs.index;
}

inline bool alloy::engine::operator>(const component& lhs,
                                     const component& rhs)
  noexcept
{
  return rhs < lhs;
}

inline bool alloy::engine::operator<=(const component& lhs,
                                      const component& rhs)
  noexcept
{
  return !(rhs < lhs);
}

inline bool alloy::engine::operator>=(const component& lhs,
                                      const component& rhs)
  noexcept
{
  return !(lhs < rhs);
}

namespace std {
  template <>
  struct hash<alloy::engine::component>
  {
    std::size_t operator()(const alloy::engine::component& c) const noexcept
    {
      return static_cast<std::size_t>(c.index);
    }
  };
} // namespace std

#endif /* ALLOY_ENGINE_COMPONENT_HPP */