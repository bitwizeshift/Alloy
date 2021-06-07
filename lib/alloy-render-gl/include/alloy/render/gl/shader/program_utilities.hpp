////////////////////////////////////////////////////////////////////////////////
/// \file program_utilities.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_RENDER_GL_SHADER_PROGRAM_UTILITIES_HPP
#define ALLOY_RENDER_GL_SHADER_PROGRAM_UTILITIES_HPP

#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/io/filesystem/file.hpp"
#include "alloy/render/gl/shader/program.hpp"
#include "alloy/render/gl/error.hpp"

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Utilities to aid in the construction of shader programs
  //////////////////////////////////////////////////////////////////////////////
  struct program_utilities
  {
    program_utilities() = delete;
    ~program_utilities() = delete;

    //--------------------------------------------------------------------------
    // Programs from source
    //--------------------------------------------------------------------------

    /// \brief Makes a shader program from the specified source strings
    ///
    /// \param vertex the vertex shader program
    /// \param fragment the fragment shader program
    /// \return the created shader, on success
    static auto make_program_from_sources(core::zstring_view vertex,
                                          core::zstring_view fragment)
      -> core::result<program, gl_error_message>;

    /// \brief Makes a shader program from the specified source strings
    ///
    /// \param vertex the vertex shader program
    /// \param fragment the fragment shader program
    /// \param geometry the geometry shader program
    /// \return the created shader, on success
    static auto make_program_from_sources(core::zstring_view vertex,
                                          core::zstring_view geometry,
                                          core::zstring_view fragment)
      -> core::result<program, gl_error_message>;

    //--------------------------------------------------------------------------
    // Programs from files
    //--------------------------------------------------------------------------

    /// \brief Makes a program from the specified files
    ///
    /// \param vertex the file containing the vertex shader
    /// \param fragment the file containing the fragment shader
    /// \return the created shader, on success
    static auto make_program_from_files(io::file& vertex,
                                        io::file& fragment)
      -> core::result<program, gl_error_message>;

    /// \brief Makes a program from the specified files
    ///
    /// \param vertex the file containing the vertex shader
    /// \param geometry the file containing the geometry shader
    /// \param fragment the file containing the fragment shader
    /// \return the created shader, on success
    static auto make_program_from_files(io::file& vertex,
                                        io::file& geometry,
                                        io::file& fragment)
      -> core::result<program, gl_error_message>;

    //--------------------------------------------------------------------------
    // Programs from paths
    //--------------------------------------------------------------------------

    /// \brief A simple utility for loading files from disk at the specified
    ///        paths
    ///
    /// This is primarily meant as a debugging utility, since in practice these
    /// shaders should be contained in archive files.
    ///
    /// \param vertex_path the path to the vertex shader
    /// \param fragment_path the path to the fragment shader
    /// \return the created shader, on success
    static auto make_program_from_paths(core::zstring_view vertex_path,
                                        core::zstring_view fragment_path)
      -> core::result<program, gl_error_message>;

    /// \brief A simple utility for loading files from disk at the specified
    ///        paths
    ///
    /// This is primarily meant as a debugging utility, since in practice these
    /// shaders should be contained in archive files.
    ///
    /// \param vertex_path the path to the vertex shader
    /// \param geometry_path the path to the geometry shader
    /// \param fragment_path the path to the fragment shader
    /// \return the created shader, on success
    static auto make_program_from_paths(core::zstring_view vertex_path,
                                        core::zstring_view geometry_path,
                                        core::zstring_view fragment_path)
      -> core::result<program, gl_error_message>;

  };

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_SHADER_PROGRAM_UTILITIES_HPP */
