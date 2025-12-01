cmake_minimum_required(VERSION 3.24)
project(Solitaire4 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------- Dependencies ---------------------
find_package(glfw3  CONFIG REQUIRED)

# Download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Now download the other headers/libraries
include(CMake/glad.cmake)
include(CMake/stb.cmake)
CPMAddPackage("gh:nlohmann/json@3.11.3")

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
        stb

        nlohmann_json::nlohmann_json
)
)
