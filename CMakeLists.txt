
cmake_minimum_required(VERSION 3.20)

project(Gravene)

add_executable(${PROJECT_NAME}
	src/Main.cpp
	src/Renderer.cpp
	src/Text.cpp
	src/Input.cpp
	src/SceneManager.cpp
	src/GameScene.cpp
	src/MenuScene.cpp
	src/LocalPlayer.cpp
	src/AsioCompile.cpp
	src/Network.cpp
	src/UI.cpp
	deps/glad-4.3/src/glad.c
	deps/stb_image/stb_image.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
add_compile_definitions(ASIO_SEPARATE_COMPILATION _WIN32_WINNT=0x0601)

target_include_directories(${PROJECT_NAME} PUBLIC
	deps/fmt-9.1.0/include/
	deps/glfw-3.3.8/include/
	deps/glad-4.3/include
	deps/glm-0.9.9.8/
	deps/asio-1.24.0/include
	deps/stb_image/
)

target_link_directories(${PROJECT_NAME} PRIVATE
	deps/fmt-9.1.0/lib/
	deps/glfw-3.3.8/lib/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	debug fmtd
	optimized fmt
	glfw3
)

add_subdirectory(src/Server/)
