////////////////////////////////////////////////////////////////////////////////
/// \file shader.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

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

#ifndef EXTRA_SDL2_BINDINGS_EXAMPLE_SDL2_WINDOW_SRC_SHADER_HPP
#define EXTRA_SDL2_BINDINGS_EXAMPLE_SDL2_WINDOW_SRC_SHADER_HPP

#include "error.hpp"

#include <alloy/core/assert.hpp>
#include <alloy/core/utilities/expected.hpp>
#include <alloy/core/utilities/string_view.hpp>
#include <alloy/core/containers/string.hpp>

#include <alloy/io/filesystem/file.hpp>

#include <gl/glew.h>

#include <limits>
#include <utility>

namespace alloy::render::gl {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An enumeration of all possible shader types
  /////////////////////////////////////////////////////////////////////////////
  enum class shader_type : ::GLenum
  {
    vertex   = GL_VERTEX_SHADER,
    compute  = GL_COMPUTE_SHADER,
    geometry = GL_GEOMETRY_SHADER,
    fragment = GL_FRAGMENT_SHADER,
    tessellation_control = GL_TESS_CONTROL_SHADER,
    tessellation_evaluation =  GL_TESS_EVALUATION_SHADER,
  };

  /////////////////////////////////////////////////////////////////////////////
  /// \brief The base class for all shaders
  ///
  /// This type exists only to give a single definition to the 'compile'
  /// function in order to prevent template code-bloat.
  /////////////////////////////////////////////////////////////////////////////
  class shader_base
  {
    //-------------------------------------------------------------------------
    // Protected Static Factories
    //-------------------------------------------------------------------------
  protected:

    /// \brief Compiles a shader of the specified \p type from the source
    ///        \p program
    ///
    /// Errors are allocated onto the default heap, since they should never
    /// occur during production.
    ///
    /// \param type the shader type
    /// \param program the shader source
    /// \return the shader ID on success
    static auto do_compile_from_source(shader_type type,
                                       core::zstring_view program)
      -> core::expected<::GLuint, gl_error_message>;

    /// \brief Compiles a shader of the specified \p type from the given \p file
    ///
    /// The allocator \p alloc may also be specified that will be used to store
    /// the file being read in a temporary buffer.
    ///
    /// Errors are allocated onto the default heap, since they should never
    /// occur during production.
    ///
    /// \param type the shader type
    /// \param file the file that contains the program to be read
    /// \param alloc the allocator that can be used to store the temporary
    ///              buffer used to read the file
    /// \return the shader ID on success
    static auto do_compile_from_file(shader_type type,
                                     io::file& file,
                                     core::allocator alloc)
      -> core::expected<::GLuint, gl_error_message>;
  };

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A template for all shader types
  ///
  /// This is a strongly-defined type per shader which allows more static
  /// detection of failures when trying to link together programs that may
  /// contain duplicates of a given shader in the pipeline, for example.
  ///
  /// The basic_shader type should never really be used directly; but rather
  /// through one of the aliases to the concrete shader types below.
  ///
  /// \tparam Type the type of the shader
  /////////////////////////////////////////////////////////////////////////////
  template <shader_type Type>
  class basic_shader : private shader_base
  {
    //-------------------------------------------------------------------------
    // Public Members
    //-------------------------------------------------------------------------
  public:

    using id_type = ::GLuint;

    //-------------------------------------------------------------------------
    // Public Static Member
    //-------------------------------------------------------------------------
  public:

    /// \brief The type of the shader
    inline static constexpr auto type = Type;

    /// \brief A value indicating an invalid shader id
    inline static constexpr auto invalid_id = std::numeric_limits<id_type>::min();

    //-------------------------------------------------------------------------
    // Public Static Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Compiles a shader from the source \p program
    ///
    /// Errors are allocated onto the default heap, since they should never
    /// occur during production.
    ///
    /// \param type the shader type
    /// \param program the shader source
    /// \return the shader ID on success
    static auto compile_from_source(core::zstring_view program)
      -> core::expected<basic_shader, gl_error_message>;


    /// \brief Compiles a shader from the given \p file
    ///
    /// The allocator \p alloc may also be specified that will be used to store
    /// the file being read in a temporary buffer.
    ///
    /// Errors are allocated onto the default heap, since they should never
    /// occur during production.
    ///
    /// \param file the file that contains the program to be read
    /// \param alloc the allocator that can be used to store the temporary
    ///              buffer used to read the file
    /// \return the shader ID on success
    static auto compile_from_file(io::file& file,
                                  core::allocator alloc = core::allocator{})
      -> core::expected<basic_shader, gl_error_message>;

    //-------------------------------------------------------------------------
    // Constructors / Assignment / Destructor
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a basic_shader that does not manager a compiled shader
    basic_shader() noexcept;

    /// \brief Constructs a basic_shader that will adopt ownership of the shader
    ///        with id \p id
    ///
    /// It is assumed the shader type being adopted is the same as Type
    ///
    /// \param id the shader id
    explicit basic_shader(id_type id);

    /// \brief Constructs this shader by moving the owned shader from \p other
    ///
    /// \post other.is_compiled() is false
    /// \param other the other shader to move
    basic_shader(basic_shader&& other) noexcept;
    basic_shader(const basic_shader&) = delete;

    //-------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to this shader
    ///
    /// \param other the other shader to assign from
    /// \return reference to (*this)
    auto operator=(basic_shader other) noexcept -> basic_shader&;

    //-------------------------------------------------------------------------

    /// \brief Disposes of the underlying shader
    ~basic_shader();

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Retrieves the underlying ID for this shader
    ///
    /// \return the id of the shader
    auto id() const noexcept -> id_type;

    /// \brief Queries whether the shader is compiled
    ///
    /// \return true if compiled
    auto is_compiled() const noexcept -> bool;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Releases ownership of this shader so that it may be managed by
    ///        a different consumer
    ///
    /// \return the id owned by this shader
    auto release() noexcept -> id_type;

    /// \brief Resets this shader so that any owned instance is deleted
    auto reset() noexcept -> void;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    id_type m_id;
  };

  using vertex_shader   = basic_shader<shader_type::vertex>;
  using compute_shader  = basic_shader<shader_type::compute>;
  using geometry_shader = basic_shader<shader_type::geometry>;
  using fragment_shader = basic_shader<shader_type::fragment>;
  using tessellation_control_shader = basic_shader<shader_type::tessellation_control>;
  using tessellation_evaluation_shader = basic_shader<shader_type::tessellation_evaluation>;

} // namespace alloy::render::gl

//=============================================================================
// class : shader_base
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

inline auto
  alloy::render::gl::shader_base::do_compile_from_source(shader_type type,
                                                           core::zstring_view program)
  -> core::expected<::GLuint, gl_error_message>
{
  const auto shader_id = ::glCreateShader(static_cast<::GLenum>(type));

  auto delete_shader = core::scope_exit{[&]{
    ::glDeleteShader(shader_id);
  }};

  const auto str = program.c_str();
  ::glShaderSource(shader_id, 1, &str, nullptr);
  ::glCompileShader(shader_id);

  auto result = ::GLint{};
  ::glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);

  // If we compiled successfully, return
  if (result == GL_TRUE) {
    delete_shader.release();
    return shader_id;
  }

  // If we failed to compile, return the error message
  auto length = ::GLint{};
  ::glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

  auto message = core::string{};
  message.resize(length, '\0');
  ::glGetShaderInfoLog(shader_id, message.length(), nullptr, message.data());

  return core::unexpected(
    gl_error_message::make(std::move(message))
  );
}

inline auto
  alloy::render::gl::shader_base::do_compile_from_file(shader_type type,
                                                         io::file& file,
                                                         core::allocator alloc)
  -> core::expected<::GLuint, gl_error_message>
{
  const auto size = file.bytes();

  if (size.has_error()) {
    return core::unexpected(gl_error_message::make("Bad file input"));
  }

  auto source = core::string{core::stl_allocator_adapter<core::string>{alloc}};
  source.resize(*size);

  file.read(io::mutable_buffer::from_data(source.data(), source.size()));

  return do_compile_from_source(type, source);
}

//=============================================================================
// class : basic_shader
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

template <alloy::render::gl::shader_type Type>
inline auto
  alloy::render::gl::basic_shader<Type>::compile_from_source(core::zstring_view program)
  -> core::expected<basic_shader, gl_error_message>
{
  const auto result = shader_base::do_compile_from_source(Type, program);

  if (result.has_error()) {
    return core::unexpected(result.error());
  }
  return basic_shader{result.value()};
}

template <alloy::render::gl::shader_type Type>
inline auto
  alloy::render::gl::basic_shader<Type>::compile_from_file(io::file& file,
                                                             core::allocator alloc)
  -> core::expected<basic_shader, gl_error_message>
{
  const auto result = shader_base::do_compile_from_file(Type, file, alloc);

  if (result.has_error()) {
    return core::unexpected(result.error());
  }

  return basic_shader{*result};
}

//-----------------------------------------------------------------------------
// Constructors / Destructors / Assignment
//-----------------------------------------------------------------------------

template <alloy::render::gl::shader_type Type>
inline alloy::render::gl::basic_shader<Type>::basic_shader()
  noexcept
  : m_id{invalid_id}
{

}

template <alloy::render::gl::shader_type Type>
inline alloy::render::gl::basic_shader<Type>::basic_shader(id_type id)
  : m_id{id}
{
  ALLOY_ASSERT(::glIsShader(id) != GL_FALSE);
}

template <alloy::render::gl::shader_type Type>
inline alloy::render::gl::basic_shader<Type>::basic_shader(basic_shader&& other)
  noexcept
  : m_id{other.release()}
{

}

//-----------------------------------------------------------------------------

template <alloy::render::gl::shader_type Type>
inline alloy::render::gl::basic_shader<Type>::~basic_shader()
{
  reset();
}

//-----------------------------------------------------------------------------

template <alloy::render::gl::shader_type Type>
inline auto
  alloy::render::gl::basic_shader<Type>::operator=(basic_shader other)
  noexcept
  -> basic_shader&
{
  std::swap(m_id, other.m_id);
  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <alloy::render::gl::shader_type Type>
inline auto alloy::render::gl::basic_shader<Type>::id()
  const noexcept
  -> id_type
{
  return m_id;
}

template <alloy::render::gl::shader_type Type>
inline auto alloy::render::gl::basic_shader<Type>::is_compiled()
  const noexcept
  -> bool
{
  return m_id != invalid_id;
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

template <alloy::render::gl::shader_type Type>
inline auto alloy::render::gl::basic_shader<Type>::release()
  noexcept
  -> id_type
{
  const auto result = m_id;
  m_id = invalid_id;
  return result;
}

template <alloy::render::gl::shader_type Type>
inline auto alloy::render::gl::basic_shader<Type>::reset()
  noexcept
  -> void
{
  if (is_compiled()) {
    ::glDeleteShader(m_id);
    m_id = invalid_id;
  }
}

#endif /* EXTRA_SDL2_BINDINGS_EXAMPLE_SDL2_WINDOW_SRC_SHADER_HPP */
