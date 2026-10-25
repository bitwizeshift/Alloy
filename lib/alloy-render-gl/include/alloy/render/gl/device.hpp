// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file device.hpp
///
/// \brief This header defines the OpenGL implementation of a render device
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_DEVICE_HPP
#define ALLOY_RENDER_GL_DEVICE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/gl/capabilities.hpp"

#include "alloy/render/gl/mesh/mesh.hpp"
#include "alloy/render/gl/shader/program.hpp"
#include "alloy/render/gl/surface/framebuffer.hpp"
#include "alloy/render/gl/surface/presentation_target.hpp"
#include "alloy/render/gl/texture/sampler_cache.hpp"
#include "alloy/render/gl/texture/texture.hpp"

#include "alloy/core/containers/vector.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/render/device.hpp"
#include "alloy/render/error.hpp"
#include "alloy/render/material/material_spec.hpp"
#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/model/model_spec.hpp"
#include "alloy/render/resource/handle_pool.hpp"
#include "alloy/render/shader/program_layout.hpp"
#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/shader/shader_data_type.hpp"
#include "alloy/render/surface/surface_spec.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include <GL/glew.h>

#include <cstdint> // std::uint32_t
#include <variant> // std::variant

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A render device that creates its resources in an OpenGL context
  ///
  /// The context that was current when the device was made must be current
  /// whenever the device, or any resource it created, is used or destroyed.
  ///
  /// Binding groups are ignored, since OpenGL has a single namespace of
  /// binding points per kind of resource.
  //////////////////////////////////////////////////////////////////////////////
  class device final : public render::device
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Makes a device that creates resources in the current context
    ///
    /// \pre an OpenGL 4.1 context is current, and GLEW has been initialized
    /// \return the device
    static auto from_current_context() noexcept -> device;

    //--------------------------------------------------------------------------
    // Constructors / Destructor
    //--------------------------------------------------------------------------
  public:

    ~device() override = default;

    //--------------------------------------------------------------------------
    // Resource Creation
    //--------------------------------------------------------------------------
  public:

    using render::device::create_surface;

    /// \brief Creates a surface that draws into the default framebuffer of
    ///        this device's context, which \p target presents
    ///
    /// \param target the target that presents the default framebuffer, which
    ///        must outlive the surface
    /// \return the surface
    auto create_surface(presentation_target& target) -> unique_surface;

    //--------------------------------------------------------------------------
    // Drawing
    //--------------------------------------------------------------------------
  public:

    /// \brief Binds the program, uniform buffers, and textures of \p material
    ///        for drawing
    ///
    /// \pre \p material, its program, and its textures are alive
    /// \param material the material to bind
    auto bind_material(material_handle material) noexcept -> void;

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A linked program, along with the layout that materials are
    ///        resolved against
    ///////////////////////////////////////////////////////////////////////////
    struct program_entry
    {
      gl::program program;
      program_layout layout;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The unique owner of a uniform buffer object, along with the
    ///        binding point that it is bound to
    ///////////////////////////////////////////////////////////////////////////
    class uniform_buffer
    {
    public:

      uniform_buffer(std::uint32_t binding, ::GLuint id) noexcept;
      uniform_buffer(uniform_buffer&& other) noexcept;
      uniform_buffer(const uniform_buffer&) = delete;

      ~uniform_buffer();

      auto operator=(uniform_buffer&&) -> uniform_buffer& = delete;
      auto operator=(const uniform_buffer&) -> uniform_buffer& = delete;

      auto binding() const noexcept -> std::uint32_t;
      auto id() const noexcept -> ::GLuint;

    private:

      std::uint32_t m_binding;
      ::GLuint m_id;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A texture that a material binds, along with the texture unit
    ///        and sampler object it is bound with
    ///////////////////////////////////////////////////////////////////////////
    struct bound_texture
    {
      std::uint32_t unit;
      texture_handle texture;
      ::GLuint sampler;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The objects that a material binds when it is drawn with
    ///////////////////////////////////////////////////////////////////////////
    struct material_entry
    {
      program_handle program;
      core::vector<uniform_buffer> uniform_buffers;
      core::vector<bound_texture> textures;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A submesh of a model, along with what draws it
    ///////////////////////////////////////////////////////////////////////////
    struct model_part
    {
      program_handle program;
      material_handle material;
      std::uint32_t submesh;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief An instance input of a model, as it is read from the values of
    ///        each instance
    ///////////////////////////////////////////////////////////////////////////
    struct instance_attribute
    {
      std::uint32_t location;
      shader_data_type type;
      std::uint32_t offset;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A mesh, along with everything needed to draw each of its
    ///        submeshes
    ///////////////////////////////////////////////////////////////////////////
    struct model_entry
    {
      mesh_handle mesh;
      core::vector<model_part> parts;
      core::vector<instance_attribute> instance_attributes;
      std::uint32_t instance_stride;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A surface that draws into the default framebuffer
    ///////////////////////////////////////////////////////////////////////////
    struct window_surface
    {
      core::not_null<presentation_target*> target;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A surface that draws into textures
    ///////////////////////////////////////////////////////////////////////////
    struct texture_surface
    {
      gl::framebuffer framebuffer;
      surface_extent extent;
      bool has_depth;
    };

    /// \brief Either kind of surface
    using surface_entry = std::variant<window_surface, texture_surface>;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    explicit device(const capabilities& caps) noexcept;

    //--------------------------------------------------------------------------
    // Virtual Hooks
    //--------------------------------------------------------------------------
  private:

    auto do_create_program(const program_spec& spec, program_layout layout)
      -> core::result<program_handle, render_error> override;
    auto do_create_texture(const texture_spec& spec)
      -> core::result<texture_handle, render_error> override;
    auto do_create_material(const material_spec& spec)
      -> core::result<material_handle, render_error> override;
    auto do_create_mesh(const render::mesh& mesh)
      -> core::result<mesh_handle, render_error> override;
    auto do_create_model(const model_spec& spec)
      -> core::result<model_handle, render_error> override;
    auto do_create_surface(const surface_spec& spec)
      -> core::result<surface_handle, render_error> override;

    auto do_dispose(program_handle handle) noexcept -> void override;
    auto do_dispose(texture_handle handle) noexcept -> void override;
    auto do_dispose(material_handle handle) noexcept -> void override;
    auto do_dispose(mesh_handle handle) noexcept -> void override;
    auto do_dispose(model_handle handle) noexcept -> void override;
    auto do_dispose(surface_handle handle) noexcept -> void override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    capabilities m_capabilities;
    sampler_cache m_samplers;
    handle_pool<program_tag, program_entry> m_programs;
    handle_pool<texture_tag, texture> m_textures;
    handle_pool<material_tag, material_entry> m_materials;
    handle_pool<mesh_tag, gl::mesh> m_meshes;
    handle_pool<model_tag, model_entry> m_models;
    handle_pool<surface_tag, surface_entry> m_surfaces;
  };

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_DEVICE_HPP */
