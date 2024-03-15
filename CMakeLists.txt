cmake_minimum_required(VERSION 3.28)
project(Chess)

include(FetchContent)

FetchContent_Declare(
        DirectXTK
        GIT_REPOSITORY https://github.com/microsoft/DirectXTK.git
        GIT_TAG 642825891c41b1e7e4d6f934171f45b2645b713e
)

FetchContent_MakeAvailable(DirectXTK)

set(CMAKE_CXX_STANDARD 23)

add_executable(Chess WIN32
        src/main.cpp
        include/Application.hpp
        src/Application.cpp
)

target_include_directories(Chess PRIVATE
        include
        ${DirectXTK_SOURCE_DIR}/Inc
)

target_link_libraries(Chess d3d11 dxgi d3dcompiler DirectXTK)

add_custom_command(TARGET Chess POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}
)