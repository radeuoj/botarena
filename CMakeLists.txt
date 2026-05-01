cmake_minimum_required(VERSION 3.20)
project(botarena)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs/raylib)

add_executable(botarena src/main.cpp)

target_link_libraries(botarena raylib)