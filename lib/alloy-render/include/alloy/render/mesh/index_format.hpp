// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file index_format.hpp
///
/// \brief This header defines the formats that mesh indices may be stored in
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_INDEX_FORMAT_HPP
#define ALLOY_RENDER_MESH_INDEX_FORMAT_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/memory/data_quantity.hpp"
#include "alloy/core/string/zstring_view.hpp"

#include <cstdint> // std::uint16_t, std::uint32_t

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The format of each index in a mesh
  //////////////////////////////////////////////////////////////////////////////
  enum class index_format {
    /// Indices are 16-bit unsigned integers, addressing up to 65536 vertices
    ///
    /// Corresponds to GL_UNSIGNED_SHORT in OpenGL, DXGI_FORMAT_R16_UINT in
    /// DX11, etc.
    u16,

    /// Indices are 32-bit unsigned integers
    ///
    /// Corresponds to GL_UNSIGNED_INT in OpenGL, DXGI_FORMAT_R32_UINT in
    /// DX11, etc.
    u32,
  };

  /// \brief Gets the number of bytes occupied by a single index of format
  ///        \p f
  ///
  /// \param f the index format
  /// \return the size of an index
  constexpr auto bytes_per_index(index_format f) noexcept -> core::bytes;

  /// \brief Converts this index format \p f into a string
  ///
  /// \warning
  /// If \p f is not a valid enumerator for `index_format`, the behavior is
  /// undefined!
  ///
  /// \param f the index_format to convert to string
  /// \return a null-terminated string
  auto to_string(index_format f) noexcept -> core::zstring_view;

} // namespace alloy::render

inline constexpr
auto alloy::render::bytes_per_index(index_format f) noexcept -> core::bytes
{
  switch (f) {
    case index_format::u16: {
      return core::bytes{sizeof(std::uint16_t)};
    }
    case index_format::u32: {
      return core::bytes{sizeof(std::uint32_t)};
    }
  }
  core::compiler::unreachable();
}

inline
auto alloy::render::to_string(index_format f) noexcept -> core::zstring_view
{
  switch (f) {
    case index_format::u16: {
      return "u16";
    }
    case index_format::u32: {
      return "u32";
    }
  }
  core::compiler::unreachable();
}
#endif /* ALLOY_RENDER_MESH_INDEX_FORMAT_HPP */
