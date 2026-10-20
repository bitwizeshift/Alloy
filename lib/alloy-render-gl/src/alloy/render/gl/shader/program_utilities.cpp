// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include <alloy/io/filesystem/disk_filesystem.hpp>
#include "alloy/render/gl/shader/program_utilities.hpp"

auto alloy::render::gl::program_utilities::make_program_from_sources(
  core::zstring_view vertex,
  core::zstring_view fragment
) -> core::result<program, gl_error_message>
{
  auto v_shader = vertex_shader::compile_from_source(vertex);
  if (!v_shader) {
    return core::fail(std::move(v_shader).error());
  }

  auto f_shader = fragment_shader::compile_from_source(fragment);
  if (!f_shader) {
    return core::fail(std::move(f_shader).error());
  }

  return program::link(
    *std::move(v_shader),
    *std::move(f_shader)
  );
}

auto alloy::render::gl::program_utilities::make_program_from_sources(
  core::zstring_view vertex,
  core::zstring_view geometry,
  core::zstring_view fragment
) -> core::result<program, gl_error_message>
{
  auto v_shader = vertex_shader::compile_from_source(vertex);
  if (!v_shader) {
    return core::fail(std::move(v_shader).error());
  }

  auto g_shader = geometry_shader::compile_from_source(geometry);
  if (!g_shader) {
    return core::fail(std::move(g_shader).error());
  }

  auto f_shader = fragment_shader::compile_from_source(fragment);
  if (!f_shader) {
    return core::fail(std::move(f_shader).error());
  }

  return program::link(
    *std::move(v_shader),
    *std::move(g_shader),
    *std::move(f_shader)
  );
}

auto alloy::render::gl::program_utilities::make_program_from_files(
  io::file& vertex,
  io::file& fragment
) -> core::result<program, gl_error_message>
{
  auto v_shader = vertex_shader::compile_from_file(vertex);
  if (!v_shader) {
    return core::fail(std::move(v_shader).error());
  }

  auto f_shader = fragment_shader::compile_from_file(fragment);
  if (!f_shader) {
    return core::fail(std::move(f_shader).error());
  }

  return program::link(
    *std::move(v_shader),
    *std::move(f_shader)
  );
}

auto alloy::render::gl::program_utilities::make_program_from_files(
  io::file& vertex,
  io::file& geometry,
  io::file& fragment
) -> core::result<program, gl_error_message>
{
  auto v_shader = vertex_shader::compile_from_file(vertex);
  if (!v_shader) {
    return core::fail(std::move(v_shader).error());
  }

  auto g_shader = geometry_shader::compile_from_file(geometry);
  if (!g_shader) {
    return core::fail(std::move(g_shader).error());
  }

  auto f_shader = fragment_shader::compile_from_file(fragment);
  if (!f_shader) {
    return core::fail(std::move(f_shader).error());
  }

  return program::link(
    *std::move(v_shader),
    *std::move(g_shader),
    *std::move(f_shader)
  );
}

auto alloy::render::gl::program_utilities::make_program_from_paths(
    core::zstring_view vertex_path,
    core::zstring_view fragment_path
) -> core::result<program, gl_error_message>
{
  constexpr auto flags = io::open_mode::read;

  auto filesystem = io::disk_filesystem{};

  auto vf = filesystem.open(vertex_path, flags);
  auto ff = filesystem.open(fragment_path, flags);

  return make_program_from_files(vf, ff);
}

auto alloy::render::gl::program_utilities::make_program_from_paths(
    core::zstring_view vertex_path,
    core::zstring_view geometry_path,
    core::zstring_view fragment_path
) -> core::result<program, gl_error_message>
{
  constexpr auto flags = io::open_mode::read;

  auto filesystem = io::disk_filesystem{};

  auto vf = filesystem.open(vertex_path, flags);
  auto gf = filesystem.open(geometry_path, flags);
  auto ff = filesystem.open(fragment_path, flags);

  return make_program_from_files(vf, gf, ff);
}
