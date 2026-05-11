#include "luabot.h"

static const char* LUABOT_REGISTRY_KEY = "BOT_ARENA";

int lua_go(lua_State* L) {
	lua_pushlightuserdata(L, (void*)LUABOT_REGISTRY_KEY);
	lua_gettable(L, LUA_REGISTRYINDEX);
	LuaBot* bot = (LuaBot*)lua_touserdata(L, -1);
	lua_pop(L, 1);

	float delta = luaL_checknumber(L, 1);
	bot->go(delta);

	return 0;
}

int lua_turn(lua_State* L) {
	lua_pushlightuserdata(L, (void*)LUABOT_REGISTRY_KEY);
	lua_gettable(L, LUA_REGISTRYINDEX);
	LuaBot* bot = (LuaBot*)lua_touserdata(L, -1);
	lua_pop(L, 1);

	float delta = luaL_checknumber(L, 1);
	bot->turn(delta);

	return 0;
}

LuaBot::LuaBot(std::string path) : Bot("Matei") {
	L = luaL_newstate();
	luaL_openlibs(L);

	lua_pushlightuserdata(L, (void*)LUABOT_REGISTRY_KEY);
	lua_pushlightuserdata(L, this);
	lua_settable(L, LUA_REGISTRYINDEX);
	
	lua_pushcfunction(L, lua_go);
	lua_setglobal(L, "go");

	lua_pushcfunction(L, lua_turn);
	lua_setglobal(L, "turn");

	luaL_dofile(L, path.c_str());

	lua_getglobal(L, "name");
	if (lua_isstring(L, -1)) {
		name = lua_tostring(L, -1);
	}
	lua_pop(L, 1);
}

LuaBot::~LuaBot() {
	lua_close(L);
}

void LuaBot::init() {
	lua_getglobal(L, "init");
	lua_call(L, 0, 0);
}

void LuaBot::update() {
	lua_getglobal(L, "update");
	lua_call(L, 0, 0);
}