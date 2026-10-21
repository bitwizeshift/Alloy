// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file signature.hpp
 *
 * \todo
 *****************************************************************************/

#ifndef ALLOY_ENGINE_SIGNATURE_HPP
#define ALLOY_ENGINE_SIGNATURE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/engine/component.hpp"
#include "alloy/core/assert.hpp"

#include <cstdint> // std::uint32_t
#include <climits> // CHAR_BIT

namespace alloy::engine {

  //============================================================================
  // struct : signature
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A signature for an entity in the entity-component system
  ///
  /// This type is used to indicate associated components
  //////////////////////////////////////////////////////////////////////////////
  class signature
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using underlying_type = std::uint32_t;

    //--------------------------------------------------------------------------
    // Public Static Members
    //--------------------------------------------------------------------------
  public:

    static constexpr auto max_components = sizeof(underlying_type) * CHAR_BIT;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    signature() noexcept;
    explicit signature(underlying_type value) noexcept;
    signature(signature&&) = default;
    signature(const signature&) = default;

    auto operator=(signature&&) -> signature& = default;
    auto operator=(const signature&) -> signature& = default;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Adds a component to this signature
    ///
    /// \param c the component to add
    auto add(component c) noexcept -> void;

    /// \brief Removes a component from this signature
    ///
    /// \param c the component to remove
    auto remove(component c) noexcept -> void;

    /// \brief Removes all components from this signature
    auto reset() noexcept -> void;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Checks if this signature contains the desired component \p c
    ///
    /// \param c the component to check
    /// \return `true` if this signature contains the component \p c
    auto contains(component c) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Members
    //--------------------------------------------------------------------------
  private:

    underlying_type m_value;

    friend auto operator==(const signature& lhs, const signature& rhs)
      noexcept -> bool;
    friend auto operator<(const signature& lhs, const signature& rhs)
      noexcept -> bool;
  };

  //============================================================================
  // functions : struct : signature
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparison Operators
  //----------------------------------------------------------------------------

  auto operator==(const signature& lhs, const signature& rhs) noexcept -> bool;
  auto operator!=(const signature& lhs, const signature& rhs) noexcept -> bool;
  auto operator<(const signature& lhs, const signature& rhs) noexcept -> bool;
  auto operator>(const signature& lhs, const signature& rhs) noexcept -> bool;
  auto operator<=(const signature& lhs, const signature& rhs) noexcept -> bool;
  auto operator>=(const signature& lhs, const signature& rhs) noexcept -> bool;

} // namespace alloy::engine

//==============================================================================
// definitions : class : signature
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline
alloy::engine::signature::signature() noexcept
  : m_value{0}
{

}

inline
alloy::engine::signature::signature(underlying_type value) noexcept
  : m_value{value}
{

}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline
auto alloy::engine::signature::add(component c) noexcept -> void
{
  ALLOY_ASSERT(c.index < max_components);
  m_value |= (1 << c.index);
}

inline
auto alloy::engine::signature::remove(component c) noexcept -> void
{
  ALLOY_ASSERT(c.index < max_components);
  m_value &= ~(1 << c.index);
}

inline
auto alloy::engine::signature::reset() noexcept -> void
{
  m_value = 0u;
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline
auto alloy::engine::signature::contains(component c) const noexcept -> bool
{
  return static_cast<bool>(m_value & (1 << c.index));
}

//==============================================================================
// definitions : non-member functions : class : signature
//==============================================================================

//------------------------------------------------------------------------------
// Comparison Operators
//------------------------------------------------------------------------------

inline
auto alloy::engine::operator==(const signature& lhs, const signature& rhs)
  noexcept -> bool
{
  return lhs.m_value == rhs.m_value;
}

inline
auto alloy::engine::operator!=(const signature& lhs, const signature& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

inline
auto alloy::engine::operator<(const signature& lhs, const signature& rhs)
  noexcept -> bool
{
  return lhs.m_value < rhs.m_value;
}

inline
auto alloy::engine::operator>(const signature& lhs, const signature& rhs)
  noexcept -> bool
{
  return rhs < lhs;
}

inline
auto alloy::engine::operator<=(const signature& lhs, const signature& rhs)
  noexcept -> bool
{
  return !(rhs < lhs);
}

inline
auto alloy::engine::operator>=(const signature& lhs, const signature& rhs)
  noexcept -> bool
{
  return !(lhs < rhs);
}

#endif /* ALLOY_ENGINE_SIGNATURE_HPP */

