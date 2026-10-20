// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file file_adapter.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_IO_FILESYSTEM_FILE_ADAPTER_HPP
#define ALLOY_IO_FILESYSTEM_FILE_ADAPTER_HPP

#include "alloy/io/api.hpp"
#include "alloy/io/filesystem/file.hpp"

namespace alloy::io {

  //===========================================================================
  // class : file_adapter
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An interface for adapting a file into a different type of file.
  ///
  /// This is used to produce higher-order files that do pre/post processing
  /// of data.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_IO_API file_adapter
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    file_adapter() = default;
    file_adapter(const file_adapter&) = default;
    file_adapter(file_adapter&&) = default;

    //-------------------------------------------------------------------------

    file_adapter& operator=(const file_adapter&) = default;
    file_adapter& operator=(file_adapter&&) = default;

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
    file adapt(file f) noexcept;

    //-------------------------------------------------------------------------
    // Virtual Hooks
    //-------------------------------------------------------------------------
  private:

    virtual file do_adapt(file f) noexcept = 0;
  };

} // namespace alloy::io

//=============================================================================
// inline definitions : class : file_adapter
//=============================================================================

//-----------------------------------------------------------------------------
// Adapter
//-----------------------------------------------------------------------------

inline alloy::io::file alloy::io::file_adapter::adapt(file f)
  noexcept
{
  if (f.is_open()) {
    return do_adapt(std::move(f));
  }
  return f;
}

#endif /* ALLOY_IO_FILESYSTEM_FILE_ADAPTER_HPP */
