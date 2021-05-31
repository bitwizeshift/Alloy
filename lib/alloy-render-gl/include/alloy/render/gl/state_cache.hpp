////////////////////////////////////////////////////////////////////////////////
/// \file state_cache.hpp
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

#ifndef ALLOY_RENDER_GL_STATE_CACHE_HPP
#define ALLOY_RENDER_GL_STATE_CACHE_HPP

#include "alloy/render/mesh/fill_mode.hpp"
#include "alloy/render/material/blend_equation.hpp"
#include "alloy/render/material/blend_factor.hpp"
#include "alloy/render/material/cull_face.hpp"
#include "alloy/render/material/depth_compare.hpp"

//#include "program.hpp"

#include <GL/glew.h>

#include <optional> // std::optional

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A small aggregate containing Open-GL specific blend configurations
  //////////////////////////////////////////////////////////////////////////////
  struct blend_config
  {
    blend_factor source      = blend_factor::zero;
    blend_factor destination = blend_factor::one;
    blend_equation equation  = blend_equation::add;
    bool enabled = false;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A small aggregate containing Open-GL specific depth configurations
  //////////////////////////////////////////////////////////////////////////////
  struct depth_config
  {
    depth_compare depth_function = depth_compare::less;
    bool enabled = false;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A small aggregate containing Open-GL specific culling configurations
  //////////////////////////////////////////////////////////////////////////////
  struct cull_config
  {
    // This one is written in terms of 'GLenum', since the API for faces does
    // not offer a 'GL_NONE' for culling, but the `cull_face` has a `none`
    // type.
    bool enabled = false;
    ::GLenum front_face = GL_BACK;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A small aggregate containing Open-GL specific polygon
  ///        configurations
  //////////////////////////////////////////////////////////////////////////////
  struct polygon_config
  {
    fill_mode fill = fill_mode::solid;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An Open-GL state cache to prevent needing to repeatedly change
  ///        states.
  ///
  /// \note
  /// ALL state-changing operations must be proxied through this state_cache,
  /// otherwise you will run the risk of the cache being out-of-date with the
  /// actual Open-Gl system -- and this can result in confusing behavior, such
  /// as programs not switching when intended if it assumes the state has not
  /// yet changed.
  ///
  /// \warning
  /// The default state of the cache contains all of the default states as
  /// required by the Open-GL spec. If any state is changed prior to creating
  /// this cache, bad things may happen; so don't do that.
  //////////////////////////////////////////////////////////////////////////////
  class state_cache final
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this state_cache with the defaults as specified by the
    ///        Open-GL spec
    state_cache() noexcept;
    state_cache(const state_cache&) = delete;
    state_cache(state_cache&&) = delete;

    //--------------------------------------------------------------------------

    auto operator=(const state_cache&) -> state_cache& = delete;
    auto operator=(state_cache&&) -> state_cache& = delete;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    auto enable_depth_test(bool enable) noexcept -> void;
    auto set_depth_function(depth_compare func) noexcept -> void;

    auto enable_blend(bool enable) noexcept -> void;
    auto set_blend_function(blend_factor source, blend_factor destination) noexcept -> void;
    auto set_blend_mode(blend_equation equation) noexcept -> void;

    auto enable_cull_face(bool enable) noexcept -> void;
    auto set_cull_face(cull_face face) noexcept -> void;

    auto set_fill_mode(fill_mode mode) noexcept -> void;

//    auto set_active_shader(const program& p) noexcept -> void;
//
    //--------------------------------------------------------------------------
    // Accessors
    //--------------------------------------------------------------------------
  public:

    auto get_blend_config() const noexcept -> const blend_config&;
    auto get_depth_config() const noexcept -> const depth_config&;
    auto get_cull_config() const noexcept -> const cull_config&;
    auto get_polygon_config() const noexcept -> const polygon_config&;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    blend_config m_blend_config;
    depth_config m_depth_config;
    cull_config m_cull_config;
    polygon_config m_polygon_mode;
  };

} // namespace alloy::render::gl

/*

inline
alloy::render::gl::state_cache::state_cache()
  noexcept
  : m_blend_config{},
    m_depth_config{},
    m_cull_config{},
    m_polygon_mode{}
{

}

inline
auto alloy::render::gl::state_cache::set_depth_config(const depth_config& config)
  noexcept -> void
{
  enable_depth_test(true);
  set_depth_function(config.depth_function);
}

inline
auto alloy::render::gl::state_cache::enable_depth_test(bool enable) noexcept -> void
{
  if (m_depth_config.depth_test_enabled == enable) {
    return;
  }
  m_depth_config.depth_test_enabled = enable;

  auto* func = enable ? &::glEnable : &::glDisable;
  (*func)(GL_DEPTH_TEST);
}

inline
auto alloy::render::gl::state_cache::set_depth_function(::GLenum func)
  noexcept -> void
{
  if (m_depth_config.depth_function == func) {
    return;
  }
  m_depth_config.depth_function = func;
  ::glDepthFunc(func);
}

inline
auto alloy::render::gl::state_cache::set_blend_config(const blend_config& config)
  noexcept -> void
{
  enable_blend(config.blend_enabled);
  set_blend_function(config.blend_source, config.blend_destination);
}

inline
auto alloy::render::gl::state_cache::enable_blend(bool enable) noexcept -> void
{
  if (m_blend_config.blend_enabled == enabled) {
    return;
  }
  m_blend_config.blend_enabled = enable;
  auto* func = enable ? &::glEnable : &::glDisable;
  (*func)(GL_BLEND);
}

inline
auto alloy::render::gl::state_cache::set_blend_function(::GLenum source,
                                                        ::GLenum destination)
  noexcept -> void
{
  if ((m_blend_config.blend_source == source) && (m_blend_config.blend_destination == destination) {
    return;
  }
  m_blend_config.blend_source = source;
  m_blend_config.blend_destination = destination;
  ::glBlendFunc(source, destination);
}

inline
auto alloy::render::gl::state_cache::set_cull_config(const cull_config& config)
  noexcept -> void
{
  enable_cull_face(config.cull_enabled);
  set_cull_face(config.front_face);
}

inline
auto alloy::render::gl::state_cache::enable_cull_face(bool enable)
  noexcept -> void
{
  if (m_cull_config.cull_enabled == enabled) {
    return;
  }
  m_cull_config.cull_enabled = enable;
  auto* func = enable ? &::glEnable : &::glDisable;
  (*func)(GL_CULL_FACE);
}

inline
auto alloy::render::gl::state_cache::set_cull_face(::GLenum face)
  noexcept -> void
{
  if (m_cull_config.front_face == face) {
    return;
  }
  m_cull_config.front_face = face;
  ::glCullFace(face);
}

inline
auto alloy::render::gl::state_cache::set_polygon_config(const polygon_config& config)
  noexcept -> void
{
  set_polygon_mode(config.polygon_mode);
}

inline
auto alloy::render::gl::state_cache::set_fill_mode(::GLenum mode)
  noexcept -> void
{
  if (m_polygon_mode.polygon_mode == mode) {
    return;
  }
  m_polygon_mode.polygon_mode = mode;
  ::glPolygonMode(GL_FRONT_AND_BACK, mode);
}

inline
auto alloy::render::gl::state_cache::set_active_shader(const program& p)
    noexcept -> void
{
  if (p.id() == m_active_shader) {
    return;
  }
  m_active_shader = p.id();
  use_program(p);
}

inline
auto alloy::render::gl::state_cache::get_blend_config()
  const noexcept -> const blend_config&
{
  return m_blend_config;
}

inline
auto alloy::render::gl::state_cache::get_depth_config()
  const noexcept -> const gl::depth_config&
{
  return m_depth_config;
}

inline
auto alloy::render::gl::state_cache::get_cull_config()
  const noexcept -> const cull_config&
{
  return m_cull_config;
}

inline
auto alloy::render::gl::state_cache::get_polygon_config()
  const noexcept -> const polygon_config&
{
  return m_polygon_mode;
}
*/

#endif /* ALLOY_RENDER_GL_STATE_CACHE_HPP */
