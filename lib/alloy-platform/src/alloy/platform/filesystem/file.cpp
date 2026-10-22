// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/platform/filesystem/file.hpp"

#include <string>
#include <utility>

// Disable this warning for the error_category message function.
// It is legal for enums to take on values outside of the switch case, and
// in this case the input can be any numeric value (due to the way that
// std::error_category works)
ALLOY_COMPILER_CLANG_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_CLANG_DIAGNOSTIC_IGNORE(-Wcovered-switch-default)

namespace {

  //===========================================================================
  // class : error_category
  //===========================================================================

  class error_category final
    : public std::error_category
  {
  public:
    const char* name() const noexcept override
    {
      return "file error";
    }

    std::string message(int ec) const override
    {
      using error_code = alloy::platform::file::error_code;

      const auto code = static_cast<error_code>(ec);

      switch (code) {
        case error_code::closed:
          return "file is closed, no result available";
        case error_code::eof:
          return "end-of-file reached";
        case error_code::not_readable:
          return "file is not able to be read";
        case error_code::not_writeable:
          return "file is not able to be written to";
        case error_code::system_error:
          return "An error occurred while operating on the file";
        default:
          break;
      }
      return "unknown error code";
    }
  };

} // namespace <anonymous>

ALLOY_COMPILER_CLANG_DIAGNOSTIC_POP()

//=============================================================================
// definitions : class : file
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

alloy::platform::file::file()
  noexcept
  : m_handle{}
{

}


alloy::platform::file::file(
  core::not_null<file_stream*> stream,
  core::not_null<file_stream_finalizer*> finalizer
) noexcept
  : m_handle{stream, finalizer}
{

}


alloy::platform::file::file(file_stream_handle handle) noexcept
  : m_handle{std::move(handle)}
{

}

//-----------------------------------------------------------------------------

alloy::platform::file::~file()
{
  core::compiler::unused(close());
}

//-----------------------------------------------------------------------------

auto alloy::platform::file::operator=(file&& other) noexcept -> file&
{
  core::compiler::unused(close());

  m_handle = std::move(other.m_handle);

  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

auto alloy::platform::file::is_open() const noexcept -> bool
{
  return m_handle.get() != nullptr;
}


auto alloy::platform::file::bytes()
  const noexcept -> core::result<size_type,std::error_code>
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  return m_handle->bytes();
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

auto alloy::platform::file::release() noexcept -> file_stream_handle
{
  return std::move(m_handle);
}

//-----------------------------------------------------------------------------
// File API
//-----------------------------------------------------------------------------

auto alloy::platform::file::close() noexcept -> core::result<void,std::error_code>
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  m_handle->close();
  m_handle.reset();

  return {};
}


auto alloy::platform::file::reset() noexcept -> core::result<void,std::error_code>
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  return m_handle->reset();
}


auto alloy::platform::file::flush() noexcept -> core::result<void,std::error_code>
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  return m_handle->flush();
}


auto alloy::platform::file::skip(offset_type offset)
  noexcept -> core::result<void,std::error_code>
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  return m_handle->skip(offset);
}


auto alloy::platform::file::read(mutable_buffer buffer)
  noexcept -> core::result<mutable_buffer,std::error_code>
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  return m_handle->read(buffer);
}


auto alloy::platform::file::write(const_buffer buffer)
  noexcept -> core::result<const_buffer,std::error_code>
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  return m_handle->write(buffer);
}

//-----------------------------------------------------------------------------
// Virtual Hooks : Reading
//-----------------------------------------------------------------------------

auto alloy::platform::file::do_read(std::span<std::byte> buffer)
  noexcept -> core::result<std::size_t,std::error_code>
{
  auto result = read(mutable_buffer{buffer.data(), buffer.size()});

  // Files signal the end with an error, whereas readers signal it by reading
  // no bytes
  if (result.has_error()) {
    if (result.error() == error_code::eof) {
      return 0u;
    }
    return core::fail(result.error());
  }
  return result->size();
}

//-----------------------------------------------------------------------------
// Virtual Hooks : Writing
//-----------------------------------------------------------------------------

auto alloy::platform::file::do_write(std::span<const std::byte> buffer)
  noexcept -> core::result<std::size_t,std::error_code>
{
  auto result = write(const_buffer{buffer.data(), buffer.size()});

  // Files signal that no more bytes fit with an error, whereas writers signal
  // it by writing no bytes
  if (result.has_error()) {
    if (result.error() == error_code::eof) {
      return 0u;
    }
    return core::fail(result.error());
  }
  return result->size();
}

//=============================================================================
// inline definitions : non-member functions : enum : file::error_code
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

auto alloy::platform::make_error_code(file::error_code ec) noexcept -> std::error_code
{
  static auto s_category = ::error_category{};

  return std::error_code{static_cast<int>(ec), s_category};
}
