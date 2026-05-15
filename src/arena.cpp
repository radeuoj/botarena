#include "arena.h"
#include <iostream>
#include <raylib.h>

Arena::Arena() : is_paused(true), draw_trail(true), tick(0) {

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

	tick++;
}

void Arena::draw(float alpha) {
	for (Bot* bot : bots) {
		bot->before_draw(alpha);
	}

	if (draw_trail) {
		for (Bot* bot : bots) {
			bot->draw_trail();
		}
	}

	for (Bot* bot : bots) {
		bot->draw();
	}

	for (Bot* bot : bots) {
		bot->draw_name();
	}
}

glm::vec2 get_random_position() {
	float x = (float)rand() / RAND_MAX * Arena::WIDTH;
	float y = (float)rand() / RAND_MAX * Arena::HEIGHT;
	return { x, y };
}

float get_random_rotation() {
	return (float)rand() / RAND_MAX * 2 * PI;
}

void Arena::add_lua_bot(const std::string& path) {
	auto bot = std::make_unique<LuaBot>(path);
	bot->position = bot->prev_position = get_random_position();
	bot->rotation = bot->prev_rotation = get_random_rotation();
	bots.push_back(bot.get());
	lua_bots.push_back(std::move(bot));
}

void Arena::resume() {
	is_paused = false;
}

void Arena::pause() {
	is_paused = true;

	for (Bot* bot : bots) {
		bot->prev_position = bot->position;
		bot->prev_rotation = bot->rotation;
		bot->prev_gun_rotation = bot->gun_rotation;
		bot->prev_radar_rotation = bot->radar_rotation;
	}
}

void Arena::start() {
	init();
	resume();
}
