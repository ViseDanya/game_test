cmake_minimum_required(VERSION 3.25)
project(Game)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Game_SOURCE_DIR}/cmake")

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(ENet REQUIRED)
find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

set(PROTO_FILES
    "src/proto/game.proto"
)

include_directories("src/" "src/Components/" "src/Networking/" "src/Systems/" "src/imgui/" "src/imgui/backends/")
include_directories(${ENET_INCLUDE_DIRS} ${Protobuf_INCLUDE_DIRS})

file(GLOB IMGUI_SOURCES
    "src/imgui/*.cpp"
    "src/imgui/backends/imgui_impl_sdl3.cpp"
    "src/imgui/backends/imgui_impl_sdlrenderer3.cpp"
    )

set(MAIN_SOURCES
    ${IMGUI_SOURCES}
    "src/Renderer.cpp"
    "src/Networking/ENetClient.cpp"
    "src/Networking/ENetServer.cpp"
    "src/Networking/Message.cpp"
    "src/GameServer.cpp"
    "src/GameClient.cpp"
    "src/LocalGame.cpp"
    "src/TextureManager.cpp"
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
    "src/main.cpp"
    )

add_executable(main ${MAIN_SOURCES})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)

protobuf_generate(TARGET main
PROTOS ${PROTO_FILES}
PROTOC_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated"
IMPORT_DIRS "src/proto"
)

option(HEADLESS "Headless server" OFF)

if(HEADLESS)
    add_compile_definitions(HEADLESS)
endif()

target_include_directories(main PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/generated" "${CMAKE_CURRENT_BINARY_DIR}/generated/src/proto")
target_link_libraries(main PRIVATE SDL3::SDL3 SDL3_image::SDL3_image EnTT::EnTT glm::glm protobuf::libprotobuf ${ENET_LIBRARIES})

if(APPLE)

    # Set the RPATH to the Frameworks directory inside the app bundle
    set_target_properties(main PROPERTIES
        INSTALL_RPATH "@executable_path/../Frameworks"
    )

    set(RESOURCE_PATH "${CMAKE_BINARY_DIR}/Game.app/Contents/Resources/")

    target_compile_definitions(main PRIVATE RESOURCE_PATH="${RESOURCE_PATH}")

    # Install the executable and the libraries into the app bundle structure
    install(TARGETS main
        DESTINATION "${CMAKE_BINARY_DIR}/Game.app/Contents/MacOS"
    )

    install(FILES   $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE:SDL3_image::SDL3_image> 
                    $<TARGET_FILE:glm::glm> $<TARGET_FILE:protobuf::libprotobuf> ${ENET_LIBRARIES} 

        DESTINATION "${CMAKE_BINARY_DIR}/Game.app/Contents/Frameworks"
    )

    # Specify the location of resources
    set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/assets")

    # Install resources into the Resources directory of the app bundle
    install(DIRECTORY ${RESOURCE_DIR}/
        DESTINATION "${CMAKE_BINARY_DIR}/Game.app/Contents/Resources"
        FILES_MATCHING PATTERN "*.png"  # or "*.jpg", "*.wav" depending on the resources you want
    )

    install(FILES ${CMAKE_SOURCE_DIR}/Info.plist
    DESTINATION ${CMAKE_BINARY_DIR}/Game.app/Contents
)
elseif(UNIX)
    # For Linux or other Unix-based systems, resources might be in a 'resources' folder
    set(RESOURCE_PATH ${RESOURCE_DIR}/)
endif()