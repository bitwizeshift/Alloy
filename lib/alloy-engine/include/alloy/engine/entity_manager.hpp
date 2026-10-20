// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file entity_manager.hpp
 *
 * \todo
 *****************************************************************************/

#ifndef ALLOY_ENGINE_ENTITY_MANAGER_HPP
#define ALLOY_ENGINE_ENTITY_MANAGER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/engine/component.hpp"
#include "alloy/engine/entity.hpp"
#include "alloy/engine/signature.hpp"

#include "alloy/core/utilities/not_null.hpp"

#include <vector>  // std::vector
#include <queue>   // std::queue
#include <cstddef> // std::size_t

namespace alloy::engine {

  //============================================================================
  // class : entity_manager
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Manages the creation and destruction of entities
  //////////////////////////////////////////////////////////////////////////////
  class entity_manager
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    class listener;

    //--------------------------------------------------------------------------
    // Public Static Members
    //--------------------------------------------------------------------------
  public:

    static constexpr auto max_entities = std::size_t{5120};

    //--------------------------------------------------------------------------
    // Constructor / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    entity_manager();
    entity_manager(entity_manager&&) = default;
    entity_manager(const entity_manager&) = default;

    //--------------------------------------------------------------------------

    entity_manager& operator=(entity_manager&&) = default;
    entity_manager& operator=(const entity_manager&) = default;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Creates a new entity
    ///
    /// \return the new entity
    entity create() noexcept;

    /// \brief Destroys a given entity
    void destroy(entity e) noexcept;

    /// \brief Sets the signature of a given entity \p e
    ///
    /// \param e the entity to set the signature to
    /// \param sig the component signature
    void set_signature(entity e, signature sig) noexcept;

    /// \brief Gets the signature for a given entity \p e
    ///
    /// \param e the entity to get the signature for
    /// \return the component signature
    signature get_signature(entity e) const noexcept;

    //--------------------------------------------------------------------------

    /// \brief Registers a new listeners to listen for entity events
    ///
    /// \note \p l is not owned by this entity_manager
    /// \pre \p l is not-null
    /// \param l the listener to register
    void register_listener(core::not_null<listener*> l);

    /// \brief Unregisters a listener to no longer listen for entity events
    ///
    /// \note \p l is not owned by this entity_manager
    /// \pre \p l is not-null
    /// \param l the listener to unregister
    void unregister_listener(core::not_null<listener*> l);

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  public:

    using listener_container = std::vector<listener*>;
    using available_entity_container = std::queue<entity>;
    using signature_container = std::vector<signature>;

    listener_container m_listeners;
    available_entity_container m_available_entities;
    signature_container m_entity_signatures;
    entity::id_type m_current_id;
    std::size_t m_total_entities;
  };

  //============================================================================
  // class : entity_manager::listener
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An observer that is invoked when an entity is created or destroyed
  //////////////////////////////////////////////////////////////////////////////
  class entity_manager::listener
  {
    friend class entity_manager;

    //--------------------------------------------------------------------------
    // Constructor / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    listener() = default;
    listener(listener&&) = default;
    listener(const listener&) = default;

    //--------------------------------------------------------------------------

    virtual ~listener() = 0;

    //--------------------------------------------------------------------------

    listener& operator=(listener&&) = default;
    listener& operator=(const listener&) = default;

    //--------------------------------------------------------------------------
    // Callbacks
    //--------------------------------------------------------------------------
  private:

    /// \brief Callback handler for when an entity is created
    ///
    /// \param e the entity that was created
    virtual void on_entity_created(entity e) noexcept;

    /// \brief Callback handler for when an entity is destroyed
    ///
    /// \param e the entity that was destroyed
    virtual void on_entity_destroyed(entity e) noexcept;

    /// \brief Callback handler for when an entity signature is changed
    ///
    /// \param e the entity that had its signature changed
    /// \param sig the new component signature for the entity
    virtual void on_signature_change(entity e,
                                     signature sig) noexcept;
  };

} // namespace alloy::engine

//==============================================================================
// inline-definitions : class : entity_manager::listener
//==============================================================================

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------

inline alloy::engine::entity_manager::listener::~listener() = default;

#endif /* ALLOY_ENGINE_ENTITY_MANAGER_HPP */
