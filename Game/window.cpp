#include "window.hpp"

auto create_window(std::int32_t width, std::int32_t height, std::string_view title)
    -> std::expected<std::shared_ptr<GLFWwindow>, std::pair<error_code_t, error_message_t>>
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    auto window_ptr = glfwCreateWindow(width,
                                       height,
                                       title.data(),
                                       // Windowed mode
                                       nullptr,
                                       // Not sharing resources
                                       nullptr);

    if (!window_ptr)
    {
        // Something bad happened, so get the last error code and string
        // from GLFW
        const char* error_string;
        const auto  error_code = glfwGetError(&error_string);
        return std::unexpected(
            std::make_pair(error_code, error_string != nullptr ? error_string : "Unknown error"));
    }

    auto window =
        std::shared_ptr< GLFWwindow >(window_ptr, [](GLFWwindow* ptr) { glfwDestroyWindow(ptr); });

    return window;
}
