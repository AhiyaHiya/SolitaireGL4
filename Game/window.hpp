#pragma once

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <cstdint>
#include <expected>
#include <memory>
#include <string_view>

using error_code_t = std::int32_t;
using error_message_t = std::string;

struct GLFWwindow;

auto create_window(std::int32_t width, std::int32_t height, std::string_view title) 
    ->std::expected< std::shared_ptr< GLFWwindow >, std::pair< error_code_t, error_message_t > >;
