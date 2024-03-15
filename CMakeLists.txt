cmake_minimum_required(VERSION 3.28)
project(Chess)

set(CMAKE_CXX_STANDARD 23)

add_executable(Chess WIN32
        src/main.cpp
        include/Application.hpp
        src/Application.cpp
)

target_include_directories(Chess PRIVATE
        include
)

target_link_libraries(Chess d3d11 dxgi d3dcompiler)

add_custom_command(TARGET Chess POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}
)