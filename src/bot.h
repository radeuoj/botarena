#pragma once

#include <glm/glm.hpp>
#include <string>
#include <deque>
#include <vector>

struct TrailPoint {
	glm::vec2 pos;
	float rotation;
};

struct Bot {
	static constexpr float SIZE = 100.0f;

	std::string name;

	glm::vec2 position;
	float rotation;

	glm::vec2 prev_position;
	float prev_rotation;

	std::deque<TrailPoint> trail;
	const size_t MAX_TRAIL_SIZE = 15;

	Bot(std::string name);

	virtual void init() {}
	void before_update();
	virtual void update() {}
	void draw(float alpha);

	void go(float speed);
	void turn(float speed);
};