#pragma once

#include <glm/glm.hpp>

struct Bot;

struct Bullet {
	static constexpr float RADIUS = 5.0f;
	static constexpr float SPEED = 30.0f;

	glm::vec2 position;
	glm::vec2 prev_position;
	glm::vec2 screen_position;

	glm::vec2 direction;
	Bot* owner;
	bool dead;

	Bullet(glm::vec2 position, glm::vec2 direction, Bot* owner);
	void before_update();
	void update();
	void before_draw(float alpha);
	void draw();
};