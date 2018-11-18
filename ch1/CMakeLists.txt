cmake_minimum_required(VERSION 3.12)
project(sdlTake2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(sdlStuff main.cpp game.h game.cpp)
target_link_libraries(sdlStuff SDL2)
