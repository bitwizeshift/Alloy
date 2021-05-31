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

#include "alloy/render/gl/state_cache.hpp"

#include "alloy/render/gl/mesh/fill_mode.hpp"
#include "alloy/render/gl/material/blend_equation.hpp"
#include "alloy/render/gl/material/blend_factor.hpp"
#include "alloy/render/gl/material/cull_face.hpp"
#include "alloy/render/gl/material/depth_compare.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {
namespace {

inline
auto gl_conditional_enable(bool enable, ::GLenum value)
  noexcept -> void
{
  auto* func = enable ? &::glEnable : &::glDisable;
  (*func)(value);
}

} // namespace <anonymous>
} // namespace alloy::render::gl

alloy::render::gl::state_cache::state_cache()
  noexcept
  : m_blend_config{},
    m_depth_config{},
    m_cull_config{},
    m_polygon_mode{}
{

}

auto alloy::render::gl::state_cache::enable_depth_test(bool enable)
  noexcept -> void
{
  if (m_depth_config.enabled == enable) {
    return;
  }
  m_depth_config.enabled = enable;

  gl_conditional_enable(enable, GL_DEPTH_TEST);
}

auto alloy::render::gl::state_cache::set_depth_function(depth_compare func)
  noexcept -> void
{
  if (m_depth_config.depth_function == func) {
    return;
  }
  m_depth_config.depth_function = func;
  ::glDepthFunc(to_opengl_type(func));
}

auto alloy::render::gl::state_cache::enable_blend(bool enable)
  noexcept -> void
{
  if (m_blend_config.enabled == enable) {
    return;
  }
  m_blend_config.enabled = enable;

  gl_conditional_enable(enable, GL_BLEND);
}

auto alloy::render::gl::state_cache::set_blend_function(blend_factor source,
                                                        blend_factor destination)
  noexcept -> void
{
  if ((m_blend_config.source == source) && (m_blend_config.destination == destination)) {
    return;
  }
  m_blend_config.source = source;
  m_blend_config.destination = destination;
  ::glBlendFunc(
    to_opengl_type(source),
    to_opengl_type(destination)
  );
}

auto alloy::render::gl::state_cache::set_blend_mode(blend_equation equation)
  noexcept -> void
{
  if (m_blend_config.equation == equation) {
    return;
  }
  m_blend_config.equation = equation;
  ::glBlendEquation(to_opengl_type(equation));
}

auto alloy::render::gl::state_cache::enable_cull_face(bool enable)
  noexcept -> void
{
  if (m_cull_config.enabled == enable) {
    return;
  }
  m_cull_config.enabled = enable;

  gl_conditional_enable(enable, GL_CULL_FACE);
}

auto alloy::render::gl::state_cache::set_cull_face(cull_face face)
  noexcept -> void
{
  // Possibly enable (or disable) culling
  auto f = to_opengl_type(face);
  enable_cull_face(f.has_value());

  if (!f.has_value() || m_cull_config.front_face == f) {
    return;
  }
  m_cull_config.front_face = *f;
  ::glCullFace(*f);
}

auto alloy::render::gl::state_cache::set_fill_mode(fill_mode mode)
  noexcept -> void
{
  if (m_polygon_mode.fill == mode) {
    return;
  }
  m_polygon_mode.fill = mode;

  ::glPolygonMode(GL_FRONT_AND_BACK, to_opengl_type(mode));
}

auto alloy::render::gl::state_cache::get_blend_config()
  const noexcept -> const blend_config&
{
  return m_blend_config;
}

auto alloy::render::gl::state_cache::get_depth_config()
  const noexcept -> const depth_config&
{
  return m_depth_config;
}

auto alloy::render::gl::state_cache::get_cull_config()
  const noexcept -> const cull_config&
{
  return m_cull_config;
}

auto alloy::render::gl::state_cache::get_polygon_config()
  const noexcept -> const polygon_config&
{
  return m_polygon_mode;
}
