// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "shader_compiler.hpp"

#include "alloy/render/gl/shader/shader.hpp"

#include "alloy/render/gl/capabilities.hpp"

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/string/string_view.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/render/error.hpp"
#include "alloy/render/shader/program_spec.hpp"

#include <algorithm> // std::equal
#include <array>     // std::array
#include <cstddef>   // std::byte
#include <span>      // std::span

auto alloy::render::gl::detect_format(std::span<const std::byte> bytecode)
  noexcept -> shader_format
{
  // The magic number 0x07230203, as written by either endianness
  static constexpr auto little_endian = std::array{
    std::byte{0x03}, std::byte{0x02}, std::byte{0x23}, std::byte{0x07},
  };
  static constexpr auto big_endian = std::array{
    std::byte{0x07}, std::byte{0x23}, std::byte{0x02}, std::byte{0x03},
  };

  if (bytecode.size() < little_endian.size()) {
    return shader_format::glsl;
  }

  const auto magic = bytecode.first<4u>();
  if (std::equal(magic.begin(), magic.end(), little_endian.begin())
      || std::equal(magic.begin(), magic.end(), big_endian.begin())) {
    return shader_format::spirv;
  }
  return shader_format::glsl;
}

auto alloy::render::gl::compile_stage(
  const shader_stage_spec& stage,
  const capabilities& caps
) -> core::result<shader, render_error>
{
  switch (detect_format(stage.bytecode)) {
    case shader_format::glsl: {
      const auto source = core::string_view{
        reinterpret_cast<const char*>(stage.bytecode.data()),
        stage.bytecode.size()
      };
      return shader::compile_glsl(stage.stage, source);
    }
    case shader_format::spirv: {
      if (!caps.spirv) {
        return core::fail(
          render_error::make(
            render_errc::unsupported_bytecode,
            "SPIR-V requires OpenGL 4.6 or ARB_gl_spirv"
          )
        );
      }
      return shader::load_spirv(stage.stage, stage.bytecode, stage.entry_point);
    }
  }
  core::compiler::unreachable();
}
