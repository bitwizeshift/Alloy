// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/shader/program_layout.hpp"

#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/shader/shader_data_type.hpp"
#include "alloy/render/shader/shader_stage.hpp"

#include "alloy/render/error.hpp"

#include "alloy/core/containers/hash_map.hpp"
#include "alloy/core/containers/hash_set.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/memory/data_quantity.hpp"
#include "alloy/core/string/string.hpp"
#include "alloy/core/string/string_view.hpp"
#include "alloy/core/utilities/optional.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstddef>    // std::size_t
#include <cstdint>    // std::uint32_t
#include <functional> // std::hash
#include <span>       // std::span
#include <utility>    // std::move
#include <variant>    // std::visit

namespace alloy::render {
namespace {

auto fail_with(render_errc code, core::string_view subject, const char* what)
  -> render_error
{
  auto details = core::string{"'"};
  details.append(subject);
  details.append("' ");
  details.append(what);
  return render_error::make(code, std::move(details));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief A uniform buffer member awaiting insertion into the layout's lookup
////////////////////////////////////////////////////////////////////////////////
struct pending_member
{
  core::string_view block;
  core::string_view name;
  member_layout layout;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Validates the kind of a single resource, converting it into the
///        kind that the layout retains
////////////////////////////////////////////////////////////////////////////////
struct resource_kind_visitor
{
  std::uint32_t resource_index;
  core::string_view resource_name;
  resource_scope scope;
  core::vector<pending_member>* members;

  auto operator()(const uniform_buffer_resource& resource)
    const -> core::result<resource_layout_kind, render_error>;

  auto operator()(const sampled_texture_resource& resource)
    const -> core::result<resource_layout_kind, render_error>;
};

auto resource_kind_visitor::operator()(
  const uniform_buffer_resource& resource
) const -> core::result<resource_layout_kind, render_error>
{
  const auto first = members->size();
  for (const auto& member : resource.members) {
    for (auto i = first; i < members->size(); ++i) {
      if ((*members)[i].name == member.name) {
        return core::fail(
          fail_with(
            render_errc::invalid_uniform_layout,
            member.name,
            "is declared more than once"
          )
        );
      }
    }

    members->push_back(
      pending_member{
        .block = resource_name,
        .name = member.name,
        .layout = member_layout{
          .resource_index = resource_index,
          .type = member.type,
          .offset = member.offset,
        },
      }
    );
  }

  return resource_layout_kind{uniform_buffer_layout{resource.byte_size}};
}

auto resource_kind_visitor::operator()(
  const sampled_texture_resource& resource
) const -> core::result<resource_layout_kind, render_error>
{
  if (scope != resource_scope::material) {
    return core::fail(
      fail_with(
        render_errc::invalid_resource_scope,
        resource_name,
        "is a texture, which only a material can supply"
      )
    );
  }
  return resource_layout_kind{resource};
}

auto validate_stages(std::span<const shader_stage_spec> stages)
  -> core::result<void, render_error>
{
  if (stages.empty()) {
    return core::fail(render_error::make(render_errc::missing_stage));
  }

  auto seen = shader_stages{};
  for (const auto& stage : stages) {
    if (seen.test(stage.stage)) {
      return core::fail(render_error::make(render_errc::duplicate_stage));
    }
    seen.set(stage.stage);
  }
  return {};
}

////////////////////////////////////////////////////////////////////////////////
/// \brief The validated inputs of a program
////////////////////////////////////////////////////////////////////////////////
struct validated_inputs
{
  core::vector<vertex_input_layout> vertex_inputs;
  core::vector<instance_input_layout> instance_inputs;
  core::bytes instance_stride;
};

auto validate_inputs(const program_spec& spec)
  -> core::result<validated_inputs, render_error>
{
  auto names = core::hash_set<core::string_view>{};
  const auto claim = [&](core::string_view name)
    -> core::result<void, render_error>
  {
    if (!names.insert(name).second) {
      return core::fail(
        fail_with(
          render_errc::duplicate_input,
          name,
          "is declared more than once"
        )
      );
    }
    return {};
  };

  auto result = validated_inputs{
    .vertex_inputs = {},
    .instance_inputs = {},
    .instance_stride = core::bytes{0u},
  };
  result.vertex_inputs.reserve(spec.vertex_inputs.size());
  result.instance_inputs.reserve(spec.instance_inputs.size());

  for (const auto& input : spec.vertex_inputs) {
    if (auto claimed = claim(input.name); !claimed) {
      return core::fail(std::move(claimed).error());
    }
    result.vertex_inputs.push_back(
      vertex_input_layout{
        .name = core::string{input.name},
        .location = input.location,
        .type = input.type,
      }
    );
  }
  for (const auto& input : spec.instance_inputs) {
    if (auto claimed = claim(input.name); !claimed) {
      return core::fail(std::move(claimed).error());
    }
    result.instance_inputs.push_back(
      instance_input_layout{
        .name = core::string{input.name},
        .location = input.location,
        .type = input.type,
        .offset = result.instance_stride,
      }
    );
    result.instance_stride += bytes_per_value(input.type);
  }
  return result;
}

} // namespace <anonymous>
} // namespace alloy::render

//==============================================================================
// class : program_layout
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::program_layout::from_spec(const program_spec& spec)
  -> core::result<program_layout, render_error>
{
  if (auto result = validate_stages(spec.stages); !result) {
    return core::fail(std::move(result).error());
  }
  auto inputs = validate_inputs(spec);
  if (!inputs) {
    return core::fail(std::move(inputs).error());
  }

  auto resources = core::vector<resource_layout>{};
  auto resource_indices = string_map<std::uint32_t>{};
  auto pending = core::vector<pending_member>{};
  resources.reserve(spec.resources.size());

  for (const auto& resource : spec.resources) {
    const auto index = static_cast<std::uint32_t>(resources.size());

    const auto [_, inserted] = resource_indices.emplace(
      core::string{resource.name},
      index
    );
    if (!inserted) {
      return core::fail(
        fail_with(
          render_errc::duplicate_resource,
          resource.name,
          "is declared more than once"
        )
      );
    }

    auto kind = std::visit(
      resource_kind_visitor{index, resource.name, resource.scope, &pending},
      resource.kind
    );
    if (!kind) {
      return core::fail(std::move(kind).error());
    }

    resources.push_back(
      resource_layout{
        .name = core::string{resource.name},
        .stages = resource.stages,
        .kind = std::move(kind).value(),
        .binding = resource.binding,
        .scope = resource.scope,
      }
    );
  }

  auto bare_name_counts = core::hash_map<core::string_view, std::uint32_t>{};
  for (const auto& member : pending) {
    ++bare_name_counts[member.name];
  }

  auto members = string_map<member_layout>{};
  for (const auto& member : pending) {
    auto qualified = core::string{member.block};
    qualified.push_back('.');
    qualified.append(member.name);
    members.emplace(std::move(qualified), member.layout);

    if (bare_name_counts[member.name] == 1u) {
      members.emplace(core::string{member.name}, member.layout);
    }
  }

  return program_layout{
    std::move(inputs->vertex_inputs),
    std::move(inputs->instance_inputs),
    inputs->instance_stride,
    std::move(resources),
    std::move(resource_indices),
    std::move(members)
  };
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::render::program_layout::vertex_inputs()
  const noexcept -> std::span<const vertex_input_layout>
{
  return m_vertex_inputs;
}

auto alloy::render::program_layout::instance_inputs()
  const noexcept -> std::span<const instance_input_layout>
{
  return m_instance_inputs;
}

auto alloy::render::program_layout::instance_stride()
  const noexcept -> core::bytes
{
  return m_instance_stride;
}

auto alloy::render::program_layout::resources()
  const noexcept -> std::span<const resource_layout>
{
  return m_resources;
}

auto alloy::render::program_layout::find_resource(core::string_view name)
  const noexcept -> core::optional<std::uint32_t>
{
  const auto it = m_resource_indices.find(name);
  if (it == m_resource_indices.end()) {
    return core::null;
  }
  return it->second;
}

auto alloy::render::program_layout::find_member(core::string_view name)
  const noexcept -> const member_layout*
{
  const auto it = m_members.find(name);
  if (it == m_members.end()) {
    return nullptr;
  }
  return &it->second;
}

//------------------------------------------------------------------------------
// Private Member Types
//------------------------------------------------------------------------------

auto alloy::render::program_layout::string_hash::operator()(
  core::string_view s
) const noexcept -> std::size_t
{
  return std::hash<core::string_view>{}(s);
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::program_layout::program_layout(
  core::vector<vertex_input_layout> vertex_inputs,
  core::vector<instance_input_layout> instance_inputs,
  core::bytes instance_stride,
  core::vector<resource_layout> resources,
  string_map<std::uint32_t> resource_indices,
  string_map<member_layout> members
) noexcept
  : m_vertex_inputs{std::move(vertex_inputs)},
    m_instance_inputs{std::move(instance_inputs)},
    m_instance_stride{instance_stride},
    m_resources{std::move(resources)},
    m_resource_indices{std::move(resource_indices)},
    m_members{std::move(members)}
{

}
