#pragma once

#include "bot.h"
#include <string>

extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

struct LuaBot : Bot {
	lua_State* L;

	LuaBot(std::string path, Arena* arena);
	~LuaBot();

	void init();
	void update();
	void on_radar_hit(glm::vec2 hit);
};