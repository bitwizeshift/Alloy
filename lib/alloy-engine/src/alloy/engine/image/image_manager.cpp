/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

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

#include "alloy/engine/image/image_manager.hpp"

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

auto alloy::engine::image_manager::load(const std::string& file_type, io::file& file)
  -> core::result<core::image,std::error_code>
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
