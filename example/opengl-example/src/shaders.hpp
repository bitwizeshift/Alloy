// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file shaders.hpp
///
/// \brief This header defines the GLSL source of the example's programs
////////////////////////////////////////////////////////////////////////////////

#ifndef SHADERS_HPP
#define SHADERS_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <algorithm>   // std::copy_n
#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <string_view> // std::string_view

namespace alloy::example {

  /// \brief Joins string literals into one array of characters, without
  ///        their null terminators
  ///
  /// \param parts the string literals to join
  /// \return the joined characters
  template <std::size_t...Sizes>
  consteval auto concatenate(const char (&...parts)[Sizes])
    -> std::array<char, ((Sizes - 1u) + ...)>;

} // namespace alloy::example

//==============================================================================
// definitions : non-member functions
//==============================================================================

template <std::size_t...Sizes>
consteval
auto alloy::example::concatenate(const char (&...parts)[Sizes])
  -> std::array<char, ((Sizes - 1u) + ...)>
{
  auto result = std::array<char, ((Sizes - 1u) + ...)>{};
  auto out = result.begin();
  const auto append = [&out](const char* part, std::size_t size) {
    out = std::copy_n(part, size - 1u, out);
  };
  (append(parts, Sizes), ...);
  return result;
}

namespace alloy::example {

  /// \brief The vertex stage shared by both programs, which places each
  ///        instance with its model matrix
  inline constexpr auto lit_vertex = std::string_view{R"glsl(
#version 410 core

layout(std140) uniform view
{
  mat4 view_projection;
  vec3 camera_position;
};

in vec3 position;
in vec2 uv;
in vec3 normal;
in mat4 model;

out vec3 v_world_position;
out vec3 v_normal;
out vec2 v_uv;

void main()
{
  vec4 world = model * vec4(position, 1.0);

  v_world_position = world.xyz;
  // Valid for rotations and uniform scales, which is all the example uses
  v_normal = mat3(model) * normal;
  v_uv = uv;
  gl_Position = view_projection * world;
}
)glsl"};

  /// \brief The version directive that begins every stage
  inline constexpr char glsl_version[] = "#version 410 core\n";

  /// \brief The lighting shared by both fragment stages: Lambert diffuse and
  ///        Blinn-Phong specular from a single point light, falling off with
  ///        distance, over an ambient term
  inline constexpr char lighting[] = R"glsl(
layout(std140) uniform view
{
  mat4 view_projection;
  vec3 camera_position;
};

layout(std140) uniform environment
{
  vec3 light_position;
  float ambient_intensity;
  vec4 light_color;
  vec3 attenuation;
  float diffuse_intensity;
};

in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_uv;

out vec4 frag_color;

vec3 shade(vec3 base, float shininess)
{
  vec3 n = normalize(v_normal);
  vec3 to_light = light_position - v_world_position;
  float distance = length(to_light);
  vec3 l = to_light / distance;
  vec3 v = normalize(camera_position - v_world_position);
  vec3 h = normalize(l + v);

  float diffuse = max(dot(n, l), 0.0);
  float specular = diffuse > 0.0 ? pow(max(dot(n, h), 0.0), shininess) : 0.0;
  float falloff = 1.0 / (
    attenuation.x
    + attenuation.y * distance
    + attenuation.z * distance * distance
  );

  vec3 lit = light_color.rgb * diffuse_intensity * falloff
    * (diffuse * base + vec3(specular));
  return ambient_intensity * base + lit;
}
)glsl";

  /// \brief The body of the cube's fragment stage, whose color comes from a
  ///        texture
  inline constexpr char lit_textured_body[] = R"glsl(
layout(std140) uniform material
{
  float shininess;
};

uniform sampler2D albedo;

void main()
{
  frag_color = vec4(shade(texture(albedo, v_uv).rgb, shininess), 1.0);
}
)glsl";

  /// \brief The body of the sphere's fragment stage, which has a single color
  inline constexpr char lit_color_body[] = R"glsl(
layout(std140) uniform material
{
  vec4 base_color;
  float shininess;
};

void main()
{
  frag_color = vec4(shade(base_color.rgb, shininess), 1.0);
}
)glsl";

  /// \brief The characters of the cube's fragment stage
  inline constexpr auto lit_textured_source = concatenate(
    glsl_version,
    lighting,
    lit_textured_body
  );

  /// \brief The characters of the sphere's fragment stage
  inline constexpr auto lit_color_source = concatenate(
    glsl_version,
    lighting,
    lit_color_body
  );

  /// \brief The fragment stage of the cube
  inline constexpr auto lit_textured_fragment = std::string_view{
    lit_textured_source.data(),
    lit_textured_source.size()
  };

  /// \brief The fragment stage of the sphere
  inline constexpr auto lit_color_fragment = std::string_view{
    lit_color_source.data(),
    lit_color_source.size()
  };

} // namespace alloy::example

#endif /* SHADERS_HPP */
