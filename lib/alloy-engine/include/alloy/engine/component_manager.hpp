/*****************************************************************************
 * \file component_manager.hpp
 *
 * \todo
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_ENGINE_COMPONENT_MANAGER_HPP
#define ALLOY_ENGINE_COMPONENT_MANAGER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/engine/component.hpp"
#include "alloy/engine/entity.hpp"
#include "alloy/core/assert.hpp"

#include <cstddef>       // std::size_t
#include <unordered_map> // std::unordered_map
#include <array>         // std::array
#include <vector>        // std::vector
#include <type_traits>   // std::is_reference, std::is_const, etc
#include <algorithm>     // std::find
#include <iterator>      // std::distance
#include <memory>        // std::unique_ptr

namespace alloy::engine {

  //============================================================================
  // class : component_manager
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The manager for all components in a system
  //////////////////////////////////////////////////////////////////////////////
  class component_manager
  {
    //--------------------------------------------------------------------------
    // Public Static Members
    //--------------------------------------------------------------------------
  public:

    static constexpr auto max_components = std::size_t{32};

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    component_manager() = default;
    component_manager(component_manager&&) = default;
    component_manager(const component_manager&) = delete;

    //--------------------------------------------------------------------------

    component_manager& operator=(component_manager&&) = default;
    component_manager& operator=(const component_manager&) = delete;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Registers a given compnent with the system
    ///
    /// \tparam Component the component to register
    /// \return the index of the component
    template <typename Component>
    component register_component();

    /// \brief Attaches a component to a given entity
    ///
    /// \param e the entity to attach the component to
    /// \param c the component to attach
    /// \return reference to the attached component
    template <typename Component>
    Component& attach_component(entity e, Component c);

    /// \brief Detaches a component from an entity
    ///
    /// \tparam Component the component to detach
    /// \param e the entity to detach the component from
    template <typename Component>
    void detach_component(entity e);

    /// \brief Detaches all components from an entity
    ///
    /// \param e the entity to detach all components from
    void detach_all_components(entity e);

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the index of the specified Component
    ///
    /// \note This function is linear in algorithmic complexity.
    ///
    /// \tparam Component the component to get the index for
    /// \return the component index
    template <typename Component>
    component get_component_index() const;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    class component_info;

    class storage_base;

    template <typename Component>
    class storage;

    template <typename Component>
    storage<Component> get_storage();

    ///<
    std::vector<std::size_t> m_component_ids;
    std::vector<std::unique_ptr<storage_base>> m_component_storages;
  };

  //============================================================================
  // class : component_manager::component_info
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Information about a given component
  //////////////////////////////////////////////////////////////////////////////
  class component_manager::component_info
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using id_type = std::size_t;

    //--------------------------------------------------------------------------
    // Static Member Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets a unique ID for a given component
    ///
    /// \pre Component must be a non-cv-qualified, non-reference/pointer type.
    ///
    /// \tparam Component the type to get the id of
    /// \return the id
    template <typename Component>
    static id_type id() noexcept;

    //--------------------------------------------------------------------------
    // Private Static Members
    //--------------------------------------------------------------------------
  private:

    static std::size_t s_current_id;
  };

  //============================================================================
  // class : component_manager::storage_base
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The base class for storage management of components
  ///
  /// This class exists to expose the 'detach_component' function in a generic
  /// way so that the component_manager is able to detach all components from a
  /// single entity.
  //////////////////////////////////////////////////////////////////////////////
  class component_manager::storage_base
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    storage_base() = default;
    storage_base(storage_base&&) = default;
    storage_base(const storage_base&) = delete;

    //--------------------------------------------------------------------------

    virtual ~storage_base() = default;

    //--------------------------------------------------------------------------

    storage_base& operator=(storage_base&&) = default;
    storage_base& operator=(const storage_base&) = delete;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Detaches a component from the given \p entity
    ///
    /// \param e the entity to detach
    virtual void detach_component(entity e) = 0;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Checks whether this component_manager has a component registered
    ///        for a given entity \p e
    ///
    /// \param e the entity to query
    /// \return \c true if t \p e has an attached component
    virtual bool has_component(entity e) const noexcept = 0;
  };

  //============================================================================
  // class : component_manager::storage
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The underlying storage system for a given component
  //////////////////////////////////////////////////////////////////////////////
  template <typename Component>
  class component_manager::storage final
    : public component_manager::storage_base
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using component_type = Component;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    storage() = default;
    storage(storage&&) = default;
    storage(const storage&) = delete;

    //--------------------------------------------------------------------------

    storage& operator=(storage&&) = default;
    storage& operator=(const storage&) = delete;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Attaches an instance of a given component \p c to a given
    ///        entity \p e
    ///
    /// \note Creating a new component may invalidate existing component
    ///       references.
    ///
    /// \param e the entity to attach the component to
    /// \param c the component to attach
    /// \return reference to the component
    component_type& attach_component(entity e, component_type c);

    /// \brief Detaches a component from the given \p entity
    ///
    /// \note Detaching a component will invalidate existing component
    ///       references.
    ///
    /// \param e the entity to detach
    void detach_component(entity e) override;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets a component from the underlying entity \p e
    ///
    /// \param e the entity to get the component for
    /// \return the component
    component_type& get_component(entity e) const;

    /// \brief Checks whether this component_manager has a component registered
    ///        for a given entity \p e
    ///
    /// \param e the entity to query
    /// \return \c true if t \p e has an attached component
    bool has_component(entity e) const noexcept override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    using index_type = int;
    using entity_to_index_map = std::unordered_map<entity,index_type>;
    using index_to_entity_map = std::unordered_map<index_type,entity>;
    using component_container = std::vector<component_type>;

    component_container m_components; ///< All available components
    entity_to_index_map m_entity_to_index; ///< Maps entities to indices
    index_to_entity_map m_index_to_entity; ///< Maps indices to entities
    std::size_t m_size; ///< Number of entries in this manager
  };

} // namespace alloy::engine

//==============================================================================
// class : component_manager::component_info
//==============================================================================

template <typename Component>
inline alloy::engine::component_manager::component_info::id_type
  alloy::engine::component_manager::component_info::id()
  noexcept
{
  // Must be a value type
  static_assert( !std::is_reference<Component>::value );
  static_assert( !std::is_pointer<Component>::value );
  static_assert( !std::is_const<Component>::value );
  static_assert( !std::is_volatile<Component>::value );
  static_assert( !std::is_void<Component>::value );

  static const auto s_id = s_current_id++;

  return s_id;
}

//==============================================================================
// class : component_manager::storage
//==============================================================================

template <typename Component>
inline typename alloy::engine::component_manager::storage<Component>::component_type&
  alloy::engine::component_manager::storage<Component>
  ::attach_component(entity e, component_type c)
{
  ALLOY_ASSERT(
    m_entity_to_index.find(e) == m_entity_to_index.end(),
    "Component already attached to entity"
  );

  // Put new entry at end and update the maps
  const auto new_index = static_cast<int>(m_components.size());
  m_entity_to_index[e] = new_index;
  m_index_to_entity[new_index] = e;
  m_components.emplace_back(std::move(c));
}

template <typename Component>
inline void alloy::engine::component_manager::storage<Component>
  ::detach_component(entity e)
{
  ALLOY_ASSERT(
    m_entity_to_index.find(e) != m_entity_to_index.end(),
    "Component was never attached to entity"
  );

  const auto removed_entity = e;
  const auto removed_index = m_entity_to_index[e];
  const auto last_index = static_cast<int>(m_components.size() - 1);
  const auto last_entity = m_index_to_entity[last_index];

  // If we aren't removing the last entity, move the last entity
  // into the newly created hole to ensure components remain densely
  // packed.
  if (removed_entity != last_entity) {
    m_components[removed_index] = std::move(m_components[last_index]);
  }
  m_components.pop_back();

  // Update map to point to moved spot
  m_entity_to_index[last_entity] = removed_index;
  m_index_to_entity[removed_index] = last_entity;

  m_entity_to_index.erase(removed_entity);
  m_index_to_entity.erase(last_index);
}

template <typename Component>
inline typename alloy::engine::component_manager::storage<Component>::component_type&
  alloy::engine::component_manager::storage<Component>
  ::get_component(entity e)
  const
{
  const auto it = m_entity_to_index.find(e);
  ALLOY_ASSERT(
    it != m_entity_to_index.end(),
    "Component was never attached to entity"
  );
  const auto index = *it;

  return m_components[index];
}

template <typename Component>
inline bool alloy::engine::component_manager::storage<Component>
  ::has_component(entity e)
  const noexcept
{
  return m_entity_to_index.find(e) != m_entity_to_index.end();
}

//==============================================================================
// class : component_manager
//==============================================================================

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <typename Component>
inline alloy::engine::component
  alloy::engine::component_manager::register_component()
{
  // Must be a value type
  static_assert( !std::is_reference<Component>::value );
  static_assert( !std::is_pointer<Component>::value );
  static_assert( !std::is_const<Component>::value );
  static_assert( !std::is_volatile<Component>::value );
  static_assert( !std::is_void<Component>::value );

  const auto id = component_info::id<Component>();

  ALLOY_ASSERT(
    std::find(
      m_component_ids.begin(),
      m_component_ids.end(),
      id
    ) == m_component_ids.end(),
    "Component must not have already been registered"
  );

  const auto component_index = m_component_ids.size();
  m_component_ids.push_back(id);

  return component{static_cast<component::id_type>(component_index)};
}

template <typename Component>
inline Component&
  alloy::engine::component_manager::attach_component(entity e, Component c)
{
  // Must be a value type
  static_assert( !std::is_reference<Component>::value );
  static_assert( !std::is_pointer<Component>::value );
  static_assert( !std::is_const<Component>::value );
  static_assert( !std::is_volatile<Component>::value );
  static_assert( !std::is_void<Component>::value );

  return get_storage<Component>().attach_component(e, std::move(c));
}

template <typename Component>
inline void alloy::engine::component_manager::detach_component(entity e)
{
  // Must be a value type
  static_assert( !std::is_reference<Component>::value );
  static_assert( !std::is_pointer<Component>::value );
  static_assert( !std::is_const<Component>::value );
  static_assert( !std::is_volatile<Component>::value );
  static_assert( !std::is_void<Component>::value );

  get_storage<Component>().detach_component(e);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <typename Component>
inline alloy::engine::component
  alloy::engine::component_manager::get_component_index()
  const
{
  // Must be a value type
  static_assert( !std::is_reference<Component>::value );
  static_assert( !std::is_pointer<Component>::value );
  static_assert( !std::is_const<Component>::value );
  static_assert( !std::is_volatile<Component>::value );
  static_assert( !std::is_void<Component>::value );

  const auto id = component_info::id<Component>();

  auto it = std::find(
    m_component_ids.begin(),
    m_component_ids.end(),
    id
  );

  ALLOY_ASSERT(it != m_component_ids.end(), "Component not registered");

  const auto diff = std::distance(
    m_component_ids.begin(),
    id
  );

  return component{static_cast<component::id_type>(diff)};
}

#endif /* ALLOY_ENGINE_COMPONENT_MANAGER_HPP */