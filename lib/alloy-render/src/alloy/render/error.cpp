// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/error.hpp"

#include "alloy/core/string/string.hpp"
#include "alloy/core/string/zstring_view.hpp"

#include <memory>       // std::shared_ptr, std::make_shared
#include <string>       // std::string, std::to_string
#include <system_error> // std::error_code, std::error_category
#include <utility>      // std::move

namespace alloy::render {
namespace {

class render_error_category final : public std::error_category
{
public:

  auto name() const noexcept -> const char* override
  {
    return "render";
  }

  auto message(int error) const -> std::string override
  {
    switch (static_cast<render_errc>(error)) {
      case render_errc::unsupported_bytecode: {
        return "unsupported shader bytecode";
      }
      case render_errc::shader_compile_failed: {
        return "shader compilation failed";
      }
      case render_errc::program_link_failed: {
        return "program link failed";
      }
      case render_errc::missing_stage: {
        return "program has no shader stages";
      }
      case render_errc::duplicate_stage: {
        return "program has a duplicate shader stage";
      }
      case render_errc::duplicate_resource: {
        return "program has a duplicate resource";
      }
      case render_errc::invalid_uniform_layout: {
        return "invalid uniform buffer layout";
      }
      case render_errc::unknown_resource: {
        return "unknown resource";
      }
      case render_errc::invalid_texture_spec: {
        return "invalid texture specification";
      }
      case render_errc::unsupported_texture_format: {
        return "unsupported texture format";
      }
      case render_errc::stale_handle: {
        return "stale resource handle";
      }
      case render_errc::parameter_not_found: {
        return "material parameter not found";
      }
      case render_errc::parameter_type_mismatch: {
        return "material parameter type mismatch";
      }
      case render_errc::dimension_mismatch: {
        return "texture dimension mismatch";
      }
    }
    return "unknown render error " + std::to_string(error);
  }
};

} // namespace <anonymous>
} // namespace alloy::render

auto alloy::render::render_category() noexcept -> const std::error_category&
{
  static const auto s_category = render_error_category{};

  return s_category;
}

auto alloy::render::make_error_code(render_errc e) noexcept -> std::error_code
{
  return std::error_code{static_cast<int>(e), render_category()};
}

//==============================================================================
// class : render_error
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::render_error::make(std::error_code code)
  noexcept -> render_error
{
  return render_error{code, nullptr};
}

auto alloy::render::render_error::make(
  std::error_code code,
  core::string details
) -> render_error
{
  // `std::allocate_shared` erroneously rejects `explicitly_copyable` types,
  // so this falls back to `std::make_shared`.
  auto ptr = std::make_shared<const core::string>(std::move(details));

  return render_error{code, std::move(ptr)};
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::render::render_error::code() const noexcept -> std::error_code
{
  return m_code;
}

auto alloy::render::render_error::details() const noexcept -> core::zstring_view
{
  if (m_details != nullptr) {
    return *m_details;
  }
  return "";
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::render_error::render_error(
  std::error_code code,
  std::shared_ptr<const core::string> details
) noexcept
  : m_code{code},
    m_details{std::move(details)}
{

}
