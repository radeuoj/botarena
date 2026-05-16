#pragma once

#include <vector>
#include <chrono>
#include "bot.h"
#include "luabot.h"
#include "bullet.h"

struct Arena {
	static constexpr int TPS = 20;
	static constexpr int WIDTH = 900;
	static constexpr int HEIGHT = 600;

	std::vector<std::unique_ptr<LuaBot>> lua_bots;
	std::vector<Bot*> bots;
	std::vector<Bullet> bullets;
	std::chrono::steady_clock::time_point last_update;
	bool is_paused;
	bool draw_trail;
	int tick;

	Arena();

	void init();
	void try_update_and_draw();
	void update();
	void draw(float alpha);
	void add_lua_bot(const std::string& path);
	void clean_bullets();
	void handle_bullet_hits();
	void clean_dead_bots();

	void resume();
	void pause();
	void start();
};