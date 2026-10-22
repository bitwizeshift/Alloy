// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file file_stream_handle.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_PLATFORM_FILESYSTEM_FILE_STREAM_HANDLE_HPP
#define ALLOY_PLATFORM_FILESYSTEM_FILE_STREAM_HANDLE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/platform/filesystem/file_stream.hpp"
#include "alloy/platform/filesystem/file_stream_finalizer.hpp"

#include "alloy/platform/api.hpp"

#include "alloy/core/utilities/not_null.hpp"

namespace alloy::platform {

  //===========================================================================
  // class : file_stream_handle
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A pointer which holds onto a file_stream, and finalizes it using
  ///        RAII on destruction
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_PLATFORM_API file_stream_handle
  {
    //-------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default constructible
    file_stream_handle() noexcept;
    file_stream_handle(
      core::not_null<file_stream*> stream,
      core::not_null<file_stream_finalizer*> finalizer
    ) noexcept;
    file_stream_handle(file_stream_handle&& other) noexcept;
    file_stream_handle(const file_stream_handle&) = delete;

    //-------------------------------------------------------------------------

    ~file_stream_handle();

    //-------------------------------------------------------------------------

    auto operator=(file_stream_handle&& other) noexcept -> file_stream_handle&;
    auto operator=(const file_stream_handle&) -> file_stream_handle& = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    explicit operator bool() const noexcept;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    auto reset() noexcept -> void;

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------
  public:

    auto get() const noexcept -> file_stream*;

    auto operator->() const noexcept -> file_stream*;
    auto operator*() const noexcept -> file_stream&;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  public:

    file_stream* m_stream;
    file_stream_finalizer* m_finalizer;
  };

} // namespace alloy::platform

//=============================================================================
// inline definition : class : file_stream_handle
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

inline
alloy::platform::file_stream_handle::file_stream_handle() noexcept
  : m_stream{nullptr},
    m_finalizer{nullptr}
{

}

inline
alloy::platform::file_stream_handle::file_stream_handle(
  core::not_null<file_stream*> stream,
  core::not_null<file_stream_finalizer*> finalizer
) noexcept
  : m_stream{stream.get()},
    m_finalizer{finalizer.get()}
{

}

inline
alloy::platform::file_stream_handle::file_stream_handle(file_stream_handle&& other)
  noexcept
  : m_stream{other.m_stream},
    m_finalizer{other.m_finalizer}
{
  other.m_stream = nullptr;
}

//-----------------------------------------------------------------------------

inline
alloy::platform::file_stream_handle::~file_stream_handle()
{
  reset();
}

//-----------------------------------------------------------------------------

inline
auto alloy::platform::file_stream_handle::operator=(file_stream_handle&& other)
  noexcept -> file_stream_handle&
{
  reset();

  m_stream = other.m_stream;
  m_finalizer = other.m_finalizer;
  other.m_stream = nullptr;

  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline
alloy::platform::file_stream_handle::operator bool() const noexcept
{
  return m_stream != nullptr;
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

inline
auto alloy::platform::file_stream_handle::reset() noexcept -> void
{
  if (m_stream != nullptr) {
    m_finalizer->finalize(m_stream);
    m_stream = nullptr;
  }
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

inline
auto alloy::platform::file_stream_handle::get() const noexcept -> file_stream*
{
  return m_stream;
}

inline
auto alloy::platform::file_stream_handle::operator->() const noexcept -> file_stream*
{
  return m_stream;
}

inline
auto alloy::platform::file_stream_handle::operator*() const noexcept -> file_stream&
{
  return (*m_stream);
}

#endif /* ALLOY_PLATFORM_FILESYSTEM_FILE_STREAM_HANDLE_HPP */
