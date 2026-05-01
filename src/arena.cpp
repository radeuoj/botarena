#include "arena.h"

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
	for (Bot* bot : bots) {
		bot->before_update();
		bot->update();
	}
}

void Arena::draw(float alpha) {
	for (Bot* bot : bots) {
		bot->draw(alpha);
	}
}
