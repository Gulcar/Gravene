
cmake_minimum_required(VERSION 3.20)

project(GraveneServer)

add_executable(${PROJECT_NAME}
    ServerMain.cpp
    Server.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
add_compile_definitions(ASIO_SEPARATE_COMPILATION _WIN32_WINNT=0x0601)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ../../deps/fmt/include/
    ../../deps/glm-0.9.9.8/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt
    GulcarNetworking
)
