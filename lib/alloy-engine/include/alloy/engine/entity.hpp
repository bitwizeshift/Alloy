/*****************************************************************************
 * \file entity.hpp
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
#ifndef ALLOY_ENGINE_ENTITY_HPP
#define ALLOY_ENGINE_ENTITY_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstdint> // std::uint16_t
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

    using id_type = std::uint16_t;

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