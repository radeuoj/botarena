#pragma once

#include "bot.h"
#include <string>

extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

struct LuaBot : Bot {
	lua_State* L;

	LuaBot(std::string path);
	~LuaBot();

	void init();
	void update();
};