cmake_minimum_required(VERSION 3.30)
project(Game)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Game_SOURCE_DIR}/cmake")

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(ENet REQUIRED)

include_directories("src/" "src/imgui/" "src/imgui/backends")
include_directories(${ENET_INCLUDE_DIRS})

file(GLOB IMGUI_SOURCES
    "src/imgui/*.cpp"
    "src/imgui/backends/imgui_impl_sdl3.cpp"
    "src/imgui/backends/imgui_impl_sdlrenderer3.cpp"
    )

set(MAIN_SOURCES
    ${IMGUI_SOURCES}
    "src/aabb.cpp"
    "src/ColorRenderer.cpp"
    "src/Draw.cpp"
    "src/dynamic_object.cpp"
    "src/game_object.cpp"
    "src/platform.cpp"
    "src/Player.cpp"
    "src/PlayerInputController.cpp"
    "src/Stage.cpp"
    "src/vec2.cpp"
    "src/main.cpp"
    "src/Client.cpp"
    "src/TextureManager.cpp"
    )

add_executable(main ${MAIN_SOURCES})
target_link_libraries(main PRIVATE SDL3::SDL3 SDL3_image::SDL3_image ${ENET_LIBRARIES})

set(SERVER_SOURCES
    "src/server.cpp"
    )

add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE ${ENET_LIBRARIES})

