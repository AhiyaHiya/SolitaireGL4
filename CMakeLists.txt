cmake_minimum_required(VERSION 3.24)
project(Solitaire4 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3  CONFIG REQUIRED)
include(CMake/glad.cmake)

add_executable(solitaire
        Game/keyboard.cpp
        Game/keyboard.hpp
        Game/main.cpp
        Game/window.cpp
        Game/window.hpp
)

target_link_libraries(solitaire
    PRIVATE
        glfw
        glad
)
