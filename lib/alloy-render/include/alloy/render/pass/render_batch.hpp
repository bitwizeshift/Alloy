// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file render_batch.hpp
///
/// \brief This header defines a collection of models to draw, along with the
///        instances of each
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_PASS_RENDER_BATCH_HPP
#define ALLOY_RENDER_PASS_RENDER_BATCH_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/model/model_spec.hpp"

#include "alloy/core/containers/vector.hpp"

#include <cstddef>     // std::byte, std::size_t
#include <cstdint>     // std::uint32_t
#include <span>        // std::span, std::as_bytes
#include <type_traits> // std::is_trivially_copyable_v

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A model in a batch, along with where its instances are found
  //////////////////////////////////////////////////////////////////////////////
  struct batch_entry
  {
    /// The model to draw
    model_handle model;

    /// The offset of the first instance's values within the batch's
    /// instance data, in bytes
    std::size_t offset;

    /// The number of instances to draw
    std::uint32_t instance_count;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of models to draw, along with the values of each of
  ///        their instances
  ///
  /// The values of every instance of every entry are kept together in a
  /// single block of memory, so that a device can send all of them at once.
  /// The values of each instance are laid out as the instance inputs of the
  /// model's programs describe.
  ///
  /// Entries are kept in the order they were added. Devices are free to draw
  /// them in whatever order is cheapest.
  ///
  /// A batch is intended to be filled, drawn, and cleared again every frame;
  /// clearing keeps the memory it has already grown to.
  //////////////////////////////////////////////////////////////////////////////
  class render_batch
  {
    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an empty batch
    render_batch() = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets every entry of this batch, in the order they were added
    ///
    /// \return the entries
    auto entries() const noexcept -> std::span<const batch_entry>;

    /// \brief Gets the values of every instance of every entry
    ///
    /// \return the instance data
    auto instance_data() const noexcept -> std::span<const std::byte>;

    /// \brief Queries whether this batch has no entries
    ///
    /// \return `true` if there is nothing to draw
    auto empty() const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Adds the \p instances of \p model to this batch
    ///
    /// Adding no instances adds nothing.
    ///
    /// \pre `Instance` is laid out as the instance inputs of \p model's
    ///      programs describe
    /// \tparam Instance the type holding the values of one instance
    /// \tparam Extent the extent of \p instances
    /// \param model the model to draw
    /// \param instances the values of each instance
    template <typename Instance, std::size_t Extent>
      requires std::is_trivially_copyable_v<Instance>
    auto add(
      model_handle model,
      std::span<const Instance, Extent> instances
    ) -> void;

    /// \brief Adds \p count instances of \p model to this batch, whose values
    ///        are \p instances
    ///
    /// Adding no instances adds nothing.
    ///
    /// \pre \p instances holds \p count instances, each laid out as the
    ///      instance inputs of \p model's programs describe
    /// \param model the model to draw
    /// \param instances the values of each instance
    /// \param count the number of instances
    auto add(
      model_handle model,
      std::span<const std::byte> instances,
      std::uint32_t count
    ) -> void;

    /// \brief Removes every entry from this batch, keeping its memory for
    ///        reuse
    auto clear() noexcept -> void;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::vector<batch_entry> m_entries;
    core::vector<std::byte> m_instance_data;
  };

} // namespace alloy::render

//==============================================================================
// definitions : class : render_batch
//==============================================================================

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <typename Instance, std::size_t Extent>
  requires std::is_trivially_copyable_v<Instance>
inline
auto alloy::render::render_batch::add(
  model_handle model,
  std::span<const Instance, Extent> instances
) -> void
{
  add(
    model,
    std::as_bytes(instances),
    static_cast<std::uint32_t>(instances.size())
  );
}

#endif /* ALLOY_RENDER_PASS_RENDER_BATCH_HPP */
