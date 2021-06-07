////////////////////////////////////////////////////////////////////////////////
/// \file program.hpp
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

#ifndef ALLOY_RENDER_GL_MATERIAL_PROGRAM_HPP
#define ALLOY_RENDER_GL_MATERIAL_PROGRAM_HPP

#include "alloy/render/gl/shader/shader.hpp"
#include "alloy/render/gl/error.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/utilities/result.hpp"

#include <gl/glew.h>

#include <limits>      // std::numeric_limits
#include <utility>     // std::move
#include <type_traits> // std::false_type
#include <initializer_list> // std::initializer_list
#include <iterator> // std::advance, std::next

namespace alloy::render::gl::detail {

  template <typename ForwardIt>
  constexpr auto is_unique(ForwardIt first, ForwardIt last)
    -> bool
  {
    for (auto it = first; it != last; ++it) {
      for (auto jt = std::next(it); jt != last; ++jt) {
        if (*it == *jt) {
          return false;
        }
      }
    }
    return true;
  }

  template <typename T>
  constexpr auto is_unique(std::initializer_list<T> ilist)
  {
    return is_unique(ilist.begin(), ilist.end());
  }

} // namespace alloy::render::gl::detail

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The uniform location in a given program
  //////////////////////////////////////////////////////////////////////////////
  enum class uniform_location   : ::GLint{};
  enum class attribute_location : ::GLint{};

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A shader program in open GL -- made up of several shader objects
  ///
  //////////////////////////////////////////////////////////////////////////////
  class program
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using id_type = ::GLuint;

    //--------------------------------------------------------------------------
    // Public Static Members
    //--------------------------------------------------------------------------
  public:

    inline static constexpr auto invalid_id = std::numeric_limits<id_type>::min();

    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    template <shader_type... types>
    static auto link(basic_shader<types>...shaders) noexcept
      -> core::result<program,gl_error_message>;

    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a program that does not manager a shader program
    program() noexcept;

    /// \brief Constructs a basic_shader that will adopt ownership of the shader
    ///        program with id \p id
    ///
    /// \param id the shader id
    explicit program(id_type id);

    /// \brief Constructs this program by moving the owned program from \p other
    ///
    /// \post other.is_compiled() is false
    /// \param other the other program to move
    program(program&& other) noexcept;
    program(const program&) = delete;

    //--------------------------------------------------------------------------

    /// \brief Moves the contents of \p other to this
    ///
    /// \param other the other program to move
    /// \return reference to (*this)
    auto operator=(program other) noexcept -> program&;

    //--------------------------------------------------------------------------

    /// \brief Disposes of the underlying program
    ~program();

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Retrieves the underlying ID for this program
    ///
    /// \return the id of the program
    auto id() const noexcept -> id_type;

    /// \brief Queries whether the program is linked
    ///
    /// \return true if compiled
    auto is_linked() const noexcept -> bool;

    /// \brief Gets a uniform location by the name of the uniform
    ///
    /// \param name the name of the uniform
    /// \return the location
    auto get_uniform_location(core::zstring_view name) const noexcept
      -> core::result<uniform_location, std::error_code>;

    /// \brief Gets a vertex attribute location by the name of the attribute
    ///
    /// \param name the name of the uniform
    /// \return the attribute name
    auto get_attribute_location(core::zstring_view name) const noexcept
      -> core::result<attribute_location, std::error_code>;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Releases ownership of this program so that it may be managed by
    ///        a different consumer
    ///
    /// \return the id owned by this program
    auto release() noexcept -> id_type;

    /// \brief Resets this program so that any owned instance is deleted
    auto reset() noexcept -> void;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    id_type m_id;

    //--------------------------------------------------------------------------
    // Private Static Factories
    //--------------------------------------------------------------------------
  private:

    static auto do_link(std::initializer_list<::GLuint> shader_ids) noexcept
      -> core::result<program,gl_error_message>;
  };

  //===========================================================================
  // non-member functions : class : program
  //===========================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  auto operator==(const program& lhs, const program& rhs) noexcept -> bool;
  auto operator!=(const program& lhs, const program& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Uses the specified program \p p as the currently active program
  ///
  /// \param p the program
  auto use_program(const program& p) noexcept -> void;

} // namespace alloy::render::gl

//=============================================================================
// class : program
//=============================================================================

//------------------------------------------------------------------------------
// Static Factory
//------------------------------------------------------------------------------

template <alloy::render::gl::shader_type...Types>
inline
auto alloy::render::gl::program::link(basic_shader<Types>...shaders)
  noexcept -> core::result<program,gl_error_message>
{
  static_assert(
    detail::is_unique({Types...}),
    "It is invalid to link multiple of the same shader type at once"
  );

  return do_link({shaders.id()...});
}

//------------------------------------------------------------------------------
// Constructors / Destructors / Assignment
//------------------------------------------------------------------------------

inline
alloy::render::gl::program::program()
  noexcept
  : m_id{invalid_id}
{

}

inline
alloy::render::gl::program::program(id_type id)
  : m_id{id}
{
  ALLOY_ASSERT(::glIsProgram(id) != GL_FALSE);
}

inline
alloy::render::gl::program::program(program&& other)
  noexcept
  : m_id{other.release()}
{

}

//------------------------------------------------------------------------------

inline
alloy::render::gl::program::~program()
{
  reset();
}

//------------------------------------------------------------------------------

inline
auto alloy::render::gl::program::operator=(program other)
  noexcept
  -> program&
{
  std::swap(m_id, other.m_id);
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline
auto alloy::render::gl::program::id()
  const noexcept -> id_type
{
  return m_id;
}

inline
auto alloy::render::gl::program::is_linked()
  const noexcept -> bool
{
  return m_id != invalid_id;
}

inline
auto alloy::render::gl::program::get_uniform_location(core::zstring_view name)
  const noexcept -> core::result<uniform_location, std::error_code>
{
  const auto location = ::glGetUniformLocation(m_id, name.c_str());

  if (location == -1) {
    return core::fail(error::uniform_not_found);
  }

  return static_cast<uniform_location>(location);
}

inline
auto alloy::render::gl::program::get_attribute_location(core::zstring_view name)
  const noexcept -> core::result<attribute_location, std::error_code>
{
  const auto location = ::glGetAttribLocation(m_id, name.c_str());

  if (location == -1) {
    return core::fail(error::attribute_not_found);
  }

  return static_cast<attribute_location>(location);
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline auto alloy::render::gl::program::release()
  noexcept -> id_type
{
  return std::exchange(m_id, invalid_id);
}

inline auto alloy::render::gl::program::reset()
  noexcept -> void
{
  if (is_linked()) {
    ::glDeleteProgram(m_id);
    m_id = invalid_id;
  }
}

//=============================================================================
// non-member functions : class : program
//=============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

inline
auto alloy::render::gl::operator==(const program& lhs, const program& rhs)
  noexcept -> bool
{
  return lhs.id() == rhs.id();
}

inline
auto alloy::render::gl::operator!=(const program& lhs, const program& rhs)
  noexcept -> bool
{
  return lhs.id() != rhs.id();
}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

inline
auto alloy::render::gl::use_program(const program& p)
  noexcept -> void
{
  ALLOY_ASSERT(p.is_linked());

  ::glUseProgram(p.id());
}

#endif /* ALLOY_RENDER_GL_MATERIAL_PROGRAM_HPP */
