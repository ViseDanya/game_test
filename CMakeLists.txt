cmake_minimum_required(VERSION 3.30)
project(Game)
set(CMAKE_CXX_STANDARD 14)

find_package(SDL3 REQUIRED)

include_directories("src/" "src/imgui/" "src/imgui/backends")

file(GLOB SOURCE_FILES 
    "src/*.cpp"
    "src/*.h"
    "src/imgui/*.cpp"
    "src/imgui/*.h"
    "src/imgui/backends/imgui_impl_sdl3.cpp"
    "src/imgui/backends/imgui_impl_sdl3.h"
    "src/imgui/backends/imgui_impl_sdlrenderer3.cpp"
    "src/imgui/backends/imgui_impl_sdlrenderer3.h"
    )

add_executable(main ${SOURCE_FILES})
target_link_libraries(main PRIVATE SDL3::SDL3)

