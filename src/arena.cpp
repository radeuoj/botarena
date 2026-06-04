#include "arena.h"
#include <iostream>
#include <raylib.h>

Arena::Arena() {
	tps = 20;
	is_paused = true;
	draw_trail = false;
	draw_radar = true;
	tick = 0;
	background_texture = LoadTexture("background.png");
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

	if (delta.count() >= 1.0 / tps) {
		last_update = now;
		update();
		draw(delta.count() / (1.0 / tps) - 1.0);
	} else {
		draw(delta.count() / (1.0 / tps));
	}
}

void Arena::update() {
	if (is_paused) return;

	for (Bullet& bullet : bullets) {
		bullet.before_update();
		bullet.update();
	}

	for (Bot* bot : bots) {
		bot->before_update();
		bot->update();
	}

	handle_bullet_hits();
	clean_bullets();
	clean_dead_bots();
	update_radars();

	tick++;
}

void Arena::handle_bullet_hits() {
	for (Bullet& bullet : bullets) {
		for (Bot* bot : bots) {
			if (bot == bullet.owner) {
				continue;
			}

			if (glm::distance(bullet.position, bot->position) <= Bullet::RADIUS + Bot::SIZE / 2.0f) {
				bot->health -= 10.0f;
				bullet.dead = true;
				break;
			}
		}
	}
}

void Arena::clean_bullets() {
	for (Bullet& bullet : bullets) {
		if (bullet.position.x <= -Bullet::RADIUS ||
			bullet.position.x >= Arena::WIDTH + Bullet::RADIUS ||
			bullet.position.y <= -Bullet::RADIUS ||
			bullet.position.y >= Arena::HEIGHT + Bullet::RADIUS) {

			bullet.dead = true;
		}
	}

	bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& bullet) {
		return bullet.dead;
	}), bullets.end());
}

void Arena::clean_dead_bots() {
	bots.erase(std::remove_if(bots.begin(), bots.end(), [](Bot* bot) {
		return bot->health <= 0.0f;
	}), bots.end());
}

void Arena::update_radars() {
	for (Bot* bot : bots) {
		glm::vec2 direction = { cos(bot->rotation + bot->radar_rotation), sin(bot->rotation + bot->radar_rotation) };
		Bot* best = nullptr;
		float best_dist = INFINITY;

		for (Bot* target : bots) {
			if (target == bot) continue;
			glm::vec2 ap = target->position - bot->position;
			float dot = glm::dot(ap, direction);
            float cross = ap.x * direction.y - ap.y * direction.x;

			if (dot >= 0.0f && abs(cross) <= Bot::SIZE / 2.0f) {
				float dist = glm::distance(bot->position, target->position);

				if (dist < best_dist) {
					best = target;
					best_dist = dist;
				}
			}
		}

		bot->radar_hit = (best != nullptr);
		if (best != nullptr) {
			bot->on_radar_hit(best->position);
		}
	}
}

void Arena::draw(float alpha) {
	if (is_paused) alpha = 1.0f;

	draw_background();

	for (Bot* bot : bots) {
		bot->before_draw(alpha);
	}

	for (Bullet& bullet : bullets) {
		bullet.before_draw(alpha);
	}

	if (draw_trail) {
		for (Bot* bot : bots) {
			bot->draw_trail();
		}
	}

	if (draw_radar) {
		for (Bot* bot : bots) {
			bot->draw_radar();
		}
	}

	for (Bot* bot : bots) {
		bot->draw();
	}

	for (Bullet& bullet : bullets) {
		bullet.draw();
	}

	for (Bot* bot : bots) {
		bot->draw_name();
	}
}

void Arena::draw_background() {
	Rectangle src = { 0, 0, WIDTH, HEIGHT };
	Rectangle dest = { 0, 0, WIDTH, HEIGHT };

	DrawTexturePro(background_texture, src, dest, { 0, 0 }, 0, WHITE);
}

glm::vec2 get_random_position() {
	float x = (float)rand() / RAND_MAX * (Arena::WIDTH - Bot::SIZE) + Bot::SIZE / 2.0f;
	float y = (float)rand() / RAND_MAX * (Arena::HEIGHT - Bot::SIZE) + Bot::SIZE / 2.0f;
	return { x, y };
}

float get_random_rotation() {
	return (float)rand() / RAND_MAX * 2 * PI;
}

void Arena::add_lua_bot(const std::string& path) {
	auto bot = std::make_unique<LuaBot>(path, this);
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
}

void Arena::start() {
	init();
	resume();
}
