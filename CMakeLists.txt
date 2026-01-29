cmake_minimum_required(VERSION 3.20)
project(engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/inc)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_compile_definitions(GLEW_STATIC)

add_executable(engine
    src/main.cpp

    src/EGL/buffer.cpp
    src/EGL/debug.cpp
    src/EGL/program.cpp
    src/EGL/shader.cpp
    src/EGL/windowContext.cpp
)

add_compile_definitions(DEBUG_BUILD) # define DEBUG_BUILD for GL_CALL error (slows down the program in release)

target_link_libraries(engine glfw3 opengl32 glew32s)