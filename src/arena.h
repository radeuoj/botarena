#pragma once

#include <vector>
#include <chrono>
#include "bot.h"

struct Arena {
	static constexpr int TPS = 20;

	std::vector<Bot*> bots;
	std::chrono::steady_clock::time_point last_update;

	void try_update_and_draw();
	void update();
	void draw(float alpha);
};