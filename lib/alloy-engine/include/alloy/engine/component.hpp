/*****************************************************************************
 * \file component.hpp
 *
 * \todo
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_ENGINE_COMPONENT_HPP
#define ALLOY_ENGINE_COMPONENT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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