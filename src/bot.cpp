#include "bot.h"
#include <raylib.h>

Bot::Bot(std::string name) : name(name) {
	this->position = this->prev_position = { 0, 0 };
	this->rotation = this->prev_rotation = 0;
}

void Bot::before_update() {
	prev_position = position;
	prev_rotation = rotation;
}

void Bot::draw(float alpha) {
	glm::vec2 screen_position = glm::mix(prev_position, position, alpha);
	float screen_rotation = glm::mix(prev_rotation, rotation, alpha);

	Rectangle rect = {
		.x = screen_position.x,
		.y = screen_position.y,
		.width = SIZE,
		.height = SIZE,
	};

	DrawRectanglePro(rect, { SIZE / 2, SIZE / 2 }, glm::degrees(screen_rotation), RED);
}


void Bot::go(float delta) {
	glm::vec2 direction(glm::cos(rotation), glm::sin(rotation));
	position += direction * delta;
}

void Bot::turn(float delta) {
	rotation += delta;
}