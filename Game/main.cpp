#include "window.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h> // Ordering is important and this file must be included after glad
#include <glm/glm.hpp>

#include <iostream>

// Empty implementation
int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    constexpr auto width  = 1400;
    constexpr auto height = 1000;

    auto result = create_window(width, height, "Solitaire");
    if (!result)
    {
        auto [error_code, error_string] = result.error();
        std::cerr << "GLFW Error (" << error_code << "): " << error_string << std::endl;
        glfwTerminate();
        return 1;
    }

    auto window = result.value();
    glfwMakeContextCurrent(window.get());

    const auto version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        printf("Failed to initialize OpenGL context\n");
        return 1;
    }

    // Successfully loaded OpenGL
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    // No Main loop for now

    glfwTerminate();
    return 0;
}
