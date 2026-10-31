// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file device.hpp
///
/// \brief This header defines the interface through which GPU resources are
///        created, along with the owners of those resources
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_DEVICE_HPP
#define ALLOY_RENDER_DEVICE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/error.hpp"
#include "alloy/render/material/material_spec.hpp"
#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/model/model_spec.hpp"
#include "alloy/render/pass/render_pass.hpp"
#include "alloy/render/resource/resource_handle.hpp"
#include "alloy/render/shader/program_layout.hpp"
#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/surface/surface_spec.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include "alloy/core/utilities/result.hpp"

#include <utility> // std::exchange

namespace alloy::render {

  template <typename Tag>
  class unique_resource;

  /// \brief The unique owner of a program created by a device
  using unique_program = unique_resource<program_tag>;

  /// \brief The unique owner of a texture created by a device
  using unique_texture = unique_resource<texture_tag>;

  /// \brief The unique owner of a material created by a device
  using unique_material = unique_resource<material_tag>;

  /// \brief The unique owner of a mesh uploaded by a device
  using unique_mesh = unique_resource<mesh_tag>;

  /// \brief The unique owner of a model created by a device
  using unique_model = unique_resource<model_tag>;

  /// \brief The unique owner of a surface that a device draws into
  using unique_surface = unique_resource<surface_tag>;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The interface through which GPU resources are created
  ///
  /// Each resource is returned in a `unique_resource`, which disposes of the
  /// resource when it is destroyed. Resources refer to one another through
  /// non-owning handles; for example, a material refers to its program and
  /// textures, and a model refers to its mesh and materials, all of which
  /// must outlive it.
  ///
  /// A device must outlive every resource it creates.
  //////////////////////////////////////////////////////////////////////////////
  class device
  {
    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    device(const device&) = delete;
    device(device&&) = delete;

    //--------------------------------------------------------------------------

    virtual ~device() = default;

    //--------------------------------------------------------------------------

    auto operator=(const device&) -> device& = delete;
    auto operator=(device&&) -> device& = delete;

    //--------------------------------------------------------------------------
    // Resource Creation
    //--------------------------------------------------------------------------
  public:

    /// \brief Creates the program described by \p spec
    ///
    /// \param spec the program spec
    /// \return the program on success
    auto create_program(const program_spec& spec)
      -> core::result<unique_program, render_error>;

    /// \brief Creates the texture described by \p spec
    ///
    /// \param spec the texture spec
    /// \return the texture on success
    auto create_texture(const texture_spec& spec)
      -> core::result<unique_texture, render_error>;

    /// \brief Creates the material described by \p spec
    ///
    /// \param spec the material spec
    /// \return the material on success
    auto create_material(const material_spec& spec)
      -> core::result<unique_material, render_error>;

    /// \brief Uploads \p mesh so that it may be drawn
    ///
    /// \param mesh the mesh to upload
    /// \return the uploaded mesh on success
    auto create_mesh(const mesh& mesh)
      -> core::result<unique_mesh, render_error>;

    /// \brief Creates the model described by \p spec
    ///
    /// \param spec the model spec
    /// \return the model on success, or
    ///         * `render_errc::stale_handle` if the mesh or a material has been
    ///           disposed,
    ///         * `render_errc::submesh_count_mismatch` if there is not exactly
    ///           one material per submesh,
    ///         * `render_errc::missing_vertex_input` if a program reads a
    ///           vertex input that the mesh does not provide, or
    ///         * `render_errc::instance_layout_mismatch` if the programs do
    ///           not declare the same instance inputs, or declare instance
    ///           inputs that the device cannot read per instance
    auto create_model(const model_spec& spec)
      -> core::result<unique_model, render_error>;

    /// \brief Creates the surface described by \p spec, which draws into
    ///        textures
    ///
    /// \param spec the surface spec
    /// \return the surface on success, or
    ///         * `render_errc::stale_handle` if a texture has been disposed, or
    ///         * `render_errc::incompatible_surface` if the textures cannot be
    ///           drawn into together
    auto create_surface(const surface_spec& spec)
      -> core::result<unique_surface, render_error>;

    //--------------------------------------------------------------------------
    // Drawing
    //--------------------------------------------------------------------------
  public:

    /// \brief Draws the batch of \p pass into \p surface
    ///
    /// The surface is cleared first if the pass asks for it. Everything in
    /// the batch is drawn, in whatever order the device finds cheapest.
    ///
    /// \pre \p surface, and every model in the batch along with everything
    ///      it refers to, are alive
    /// \param surface the surface to draw into
    /// \param pass the pass to draw
    /// \return nothing on success, or an error if a global parameter of the
    ///         pass names nothing that a program in the batch declares, or
    ///         does not match the type it is bound to. Nothing is drawn on
    ///         failure, although the surface may have been cleared.
    virtual auto render(surface_handle surface, const render_pass& pass)
      -> core::result<void, render_error> = 0;

    /// \brief Presents everything drawn into \p surface since it was last
    ///        presented
    ///
    /// Surfaces that draw into textures have nothing to present; what is
    /// drawn into them may be sampled as soon as it has been drawn.
    ///
    /// \pre \p surface is alive
    /// \param surface the surface to present
    virtual auto present(surface_handle surface) -> void = 0;

    //--------------------------------------------------------------------------
    // Protected Constructors
    //--------------------------------------------------------------------------
  protected:

    device() noexcept = default;

    //--------------------------------------------------------------------------
    // Protected Resource Ownership
    //--------------------------------------------------------------------------
  protected:

    /// \brief Takes ownership of a resource that this device created
    ///        outside of the creation functions, such as a surface for a
    ///        target that only a particular device can draw into
    ///
    /// \pre \p handle refers to a live resource created by this device
    /// \param handle the handle of the resource
    /// \return the owner of the resource
    template <typename Tag>
    auto adopt(resource_handle<Tag> handle) noexcept -> unique_resource<Tag>;

    //--------------------------------------------------------------------------
    // Virtual Hooks
    //--------------------------------------------------------------------------
  private:

    /// \brief Creates the program described by \p spec
    ///
    /// \param spec the program spec
    /// \param layout the validated layout of \p spec
    /// \return the handle of the program on success
    virtual auto do_create_program(
      const program_spec& spec,
      program_layout layout
    ) -> core::result<program_handle, render_error> = 0;

    /// \brief Creates the texture described by \p spec
    ///
    /// \param spec the texture spec
    /// \return the handle of the texture on success
    virtual auto do_create_texture(const texture_spec& spec)
      -> core::result<texture_handle, render_error> = 0;

    /// \brief Creates the material described by \p spec
    ///
    /// \param spec the material spec
    /// \return the handle of the material on success
    virtual auto do_create_material(const material_spec& spec)
      -> core::result<material_handle, render_error> = 0;

    /// \brief Uploads \p mesh
    ///
    /// \param mesh the mesh to upload
    /// \return the handle of the mesh on success
    virtual auto do_create_mesh(const mesh& mesh)
      -> core::result<mesh_handle, render_error> = 0;

    /// \brief Creates the model described by \p spec
    ///
    /// \param spec the model spec
    /// \return the handle of the model on success
    virtual auto do_create_model(const model_spec& spec)
      -> core::result<model_handle, render_error> = 0;

    /// \brief Creates the surface described by \p spec
    ///
    /// \param spec the surface spec
    /// \return the handle of the surface on success
    virtual auto do_create_surface(const surface_spec& spec)
      -> core::result<surface_handle, render_error> = 0;

    /// \{
    /// \brief Disposes of the resource identified by \p handle
    ///
    /// \pre \p handle refers to a live resource created by this device
    /// \param handle the handle of the resource
    virtual auto do_dispose(program_handle handle) noexcept -> void = 0;
    virtual auto do_dispose(texture_handle handle) noexcept -> void = 0;
    virtual auto do_dispose(material_handle handle) noexcept -> void = 0;
    virtual auto do_dispose(mesh_handle handle) noexcept -> void = 0;
    virtual auto do_dispose(model_handle handle) noexcept -> void = 0;
    virtual auto do_dispose(surface_handle handle) noexcept -> void = 0;
    /// \}

    template <typename>
    friend class unique_resource;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The unique owner of a resource created by a device
  ///
  /// The resource is disposed of when its owner is destroyed or reset, and
  /// may be referred to by others through the non-owning handle returned by
  /// `get()`.
  ///
  /// \tparam Tag the family of resource that is owned
  //////////////////////////////////////////////////////////////////////////////
  template <typename Tag>
  class unique_resource
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using handle_type = resource_handle<Tag>;

    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an owner that owns no resource
    unique_resource() noexcept = default;

    /// \brief Constructs an owner by taking ownership from \p other
    ///
    /// \post `other` owns no resource
    /// \param other the owner to take ownership from
    unique_resource(unique_resource&& other) noexcept;
    unique_resource(const unique_resource&) = delete;

    //--------------------------------------------------------------------------

    /// \brief Disposes of the owned resource, if any
    ~unique_resource();

    //--------------------------------------------------------------------------

    /// \brief Disposes of the owned resource, if any, and takes ownership
    ///        from \p other
    ///
    /// \post `other` owns no resource
    /// \param other the owner to take ownership from
    /// \return reference to `(*this)`
    auto operator=(unique_resource&& other) noexcept -> unique_resource&;
    auto operator=(const unique_resource&) -> unique_resource& = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets a non-owning handle to the owned resource
    ///
    /// \return the handle, which is null if no resource is owned
    auto get() const noexcept -> handle_type;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gives up ownership of the owned resource without disposing of
    ///        it
    ///
    /// \return the handle of the resource that was owned
    auto release() noexcept -> handle_type;

    /// \brief Disposes of the owned resource, if any
    auto reset() noexcept -> void;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    unique_resource(device* owner, handle_type handle) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    device* m_device = nullptr;
    handle_type m_handle = {};

    friend class device;
  };

} // namespace alloy::render

//==============================================================================
// definitions : class : device
//==============================================================================

//------------------------------------------------------------------------------
// Protected Resource Ownership
//------------------------------------------------------------------------------

template <typename Tag>
inline
auto alloy::render::device::adopt(resource_handle<Tag> handle)
  noexcept -> unique_resource<Tag>
{
  return unique_resource<Tag>{this, handle};
}

//==============================================================================
// definitions : class : unique_resource
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//------------------------------------------------------------------------------

template <typename Tag>
inline
alloy::render::unique_resource<Tag>::unique_resource(unique_resource&& other)
  noexcept
  : m_device{std::exchange(other.m_device, nullptr)},
    m_handle{std::exchange(other.m_handle, handle_type{})}
{

}

template <typename Tag>
inline
alloy::render::unique_resource<Tag>::~unique_resource<Tag>()
{
  reset();
}

template <typename Tag>
inline
auto alloy::render::unique_resource<Tag>::operator=(unique_resource&& other)
  noexcept -> unique_resource&
{
  if (this != &other) {
    reset();
    m_device = std::exchange(other.m_device, nullptr);
    m_handle = std::exchange(other.m_handle, handle_type{});
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <typename Tag>
inline
auto alloy::render::unique_resource<Tag>::get() const noexcept -> handle_type
{
  return m_handle;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <typename Tag>
inline
auto alloy::render::unique_resource<Tag>::release() noexcept -> handle_type
{
  m_device = nullptr;
  return std::exchange(m_handle, handle_type{});
}

template <typename Tag>
inline
auto alloy::render::unique_resource<Tag>::reset() noexcept -> void
{
  if (m_device == nullptr) {
    return;
  }
  std::exchange(m_device, nullptr)->do_dispose(
    std::exchange(m_handle, handle_type{})
  );
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

template <typename Tag>
inline
alloy::render::unique_resource<Tag>::unique_resource(
  device* owner,
  handle_type handle
) noexcept
  : m_device{owner},
    m_handle{handle}
{

}

#endif /* ALLOY_RENDER_DEVICE_HPP */
