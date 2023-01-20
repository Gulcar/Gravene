
cmake_minimum_required(VERSION 3.20)

project(GraveneServer)

add_executable(${PROJECT_NAME}
	ServerMain.cpp
	AsioCompile.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
add_compile_definitions(ASIO_SEPARATE_COMPILATION _WIN32_WINNT=0x0601)

target_include_directories(${PROJECT_NAME} PUBLIC
	../../deps/fmt-9.1.0/include/
	../../deps/glm-0.9.9.8/
	../../deps/asio-1.24.0/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
	../../deps/fmt-9.1.0/lib/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	debug fmtd
	optimized fmt
)
