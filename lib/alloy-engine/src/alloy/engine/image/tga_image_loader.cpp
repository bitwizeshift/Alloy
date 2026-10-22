// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/engine/image/tga_image_loader.hpp"

#include "alloy/core/containers/vector.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/model/image.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/platform/filesystem/file.hpp"

#include <tga.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <system_error>
#include <utility>

namespace alloy::engine {
namespace {

  class tga_file_interface : public tga::FileInterface
  {
  public:
    tga_file_interface(core::not_null<platform::file*> file)
      : m_file{file},
        m_ok{true}
    {
    }

    auto ok() const -> bool override { return m_ok; }
    auto tell() -> std::size_t override { return 0u; /* This is unused */ }
    auto seek(std::size_t) -> void override { /* this is unused */ }
    auto read8() -> uint8_t override
    {
      auto result = m_file->read_object<std::uint8_t>();
      m_ok = result.has_value();
      return result.value_or(255u);
    }
    auto write8(uint8_t) -> void override
    {
      // not used
    }

  private:
    core::not_null<platform::file*> m_file;
    bool m_ok;
  };

  class tga_error_category : public std::error_category
  {
  public:
    auto name() const noexcept -> const char* override
    {
      return "tga";
    }
    auto message(int ec) const -> std::string override
    {
      switch (static_cast<tga_image_loader::error>(ec)) {
        case tga_image_loader::error::none: {
          return "no error";
        }
        case tga_image_loader::error::bad_image_data: {
          return "unable to parse image data; either invalid format,"
                 " or too little data was available";
        }
        case tga_image_loader::error::invalid_tga_header: {
          return "unable to parse TGA header; TGA header is invalid";
        }
      }
      return "invalid TGA error code " + std::to_string(ec);
    }
  };

} // namespace <anonymous>
} // namespace alloy::engine

alloy::engine::tga_image_loader::tga_image_loader(core::allocator alloc)
  noexcept
  : m_allocator{alloc}
{

}

auto alloy::engine::tga_image_loader::load(platform::file& file)
  -> core::result<core::image,std::error_code>
{
  auto interface = tga_file_interface{&file};

  auto decoder = tga::Decoder{&interface};
  auto header = tga::Header{};

  if (!decoder.readHeader(header)) {
    return core::fail(error::invalid_tga_header);
  }

  auto image = tga::Image{};
  image.bytesPerPixel = header.bytesPerPixel();
  image.rowstride     = header.width * header.bytesPerPixel();

  auto buffer = core::vector<std::byte>{
    core::stl_allocator_adapter<std::byte>{m_allocator}
  };
  const auto buffer_size = image.rowstride * header.height;
  buffer.resize(static_cast<decltype(buffer)::size_type>(buffer_size));
  image.pixels = reinterpret_cast<std::uint8_t*>(&buffer[0]);

  if (!decoder.readImage(header, image, nullptr)) {
    return core::fail(error::bad_image_data);
  }

  decoder.postProcessImage(header, image);

  static_cast<void>(file.close());

  return alloy::core::image::make_image_from_buffer(
    std::move(buffer),
    alloy::core::image::dimensions{header.width, header.height},
    alloy::core::image::pixel_format::rgba
  );
}

auto alloy::engine::make_error_code(tga_image_loader::error error)
  noexcept -> std::error_code
{
  static const auto s_category = tga_error_category{};

  return std::error_code{static_cast<int>(error), s_category};
}
