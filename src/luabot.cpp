#include "luabot.h"
#include "arena.h"

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

int lua_turn_gun(lua_State* L) {
	lua_pushlightuserdata(L, (void*)LUABOT_REGISTRY_KEY);
	lua_gettable(L, LUA_REGISTRYINDEX);
	LuaBot* bot = (LuaBot*)lua_touserdata(L, -1);
	lua_pop(L, 1);

	float delta = luaL_checknumber(L, 1);
	bot->turn_gun(delta);

	return 0;
}

int lua_turn_radar(lua_State* L) {
	lua_pushlightuserdata(L, (void*)LUABOT_REGISTRY_KEY);
	lua_gettable(L, LUA_REGISTRYINDEX);
	LuaBot* bot = (LuaBot*)lua_touserdata(L, -1);
	lua_pop(L, 1);

	float delta = luaL_checknumber(L, 1);
	bot->turn_radar(delta);

	return 0;
}

int lua_shoot(lua_State* L) {
	lua_pushlightuserdata(L, (void*)LUABOT_REGISTRY_KEY);
	lua_gettable(L, LUA_REGISTRYINDEX);
	LuaBot* bot = (LuaBot*)lua_touserdata(L, -1);
	lua_pop(L, 1);

	bot->shoot();

	return 0;
}

LuaBot::LuaBot(std::string path, Arena* arena) : Bot("Matei", arena) {
	L = luaL_newstate();
	luaL_openlibs(L);

	lua_pushlightuserdata(L, (void*)LUABOT_REGISTRY_KEY);
	lua_pushlightuserdata(L, this);
	lua_settable(L, LUA_REGISTRYINDEX);
	
	lua_pushcfunction(L, lua_go);
	lua_setglobal(L, "go");

	lua_pushcfunction(L, lua_turn);
	lua_setglobal(L, "turn");

	lua_pushcfunction(L, lua_turn_gun);
	lua_setglobal(L, "turnGun");

	lua_pushcfunction(L, lua_turn_radar);
	lua_setglobal(L, "turnRadar");

	lua_pushcfunction(L, lua_shoot);
	lua_setglobal(L, "shoot");

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

	if (lua_isfunction(L, -1)) {
		lua_call(L, 0, 0);
	} else {
		lua_pop(L, 1);
	}
}

void LuaBot::update() {
	lua_pushnumber(L, position.x);
	lua_setglobal(L, "positionX");

	lua_pushnumber(L, position.y);
	lua_setglobal(L, "positionY");

	lua_pushnumber(L, rotation);
	lua_setglobal(L, "rotation");

	lua_pushnumber(L, gun_rotation);
	lua_setglobal(L, "gunRotation");

	lua_pushnumber(L, radar_rotation);
	lua_setglobal(L, "radarRotation");

	lua_pushnumber(L, health);
	lua_setglobal(L, "health");

	lua_pushnumber(L, arena->tick);
	lua_setglobal(L, "tick");

	lua_getglobal(L, "update");

	if (lua_isfunction(L, -1)) {
		lua_call(L, 0, 0);
	} else {
		lua_pop(L, 1);
	}
}

void LuaBot::on_radar_hit(glm::vec2 hit) {
	lua_getglobal(L, "onRadarHit");

	if (lua_isfunction(L, -1)) {
		lua_pushnumber(L, hit.x);
		lua_pushnumber(L, hit.y);
		lua_call(L, 2, 0);
	} else {
		lua_pop(L, 1);
	}
}