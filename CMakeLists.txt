
cmake_minimum_required(VERSION 3.20)

project(Gravene)

option(GRAVENE_SERVER_ONLY "only compile the server and not the client" Off)

if (GRAVENE_SERVER_ONLY)
    add_subdirectory(deps/fmt/)
    add_subdirectory(deps/GulcarNetworking/)
    add_subdirectory(src/Server/)
    return()
endif()

add_executable(${PROJECT_NAME}
    src/Main.cpp
    src/Renderer.cpp
    src/Text.cpp
    src/Input.cpp
    src/SceneManager.cpp
    src/GameScene.cpp
    src/MenuScene.cpp
    src/LocalPlayer.cpp
    src/Network.cpp
    src/UI.cpp
    src/ParticleSystem.cpp
    deps/glad-4.3/src/glad.c
    deps/stb_image/stb_image.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PUBLIC
    deps/fmt/include/
    deps/glfw/include/
    deps/glad-4.3/include
    deps/glm-0.9.9.8/
    deps/stb_image/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt
    glfw
    GulcarNetworking
)

add_subdirectory(deps/fmt/)
add_subdirectory(deps/glfw/)
add_subdirectory(src/Server/)
add_subdirectory(deps/GulcarNetworking/)
