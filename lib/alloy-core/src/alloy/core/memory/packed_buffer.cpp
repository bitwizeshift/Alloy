#include "alloy/core/memory/packed_buffer.hpp"

#include <utility>   // std::move

//=============================================================================
// class : packed_buffer
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

alloy::core::packed_buffer::packed_buffer(allocator alloc)
  noexcept
  : m_buffer{stl_allocator_adapter<std::byte>{alloc}}
{

}

alloy::core::packed_buffer::packed_buffer(vector<std::byte> buffer)
  noexcept
  : m_buffer{std::move(buffer)}
{

}

//-----------------------------------------------------------------------------

auto alloy::core::packed_buffer::copy()
  const -> packed_buffer
{
  return packed_buffer{m_buffer.copy()};
}

auto alloy::core::packed_buffer::copy_with(allocator alloc)
  const -> packed_buffer
{
  return packed_buffer{m_buffer.copy_with(stl_allocator_adapter<std::byte>{alloc})};
}

//-----------------------------------------------------------------------------
// Capacity
//-----------------------------------------------------------------------------

auto alloy::core::packed_buffer::reserve(bytes new_size)
  -> void
{
  m_buffer.reserve(static_cast<std::size_t>(new_size.count()));
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

auto alloy::core::packed_buffer::push_byte(std::byte b)
  -> void
{
  m_buffer.push_back(b);
}

auto alloy::core::packed_buffer::push_bytes(span<const std::byte> bytes)
  -> void
{
  m_buffer.insert(m_buffer.end(), bytes.begin(), bytes.end());
}

//=============================================================================
// class : packed_buffer_writer
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

alloy::core::packed_buffer_writer::packed_buffer_writer(not_null<packed_buffer*> buffer)
  noexcept
  : m_buffer{buffer}
{

}

//=============================================================================
// class : packed_buffer_reader
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

alloy::core::packed_buffer_reader::packed_buffer_reader(not_null<const packed_buffer*> buffer)
  noexcept
  : m_buffer{buffer},
    m_read_index{0u}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

auto alloy::core::packed_buffer_reader::seek(std::size_t index)
  noexcept -> void
{
  const auto max_size = m_buffer->size().count();

  m_read_index = narrow_cast<std::size_t>(index >= max_size ? max_size : index);
}

//-----------------------------------------------------------------------------
// Reading
//-----------------------------------------------------------------------------

auto alloy::core::packed_buffer_reader::read_bytes(span<std::byte> destination)
  noexcept -> span<std::byte>
{
  if (m_read_index >= m_buffer->size().count()) {
    return span<std::byte>{destination.data(), std::size_t{0}};
  }

  const auto requested = destination.size();
  const auto available = m_buffer->size().count() - m_read_index;

  const auto to_read = narrow_cast<std::size_t>(
    requested < available ? requested : available
  );
  const auto source = m_buffer->data() + m_read_index;

  std::memcpy(destination.data(), source, to_read);

  m_read_index += to_read;

  return span<std::byte>{destination.data(), to_read};
}

//=============================================================================
// non-member functions : class : packed_buffer_reader
//=============================================================================

namespace {
  class packed_buffer_reader_category : public std::error_category
  {
  public:

    auto message(int n) const -> std::string override
    {
      using error = alloy::core::packed_buffer_reader::error;

      switch (n) {
        case static_cast<int>(error::out_of_bytes): {
          return "Not enough bytes";
        }
        default: {
          return "unknown";
        }
      }
    }

    auto name() const noexcept -> const char* override
    {
      return "packed_buffer_reader";
    }
  };
} // namespace <anonymous>

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

auto alloy::core::make_error_code(packed_buffer_reader::error e)
  noexcept -> std::error_code
{
  static const auto s_category = ::packed_buffer_reader_category{};

  return std::error_code{
    static_cast<int>(e),
    s_category
  };
}
