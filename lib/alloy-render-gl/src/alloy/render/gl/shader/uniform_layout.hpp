// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file uniform_layout.hpp
///
/// \brief This header defines how uniform buffer data is laid out in memory,
///        following the std140 layout rules
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_SHADER_UNIFORM_LAYOUT_HPP
#define ALLOY_RENDER_GL_SHADER_UNIFORM_LAYOUT_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/containers/vector.hpp"

#include "alloy/render/shader/resolved_parameters.hpp"
#include "alloy/render/shader/shader_data_type.hpp"

#include <cstddef> // std::byte
#include <cstdint> // std::uint32_t

namespace alloy::render::gl {

  /// \brief Gets the number of bytes that a value of type \p t occupies in a
  ///        std140 uniform buffer
  ///
  /// Matrices occupy one 16-byte column per column of the matrix.
  ///
  /// \param t the data type
  /// \return the size of the value
  auto std140_size(shader_data_type t) noexcept -> std::uint32_t;

  /// \brief Packs the values of \p block into the bytes of a std140 uniform
  ///        buffer
  ///
  /// \pre the members of \p block fit within it without overlapping
  /// \param block the resolved uniform block
  /// \return the contents of the buffer
  auto pack_uniform_block(const uniform_block& block)
    -> core::vector<std::byte>;

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_SHADER_UNIFORM_LAYOUT_HPP */
