#ifndef _GAME_KEYBOARD_HPP__
#define _GAME_KEYBOARD_HPP__

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <memory>

/// Callback function for keyboard input handling
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

#endif // _GAME_KEYBOARD_HPP__
