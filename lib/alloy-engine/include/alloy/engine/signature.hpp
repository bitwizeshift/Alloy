/*****************************************************************************
 * \file signature.hpp
 *
 * \todo
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019, 2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_ENGINE_SIGNATURE_HPP
#define ALLOY_ENGINE_SIGNATURE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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

    signature& operator=(signature&&) = default;
    signature& operator=(const signature&) = default;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Adds a component to this signature
    ///
    /// \param c the component to add
    void add(component c) noexcept;

    /// \brief Removes a component from this signature
    ///
    /// \param c the component to remove
    void remove(component c) noexcept;

    /// \brief Removes all components from this signature
    void reset() noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Checks if this signature contains the desired component \p c
    ///
    /// \param c the component to check
    /// \return `true` if this signature contains the component \p c
    bool contains(component c) const noexcept;

    //--------------------------------------------------------------------------
    // Members
    //--------------------------------------------------------------------------
  private:

    underlying_type m_value;

    friend bool operator==(const signature& lhs, const signature& rhs) noexcept;
    friend bool operator<(const signature& lhs, const signature& rhs) noexcept;
  };

  //============================================================================
  // functions : struct : signature
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparison Operators
  //----------------------------------------------------------------------------

  bool operator==(const signature& lhs, const signature& rhs) noexcept;
  bool operator!=(const signature& lhs, const signature& rhs) noexcept;
  bool operator<(const signature& lhs, const signature& rhs) noexcept;
  bool operator>(const signature& lhs, const signature& rhs) noexcept;
  bool operator<=(const signature& lhs, const signature& rhs) noexcept;
  bool operator>=(const signature& lhs, const signature& rhs) noexcept;

} // namespace alloy::engine

//==============================================================================
// definitions : class : signature
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline alloy::engine::signature::signature()
  noexcept
  : m_value{0}
{

}

inline alloy::engine::signature::signature(underlying_type value)
  noexcept
  : m_value{value}
{

}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline void alloy::engine::signature::add(component c)
  noexcept
{
  ALLOY_ASSERT(c.index < max_components);
  m_value |= (1 << c.index);
}

inline void alloy::engine::signature::remove(component c)
  noexcept
{
  ALLOY_ASSERT(c.index < max_components);
  m_value &= ~(1 << c.index);
}

inline void alloy::engine::signature::reset()
  noexcept
{
  m_value = 0u;
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline bool alloy::engine::signature::contains(component c)
  const noexcept
{
  return static_cast<bool>(m_value & (1 << c.index));
}

//==============================================================================
// definitions : non-member functions : class : signature
//==============================================================================

//------------------------------------------------------------------------------
// Comparison Operators
//------------------------------------------------------------------------------

inline bool alloy::engine::operator==(const signature& lhs,
                                      const signature& rhs)
  noexcept
{
  return lhs.m_value == rhs.m_value;
}

inline bool alloy::engine::operator!=(const signature& lhs,
                                      const signature& rhs)
  noexcept
{
  return !(lhs == rhs);
}

inline bool alloy::engine::operator<(const signature& lhs,
                                     const signature& rhs)
  noexcept
{
  return lhs.m_value < rhs.m_value;
}

inline bool alloy::engine::operator>(const signature& lhs,
                                     const signature& rhs)
  noexcept
{
  return rhs < lhs;
}

inline bool alloy::engine::operator<=(const signature& lhs,
                                      const signature& rhs)
  noexcept
{
  return !(rhs < lhs);
}

inline bool alloy::engine::operator>=(const signature& lhs,
                                      const signature& rhs)
  noexcept
{
  return !(lhs < rhs);
}

#endif /* ALLOY_ENGINE_SIGNATURE_HPP */

