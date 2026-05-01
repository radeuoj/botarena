#pragma once

#include <glm/glm.hpp>

struct Bot {
	static constexpr float SIZE = 100.0f;

	glm::vec2 position;
	float rotation;

	glm::vec2 prev_position;
	float prev_rotation;

	Bot(glm::vec2 position);

	virtual void init() {}
	void before_update();
	virtual void update() {}
	void draw(float alpha);

	void go(float speed);
	void turn(float speed);
};