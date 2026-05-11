add_subdirectory(raylib)
add_subdirectory(glm)

add_library(lua
	lua/lapi.c
	lua/lauxlib.c
	lua/lbaselib.c
	lua/lcode.c
	lua/lcorolib.c
	lua/lctype.c
	lua/ldblib.c
	lua/ldebug.c
	lua/ldo.c
	lua/ldump.c
	lua/lfunc.c
	lua/lgc.c
	lua/linit.c
	lua/liolib.c
	lua/llex.c
	lua/lmathlib.c
	lua/lmem.c
	lua/loadlib.c
	lua/lobject.c
	lua/lopcodes.c
	lua/loslib.c
	lua/lparser.c
	lua/lstate.c
	lua/lstring.c
	lua/lstrlib.c
	lua/ltable.c
	lua/ltablib.c
	lua/ltests.c
	lua/ltm.c
	lua/lua.c
	lua/lundump.c
	lua/lutf8lib.c
	lua/lvm.c
	lua/lzio.c
)

target_include_directories(lua PUBLIC lua)

add_library(imgui
	imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/backends/imgui_impl_glfw.cpp
	imgui/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(imgui raylib glfw)
target_include_directories(imgui PUBLIC imgui PUBLIC rlimgui)