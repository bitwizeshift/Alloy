#include "gl/error.hpp"
#include "gl/shader.hpp"
#include "gl/program.hpp"
#include "gl/active_program.hpp"

#include <alloy/core/math/angle/degree.hpp>
#include <alloy/core/utilities/signal.hpp>
#include <alloy/core/intrinsics.hpp>
#include <alloy/core/math/quaternion.hpp>
#include <alloy/core/math/angle/degree.hpp>
#include <alloy/core/math/angle/gradian.hpp>
#include <alloy/core/math/matrix/matrix4_constants.hpp>
#include <alloy/core/model/camera.hpp>
#include <alloy/core/model/projection.hpp>
#include <alloy/core/model/lazy_transform.hpp>
#include <alloy/io/message_pump.hpp>
#include <alloy/io/window.hpp>
#include <alloy/io/events/keyboard_events.hpp>
#include <alloy/extra/sdl2-bindings/sdl2_gl_window.hpp>
#include <alloy/extra/sdl2-bindings/sdl2_pump_source.hpp>
#include <alloy/core/model/transform.hpp>

#include <alloy/core/model/mesh.hpp>

#include <alloy/core/geometry/point.hpp>

#include <alloy/io/filesystem/disk_filesystem.hpp>
#include <alloy/core/containers/map.hpp>
#include <alloy/core/utilities/string_tokenizer.hpp>

#include <SDL2/SDL.h> // ::SDL_Init, ::SDL_Quit, etc
#include <gl/glew.h>
#include <windows.h>

#include <iostream> // std::cout, std::endl
#include <chrono>   // std::chrono::steady_clock
#include <random>   // std::random
#include <charconv> // from_chars

namespace {

  // Flag to keep the application running
  bool g_keep_running = true;

  auto g_is_orthographic = false;
  auto g_projection = alloy::core::lazy_projection{};

  const auto g_render_types = std::array{
    GL_POINTS,
    GL_LINE_STRIP,
    GL_LINE_LOOP,
    GL_LINES,
    GL_LINE_STRIP_ADJACENCY,
    GL_LINES_ADJACENCY,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP_ADJACENCY,
    GL_TRIANGLES_ADJACENCY,
  };
  const auto g_render_strings = std::array{
    "GL_POINTS",
    "GL_LINE_STRIP",
    "GL_LINE_LOOP",
    "GL_LINES",
    "GL_LINE_STRIP_ADJACENCY",
    "GL_LINES_ADJACENCY",
    "GL_TRIANGLE_STRIP",
    "GL_TRIANGLE_FAN",
    "GL_TRIANGLES",
    "GL_TRIANGLE_STRIP_ADJACENCY",
    "GL_TRIANGLES_ADJACENCY",
  };
  auto g_render_index = 8;

  auto render_type() -> ::GLenum
  {
    return g_render_types[g_render_index];
  }

  auto g_transform = alloy::core::transform{};
  auto g_camera = alloy::core::camera{};

  auto circle_coordinate = alloy::core::degree{0};

  //////////////////////////////////////////////////////////////////////////////
  /// A simple listener that handles window events
  //////////////////////////////////////////////////////////////////////////////
  class window_listener
    : public alloy::io::message_pump::listener
  {
  public:
    void handle_message(const alloy::io::event& e) noexcept override;
  };

  void window_listener::handle_message(const alloy::io::event& e)
    noexcept
  {
    if (e.is<alloy::io::window_show_event>()) {
      std::cout << "window_show_event{}" << std::endl;
    } else if (e.is<alloy::io::window_hide_event>()) {
      std::cout << "window_hide_event{}" << std::endl;
    } else if (auto* event = e.try_as<alloy::io::window_move_event>()) {
      std::cout << "window_move_event{" << event->x << ", " << event->y << "}" << std::endl;
    } else if (auto* event = e.try_as<alloy::io::window_resize_event>()) {
      std::cout << "window_resize_event{" << event->width << ", " << event->height << "}" << std::endl;
    } else if (e.is<alloy::io::window_minimize_event>()) {
      std::cout << "window_minimize_event{}" << std::endl;
    } else if (e.is<alloy::io::window_maximize_event>()) {
      std::cout << "window_maximize_event{}" << std::endl;
    } else if (e.is<alloy::io::window_close_event>()) {
      std::cout << "window_close_event{}" << std::endl;
      ::g_keep_running = false;
    } else if (auto* event = e.try_as<alloy::io::keyboard_event>()) {
//      std::cout << "keyboard_event{ {"
//        << (int) event->symbol.key_code << ", "
//        << (int) event->symbol.scan_code << ", "
//        << (int) event->symbol.modifiers << "}, "
//        << (int) event->state << "}" << std::endl;


      const auto rotation = alloy::core::casts::to_radian(
        alloy::core::degree{3}
      );

      if (event->state == alloy::io::keyboard::key_state::pressed) {
        const auto code = event->symbol.key_code;
        if (code == alloy::io::keyboard::key_code::arrow_up) {
          g_render_index = (g_render_index + 1) % g_render_types.size();
          std::cout << "Current renderer: " << g_render_strings[g_render_index] << std::endl;
        }
        else if (code == alloy::io::keyboard::key_code::arrow_down) {
          if (g_render_index == 0) {
            g_render_index = g_render_types.size() - 1;
          } else {
            g_render_index = (g_render_index - 1) % g_render_types.size();
          }
          std::cout << "Current renderer: " << g_render_strings[g_render_index] << std::endl;
        }
        else if (code == alloy::io::keyboard::key_code::arrow_left) {
          circle_coordinate += alloy::core::degree{3};

          const auto x = alloy::core::trigonometry::sin(circle_coordinate) * 2;
          const auto z = alloy::core::trigonometry::cos(circle_coordinate) * 2;
          ::g_camera.set_origin(x, 1, z);
          ::g_camera.look_at(0, 0, 0);
        }
        else if (code == alloy::io::keyboard::key_code::arrow_right) {
          circle_coordinate -= alloy::core::degree{3};

          const auto x = alloy::core::trigonometry::sin(circle_coordinate) * 2;
          const auto z = alloy::core::trigonometry::cos(circle_coordinate) * 2;
          ::g_camera.set_origin(x, 1, z);
          ::g_camera.look_at(0, 0, 0);
        }
        else if (code == alloy::io::keyboard::key_code::letter_q) {
          g_transform.rotate_roll(rotation);
        }
        else if (code == alloy::io::keyboard::key_code::letter_e) {
          g_transform.rotate_roll(-rotation);
        }
        else if (code == alloy::io::keyboard::key_code::letter_a) {
          g_transform.rotate_yaw(rotation);
        }
        else if (code == alloy::io::keyboard::key_code::letter_d) {
          g_transform.rotate_yaw(-rotation);
        }
        else if (code == alloy::io::keyboard::key_code::letter_w) {
          g_transform.rotate_pitch(rotation);
        }
        else if (code == alloy::io::keyboard::key_code::letter_s) {
          g_transform.rotate_pitch(-rotation);
        }
        else if (code == alloy::io::keyboard::key_code::letter_z) {
          g_transform.scale(1.1);
        }
        else if (code == alloy::io::keyboard::key_code::letter_x) {
          g_transform.scale(0.9);
        }
        else if (code == alloy::io::keyboard::key_code::letter_i) {
          g_transform.translate_x(0.05);
        }
        else if (code == alloy::io::keyboard::key_code::letter_k) {
          g_transform.translate_x(-0.05);
        }
        else if (code == alloy::io::keyboard::key_code::letter_j) {
          g_transform.translate_z(0.05);
        }
        else if (code == alloy::io::keyboard::key_code::letter_l) {
          g_transform.translate_z(-0.05);
        }
        else if (code == alloy::io::keyboard::key_code::letter_u) {
          g_transform.translate_y(0.05);
        }
        else if (code == alloy::io::keyboard::key_code::letter_o) {
          g_transform.translate_y(-0.05);
        }
        else if (code == alloy::io::keyboard::key_code::space) {
          ::g_is_orthographic = !::g_is_orthographic;
          if (::g_is_orthographic) {
            g_projection->orthographic(
              -1, 1,
              -1, 1,
              -3, 3
            );
          } else {
            g_projection->perspective(
              alloy::core::degree{60},
              alloy::core::real{640} / alloy::core::real{480},
              {0.01, 5}
            );
          }
        }
      }
    }
  }
}

auto vertex_shader_program =
  "#version 330 core\n"
  "\n"
  "// Vertex inputs\n"
  "in vec3 in_vertex;\n"
  "in vec3 in_normal;\n"
  "in vec4 in_color;\n"
  "in vec2 in_texcoord;\n"
  "\n"
  "// Uniform inputs\n"
  "uniform mat4 u_world;\n"
  "uniform mat4 u_view;\n"
  "uniform mat4 u_projection;\n"
  "\n"
  "// Fragment outputs\n"
  "out vec3 frag_position;\n"
  "out vec3 frag_normal;\n"
  "out vec4 frag_color;\n"
  "void main() {\n"
  "  mat4 mvp = u_projection * u_view * u_world;"
  "  gl_Position = mvp * vec4(in_vertex,1.0);\n"
  "  \n"
  "  frag_color = in_color;\n"
  "  frag_position = vec3(u_world * vec4(in_vertex,1.0));\n"
  "  frag_normal = vec3(u_world * vec4(in_normal,1.0));\n"
  "}\n";

auto fragment_shader_program =
  "#version 330 core\n"
  "in vec3 frag_position;\n"
  "in vec3 frag_normal;\n"
  "in vec4 frag_color;\n"
  "\n"
  "layout(location = 0) out vec4 diffuse_color;\n"
  "\n"
  "void main(){\n"
  "  vec3 light_pos = vec3(1,1,1);\n"
  "  vec3 light_color = vec3(1,1,1);\n"
  "\n"
  "  vec3 norm      = normalize(frag_normal)\n;"
  "  vec3 light_dir = normalize(light_pos - frag_position);\n"
  "\n"
  "  float diffuse = max(dot(norm, light_dir), 0.1);\n"
  "  float distance = length(light_pos - frag_position);\n"
  "  diffuse = diffuse * (1.0 / (1.0 + (0.25 * distance * distance)));\n"
  "  vec3 color = diffuse * light_color;\n"
  "\n"
  "  diffuse_color = frag_color * vec4(color,1.0);\n"
  "}";

namespace alloy::core::experimental {
  auto make_cube() -> mesh
  {
    auto builder = mesh_builder{};

    builder.reserve(
      quantity<vertex_data>{32},
      quantity<mesh::index_type>{36}
    );

    { // Bottom-face
      const auto i0 = builder.add_vertex(
        {-1,-1,-1},
        normal::make_unchecked(0,-1,0),
        {0,0},
        color_constants::green
      );
      const auto i1 = builder.add_vertex(
        {1,-1,-1},
        normal::make_unchecked(0,-1,0),
        {0,0},
        color_constants::green
      );
      const auto i2 = builder.add_vertex(
        {1,-1,1},
        normal::make_unchecked(0,-1,0),
        {0,0},
        color_constants::green
      );
      const auto i3 = builder.add_vertex(
        {-1,-1,1},
        normal::make_unchecked(0,-1,0),
        {0,0},
        color_constants::green
      );
      builder.add_face(i0, i1, i2, i3);
    }

    { // Back-face
      const auto i0 = builder.add_vertex(
        {1,-1,-1},
        normal::make_unchecked(0,0,-1),
        {0,0},
        color_constants::yellow
      );
      const auto i1 = builder.add_vertex(
        {-1,-1,-1},
        normal::make_unchecked(0,0,-1),
        {0,0},
        color_constants::yellow
      );
      const auto i2 = builder.add_vertex(
        {-1,1,-1},
        normal::make_unchecked(0,0,-1),
        {0,0},
        color_constants::yellow
      );
      const auto i3 = builder.add_vertex(
        {1,1,-1},
        normal::make_unchecked(0,0,-1),
        {0,0},
        color_constants::yellow
      );
      builder.add_face(i0, i1, i2, i3);
    }

    { // Top-face
      const auto i0 = builder.add_vertex(
        {-1,1,-1},
        normal::make_unchecked(0,1,0),
        {0,0},
        color_constants::red
      );
      const auto i1 = builder.add_vertex(
        {1,1,-1},
        normal::make_unchecked(0,1,0),
        {0,0},
        color_constants::red
      );
      const auto i2 = builder.add_vertex(
        {1,1,1},
        normal::make_unchecked(0,1,0),
        {0,0},
        color_constants::red
      );
      const auto i3 = builder.add_vertex(
        {-1,1,1},
        normal::make_unchecked(0,1,0),
        {0,0},
        color_constants::red
      );
      builder.add_face(i0, i1, i2, i3);
    }

    { // Front-face
      const auto i0 = builder.add_vertex(
        {1,-1,1},
        normal::make_unchecked(0,0,1),
        {0,0},
        color_constants::blue
      );
      const auto i1 = builder.add_vertex(
        {-1,-1,1},
        normal::make_unchecked(0,0,1),
        {0,0},
        color_constants::blue
      );
      const auto i2 = builder.add_vertex(
        {-1,1,1},
        normal::make_unchecked(0,0,1),
        {0,0},
        color_constants::blue
      );
      const auto i3 = builder.add_vertex(
        {1,1,1},
        normal::make_unchecked(0,0,1),
        {0,0},
        color_constants::blue
      );
      builder.add_face(i0, i1, i2, i3);
    }

    { // Left-face
      const auto i0 = builder.add_vertex(
        {-1,-1,1},
        normal::make_unchecked(-1,0,0),
        {0,0},
        color_constants::cyan
      );
      const auto i1 = builder.add_vertex(
        {-1,-1,-1},
        normal::make_unchecked(-1,0,0),
        {0,0},
        color_constants::cyan
      );
      const auto i2 = builder.add_vertex(
        {-1,1,-1},
        normal::make_unchecked(-1,0,0),
        {0,0},
        color_constants::cyan
      );
      const auto i3 = builder.add_vertex(
        {-1,1,1},
        normal::make_unchecked(-1,0,0),
        {0,0},
        color_constants::cyan
      );
      builder.add_face(i0, i1, i2, i3);
    }

    { // Right-face
      const auto i0 = builder.add_vertex(
        {1,-1,1},
        normal::make_unchecked(1,0,0),
        {0,0},
        color_constants::magenta
      );
      const auto i1 = builder.add_vertex(
        {1,-1,-1},
        normal::make_unchecked(1,0,0),
        {0,0},
        color_constants::magenta
      );
      const auto i2 = builder.add_vertex(
        {1,1,-1},
        normal::make_unchecked(1,0,0),
        {0,0},
        color_constants::magenta
      );
      const auto i3 = builder.add_vertex(
        {1,1,1},
        normal::make_unchecked(1,0,0),
        {0,0},
        color_constants::magenta
      );
      builder.add_face(i0, i1, i2, i3);
    }

    return std::move(builder).build();
  }

}

namespace alloy::engine::experimental {

  class obj_mesh_loader
  {
  public:

    obj_mesh_loader() = default;

    auto load(io::file& file) -> core::expected<core::mesh>
    {
      if (!file.is_open()) {
        return core::unexpected(io::file::error_code::closed);
      }

      const auto buffer_size = file.bytes();
      if (!buffer_size) {
        return core::unexpected(buffer_size.error());
      }
      auto buffer = core::vector<char>{};
      buffer.resize(*buffer_size);

      auto read_result = file.read(io::mutable_buffer::from_container(buffer));
      if (!read_result) {
        return core::unexpected(read_result.error());
      }

      for_each_line(buffer, [&](core::string_view line){
        if (line.empty()) {
          return;
        }

        auto tokenizer = core::string_tokenizer{line, " \n"};

        const auto directive = tokenizer.next();

        if (directive->empty()) {
          return;
        }

        if (*directive == "v") {
          parse_vertex_data(tokenizer);
        } else if (*directive == "vt") {
          parse_texture_coordinate_data(tokenizer);
        } else if (*directive == "vn") {
          parse_normal_data(tokenizer);
        } else if (*directive == "f") {
          parse_face_data(tokenizer);
        }
      });

      return std::move(m_builder).build();
    }

  private:

    using index_type = core::mesh::index_type;

    struct index_triple
    {
      std::optional<core::mesh::index_type> vertex_index;
      std::optional<core::mesh::index_type> normal_index;
      std::optional<core::mesh::index_type> texture_coordinate_index;
    };

    auto parse_vertex_data(core::string_tokenizer& tokenizer) -> void
    {
      const auto v0 = tokenizer.next();
      const auto v1 = tokenizer.next();
      const auto v2 = tokenizer.next();

      if (!v0 || !v1 || !v2) {
        std::cerr << "Error parsing vertex data (";
        std::cerr.write(tokenizer.buffer().data(), tokenizer.buffer().size());
        std::cerr << std::endl;
        return;
      }

      const auto vertex = core::vertex{
        std::stof(v0->data()),
        std::stof(v1->data()),
        std::stof(v2->data()),
      };

      m_vertices.push_back(vertex);
    }

    auto parse_texture_coordinate_data(core::string_tokenizer& tokenizer) -> void
    {
      const auto v0 = tokenizer.next();
      const auto v1 = tokenizer.next();

      if (!v0 || !v1) {
        return;
      }

      const auto texture_coordinate = core::texture_coordinate{
        std::stof(v0->data()),
        std::stof(v1->data()),
      };

      m_texture_coordinates.push_back(texture_coordinate);
    }

    auto parse_normal_data(core::string_tokenizer& tokenizer) -> void
    {
      const auto v0 = tokenizer.next();
      const auto v1 = tokenizer.next();
      const auto v2 = tokenizer.next();

      if (!v0 || !v1 || !v2) {
        std::cerr << "Error parsing normal data (";
        std::cerr.write(tokenizer.buffer().data(), tokenizer.buffer().size());
        std::cerr << std::endl;
        return;
      }

      const auto normal = core::normal::make(
        std::stof(v0->data()),
        std::stof(v1->data()),
        std::stof(v2->data())
      );

      m_normals.push_back(normal);
    }

    auto parse_triple(core::string_view triple) -> index_triple
    {
      auto tokenizer = core::string_tokenizer{triple, "/"};

      const auto compute_value = [](const auto& exp) -> std::optional<core::mesh::index_type> {
        if (!exp) {
          return std::nullopt;
        }
        if (exp->empty()) {
          return std::nullopt;
        }
        return std::stoi( static_cast<std::string>(*exp) );
      };

      const auto t0 = tokenizer.next();
      const auto t1 = tokenizer.next();
      const auto t2 = tokenizer.next();

      return index_triple{
        compute_value(t0),
        compute_value(t2),
        compute_value(t1),
      };
    }

    auto parse_face_data(core::string_tokenizer& tokenizer) -> void
    {
      const auto i0 = tokenizer.next();
      const auto i1 = tokenizer.next();
      const auto i2 = tokenizer.next();

      if (!i0 || !i1 || !i2) {
        std::cerr << "Error parsing face data (";
        std::cerr.write(tokenizer.buffer().data(), tokenizer.buffer().size());
        std::cerr << std::endl;
        return;
      }

      const auto t0 = parse_triple(*i0);
      const auto t1 = parse_triple(*i1);
      const auto t2 = parse_triple(*i2);

      add_face(t0,t1,t2);
    }

    template <typename Fn>
    auto for_each_line(const core::vector<char>& buffer, Fn&& fn) -> void
    {
      auto start = 0u;

      const auto buffer_size = buffer.size();
      for (auto i = 0u; i < buffer_size; ++i) {

        if (buffer[i] == '\n' || i == (buffer_size - 1)) {
          fn(core::string_view{buffer.data() + start, (i - start)});
          // Set start to the next index
          start = i + 1;
        }
      }
    }

    auto add_face(const index_triple& t0,
                  const index_triple& t1,
                  const index_triple& t2) -> void
    {
      const auto i0 = add_vertex(t0);
      const auto i1 = add_vertex(t1);
      const auto i2 = add_vertex(t2);

      m_builder.add_face(i0, i1, i2);
    }

    auto add_vertex(const index_triple& triple) -> index_type
    {
      auto it = m_index_map.find(triple);

      if (it != m_index_map.end()) {
        return it->second;
      }

      const auto default_normal = core::normal::make(0,1,0);
      const auto default_texture_coordinate = core::texture_coordinate{0,0};

      const auto vertex = m_vertices.at(*triple.vertex_index - 1);
      const auto normal = [&]{
        if (!triple.normal_index) {
          return default_normal;
        }
        return m_normals[*triple.normal_index - 1];
      }();
      const auto texture_coordinate = [&]{
        if (!triple.texture_coordinate_index) {
          return default_texture_coordinate;
        }
        return m_texture_coordinates[*triple.texture_coordinate_index - 1];
      }();

      const auto index = m_builder.add_vertex(vertex, normal, texture_coordinate);

      // Cache it for later lookups
      m_index_map.insert(it, std::make_pair(triple, index));

      return index;
    }

    struct index_triple_comparator {
      auto operator()(const index_triple& lhs, const index_triple& rhs) const noexcept -> bool
      {
        return
          std::tie(lhs.vertex_index, lhs.normal_index, lhs.texture_coordinate_index) <
          std::tie(rhs.vertex_index, rhs.normal_index, rhs.texture_coordinate_index);
      }
    };

    using vertex_map = core::map<
      index_triple,
      core::mesh_builder::index_type,
      index_triple_comparator
    >;
    core::vector<core::vertex> m_vertices;
    core::vector<core::normal> m_normals;
    core::vector<core::texture_coordinate> m_texture_coordinates;
    vertex_map m_index_map;
    core::mesh_builder m_builder;
  };

} // namespace alloy::engine::experimental


int main(int argc, char** argv)
{
  using namespace std::chrono_literals;

  alloy::core::compiler::unused(argc);
  alloy::core::compiler::unused(argv);
  ::SDL_Init(0);
  ::SDL_InitSubSystem( SDL_INIT_VIDEO );

  // random number generator to generate random color frames per second
  auto random_device = std::random_device{};
  auto generator     = std::mt19937{random_device()};
  auto distribution  = std::uniform_real_distribution<float>{0.0f, 1.0f};

  auto message_pump = alloy::io::message_pump{};
  auto listener = ::window_listener{};
  auto& sdl2_pump_source = alloy::extra::sdl2_pump_source::instance();
  auto window = alloy::extra::sdl2_gl_window::from_window_data(
    "Hello World",
    alloy::io::window::dimensions{640, 480},
    alloy::extra::sdl_gl_version{3,2}
  );

  // allow the window to produce events
  sdl2_pump_source.attach_window(&window);

  const auto err = ::glewInit();
  if (GLEW_OK != err)
  {
    std::fprintf(stderr, "Error: %s\n", ::glewGetErrorString(err));
    return 1;
  }
  std::fprintf(stdout, "Status: Using GLEW %s\n", ::glewGetString(GLEW_VERSION));

  // Ensure that the pump source and event source are removed at the end of
  // this scope
  const auto source_conn = alloy::core::scoped_connection{
    message_pump.on_poll().connect(&sdl2_pump_source)
  };
  const auto listener_conn = alloy::core::scoped_connection{
    message_pump.on_event().connect(&listener)
  };

  // update loop:
  // * handle i/o by pumping event sources
  // * process new frame (1 fps)
  // * update window
  using clock = std::chrono::steady_clock;

//  auto model = alloy::core::cube::make_colored(0.5, alloy::core::color{1,0,0,0});
//  auto vertices = model.vertices();
//  auto normals  = model.normals();
//  auto colors   = model.colors();
//  auto texcoords = model.texture_coordinates();

  auto vertex_shader = alloy::render::gl::vertex_shader::compile_from_source(::vertex_shader_program);
  auto fragment_shader = alloy::render::gl::fragment_shader::compile_from_source(::fragment_shader_program);

  if (!vertex_shader) {
    std::cerr << "vertex shader error: " << vertex_shader.error().message() << std::endl;
    return EXIT_FAILURE;
  }
  if (!fragment_shader) {
    std::cerr << "fragment shader error: " <<fragment_shader.error().message() << std::endl;
    return EXIT_FAILURE;
  }
  auto program = alloy::render::gl::program::link(
    std::move(*vertex_shader),
    std::move(*fragment_shader)
  );

  if (!program) {
    std::cerr << program.error().message() << std::endl;
    return EXIT_FAILURE;
  }

  auto u_world = program->get_uniform_location("u_world");
  auto u_view = program->get_uniform_location("u_view");
  auto u_projection = program->get_uniform_location("u_projection");

  if (!u_world) {
    std::cerr << u_world.error().message() << std::endl;
    return EXIT_FAILURE;
  }
  if (!u_view) {
    std::cerr << u_view.error().message() << std::endl;
    return EXIT_FAILURE;
  }

  auto fs = alloy::io::disk_filesystem{};
  auto file = fs.open("teapot.obj",alloy::io::open_mode::read);
  auto loader = alloy::engine::experimental::obj_mesh_loader{};

  auto cube_result = loader.load(file);
  if (!cube_result) {
    std::cerr << cube_result.error().message() << std::endl;
    return 1;
  }
  auto& cube = *cube_result;


//  auto cube = alloy::core::experimental::make_cube();//std::move(triangle_builder).build();
  auto vao = ::GLuint{};
  auto vbos = std::array<::GLuint,2>{};

  {
    const auto& vertex_vbo = vbos[0];
    const auto& index_vbo = vbos[1];

    ::glGenVertexArrays(1, &vao);
    ::glBindVertexArray(vao);

    ::glGenBuffers(2, vbos.data());

    // Vertices
    const auto data = cube.vertex_data();
    ::glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
    ::glBufferData(GL_ARRAY_BUFFER,
      data.size_bytes(),
      data.data(),
      GL_STATIC_DRAW
    );
    const auto stride = cube.vertex_data_stride().count();
    const auto vertex_offset = reinterpret_cast<void*>(cube.vertex_offset().count());
    ::glEnableVertexAttribArray(0);
    ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, vertex_offset);

    const auto normal_offset = reinterpret_cast<void*>(cube.normal_offset().count());
    ::glEnableVertexAttribArray(1);
    ::glVertexAttribPointer(1, 3, GL_SHORT, GL_TRUE, stride, normal_offset);

    const auto color_offset = reinterpret_cast<void*>(cube.color_offset().count());
    ::glEnableVertexAttribArray(2);
    ::glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, color_offset);

    const auto texture_coordinate_offset = reinterpret_cast<void*>(cube.texture_coordinate_offset().count());
    ::glEnableVertexAttribArray(3);
    ::glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, texture_coordinate_offset);

    // Indices
    const auto indices = cube.index_data();
    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
    ::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
      indices.size_bytes(),
      indices.data(),
      GL_STATIC_DRAW
    );
  }

  auto axis_vao = ::GLuint{};
  auto axis_vbos = std::array<::GLuint,4>{};
  {
    auto axis_vertices = std::array<alloy::core::real,18>{
      0, 0, 0,
      1, 0, 0,
      0, 0, 0,
      0, 1, 0,
      0, 0, 0,
      0, 0, 1,
    };
    auto axis_colors = std::array<alloy::core::real,18>{
      1, 0, 0,
      1, 0, 0,
      0, 1, 0,
      0, 1, 0,
      0, 0, 1,
      0, 0, 1,
    };

    const auto& vertex_vbo   = axis_vbos[0];
//    const auto& normal_vbo   = axis_vbos[1];
    const auto& color_vbo    = axis_vbos[2];
//    const auto& texcoord_vbo = axis_vbos[3];

    ::glGenVertexArrays(1, &axis_vao);
    ::glBindVertexArray(axis_vao);

    ::glGenBuffers(4, axis_vbos.data());

    // Vertices
    ::glEnableVertexAttribArray(0);
    ::glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
    ::glBufferData(GL_ARRAY_BUFFER,
      axis_vertices.size() * sizeof(alloy::core::real),
      axis_vertices.data(),
      GL_STATIC_DRAW
    );
    ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Colors
    ::glEnableVertexAttribArray(2);
    ::glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
    ::glBufferData(GL_ARRAY_BUFFER,
      axis_colors.size() * sizeof(alloy::core::real),
      axis_colors.data(),
      GL_STATIC_DRAW
    );
    ::glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  }

  alloy::core::set_assert_handler([](const char* message) noexcept {
    ::MessageBoxA(nullptr, message, "Assertion Failure", MB_OK | MB_ICONERROR);
  });

  ::glEnable(GL_DEPTH_TEST);
  ::glDepthFunc(GL_LESS);

  g_camera.set_origin(0, 1, 2);
  g_camera.look_at(0, 0, 0);

  auto last_frame = clock::now();
  auto projection = alloy::core::matrix4{};
  ::g_projection->perspective(
    alloy::core::degree{60},
    alloy::core::real{640} / alloy::core::real{480},
    {0.01, 5}
  );
  ::g_transform.set_scale(0.5);
  while (::g_keep_running) {
    message_pump.pump();

    ::g_projection.update_matrix(&projection);

    const auto now = clock::now();
    if ((now - last_frame) >= 16ms) {

      ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      alloy::render::gl::use_program(*program);

      const auto world = g_transform.to_matrix4();
      const auto view = g_camera.to_matrix4();

      alloy::render::gl::active_program::bind_uniform(*u_world, world);
      alloy::render::gl::active_program::bind_uniform(*u_view, view);
      alloy::render::gl::active_program::bind_uniform(*u_projection, projection);

      ::glBindVertexArray(vao);
      ::glDrawElements(::render_type(), cube.index_data().size(), GL_UNSIGNED_SHORT, nullptr);

      alloy::render::gl::active_program::bind_uniform(*u_world, alloy::core::matrix4_constants::identity);

      ::glBindVertexArray(axis_vao);
      ::glDrawArrays(GL_LINES, 0, 6);

      last_frame = now;
      window.update();
    }
  }

  sdl2_pump_source.detach_window(&window);

  ::SDL_QuitSubSystem( SDL_INIT_VIDEO );
  ::SDL_Quit();
  return 0;
}
