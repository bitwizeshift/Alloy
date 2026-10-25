// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/shader/program_layout.hpp"

#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/shader/shader_stage.hpp"

#include "alloy/render/error.hpp"

#include "alloy/core/containers/hash_map.hpp"
#include "alloy/core/containers/vector.hpp"
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

/// \brief A uniform buffer member awaiting insertion into the layout's lookup
struct pending_member
{
  core::string_view block;
  core::string_view name;
  member_layout layout;
};

/// \brief Validates the kind of a single resource, converting it into the
///        kind that the layout retains
struct resource_kind_visitor
{
  std::uint32_t resource_index;
  core::string_view resource_name;
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
      resource_kind_visitor{index, resource.name, &pending},
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
    std::move(resources),
    std::move(resource_indices),
    std::move(members)
  };
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

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
  core::vector<resource_layout> resources,
  string_map<std::uint32_t> resource_indices,
  string_map<member_layout> members
) noexcept
  : m_resources{std::move(resources)},
    m_resource_indices{std::move(resource_indices)},
    m_members{std::move(members)}
{

}
