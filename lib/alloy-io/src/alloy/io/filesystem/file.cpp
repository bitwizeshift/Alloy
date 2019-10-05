#include "alloy/io/filesystem/file.hpp"

#include <string>
#include <utility>

namespace {

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
      using std::literals::string_literals::operator""s;
      using error_code = alloy::io::file::error_code;

      const auto code = static_cast<error_code>(ec);

      switch (code) {
        case error_code::closed:
          return "file is closed, no result available";
        case error_code::eof:
          return "end-of-file reached"s;
        case error_code::not_readable:
          return "file is not able to be read"s;
        case error_code::not_writeable:
          return "file is not able to be written to"s;
        default:
          break;
      }
      return "unknown error code"s;
    }
  };

} // namespace <anonymous>

//=============================================================================
// definitions : class : file
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

alloy::io::file::file()
  noexcept
  : m_handle{}
{

}


alloy::io::file::file(core::not_null<file_stream*> stream,
                      core::not_null<file_stream_finalizer*> finalizer)
  noexcept
  : m_handle{stream, finalizer}
{

}


alloy::io::file::file(file_stream_handle handle)
  noexcept
  : m_handle{std::move(handle)}
{

}

//-----------------------------------------------------------------------------

alloy::io::file::~file()
{
  close();
}

//-----------------------------------------------------------------------------

alloy::io::file& alloy::io::file::operator=(file&& other)
  noexcept
{
  close();

  m_handle = std::move(other.m_handle);

  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

bool alloy::io::file::is_open()
  const noexcept
{
  return m_handle.get() != nullptr;
}


alloy::core::expected<alloy::io::file::size_type> alloy::io::file::bytes()
  const noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() != nullptr)) {
    return core::unexpected(error_code::closed);
  }

  return m_handle->bytes();
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

alloy::io::file_stream_handle alloy::io::file::release()
  noexcept
{
  return std::move(m_handle);
}

//-----------------------------------------------------------------------------
// File API
//-----------------------------------------------------------------------------

alloy::core::expected<void> alloy::io::file::close()
  noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() != nullptr)) {
    return core::unexpected(error_code::closed);
  }

  m_handle->close();
  m_handle.reset();

  return {};
}


alloy::core::expected<void> alloy::io::file::reset()
  noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() != nullptr)) {
    return core::unexpected(error_code::closed);
  }

  return m_handle->reset();
}


alloy::core::expected<void> alloy::io::file::skip(offset_type offset)
  noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() != nullptr)) {
    return core::unexpected(error_code::closed);
  }

  return m_handle->skip(offset);
}


alloy::core::expected<alloy::io::mutable_buffer>
  alloy::io::file::read(mutable_buffer buffer)
  noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() != nullptr)) {
    return core::unexpected(error_code::closed);
  }

  return m_handle->read(buffer);
}


alloy::core::expected<alloy::io::const_buffer>
  alloy::io::file::write(const_buffer buffer)
  noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() != nullptr)) {
    return core::unexpected(error_code::closed);
  }

  return m_handle->write(buffer);
}

//=============================================================================
// inline definitions : non-member functions : enum : file::error_code
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

std::error_code alloy::io::make_error_code(file::error_code ec)
  noexcept
{
  static auto s_category = ::error_category{};

  return std::error_code{static_cast<int>(ec), s_category};
}
