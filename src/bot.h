#pragma once

#include <glm/glm.hpp>
#include <string>
#include <deque>
#include <vector>
#include "bullet.h"
#include <chrono>

struct TrailPoint {
	glm::vec2 pos;
	float rotation;
};

struct Arena;

struct Bot {
	static constexpr float SIZE = 100.0f;
	static constexpr size_t MAX_TRAIL_SIZE = 50;
	static constexpr int SHOOT_DELTA_TICKS = 20;

	std::string name;
	float health;

	glm::vec2 position;
	float rotation;
	float gun_rotation; // relative to rotation
	float radar_rotation; // relative to rotation

	glm::vec2 prev_position;
	float prev_rotation;
	float prev_gun_rotation;
	float prev_radar_rotation;

	glm::vec2 screen_position;
	float screen_rotation;
	float screen_gun_rotation;
	float screen_radar_rotation;

	std::deque<TrailPoint> trail;
	int last_shoot_tick;

	Arena* arena;

	Bot(std::string name, Arena* arena);

	virtual void init() {}
	void before_update();
	virtual void update() {}
	void before_draw(float alpha);
	void draw();
	void draw_trail();
	void draw_name();

	void go(float delta);
	void turn(float delta);
	void turn_gun(float delta);
	void shoot();
};