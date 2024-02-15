# Setup CMake
#set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_COLOR_DIAGNOSTICS ON)

cmake_minimum_required(VERSION 3.20)

# Project Setup
#C:/Users/JTK6759/Documents/apps/msys2/ucrt64/bin/x86_64-w64-mingw32-g++.exe
project(carSimulator VERSION 1.0)


set(SOURCES 
"src/main.cpp"
"src/Window.hpp"
"src/include/FileHandler.hpp"
"src/include/Vector2D.hpp"
"src/include/Car.hpp"
)

if(WIN32)
	add_link_options("-lmingw32" "-lSDL2main" "-lSDL2" "-mwindows" "-lfreeglut" "-lglew32" "-lopengl32")
	set(CMAKE_CXX_COMPILER "C:/Users/JTK6759/Documents/apps/msys2/ucrt64/bin/g++.exe")
	set(COMPILE_FLAGS="-g -Wall -mwindows -Dmain=SDL_main")
	add_executable(carSimulator WIN32)
else()
	add_link_options("-lSDL2" "-lGLEW" "-GL")
	set(CMAKE_CXX_COMPILER "/usr/bin/g++")
	set(COMPILE_FLAGS="-g -Wall")
	add_executable(carSimulator)
endif()

target_sources(carSimulator PUBLIC ${SOURCES})
target_include_directories(carSimulator PUBLIC "src/" "src/include/")

if(WIN32)
	target_compile_definitions(carSimulator PUBLIC DEBUG WIN)
	target_link_libraries(carSimulator PUBLIC SDL2 opengl32 glew32 freeglut)
else()
	target_compile_definitions(carSimulator PUBLIC DEBUG)
	target_link_libraries(carSimulator PUBLIC SDL2 GL GLEW)
endif()