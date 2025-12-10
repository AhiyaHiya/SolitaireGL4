#include "cards.hpp"
#include "keyboard.hpp"
#include "types.hpp"
#include "window.hpp"

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> // Ordering is important and this file must be included after glad
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// clang-format on

#include <iostream>

constexpr auto generic_error = 1;
constexpr auto no_error      = 0;

auto load_opengl() -> std::pair<bool, error_message_t>
{
    const auto version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        return {false, "Failed to initialize OpenGL context"};
    }

    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    return {true, ""};
}

// ----------------------------------------------------------------
/// @brief No description needed
/// @return 0 for no error, everything else is error
int main()
{
    // Init
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return generic_error;
    }

    // Window
    constexpr auto width  = 1400;
    constexpr auto height = 1000;
    auto           result = create_window(width, height, "Solitaire");
    if (!result)
    {
        auto [error_code, error_string] = result.error();
        glfwTerminate();
        std::cerr << "Failed to create window" << error_string << "\n";
        std::cerr << "\tError code:" << error_code << "\n";
        std::cerr << "\tError msg:" << error_string << "\n";
        return generic_error;
    }

    // Context
    auto window = result.value();
    glfwMakeContextCurrent(window.get());

    // Load OpenGL
    if (auto [success, error_message] = load_opengl(); !success)
    {
        std::cerr << error_message << std::endl;
        return generic_error;
    }

    auto create_card_renderer_result = create_card_renderer();
    if (!create_card_renderer_result.has_value())
    {
        std::cout << "Failed to create card renderer: " << create_card_renderer_result.error();
        return generic_error;
    }
    auto cr = create_card_renderer_result.value();

    // OpenGL states (one-time)
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set projection (one-time, pixel-perfect bottom-left origin)
    auto projection = glm::ortho(0.0f,                       // left
                                 static_cast<float>(width),  // right
                                 static_cast<float>(height), // bottom
                                 0.0f,                       // top
                                 -1.0f,                      // near
                                 1.0f);                      // far
    glUniformMatrix4fv(cr->uProjection,                      // location
                       1,                                    // count
                       GL_FALSE,                             // transpose
                       glm::value_ptr(projection));

    // Set texture sampler unit (one-time)
    glUniform1i(glGetUniformLocation(cr->shader_program, "uCardTextures"), 0);

    // Exit app when ESC is pressed
    glfwSetKeyCallback(window.get(), key_callback);

    glViewport(0, 0, width, height);

    // Game loop
    while (!glfwWindowShouldClose(window.get()))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_cards(cr);

        glfwSwapBuffers(window.get());
    }

    glfwTerminate();
    return 0;
}
