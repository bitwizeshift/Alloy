// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/model/image.hpp"

#include "alloy/core/model/color.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/precision/real.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstddef>
#include <cstdint>      // std::uint8_t
#include <string>       // std::string
#include <system_error>
#include <utility>      // std::move

namespace {

// Disable this warning for the error_category message function.
// It is legal for enums to take on values outside of the switch case, and
// in this case the input can be any numeric value (due to the way that
// std::error_category works)
ALLOY_COMPILER_CLANG_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_CLANG_DIAGNOSTIC_IGNORE(-Wcovered-switch-default)

  //===========================================================================
  // class : error_category
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Error category for the image::error_code enum
  /////////////////////////////////////////////////////////////////////////////
  class error_category final
    : public std::error_category
  {
  public:
    std::string message(int ec) const override
    {
      using error_code = ::alloy::core::image::error_code;
      auto code = static_cast<error_code>(ec);

      switch (code) {
        case error_code::invalid_pixel_format: {
          return "Invalid pixel format value specified";
        }
        case error_code::bad_width: {
          return "Invalid width specified. Width must be > 0";
        }
        case error_code::bad_height: {
          return "Invalid height specified. Height must be > 0";
        }
        case error_code::incorrect_dimensions: {
          return "Dimensions don't match the supplied byte buffer";
        }
        default: {
          return "Unknown error code";
        }
      }
    }

    const char* name() const noexcept override
    {
      return "image error";
    }
  };

ALLOY_COMPILER_CLANG_DIAGNOSTIC_POP()

  //===========================================================================
  // enum : rgba
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief One of the primary colors
  /////////////////////////////////////////////////////////////////////////////
  enum class rgba {
    r,g,b,a
  };

  //===========================================================================
  // non-member functions
  //===========================================================================

  /// \brief Gets the size of a pixel given the format
  ///
  /// \param format the pixel format of the image
  /// \return the size of a given pixel color
  inline auto pixel_format_size(alloy::core::image::pixel_format format)
    noexcept -> std::size_t
  {
    using pixel_format = alloy::core::image::pixel_format;

    switch (format) {
      case pixel_format::bgr:
        [[fallthrough]];
      case pixel_format::rgb: {
        return 3u;
      }
      case pixel_format::bgra:
        [[fallthrough]];
      case pixel_format::rgba: {
        return 4u;
      }
    }
    ALLOY_UNREACHABLE();
  }

  /// \brief Gets whether the input format is valid
  ///
  /// \param format the fomat
  /// \return `true` if the format is correct
  inline auto is_valid_format(alloy::core::image::pixel_format format)
    noexcept -> bool
  {
    using pixel_format = alloy::core::image::pixel_format;

    switch (format) {
      case pixel_format::bgr:
      case pixel_format::bgra:
      case pixel_format::rgb: [[fallthrough]];
      case pixel_format::rgba: {
        return true;
      }
    }

    return false;
  }

  using rgba_index_function = rgba(*)(std::size_t pixel);

  inline auto bgr_index(std::size_t pixel) -> rgba
  {
    ALLOY_ASSERT(pixel < 3u);

    switch (pixel) {
      case 0: return rgba::b;
      case 1: return rgba::g;
      case 2: return rgba::r;
      default: break;
    }
    ALLOY_UNREACHABLE();
  }


  inline auto rgba_index(std::size_t pixel) -> rgba
  {
    ALLOY_ASSERT(pixel < 4u);

    switch (pixel) {
      case 0: return rgba::r;
      case 1: return rgba::g;
      case 2: return rgba::b;
      case 3: return rgba::a;
      default: break;
    }
    ALLOY_UNREACHABLE();
  }


  inline auto bgra_index(std::size_t pixel) -> rgba
  {
    ALLOY_ASSERT(pixel < 4u);

    switch (pixel) {
      case 0: return rgba::b;
      case 1: return rgba::g;
      case 2: return rgba::r;
      case 3: return rgba::a;
      default: break;
    }
    ALLOY_UNREACHABLE();
  }

  inline auto rgb_index(std::size_t pixel) -> rgba
  {
    ALLOY_ASSERT(pixel < 3u);

    switch (pixel) {
      case 0: return rgba::r;
      case 1: return rgba::g;
      case 2: return rgba::b;
      default: break;
    }
    ALLOY_UNREACHABLE();
  }

  /// \brief Gets a function to convert an integral index between 0..3 to
  ///        the corresponding pixel color
  ///
  /// \param format the pixel format
  /// \return A function pointer to a conversion function
  inline auto get_pixel_index_function(alloy::core::image::pixel_format format)
    -> rgba_index_function
  {
    using pixel_format = alloy::core::image::pixel_format;

    ALLOY_ASSERT(::is_valid_format(format));

    switch (format) {
      case pixel_format::bgr: {
        return &::bgr_index;
      }
      case pixel_format::rgb: {
        return &::rgb_index;
      }
      case pixel_format::bgra: {
        return &::bgra_index;
      }
      case pixel_format::rgba: {
        return &::rgba_index;
      }
    }
    ALLOY_UNREACHABLE();
  }

  template <typename T>
  auto to_byte(T v) -> std::byte
  {
    return static_cast<std::byte>(static_cast<std::uint8_t>(v));
  }
} // namespace <anonymous>

//=============================================================================
// class : image
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

auto alloy::core::image::make_image_from_buffer(
  vector<std::byte> buffer,
  dimensions d,
  pixel_format format
) -> result<image,std::error_code>
{
  if (ALLOY_UNLIKELY(!::is_valid_format(format))) {
    return fail(error_code::invalid_pixel_format);
  }
  const auto pixel_size = ::pixel_format_size(format);
  const auto expected_size = d.width * d.height * pixel_size;

  if (ALLOY_UNLIKELY(d.width == 0u)) {
    return fail(error_code::bad_width);
  }
  if (ALLOY_UNLIKELY(d.height == 0u)) {
    return fail(error_code::bad_height);
  }
  if (ALLOY_UNLIKELY(buffer.size() != expected_size)) {
    return fail(error_code::incorrect_dimensions);
  }

  return image{std::move(buffer), d, format};
}


auto alloy::core::image::make_blank_image(
  dimensions d,
  pixel_format format,
  color background
) -> result<image,std::error_code>
{
  return make_blank_image(d, format, background, allocator{});
}


auto alloy::core::image::make_blank_image(
  dimensions d,
  pixel_format format,
  color background,
  allocator alloc
) -> result<image,std::error_code>
{
  const auto pixel_size = pixel_format_size(format);
  auto adapter = stl_allocator_adapter<std::byte>{alloc};

  auto vec = vector<std::byte>{adapter};
  vec.resize(d.width * d.height * pixel_size);

  const auto convert = ::get_pixel_index_function(format);
  auto pixel = std::size_t{0u};

  for (auto& v : vec) {
    const auto color_segment = (*convert)(pixel);

    switch (color_segment) {
    case rgba::r:
      v = ::to_byte(background.r8());
      break;
    case rgba::g:
      v = ::to_byte(background.g8());
      break;
    case rgba::b:
      v = ::to_byte(background.b8());
      break;
    case rgba::a:
      v = ::to_byte(background.a8());
      break;
    }

    pixel = (pixel + 1u) % pixel_size;
  }

  return make_image_from_buffer(std::move(vec), d, format);
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

alloy::core::image::image() noexcept
  : m_bytes{},
    m_width{0u},
    m_height{0u},
    m_format{pixel_format::rgb}
{

}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

auto alloy::core::image::set_pixel(std::size_t x, std::size_t y, color c)
  noexcept -> void
{
  ALLOY_ASSERT(x < m_width);
  ALLOY_ASSERT(y < m_height);

  const auto pixel_size = ::pixel_format_size(m_format);
  const auto index = (y * m_width * pixel_size) + x * pixel_size;
  const auto convert = ::get_pixel_index_function(m_format);

  for (auto i = 0u; i < pixel_size; ++i) {
    const auto color_component = (*convert)(i);

    switch (color_component) {
    case rgba::r:
      m_bytes[index + i] = ::to_byte(c.r8());
      break;
    case rgba::g:
      m_bytes[index + i] = ::to_byte(c.g8());
      break;
    case rgba::b:
      m_bytes[index + i] = ::to_byte(c.b8());
      break;
    case rgba::a:
      m_bytes[index + i] = ::to_byte(c.a8());
      break;
    }
  }
}

//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

auto alloy::core::image::get_pixel(std::size_t x, std::size_t y)
  const noexcept -> color
{
  ALLOY_ASSERT(x < m_width);
  ALLOY_ASSERT(y < m_height);

  const auto pixel_size = ::pixel_format_size(m_format);
  const auto index = (y * m_width * pixel_size) + x * pixel_size;
  const auto convert = ::get_pixel_index_function(m_format);

  auto result = color{};

  for (auto i = 0u; i < pixel_size; ++i) {
    const auto color_component = (*convert)(i);

    switch (color_component) {
    case rgba::r:
      result.r() = static_cast<real>(m_bytes[index + i]) / real{255};
      break;
    case rgba::g:
      result.g() = static_cast<real>(m_bytes[index + i]) / real{255};
      break;
    case rgba::b:
      result.b() = static_cast<real>(m_bytes[index + i]) / real{255};
      break;
    case rgba::a:
      result.a() = static_cast<real>(m_bytes[index + i]) / real{255};
      break;
    }
  }

  return result;
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

auto alloy::core::image::copy() const -> image
{
  return image{
    m_bytes.copy(),
    dimensions{m_width, m_height},
    m_format
  };
}


auto alloy::core::image::copy_with(allocator alloc) const -> image
{
  const auto adapter = stl_allocator_adapter<std::byte>{alloc};

  auto vec = vector<std::byte>{adapter};
  vec.reserve(m_bytes.size());
  vec.assign(m_bytes.begin(), m_bytes.end());

  return image{
    std::move(vec),
    dimensions{m_width, m_height},
    m_format
  };
}

//-----------------------------------------------------------------------------
// Private : Constructor
//-----------------------------------------------------------------------------

alloy::core::image::image(
  vector<std::byte> bytes,
  dimensions d,
  pixel_format format
) noexcept
  : m_bytes{std::move(bytes)},
    m_width{d.width},
    m_height{d.height},
    m_format{format}
{

}

//=============================================================================
// non-member functions : enum : image::error_code
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

auto alloy::core::make_error_code(image::error_code ec)
  noexcept -> std::error_code
{
  static auto s_error_category = ::error_category{};

  return std::error_code{
    static_cast<int>(ec),
    s_error_category
  };
}
