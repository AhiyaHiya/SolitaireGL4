#ifndef _GAME_WINDOW_HPP__
#define _GAME_WINDOW_HPP__

#include "types.hpp"

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <cstdint>
#include <expected>
#include <memory>
#include <string_view>

struct GLFWwindow;

///
/// @brief Creates a window with the specified dimensions and title.
/// @param width The width of the window in pixels.
/// @param height The height of the window in pixels.
/// @param title The title of the window.
/// @return A std::expected containing a shared pointer to the GLFWwindow on success, or an error code and message on failure.
/// @remarks The returned shared pointer will automatically destroy the window when it goes out of scope.
auto create_window(std::int32_t width, std::int32_t height, std::string_view title) 
    ->std::expected< std::shared_ptr< GLFWwindow >, std::pair< error_code_t, error_message_t > >;

#endif // _GAME_WINDOW_HPP__
