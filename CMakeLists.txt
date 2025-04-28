cmake_minimum_required(VERSION 3.25)
project(Game)
set(CMAKE_CXX_STANDARD 14)
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Game_SOURCE_DIR}/cmake")

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(ENet REQUIRED)
find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

add_executable(main)

target_include_directories(main PRIVATE
    "src" 
    "src/Components" 
    "src/Networking" 
    "src/proto"
    "src/Screen"
    "src/Systems" 
    "src/imgui" 
    "src/imgui/backends"
    "src/imgui/misc/cpp"
    ${ENET_INCLUDE_DIRS}
    ${Protobuf_INCLUDE_DIRS})

file(GLOB IMGUI_SOURCES
    "src/imgui/*.cpp"
    "src/imgui/misc/cpp/*.cpp"
    "src/imgui/backends/imgui_impl_sdl3.cpp"
    "src/imgui/backends/imgui_impl_sdlrenderer3.cpp")

target_sources(main PRIVATE
    ${IMGUI_SOURCES}
    "src/SDLRenderUtils.cpp"
    "src/Networking/ENetClient.cpp"
    "src/Networking/ENetServer.cpp"
    "src/Networking/Message.cpp"
    # "src/GameServer.cpp"
    "src/GameClient.cpp"
    # "src/LocalGame.cpp"
    "src/Constants.cpp"
    "src/Systems/AnimationSystem.cpp"
    "src/Systems/CollisionSystem.cpp"
    "src/Systems/InputSystem.cpp"
    "src/Systems/PhysicsSystem.cpp"
    "src/Systems/StateSystem.cpp"
    "src/Systems/RenderSystem.cpp"
    "src/Entity.cpp"
    "src/Direction.cpp"
    "src/Scene.cpp"
    "src/SDLApp.cpp"
    "src/SDLRenderUtils.cpp"
    "src/SDLTextureManager.cpp"
    "src/Screen/LocalGameScreen.cpp"
    "src/Screen/ClientConnectToServerScreen.cpp"
    "src/Screen/ClientGameScreen.cpp"
    "src/Screen/MainMenuScreen.cpp"
    "src/Screen/Screen.cpp"
    "src/main.cpp")

set(PROTO_DIR "src/proto")
set(PROTO_FILES "${PROTO_DIR}/game.proto")

protobuf_generate(TARGET main 
    PROTOS ${PROTO_FILES}
    PROTOC_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_DIR}
    IMPORT_DIRS ${PROTO_DIR})

option(HEADLESS "Headless server" OFF)

if(HEADLESS)
    add_compile_definitions(HEADLESS)
endif()

target_link_libraries(main PRIVATE 
    SDL3::SDL3
    SDL3_image::SDL3_image
    EnTT::EnTT
    glm::glm
    protobuf::libprotobuf
    ${ENET_LIBRARIES})

set(RESOURCE_FILES 
    "assets/ceiling.png"
    "assets/conveyor_left.png"
    "assets/conveyor_right.png"
    "assets/fake.png"
    "assets/normal.png"
    "assets/player.png"
    "assets/spikes.png"
    "assets/trampoline.png"
    "assets/wall.png")

target_sources(main PRIVATE ${RESOURCE_FILES})

if(APPLE)
    set_target_properties(main PROPERTIES
        MACOSX_BUNDLE TRUE)
    
    set_source_files_properties(${RESOURCE_FILES} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources/assets")

    set(CMAKE_INSTALL_RPATH "@loader_path")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
    install(SCRIPT cmake/bundle.cmake)
endif()