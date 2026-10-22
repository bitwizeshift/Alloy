// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/engine/image/image_manager.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/model/image.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/platform/filesystem/file.hpp"

#include <string>
#include <system_error>

namespace alloy::engine {
namespace {

  class image_manager_error_category : public std::error_category
  {
  public:
    auto name() const noexcept -> const char* override
    {
      return "image manager";
    }
    auto message(int ec) const -> std::string override
    {
      switch (static_cast<image_manager::error>(ec)) {
        case image_manager::error::none: {
          return "no error";
        }
        case image_manager::error::file_type_not_supported: {
          return "no image loader has been associated to the requested type";
        }
      }
      return "invalid TGA error code " + std::to_string(ec);
    }
  };

} // namespace <anonymous>
} // namespace alloy::engine


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------

alloy::engine::image_manager::image_manager()
  : m_loaders{}
{

}

//------------------------------------------------------------------------------
// Image Loading
//------------------------------------------------------------------------------

auto alloy::engine::image_manager::load(
  const std::string& file_type,
  platform::file& file
) -> core::result<core::image,std::error_code>
{
  auto it = m_loaders.find(file_type);
  if (it == m_loaders.end()) {
    // Load failed!
    ALLOY_ASSERT(false); // TODO: change to error_code
  }
  return (*it->second).load(file);
}

auto alloy::engine::make_error_code(image_manager::error error)
  noexcept -> std::error_code
{
  static const auto s_category = image_manager_error_category{};

  return std::error_code{static_cast<int>(error), s_category};
}
