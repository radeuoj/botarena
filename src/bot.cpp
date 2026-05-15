#include "bot.h"
#include <raylib.h>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <iostream>

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

void Bot::draw_trail() {
    for (size_t i = 0; i < trail.size(); i++) {
        // Calculăm transparența: punctele mai vechi sunt mai transparente
        // i = 0 e cel mai nou, i = trail.size()-1 e cel mai vechi
        float trailAlpha = 1.0f - ((float)i / trail.size());

        // Culoarea robotului (de ex RED) cu transparență variabilă
        Color culoare = GetColor(0x2C3E50FF);
        Color color = Fade(culoare, trailAlpha * 0.2f); // 0.5f pentru a fi mai discret

        Rectangle trailRect = {
            .x = trail[i].pos.x,
            .y = trail[i].pos.y,
            .width = SIZE,
            .height = SIZE,
        };

        DrawRectanglePro(trailRect, { SIZE / 2, SIZE / 2 }, glm::degrees(trail[i].rotation), color);
    }
}

void Bot::draw_name(float alpha) {
    glm::vec2 screen_position = glm::mix(prev_position, position, alpha);
    float screen_rotation = glm::mix(prev_rotation, rotation, alpha);

    const char* text = name.c_str();
    int font_size = 20;
    int x = glm::round(screen_position.x - MeasureText(text, font_size) / 2.0f);
    int y = glm::round(screen_position.y - 0.75f * SIZE);

    DrawText(text, x, y, font_size, WHITE);
}

void Bot::go(float delta) {
    delta = glm::clamp(delta, -10.0f, 10.0f);

    // Înainte de a schimba poziția, o salvăm pentru trail
    trail.push_front({ position, rotation });
    if (trail.size() > MAX_TRAIL_SIZE) {
        trail.pop_back();
    }

    // Logica ta existentă de mișcare și coliziune cu pereții
    glm::vec2 direction(glm::cos(rotation), glm::sin(rotation));
    position += direction * delta;

    // Calculăm limitele ținând cont de centrul robotului
    float halfSize = SIZE / 2.0f;

    // Constrângem poziția X între margini
    // GetScreenWidth() returnează 900 (valoarea setată în main)
    if (position.x < halfSize) {
        position.x = halfSize;
    }
    else if (position.x > GetScreenWidth() - halfSize) {
        position.x = GetScreenWidth() - halfSize;
    }

    // Constrângem poziția Y între margini
    // GetScreenHeight() returnează 600
    if (position.y < halfSize) {
        position.y = halfSize;
    }
    else if (position.y > GetScreenHeight() - halfSize) {
        position.y = GetScreenHeight() - halfSize;
    }
}

void Bot::turn(float delta) {
    delta = glm::clamp(delta, -PI / 20, PI / 20);

	rotation += delta;
}