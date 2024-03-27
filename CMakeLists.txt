# Setup CMake
#set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_COLOR_DIAGNOSTICS ON)

cmake_minimum_required(VERSION 3.20)

# Project Setup
project(carSimulator VERSION 1.0)

set(SOURCES 
	"src/main.cpp"
	"src/Window.hpp"
	"src/include/Mesh.hpp"
	"src/include/Model.hpp"
	"src/include/FileHandler.hpp"
	"src/include/Vector2D.hpp"
	"src/include/Car.hpp"
	"src/include/shader/Shader.hpp"
)

if(WIN32)
	set(CMAKE_CXX_COMPILER "C:/Users/JTK6759/Documents/apps/msys2/ucrt64/bin/g++.exe")
	set(COMPILE_FLAGS="-g -Wall -Dmain=SDL_main")

	add_link_options("-lmingw32" "-lSDL2" "-lSDL2main" "-lSOIL" "-lfreeglut" "-lglew32" "-lopengl32" "-lassimp")
	add_executable(carSimulator)

	target_compile_definitions(carSimulator PUBLIC DEBUG)
	target_link_libraries(carSimulator PUBLIC SDL2 SOIL opengl32 glew32 freeglut assimp)
else()
	set(CMAKE_CXX_COMPILER "/usr/bin/g++")
	set(COMPILE_FLAGS="-g -Wall")
	
	add_link_options("-lSDL2" "-lGLEW" "-lGL" "-lSOIL" "-lassimp")
	
	add_executable(carSimulator)
	
	target_compile_definitions(carSimulator PUBLIC DEBUG)
	target_link_libraries(carSimulator PUBLIC SDL2 GL GLEW SOIL assimp)
endif()

target_sources(carSimulator PUBLIC ${SOURCES})
target_include_directories(carSimulator PUBLIC "src/" "src/include/")