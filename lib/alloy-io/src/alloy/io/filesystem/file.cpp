#include "alloy/io/filesystem/file.hpp"

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
      using error_code = alloy::io::file::error_code;

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
  core::compiler::unused(close());
}

//-----------------------------------------------------------------------------

alloy::io::file& alloy::io::file::operator=(file&& other)
  noexcept
{
  core::compiler::unused(close());

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


alloy::core::result<alloy::io::file::size_type,std::error_code> alloy::io::file::bytes()
  const noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
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

alloy::core::result<void,std::error_code> alloy::io::file::close()
  noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  m_handle->close();
  m_handle.reset();

  return {};
}


alloy::core::result<void,std::error_code> alloy::io::file::reset()
  noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  return m_handle->reset();
}


alloy::core::result<void,std::error_code> alloy::io::file::flush()
  noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  return m_handle->flush();
}


alloy::core::result<void,std::error_code> alloy::io::file::skip(offset_type offset)
  noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  return m_handle->skip(offset);
}


alloy::core::result<alloy::io::mutable_buffer,std::error_code>
  alloy::io::file::read(mutable_buffer buffer)
  noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
  }

  return m_handle->read(buffer);
}


alloy::core::result<alloy::io::const_buffer,std::error_code>
  alloy::io::file::write(const_buffer buffer)
  noexcept
{
  if (ALLOY_UNLIKELY(m_handle.get() == nullptr)) {
    return core::fail(error_code::closed);
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
