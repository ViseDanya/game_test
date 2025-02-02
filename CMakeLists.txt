cmake_minimum_required(VERSION 3.30)
project(Game)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Game_SOURCE_DIR}/cmake")

find_package(SDL3 REQUIRED)
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
    "src/color_renderable.cpp"
    "src/dynamic_object.cpp"
    "src/game_object.cpp"
    "src/platform.cpp"
    "src/player.cpp"
    "src/Stage.cpp"
    "src/vec2.cpp"
    "src/main.cpp"
    )

add_executable(main ${MAIN_SOURCES})
target_link_libraries(main PRIVATE SDL3::SDL3 ${ENET_LIBRARIES})

set(SERVER_SOURCES
    "src/server.cpp"
    )

add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE ${ENET_LIBRARIES})

set(CLIENT_SOURCES
    "src/Client.cpp"
    )

add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client PRIVATE ${ENET_LIBRARIES})

