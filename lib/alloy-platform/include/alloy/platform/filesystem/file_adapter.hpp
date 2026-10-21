// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file file_adapter.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_PLATFORM_FILESYSTEM_FILE_ADAPTER_HPP
#define ALLOY_PLATFORM_FILESYSTEM_FILE_ADAPTER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/platform/api.hpp"
#include "alloy/platform/filesystem/file.hpp"

namespace alloy::platform {

  //===========================================================================
  // class : file_adapter
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An interface for adapting a file into a different type of file.
  ///
  /// This is used to produce higher-order files that do pre/post processing
  /// of data.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_PLATFORM_API file_adapter
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    file_adapter() = default;
    file_adapter(const file_adapter&) = default;
    file_adapter(file_adapter&&) = default;

    //-------------------------------------------------------------------------

    auto operator=(const file_adapter&) -> file_adapter& = default;
    auto operator=(file_adapter&&) -> file_adapter& = default;

    //-------------------------------------------------------------------------
    // Destructor
    //-------------------------------------------------------------------------
  protected:

    // Disallow owning a file_adapter by a handle to this class
    ~file_adapter() = default;

    //-------------------------------------------------------------------------
    // Adapter
    //-------------------------------------------------------------------------
  public:

    /// \brief Adapts file \p f
    ///
    /// \param f the file to adapt
    /// \return the adapted file
    auto adapt(file f) noexcept -> file;

    //-------------------------------------------------------------------------
    // Virtual Hooks
    //-------------------------------------------------------------------------
  private:

    virtual auto do_adapt(file f) noexcept -> file = 0;
  };

} // namespace alloy::platform

//=============================================================================
// inline definitions : class : file_adapter
//=============================================================================

//-----------------------------------------------------------------------------
// Adapter
//-----------------------------------------------------------------------------

inline
auto alloy::platform::file_adapter::adapt(file f) noexcept -> file
{
  if (f.is_open()) {
    return do_adapt(std::move(f));
  }
  return f;
}

#endif /* ALLOY_PLATFORM_FILESYSTEM_FILE_ADAPTER_HPP */
