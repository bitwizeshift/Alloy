// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file active_program.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_SHADER_ACTIVE_PROGRAM_HPP
#define ALLOY_RENDER_GL_SHADER_ACTIVE_PROGRAM_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/gl/shader/program.hpp"

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/matrix/matrix2.hpp"
#include "alloy/core/math/matrix/matrix3.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector4.hpp"
#include "alloy/core/model/color.hpp"

#include <GL/glew.h>

#include <span>

namespace alloy::render::gl {
  struct transpose_type {};

  inline constexpr auto transpose = transpose_type{};

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A static class for managing the currently active program.
  ///
  /// The OpenGL library only allows setting values to the active program, but
  /// not to any inactive program. As a result, this functionality has been
  /// modeled as a static class.
  //////////////////////////////////////////////////////////////////////////////
  struct active_program
  {
    active_program() = delete;
    ~active_program() = delete;

    //--------------------------------------------------------------------------
    // Scalar Uniforms
    //--------------------------------------------------------------------------

    /// \brief Binds a single uniform bool value to \p loc
    ///
    /// \param loc the uniform location
    /// \param x the bool value
    static auto bind_uniform(uniform_location loc, bool x) noexcept -> void;

    /// \brief Binds a single uniform int value to \p loc
    ///
    /// \param loc the uniform location
    /// \param x the int value
    static auto bind_uniform(uniform_location loc, int x) noexcept -> void;

    /// \brief Binds a single uniform float value to \p loc
    ///
    /// \param loc the uniform location
    /// \param x the floating point value
    static auto bind_uniform(uniform_location loc, float x) noexcept -> void;

    //--------------------------------------------------------------------------

    /// \brief Binds two floats to a 2-component vector at \p loc
    ///
    /// \param loc the uniform location
    /// \param x the first part of the vector
    /// \param y the second part of the vector
    static auto bind_uniform(uniform_location loc, float x, float y)
      noexcept -> void;

    /// \brief Binds three floats to a 3-component vector at \p loc
    ///
    /// \param loc the uniform location
    /// \param x the first part of the vector
    /// \param y the second part of the vector
    /// \param z the third part of the vector
    static auto bind_uniform(uniform_location loc, float x, float y, float z)
      noexcept -> void;

    /// \brief Binds four floats to a 4-component vector at \p loc
    ///
    /// \param loc the uniform location
    /// \param x the first part of the vector
    /// \param y the second part of the vector
    /// \param z the third part of the vector
    /// \param w the fourth part of the vector
    static auto bind_uniform(
      uniform_location loc,
      float x,
      float y,
      float z,
      float w
    ) noexcept -> void;

    /// \brief Binds a color to a 4-component vector at \p loc
    ///
    /// \param loc the uniform location
    /// \param color the color to bind
    static auto bind_uniform(uniform_location loc, const core::color& color)
      noexcept -> void;

    /// \brief Binds a vector2 to a 2-component vector at \p loc
    ///
    /// \param loc the uniform location
    /// \param vec the vector to bind
    static auto bind_uniform(uniform_location loc, const core::vector2& vec)
      noexcept -> void;

    /// \brief Binds a vector3 to a 3-component vector at \p loc
    ///
    /// \param loc the uniform location
    /// \param vec the vector to bind
    static auto bind_uniform(uniform_location loc, const core::vector3& vec)
      noexcept -> void;

    /// \brief Binds a vector4 to a 4-component vector at \p loc
    ///
    /// \param loc the uniform location
    /// \param vec the vector to bind
    static auto bind_uniform(uniform_location loc, const core::vector4& vec)
      noexcept -> void;

    /// \{
    /// \brief Binds a matrix2 to a matrix at \p loc
    ///
    /// \param loc the uniform location
    /// \param vec the matrix2 to bind
    static auto bind_uniform(uniform_location loc, const core::matrix2& mat)
      noexcept -> void;
    static auto bind_uniform(
      uniform_location loc,
      const core::matrix2& mat,
      transpose_type
    ) noexcept -> void;
    /// \}

    /// \{
    /// \brief Binds a matrix3 to a matrix at \p loc
    ///
    /// \param loc the uniform location
    /// \param vec the matrix3 to bind
    static auto bind_uniform(uniform_location loc, const core::matrix3& mat)
      noexcept -> void;
    static auto bind_uniform(
      uniform_location loc,
      const core::matrix3& mat,
      transpose_type
    ) noexcept -> void;
    /// \}

    /// \{
    /// \brief Binds a matrix2 to a matrix at \p loc
    ///
    /// \param loc the uniform location
    /// \param vec the matrix2 to bind
    static auto bind_uniform(uniform_location loc, const core::matrix4& mat)
      noexcept -> void;
    static auto bind_uniform(
      uniform_location loc,
      const core::matrix4& mat,
      transpose_type
    ) noexcept -> void;
    /// \}

    //--------------------------------------------------------------------------
    // Vector Uniforms
    //--------------------------------------------------------------------------

    /// \{
    /// \brief Binds a series of uniforms
    ///
    /// \param loc the location of the uniform
    /// \param v the sequence of the uniform to bind
    static auto bind_uniforms(uniform_location loc, std::span<core::color> v)
      noexcept -> void;
    static auto bind_uniforms(uniform_location loc, std::span<core::vector2> v)
      noexcept -> void;
    static auto bind_uniforms(uniform_location loc, std::span<core::vector3> v)
      noexcept -> void;
    static auto bind_uniforms(uniform_location loc, std::span<core::vector4> v)
      noexcept -> void;
    static auto bind_uniforms(uniform_location loc, std::span<core::matrix2> v)
      noexcept -> void;
    static auto bind_uniforms(
      uniform_location loc,
      std::span<core::matrix2> v,
      transpose_type
    ) noexcept -> void;
    static auto bind_uniforms(uniform_location loc, std::span<core::matrix3> v)
      noexcept -> void;
    static auto bind_uniforms(
      uniform_location loc,
      std::span<core::matrix3> v,
      transpose_type
    ) noexcept -> void;
    static auto bind_uniforms(uniform_location loc, std::span<core::matrix4> v)
      noexcept -> void;
    static auto bind_uniforms(
      uniform_location loc,
      std::span<core::matrix4> v,
      transpose_type
    ) noexcept -> void;
    /// \}
  };

} // namespace alloy::render::gl

//------------------------------------------------------------------------------
// Scalar Uniforms
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  bool x
) noexcept -> void
{
  ::glUniform1i(static_cast<::GLint>(loc), static_cast<int>(x));
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  int x
) noexcept -> void
{
  ::glUniform1i(static_cast<::GLint>(loc), x);
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  float x
) noexcept -> void
{
  ::glUniform1f(static_cast<::GLint>(loc), x);
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  float x,
  float y
) noexcept -> void
{
  ::glUniform2f(static_cast<::GLint>(loc), x, y);
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  float x,
  float y,
  float z
) noexcept -> void
{
  ::glUniform3f(static_cast<::GLint>(loc), x, y, z);
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  float x,
  float y,
  float z,
  float w
) noexcept -> void
{
  ::glUniform4f(static_cast<::GLint>(loc), x, y, z, w);
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  const core::color& color
) noexcept -> void
{
  ::glUniform4fv(static_cast<::GLint>(loc), 1, color.data());
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  const core::vector2& vec
) noexcept -> void
{
  ::glUniform2fv(static_cast<::GLint>(loc), 1, vec.data());
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  const core::vector3& vec
) noexcept -> void
{
  ::glUniform3fv(static_cast<::GLint>(loc), 1, vec.data());
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  const core::vector4& vec
) noexcept -> void
{
  ::glUniform4fv(static_cast<::GLint>(loc), 1, vec.data());
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  const core::matrix2& mat
) noexcept -> void
{
  ::glUniformMatrix2fv(static_cast<::GLint>(loc), 1, false, mat.data());
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  const core::matrix2& mat,
  transpose_type
) noexcept -> void
{
  ::glUniformMatrix2fv(static_cast<::GLint>(loc), 1, true, mat.data());
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  const core::matrix3& mat
) noexcept -> void
{
  ::glUniformMatrix3fv(static_cast<::GLint>(loc), 1, false, mat.data());
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  const core::matrix3& mat,
  transpose_type
) noexcept -> void
{
  ::glUniformMatrix3fv(static_cast<::GLint>(loc), 1, true, mat.data());
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  const core::matrix4& mat
) noexcept -> void
{
  ::glUniformMatrix4fv(static_cast<::GLint>(loc), 1, false, mat.data());
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniform(
  uniform_location loc,
  const core::matrix4& mat,
  transpose_type
) noexcept -> void
{
  ::glUniformMatrix4fv(static_cast<::GLint>(loc), 1, true, mat.data());
}

//------------------------------------------------------------------------------
// Vector Uniforms
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniforms(
  uniform_location loc,
  std::span<core::color> v
) noexcept -> void
{
  ::glUniform4fv(
    static_cast<::GLint>(loc),
    static_cast<::GLsizei>(v.size()),
    v[0].data()
  );
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniforms(
  uniform_location loc,
  std::span<core::vector2> v
) noexcept -> void
{
  ::glUniform2fv(
    static_cast<::GLint>(loc),
    static_cast<::GLsizei>(v.size()),
    v[0].data()
  );
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniforms(
  uniform_location loc,
  std::span<core::vector3> v
) noexcept -> void
{
  ::glUniform3fv(
    static_cast<::GLint>(loc),
    static_cast<::GLsizei>(v.size()),
    v[0].data()
  );
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniforms(
  uniform_location loc,
  std::span<core::vector4> v
) noexcept -> void
{
  ::glUniform4fv(
    static_cast<::GLint>(loc),
    static_cast<::GLsizei>(v.size()),
    v[0].data()
  );
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniforms(
  uniform_location loc,
  std::span<core::matrix2> v
) noexcept -> void
{
  ::glUniformMatrix2fv(
    static_cast<::GLint>(loc),
    static_cast<::GLsizei>(v.size()),
    false,
    v[0].data()
  );
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniforms(
  uniform_location loc,
  std::span<core::matrix2> v,
  transpose_type
) noexcept -> void
{
  ::glUniformMatrix2fv(
    static_cast<::GLint>(loc),
    static_cast<::GLsizei>(v.size()),
    true,
    v[0].data()
  );
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniforms(
  uniform_location loc,
  std::span<core::matrix3> v
) noexcept -> void
{
  ::glUniformMatrix3fv(
    static_cast<::GLint>(loc),
    static_cast<::GLsizei>(v.size()),
    false,
    v[0].data()
  );
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniforms(
  uniform_location loc,
  std::span<core::matrix3> v,
  transpose_type
) noexcept -> void
{
  ::glUniformMatrix3fv(
    static_cast<::GLint>(loc),
    static_cast<::GLsizei>(v.size()),
    true,
    v[0].data()
  );
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniforms(
  uniform_location loc,
  std::span<core::matrix4> v
) noexcept -> void
{
  ::glUniformMatrix4fv(
    static_cast<::GLint>(loc),
    static_cast<::GLsizei>(v.size()),
    false,
    v[0].data()
  );
}

ALLOY_FORCE_INLINE
auto alloy::render::gl::active_program::bind_uniforms(
  uniform_location loc,
  std::span<core::matrix4> v,
  transpose_type
) noexcept -> void
{
  ::glUniformMatrix4fv(
    static_cast<::GLint>(loc),
    static_cast<::GLsizei>(v.size()),
    true,
    v[0].data()
  );
}

#endif /* ALLOY_RENDER_GL_SHADER_ACTIVE_PROGRAM_HPP */
