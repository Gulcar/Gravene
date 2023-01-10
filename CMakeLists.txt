
cmake_minimum_required(VERSION 3.20)

project(Gravene)

add_executable(${PROJECT_NAME}
	src/Main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
	deps/fmt-9.1.0/include/
	deps/glfw-3.3.8/include/
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