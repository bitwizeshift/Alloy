////////////////////////////////////////////////////////////////////////////////
/// \file error.hpp
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

#ifndef ALLOY_RENDER_GL_ERROR_HPP
#define ALLOY_RENDER_GL_ERROR_HPP

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/string/string.hpp"
#include "alloy/core/string/zstring_view.hpp"

#include <GL/glew.h>

#include <system_error> // std::error_category
#include <memory> // std::shared_ptr
#include <utility> // std::move
#include <type_traits> // std::true_type

namespace alloy::render::gl {

  //===========================================================================
  // non-member functions : errors
  //===========================================================================

  namespace error {

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Error codes from the OpenGL library
    ///////////////////////////////////////////////////////////////////////////
    enum basic_errors : ::GLenum {
      invalid_enum = GL_INVALID_ENUM,
      invalid_value = GL_INVALID_VALUE,
      invalid_operation = GL_INVALID_OPERATION,
      stack_overflow = GL_STACK_OVERFLOW,
      stack_underflow = GL_STACK_UNDERFLOW,
      out_of_memory = GL_OUT_OF_MEMORY,
      invalid_framebuffer_operation = GL_INVALID_FRAMEBUFFER_OPERATION,
      context_lost = GL_CONTEXT_LOST,
      table_too_large = GL_TABLE_TOO_LARGE,
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Extended error codes not part of OpenGL specifically, but added
    ///        so that coherent values can be returned as expecteds to the
    ///        caller.
    ///////////////////////////////////////////////////////////////////////////
    enum extended_errors {
      uniform_not_found   = 1,
      attribute_not_found = 2,
    };

    /// \brief Gets the basic opengl category
    ///
    /// \return an error category
    auto gl_basic_category() -> const std::error_category&;

    /// \brief Gets the extended opengl category
    ///
    /// \return an error category
    auto gl_extended_category() -> const std::error_category&;

    /// \{
    /// \brief Makes an error code from the specified \p error
    ///
    /// \param error the error
    /// \return the error code
    auto make_error_code(basic_errors error) noexcept -> std::error_code;
    auto make_error_code(extended_errors error) noexcept -> std::error_code;
    /// \}

  } // namespace error

  //===========================================================================
  // class : gl_error_message
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A lightweight string for carrying OpenGL error messages
  ///
  /// A large amount of OpenGL errors dealing with user input is handled through
  /// explicit error messages that contain details about the failure;
  /// for example, the various shader compiling/linking errors are strictly
  /// handled as error logs.
  ///
  /// As a result, this type has been added to satisfy the requirements of the
  /// 'unexpected' type in the core::expected<T,E> type, so that the messages
  /// can be transferred back to the caller in the case of failure.
  /////////////////////////////////////////////////////////////////////////////
  class gl_error_message
  {
    //-------------------------------------------------------------------------
    // Static Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Makes an error message with the specified \p message
    ///
    /// \param message the message
    /// \return an error message
    static auto make(core::string message) -> gl_error_message;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a gl_error_message without any error message
    gl_error_message() noexcept = default;

    /// \brief Constructs an error message
    ///
    /// \param message the message
    explicit gl_error_message(std::shared_ptr<core::string> message) noexcept;

    /// \brief Constructs this error message by moving the contents of \p other
    ///
    /// \param other the other message to move
    gl_error_message(gl_error_message&& other) noexcept = default;

    /// \brief Constructs this error message by copying the contents of \p other
    ///
    /// \param other the other message to copy
    gl_error_message(const gl_error_message& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Move-assigns the contents of \p other
    ///
    /// \param other the other message to move
    /// \return reference to (*this)
    auto operator=(gl_error_message&& other) noexcept -> gl_error_message& = default;

    /// \brief Copy-assigns the contents of \p other
    ///
    /// \param other the other message to copy
    /// \return reference to (*this)
    auto operator=(const gl_error_message& other) noexcept -> gl_error_message& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the message from this error message
    ///
    /// \return the message
    auto message() const noexcept -> core::zstring_view;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    std::shared_ptr<core::string> m_message;
  };

} // namespace alloy::render::gl

namespace std {

  template <>
  struct is_error_code_enum<alloy::render::gl::error::basic_errors> : std::true_type{};

  template <>
  struct is_error_code_enum<alloy::render::gl::error::extended_errors> : std::true_type{};

} // namespace std

#endif /* ALLOY_RENDER_GL_ERROR_HPP */
