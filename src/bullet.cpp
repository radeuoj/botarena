#include "bullet.h"
#include <raylib.h>

Bullet::Bullet(glm::vec2 position, glm::vec2 direction, Bot* owner) {
	this->position = prev_position = screen_position = position;
	this->direction = direction;
	this->owner = owner;
	dead = false;
}

void Bullet::before_update() {
	prev_position = position;
}

void Bullet::update() {
	position += direction * SPEED;
}

void Bullet::before_draw(float alpha) {
	screen_position = glm::mix(prev_position, position, alpha);
}

void Bullet::draw() {
	DrawCircle(screen_position.x, screen_position.y, RADIUS, WHITE);
}
