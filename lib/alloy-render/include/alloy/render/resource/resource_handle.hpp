// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file resource_handle.hpp
///
/// \brief This header defines the lightweight, non-owning handle that refers
///        to a resource created by a render device
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_RESOURCE_RESOURCE_HANDLE_HPP
#define ALLOY_RENDER_RESOURCE_RESOURCE_HANDLE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <cstddef>    // std::size_t
#include <cstdint>    // std::uint32_t
#include <functional> // std::hash

namespace alloy::render {

  template <typename Tag, typename Object>
  class handle_pool;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A lightweight, opaque reference to a resource owned by a device
  ///
  /// A handle does not own the resource it refers to; ownership belongs to the
  /// `unique_resource` that the device returned on creation. Handles are cheap
  /// to copy and may be freely stored by anything that needs to refer to the
  /// resource, such as a material referring to its program.
  ///
  /// A handle to a resource that has since been disposed is detected as stale
  /// by the device that created it, even if the underlying storage has been
  /// reused by a newer resource.
  ///
  /// Handles are ordered so that they may be sorted, such as to group draws
  /// that use the same resource. The order is consistent with equality, but
  /// has no other meaning.
  ///
  /// \tparam Tag a type that distinguishes each family of resource
  //////////////////////////////////////////////////////////////////////////////
  template <typename Tag>
  class resource_handle
  {
    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a null handle that refers to no resource
    constexpr resource_handle() noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Queries whether this handle refers to no resource
    ///
    /// \return `true` if this handle is null
    constexpr auto is_null() const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    constexpr explicit resource_handle(std::uint32_t value) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    std::uint32_t m_value = 0u;

    template <typename, typename>
    friend class handle_pool;

    template <typename T>
    friend constexpr auto operator==(
      const resource_handle<T>& lhs,
      const resource_handle<T>& rhs
    ) noexcept -> bool;

    template <typename T>
    friend constexpr auto operator<(
      const resource_handle<T>& lhs,
      const resource_handle<T>& rhs
    ) noexcept -> bool;

    friend struct std::hash<resource_handle>;
  };

  //============================================================================
  // non-member functions : class : resource_handle
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  template <typename Tag>
  constexpr auto operator==(
    const resource_handle<Tag>& lhs,
    const resource_handle<Tag>& rhs
  ) noexcept -> bool;

  template <typename Tag>
  constexpr auto operator!=(
    const resource_handle<Tag>& lhs,
    const resource_handle<Tag>& rhs
  ) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Comparison
  //----------------------------------------------------------------------------

  template <typename Tag>
  constexpr auto operator<(
    const resource_handle<Tag>& lhs,
    const resource_handle<Tag>& rhs
  ) noexcept -> bool;

  template <typename Tag>
  constexpr auto operator>(
    const resource_handle<Tag>& lhs,
    const resource_handle<Tag>& rhs
  ) noexcept -> bool;

  template <typename Tag>
  constexpr auto operator<=(
    const resource_handle<Tag>& lhs,
    const resource_handle<Tag>& rhs
  ) noexcept -> bool;

  template <typename Tag>
  constexpr auto operator>=(
    const resource_handle<Tag>& lhs,
    const resource_handle<Tag>& rhs
  ) noexcept -> bool;

} // namespace alloy::render

template <typename Tag>
struct std::hash<alloy::render::resource_handle<Tag>>
{
  auto operator()(const alloy::render::resource_handle<Tag>& handle)
    const noexcept -> std::size_t
  {
    return std::hash<std::uint32_t>{}(handle.m_value);
  }
};

//==============================================================================
// definitions : class : resource_handle
//==============================================================================

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <typename Tag>
inline constexpr
auto alloy::render::resource_handle<Tag>::is_null() const noexcept -> bool
{
  return m_value == 0u;
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

template <typename Tag>
inline constexpr
alloy::render::resource_handle<Tag>::resource_handle(std::uint32_t value)
  noexcept
  : m_value{value}
{

}

//==============================================================================
// definitions : non-member functions : class : resource_handle
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

template <typename Tag>
inline constexpr
auto alloy::render::operator==(
  const resource_handle<Tag>& lhs,
  const resource_handle<Tag>& rhs
) noexcept -> bool
{
  return lhs.m_value == rhs.m_value;
}

template <typename Tag>
inline constexpr
auto alloy::render::operator!=(
  const resource_handle<Tag>& lhs,
  const resource_handle<Tag>& rhs
) noexcept -> bool
{
  return !(lhs == rhs);
}

//------------------------------------------------------------------------------
// Comparison
//------------------------------------------------------------------------------

template <typename Tag>
inline constexpr
auto alloy::render::operator<(
  const resource_handle<Tag>& lhs,
  const resource_handle<Tag>& rhs
) noexcept -> bool
{
  return lhs.m_value < rhs.m_value;
}

template <typename Tag>
inline constexpr
auto alloy::render::operator>(
  const resource_handle<Tag>& lhs,
  const resource_handle<Tag>& rhs
) noexcept -> bool
{
  return rhs < lhs;
}

template <typename Tag>
inline constexpr
auto alloy::render::operator<=(
  const resource_handle<Tag>& lhs,
  const resource_handle<Tag>& rhs
) noexcept -> bool
{
  return !(rhs < lhs);
}

template <typename Tag>
inline constexpr
auto alloy::render::operator>=(
  const resource_handle<Tag>& lhs,
  const resource_handle<Tag>& rhs
) noexcept -> bool
{
  return !(lhs < rhs);
}

#endif /* ALLOY_RENDER_RESOURCE_RESOURCE_HANDLE_HPP */
