////////////////////////////////////////////////////////////////////////////////
/// \file mesh.hpp
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

#ifndef ALLOY_RENDER_GL_MESH_MESH_HPP
#define ALLOY_RENDER_GL_MESH_MESH_HPP

#include "alloy/render/mesh/mesh.hpp"

#include <GL/glew.h>

#include <array>

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A generic mesh written in terms of OpenGL vertex array objects
  //////////////////////////////////////////////////////////////////////////////
  class mesh
  {
    //--------------------------------------------------------------------------
    // Public Members
    //--------------------------------------------------------------------------
  public:

    static auto make(const render::mesh& m) -> mesh;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    mesh(mesh&& other);
    mesh(const mesh&) = delete;

    //--------------------------------------------------------------------------

    ~mesh();

    //--------------------------------------------------------------------------

    auto operator=(mesh&&) -> mesh& = delete;
    auto operator=(const mesh&) -> mesh& = delete;

    //--------------------------------------------------------------------------
    // Render
    //--------------------------------------------------------------------------
  public:

    /// \brief Draws elements from this mesh
    auto draw() -> void;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    ::GLuint m_vao;
    ::GLuint m_vbo;
    ::GLuint m_ebo;
    ::GLenum m_topology;
    std::size_t m_indices;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    mesh(::GLuint vao, ::GLuint vbo, ::GLuint ebo, ::GLenum topology, std::size_t indices);

  };

} // namespace alloy::render::gl


#endif /* ALLOY_RENDER_GL_MESH_MESH_HPP */
