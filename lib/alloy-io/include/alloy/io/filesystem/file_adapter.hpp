/*****************************************************************************
 * \file file_adapter.hpp
 *
 * \brief
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
#ifndef ALLOY_IO_FILESYSTEM_FILE_ADAPTER_HPP
#define ALLOY_IO_FILESYSTEM_FILE_ADAPTER_HPP

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
  class file_adapter
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