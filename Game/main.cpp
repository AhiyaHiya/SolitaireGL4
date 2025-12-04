#include "cards.hpp"
#include "keyboard.hpp"
#include "window.hpp"

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> // Ordering is important and this file must be included after glad
#include <glm/glm.hpp>
// clang-format on

#include <iostream>

auto compile_shaders() -> std::expected<std::pair<GLuint, GLuint>, std::string>
{
    const auto vertex_shader_path   = std::string("Shaders/card.vert");
    const auto fragment_shader_path = std::string("Shaders/card.frag");

    auto vertex_compiled_result = compile_card_shader(vertex_shader_path, GL_VERTEX_SHADER);
    if (!vertex_compiled_result)
    {
        return std::unexpected("Failed to compile vertex shader: " + vertex_compiled_result.error());
    }
    auto vertex_shader_id = vertex_compiled_result.value();

    auto fragment_compiled_result = compile_card_shader(fragment_shader_path, GL_FRAGMENT_SHADER);
    if (!fragment_compiled_result)
    {
        return std::unexpected("Failed to compile fragment shader: " + fragment_compiled_result.error());
    }
    auto fragment_shader_id = fragment_compiled_result.value();

    return std::make_pair(vertex_shader_id, fragment_shader_id);
}

// Returns true/false, error message
auto init_window(const std::int32_t width, const std::int32_t height) -> std::expected<std::shared_ptr<GLFWwindow>, std::string>
{
    if (!glfwInit())
    {
        return std::unexpected("Failed to initialize GLFW");
    }

    auto result = create_window(width, height, "Solitaire");
    if (!result)
    {
        auto [error_code, error_string] = result.error();
        glfwTerminate();
        return std::unexpected("Failed to create window (" + std::to_string(error_code) + ")");
    }

    auto window = result.value();
    glfwMakeContextCurrent(window.get());
    return window;
}

auto link_shader_program(GLuint vertex_shader_id, GLuint fragment_shader_id) -> std::expected<GLuint, std::string>
{
    auto program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    auto program_linked = GLint();
    glGetProgramiv(program_id, GL_LINK_STATUS, &program_linked);
    if (program_linked != GL_TRUE)
    {
        auto log_length = GLsizei(0);
        const auto size = 1024;
        auto err_message = std::string(size, '\0');
        glGetProgramInfoLog(program_id, size, &log_length, err_message.data());
        return std::unexpected(err_message);
    }

    return program_id;
}

auto load_opengl() -> std::pair<bool, std::string>
{
    const auto version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        return {false, "Failed to initialize OpenGL context"};
    }

    // Successfully loaded OpenGL
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    return {true, ""};
}

// Empty implementation
int main()
{
    constexpr auto width  = 1400;
    constexpr auto height = 1000;
    auto window_result = init_window(width, height);
    if(!window_result)
    {
        std::cerr << window_result.error() << std::endl;
        return 1;
    }
    auto window = window_result.value();

    if(auto [success, error_message] = load_opengl(); !success)
    {
        std::cerr << error_message << std::endl;
        return 1;
    }

    // Compile shaders
    auto compile_shaders_result = compile_shaders();
    if (!compile_shaders_result)
    {
        std::cerr << "Failed to compile shaders: " << compile_shaders_result.error() << "\n";
        return 1;
    }
    auto [vertex_shader_id, fragment_shader_id] = compile_shaders_result.value();

    // Link shader program
    auto program_id_result = link_shader_program(vertex_shader_id, fragment_shader_id);
    if (!program_id_result)
    {
        std::cerr << "Failed to link shader program: " << program_id_result.error() << "\n";
        return 1;
    }
    auto program_id = program_id_result.value();

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
    glUseProgram(program_id);
    
    // Load up the assets
    if (!load_card_textures())
    {
        // TODO: convert to spdlog logging
        std::cerr << "Failed to load card textures\n";
        return 1;
    }

    // Exit app when ESC is pressed
    glfwSetKeyCallback(window.get(), key_callback);

    glViewport(0, 0, width, height);

    // Game loop
    while (!glfwWindowShouldClose(window.get()))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window.get());
    }

    glfwTerminate();
    return 0;
}
