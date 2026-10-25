// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/shader/resolved_parameters.hpp"

#include "alloy/render/shader/program_layout.hpp"
#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/shader/shader_data_type.hpp"
#include "alloy/render/shader/shader_parameter.hpp"

#include "alloy/render/error.hpp"
#include "alloy/render/texture/texture_dimension.hpp"

#include "alloy/core/containers/vector.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/matrix/matrix2.hpp"
#include "alloy/core/math/matrix/matrix3.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector4.hpp"
#include "alloy/core/model/color.hpp"
#include "alloy/core/string/string.hpp"
#include "alloy/core/string/string_view.hpp"
#include "alloy/core/utilities/optional.hpp"
#include "alloy/core/utilities/result.hpp"

#include <algorithm> // std::find_if, std::sort
#include <cstdint>   // std::int32_t, std::uint32_t
#include <span>      // std::span
#include <utility>   // std::move
#include <variant>   // std::visit

namespace alloy::render {
namespace {

auto fail_with(render_errc code, core::string_view name, const char* what)
  -> render_error
{
  auto details = core::string{"parameter '"};
  details.append(name);
  details.append("' ");
  details.append(what);
  return render_error::make(code, std::move(details));
}

/// \brief Gets the size of the uniform buffer resource a member belongs to
struct buffer_size_visitor
{
  auto operator()(const uniform_buffer_layout& buffer)
    const noexcept -> std::uint32_t;

  auto operator()(const sampled_texture_resource& texture)
    const noexcept -> std::uint32_t;
};

auto buffer_size_visitor::operator()(const uniform_buffer_layout& buffer)
  const noexcept -> std::uint32_t
{
  return buffer.byte_size;
}

auto buffer_size_visitor::operator()(const sampled_texture_resource&)
  const noexcept -> std::uint32_t
{
  // Members only ever belong to uniform buffers
  core::compiler::unreachable();
}

/// \brief Gets the dimension of a sampled texture resource, or null for any
///        other kind of resource
struct texture_dimension_visitor
{
  auto operator()(const uniform_buffer_layout& buffer)
    const noexcept -> core::optional<texture_dimension>;

  auto operator()(const sampled_texture_resource& texture)
    const noexcept -> core::optional<texture_dimension>;
};

auto texture_dimension_visitor::operator()(const uniform_buffer_layout&)
  const noexcept -> core::optional<texture_dimension>
{
  return core::null;
}

auto texture_dimension_visitor::operator()(
  const sampled_texture_resource& texture
) const noexcept -> core::optional<texture_dimension>
{
  return texture.dimension;
}

/// \brief Parameters that have been resolved, sorted by resource
struct bound_parameters
{
  core::vector<uniform_block> uniform_blocks;
  core::vector<texture_slot> textures;
};

/// \brief Accumulates the parameters bound to the resources of one scope
class parameter_builder
{
public:

  parameter_builder(const program_layout* layout, resource_scope scope) noexcept;

  auto bind_uniform(
    core::string_view name,
    shader_data_type type,
    uniform_data data
  ) -> core::result<void, render_error>;

  auto bind_texture(core::string_view name, const texture_binding& binding)
    -> core::result<void, render_error>;

  auto finish() && -> bound_parameters;

private:

  const program_layout* m_layout;
  resource_scope m_scope;
  bound_parameters m_parameters;

  auto in_scope(std::uint32_t resource_index) const noexcept -> bool;
  auto block_for(std::uint32_t resource_index) -> uniform_block&;
};

parameter_builder::parameter_builder(
  const program_layout* layout,
  resource_scope scope
) noexcept
  : m_layout{layout},
    m_scope{scope},
    m_parameters{}
{

}

auto parameter_builder::bind_uniform(
  core::string_view name,
  shader_data_type type,
  uniform_data data
) -> core::result<void, render_error>
{
  const auto* member = m_layout->find_member(name);
  if (member != nullptr && !in_scope(member->resource_index)) {
    member = nullptr;
  }
  if (member == nullptr) {
    const auto resource = m_layout->find_resource(name);
    const auto code = (resource.has_value() && in_scope(*resource))
      ? render_errc::parameter_type_mismatch
      : render_errc::parameter_not_found;
    return core::fail(
      fail_with(code, name, "names no uniform member of the program")
    );
  }
  if (member->type != type) {
    return core::fail(
      fail_with(
        render_errc::parameter_type_mismatch,
        name,
        "does not match the type of its uniform member"
      )
    );
  }

  auto& values = block_for(member->resource_index).values;
  const auto duplicate = std::find_if(
    values.begin(),
    values.end(),
    [&](const uniform_value& value) { return value.offset == member->offset; }
  );
  if (duplicate != values.end()) {
    return core::fail(
      fail_with(
        render_errc::duplicate_parameter,
        name,
        "binds a uniform member that is already bound"
      )
    );
  }

  values.push_back(uniform_value{type, member->offset, std::move(data)});
  return {};
}

auto parameter_builder::bind_texture(
  core::string_view name,
  const texture_binding& binding
) -> core::result<void, render_error>
{
  auto index = m_layout->find_resource(name);
  if (index.has_value() && !in_scope(*index)) {
    index = core::null;
  }
  if (!index.has_value()) {
    const auto* const member = m_layout->find_member(name);
    const auto code = (member != nullptr && in_scope(member->resource_index))
      ? render_errc::parameter_type_mismatch
      : render_errc::parameter_not_found;
    return core::fail(
      fail_with(code, name, "names no texture of the program")
    );
  }

  const auto& resource = m_layout->resources()[*index];
  const auto dimension = std::visit(texture_dimension_visitor{}, resource.kind);
  if (!dimension.has_value()) {
    return core::fail(
      fail_with(
        render_errc::parameter_type_mismatch,
        name,
        "names a uniform buffer, but was given a texture"
      )
    );
  }

  auto& textures = m_parameters.textures;
  const auto duplicate = std::find_if(
    textures.begin(),
    textures.end(),
    [&](const texture_slot& slot) { return slot.resource_index == *index; }
  );
  if (duplicate != textures.end()) {
    return core::fail(
      fail_with(
        render_errc::duplicate_parameter,
        name,
        "binds a texture that is already bound"
      )
    );
  }

  textures.push_back(
    texture_slot{
      .resource_index = *index,
      .binding = resource.binding,
      .dimension = *dimension,
      .texture = binding.texture,
      .sampler = binding.sampler,
    }
  );
  return {};
}

auto parameter_builder::finish() && -> bound_parameters
{
  const auto by_resource = [](const auto& lhs, const auto& rhs) {
    return lhs.resource_index < rhs.resource_index;
  };
  std::sort(
    m_parameters.uniform_blocks.begin(),
    m_parameters.uniform_blocks.end(),
    by_resource
  );
  std::sort(m_parameters.textures.begin(), m_parameters.textures.end(), by_resource);

  return std::move(m_parameters);
}

auto parameter_builder::in_scope(std::uint32_t resource_index)
  const noexcept -> bool
{
  return m_layout->resources()[resource_index].scope == m_scope;
}

auto parameter_builder::block_for(std::uint32_t resource_index)
  -> uniform_block&
{
  auto& blocks = m_parameters.uniform_blocks;
  const auto it = std::find_if(
    blocks.begin(),
    blocks.end(),
    [&](const uniform_block& block) {
      return block.resource_index == resource_index;
    }
  );
  if (it != blocks.end()) {
    return *it;
  }

  const auto& resource = m_layout->resources()[resource_index];
  blocks.push_back(
    uniform_block{
      .resource_index = resource_index,
      .binding = resource.binding,
      .byte_size = std::visit(buffer_size_visitor{}, resource.kind),
      .values = {},
    }
  );
  return blocks.back();
}

/// \brief Resolves a single parameter value into the parameters being built
struct parameter_visitor
{
  core::string_view name;
  parameter_builder* builder;

  auto operator()(bool value) const -> core::result<void, render_error>;
  auto operator()(std::int32_t value) const -> core::result<void, render_error>;
  auto operator()(std::uint32_t value) const -> core::result<void, render_error>;
  auto operator()(float value) const -> core::result<void, render_error>;
  auto operator()(const core::vector2& value) const -> core::result<void, render_error>;
  auto operator()(const core::vector3& value) const -> core::result<void, render_error>;
  auto operator()(const core::vector4& value) const -> core::result<void, render_error>;
  auto operator()(const core::color& value) const -> core::result<void, render_error>;
  auto operator()(const core::matrix2& value) const -> core::result<void, render_error>;
  auto operator()(const core::matrix3& value) const -> core::result<void, render_error>;
  auto operator()(const core::matrix4& value) const -> core::result<void, render_error>;
  auto operator()(const texture_binding& value) const -> core::result<void, render_error>;
};

auto parameter_visitor::operator()(bool value)
  const -> core::result<void, render_error>
{
  return builder->bind_uniform(name, shader_data_type::boolean, value);
}

auto parameter_visitor::operator()(std::int32_t value)
  const -> core::result<void, render_error>
{
  return builder->bind_uniform(name, shader_data_type::int1, value);
}

auto parameter_visitor::operator()(std::uint32_t value)
  const -> core::result<void, render_error>
{
  return builder->bind_uniform(name, shader_data_type::uint1, value);
}

auto parameter_visitor::operator()(float value)
  const -> core::result<void, render_error>
{
  return builder->bind_uniform(name, shader_data_type::float1, value);
}

auto parameter_visitor::operator()(const core::vector2& value)
  const -> core::result<void, render_error>
{
  return builder->bind_uniform(name, shader_data_type::float2, value);
}

auto parameter_visitor::operator()(const core::vector3& value)
  const -> core::result<void, render_error>
{
  return builder->bind_uniform(name, shader_data_type::float3, value);
}

auto parameter_visitor::operator()(const core::vector4& value)
  const -> core::result<void, render_error>
{
  return builder->bind_uniform(name, shader_data_type::float4, value);
}

auto parameter_visitor::operator()(const core::color& value)
  const -> core::result<void, render_error>
{
  return builder->bind_uniform(name, shader_data_type::float4, value);
}

auto parameter_visitor::operator()(const core::matrix2& value)
  const -> core::result<void, render_error>
{
  return builder->bind_uniform(name, shader_data_type::float2x2, value);
}

auto parameter_visitor::operator()(const core::matrix3& value)
  const -> core::result<void, render_error>
{
  return builder->bind_uniform(name, shader_data_type::float3x3, value);
}

auto parameter_visitor::operator()(const core::matrix4& value)
  const -> core::result<void, render_error>
{
  return builder->bind_uniform(name, shader_data_type::float4x4, value);
}

auto parameter_visitor::operator()(const texture_binding& value)
  const -> core::result<void, render_error>
{
  return builder->bind_texture(name, value);
}

} // namespace <anonymous>
} // namespace alloy::render

//==============================================================================
// class : resolved_parameters
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::resolved_parameters::from_parameters(
  const program_layout& layout,
  resource_scope scope,
  std::span<const shader_parameter> parameters
) -> core::result<resolved_parameters, render_error>
{
  auto builder = parameter_builder{&layout, scope};

  for (const auto& parameter : parameters) {
    auto result = std::visit(
      parameter_visitor{parameter.name, &builder},
      parameter.value
    );
    if (!result) {
      return core::fail(std::move(result).error());
    }
  }

  auto resolved = std::move(builder).finish();
  return resolved_parameters{
    std::move(resolved.uniform_blocks),
    std::move(resolved.textures)
  };
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::render::resolved_parameters::uniform_blocks()
  const noexcept -> std::span<const uniform_block>
{
  return m_uniform_blocks;
}

auto alloy::render::resolved_parameters::textures()
  const noexcept -> std::span<const texture_slot>
{
  return m_textures;
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::resolved_parameters::resolved_parameters(
  core::vector<uniform_block> uniform_blocks,
  core::vector<texture_slot> textures
) noexcept
  : m_uniform_blocks{std::move(uniform_blocks)},
    m_textures{std::move(textures)}
{

}
