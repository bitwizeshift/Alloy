// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file shader_compiler.hpp
///
/// \brief This header defines the compilation of a stage's bytecode into an
///        OpenGL shader, in whatever form that bytecode takes
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_SHADER_SHADER_COMPILER_HPP
#define ALLOY_RENDER_GL_SHADER_SHADER_COMPILER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/gl/capabilities.hpp"
#include "alloy/render/gl/shader/shader.hpp"

#include "alloy/core/utilities/result.hpp"

#include "alloy/render/error.hpp"
#include "alloy/render/shader/program_spec.hpp"

#include <cstddef> // std::byte
#include <span>    // std::span

namespace alloy::render::gl {

  /// \brief Determines the form of shader code that \p bytecode holds
  ///
  /// Bytecode that begins with the SPIR-V magic number is a SPIR-V binary;
  /// anything else is treated as GLSL source.
  ///
  /// \param bytecode the bytecode of a stage
  /// \return the format of the bytecode
  auto detect_format(std::span<const std::byte> bytecode)
    noexcept -> shader_format;

  /// \brief Compiles the bytecode of \p stage into a shader
  ///
  /// \param stage the stage to compile
  /// \param caps the capabilities of the current context
  /// \return the shader on success, or `render_errc::unsupported_bytecode` if
  ///         the context cannot consume the bytecode
  auto compile_stage(const shader_stage_spec& stage, const capabilities& caps)
    -> core::result<shader, render_error>;

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_SHADER_SHADER_COMPILER_HPP */
