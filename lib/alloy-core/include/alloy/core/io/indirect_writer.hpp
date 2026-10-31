// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file indirect_writer.hpp
///
/// \brief This header contains the definition of the indirect_writer, a writer
///        that writes to another writer that it refers to
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_IO_INDIRECT_WRITER_HPP
#define ALLOY_CORE_IO_INDIRECT_WRITER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/io/writer.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code

namespace alloy::core {

  //===========================================================================
  // class : indirect_writer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A writer that forwards every write to another writer
  ///
  /// This allows a writer to be handed to something that takes ownership of
  /// the writer it is given, while the caller keeps the original.
  ///
  /// \note The indirect_writer does not own the writer it refers to. Care
  ///       must be taken to ensure that the indirect_writer does not outlive
  ///       it.
  /////////////////////////////////////////////////////////////////////////////
  class indirect_writer final : public writer
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs an indirect_writer that writes to \p destination
    ///
    /// \pre \p destination must outlive the indirect_writer
    ///
    /// \param destination the writer to write to
    explicit indirect_writer(not_null<writer*> destination) noexcept;
    indirect_writer(indirect_writer&&) noexcept = default;
    indirect_writer(const indirect_writer&) noexcept = default;

    //-------------------------------------------------------------------------

    auto operator=(indirect_writer&&) noexcept -> indirect_writer& = default;
    auto operator=(const indirect_writer&) noexcept
      -> indirect_writer& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the writer that this indirect_writer writes to
    ///
    /// \return the underlying writer
    [[nodiscard]]
    auto destination() const noexcept -> writer&;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Writing
    //-------------------------------------------------------------------------
  private:

    auto do_write(std::span<const std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code> override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    not_null<writer*> m_destination;
  };

} // namespace alloy::core

//=============================================================================
// inline definitions : class : indirect_writer
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
alloy::core::indirect_writer::indirect_writer(not_null<writer*> destination)
  noexcept
  : m_destination{destination}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::indirect_writer::destination() const noexcept -> writer&
{
  return *m_destination;
}

//-----------------------------------------------------------------------------
// Virtual Hooks : Writing
//-----------------------------------------------------------------------------

inline
auto alloy::core::indirect_writer::do_write(std::span<const std::byte> buffer)
  noexcept -> result<std::size_t,std::error_code>
{
  return m_destination->write(buffer);
}

#endif /* ALLOY_CORE_IO_INDIRECT_WRITER_HPP */
