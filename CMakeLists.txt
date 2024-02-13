# Setup CMake
#set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(COMPILE_FLAGS="-g -Wall")

cmake_minimum_required(VERSION 3.20)

# Project Setup
project(carSimulator VERSION 1.0)
add_link_options("-lSDL2" "-lSDL2_image" "-lGL" "-lGLEW")
set(SOURCES 
	"src/main.cpp"
    "src/Window.hpp"
    "src/include/Vector2D.hpp"
    "src/include/Car.hpp"
)
add_executable(carSimulator)

target_sources(carSimulator PUBLIC ${SOURCES})
target_include_directories(carSimulator PUBLIC "src/" "src/include/")
target_compile_definitions(carSimulator PUBLIC DEBUG)
target_link_libraries(carSimulator PUBLIC SDL2 GL GLEW)