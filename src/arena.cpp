#include "arena.h"
#include <iostream>

Arena::Arena() : is_paused(true) {

}

void Arena::init() {
	for (Bot* bot : bots) {
		bot->init();
		std::cout << "Initialized " << bot->name << '\n';
	}
}

void Arena::try_update_and_draw() {
	auto now = std::chrono::steady_clock::now();
	auto delta = std::chrono::duration<double>(now - last_update);

	if (delta.count() >= 1.0 / TPS) {
		last_update = now;
		update();
		draw(delta.count() / (1.0 / TPS) - 1.0);
	} else {
		draw(delta.count() / (1.0 / TPS));
	}
}

void Arena::update() {
	if (is_paused) return;

	for (Bot* bot : bots) {
		bot->before_update();
		bot->update();
	}
}

void Arena::draw(float alpha) {
	for (Bot* bot : bots) {
		bot->draw_trail();
	}

	for (Bot* bot : bots) {
		bot->draw(alpha);
	}
}

void Arena::add_lua_bot(const std::string& path) {
	if (lua_bots.contains(path)) {
		bots.erase(std::find(bots.begin(), bots.end(), lua_bots[path].get()));
	}

	lua_bots[path] = std::make_unique<LuaBot>(path);
	bots.push_back(lua_bots[path].get());
}

void Arena::resume() {
	is_paused = false;
}

void Arena::pause() {
	is_paused = true;

	for (Bot* bot : bots) {
		bot->prev_position = bot->position;
		bot->prev_rotation = bot->rotation;
	}
}

void Arena::start() {
	init();
	resume();
}
