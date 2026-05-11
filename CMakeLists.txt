cmake_minimum_required(VERSION 3.20)
project(botarena)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs)

add_executable(botarena 
	"src/main.cpp" 
	"src/bot.h" 
	"src/bot.cpp" 
	"src/arena.h" 
	"src/arena.cpp"
	"src/luabot.h"
	"src/luabot.cpp"
)

target_link_libraries(botarena raylib glm lua imgui glfw)