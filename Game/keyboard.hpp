#pragma once

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <memory>

/// Callback function for keyboard input handling
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
