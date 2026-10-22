// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file indirect_reader.hpp
///
/// \brief This header contains the definition of the indirect_reader, a reader
///        that reads from another reader that it refers to
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_IO_INDIRECT_READER_HPP
#define ALLOY_CORE_IO_INDIRECT_READER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/io/reader.hpp"

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code

namespace alloy::core {

  //===========================================================================
  // class : indirect_reader
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A reader that forwards every read to another reader
  ///
  /// This allows a reader to be handed to something that takes ownership of
  /// the reader it is given, while the caller keeps the original.
  ///
  /// \note The indirect_reader does not own the reader it refers to. Care
  ///       must be taken to ensure that the indirect_reader does not outlive
  ///       it.
  /////////////////////////////////////////////////////////////////////////////
  class indirect_reader final : public reader
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs an indirect_reader that reads from \p source
    ///
    /// \pre \p source must outlive the indirect_reader
    ///
    /// \param source the reader to read from
    explicit indirect_reader(not_null<reader*> source) noexcept;
    indirect_reader(indirect_reader&&) noexcept = default;
    indirect_reader(const indirect_reader&) noexcept = default;

    //-------------------------------------------------------------------------

    auto operator=(indirect_reader&&) noexcept -> indirect_reader& = default;
    auto operator=(const indirect_reader&) noexcept
      -> indirect_reader& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the reader that this indirect_reader reads from
    ///
    /// \return the underlying reader
    [[nodiscard]]
    auto source() const noexcept -> reader&;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Reading
    //-------------------------------------------------------------------------
  private:

    auto do_read(std::span<std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code> override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    not_null<reader*> m_source;
  };

} // namespace alloy::core

//=============================================================================
// inline definitions : class : indirect_reader
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
alloy::core::indirect_reader::indirect_reader(not_null<reader*> source)
  noexcept
  : m_source{source}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::indirect_reader::source() const noexcept -> reader&
{
  return *m_source;
}

//-----------------------------------------------------------------------------
// Virtual Hooks : Reading
//-----------------------------------------------------------------------------

inline
auto alloy::core::indirect_reader::do_read(std::span<std::byte> buffer)
  noexcept -> result<std::size_t,std::error_code>
{
  return m_source->read(buffer);
}

#endif /* ALLOY_CORE_IO_INDIRECT_READER_HPP */
