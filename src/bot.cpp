#include "bot.h"
#include <raylib.h>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <iostream>

Bot::Bot(std::string name) : name(name) {
	position = prev_position = screen_position = { 0, 0 };
    rotation = prev_rotation = screen_rotation = 0;
    gun_rotation = prev_gun_rotation = screen_gun_rotation = 0;
    radar_rotation = prev_radar_rotation = screen_radar_rotation = 0;
    health = 100.0f;
}

void Bot::before_update() {
	prev_position = position;
	prev_rotation = rotation;
    prev_gun_rotation = gun_rotation;
    prev_radar_rotation = radar_rotation;
}

Vector2 raylib_vec_from_glm_vec(glm::vec2 v) {
    return { v.x, v.y };
}

void Bot::before_draw(float alpha) {
    screen_position = glm::mix(prev_position, position, alpha);
    screen_rotation = glm::mix(prev_rotation, rotation, alpha);
    screen_gun_rotation = glm::mix(prev_gun_rotation, gun_rotation, alpha);
    screen_radar_rotation = glm::mix(prev_radar_rotation, radar_rotation, alpha);
}

void draw_direction_triangle(glm::vec2 position, float rotation) {
    glm::vec2 dir = { cos(rotation), sin(rotation) };
    glm::vec2 a = position + dir * Bot::SIZE / 2.0f;

    rotation += 2.0f * PI / 3.0f;
    dir = { cos(rotation), sin(rotation) };
    glm::vec2 b = position + dir * Bot::SIZE / 10.0f;

    rotation += 2.0f * PI / 3.0f;
    dir = { cos(rotation), sin(rotation) };
    glm::vec2 c = position + dir * Bot::SIZE / 10.0f;

    DrawTriangle(
        raylib_vec_from_glm_vec(c),
        raylib_vec_from_glm_vec(b),
        raylib_vec_from_glm_vec(a),
        YELLOW
    );
}

void draw_gun_triangle(glm::vec2 position, float rotation) {
    glm::vec2 dir = { cos(rotation), sin(rotation) };
    glm::vec2 a = position + dir * Bot::SIZE / 10.0f;

    rotation += 2.0f * PI / 3.0f;
    dir = { cos(rotation), sin(rotation) };
    glm::vec2 b = position + dir * Bot::SIZE / 10.0f;

    rotation += 2.0f * PI / 3.0f;
    dir = { cos(rotation), sin(rotation) };
    glm::vec2 c = position + dir * Bot::SIZE / 10.0f;

    DrawTriangle(
        raylib_vec_from_glm_vec(c),
        raylib_vec_from_glm_vec(b),
        raylib_vec_from_glm_vec(a),
        BLUE
    );
}

void Bot::draw() {
    DrawCircle(screen_position.x, screen_position.y, SIZE / 2, RED);
    draw_direction_triangle(screen_position, screen_rotation);
    draw_gun_triangle(
        screen_position + glm::vec2(cos(screen_rotation + screen_gun_rotation), sin(screen_rotation + screen_gun_rotation)) * SIZE / 2.0f,
        screen_rotation + screen_gun_rotation
    );
}

void Bot::draw_trail() {
    trail.push_front({ screen_position, screen_rotation });
    if (trail.size() > MAX_TRAIL_SIZE) {
        trail.pop_back();
    }

    for (size_t i = 0; i < trail.size(); i++) {
        // Calculăm transparența: punctele mai vechi sunt mai transparente
        // i = 0 e cel mai nou, i = trail.size()-1 e cel mai vechi
        float trailAlpha = 1.0f - ((float)i / trail.size());

        // Culoarea robotului (de ex RED) cu transparență variabilă
        Color culoare = GetColor(0x2C3E50FF);
        Color color = Fade(culoare, trailAlpha * 0.2f); // 0.5f pentru a fi mai discret

        DrawCircle(trail[i].pos.x, trail[i].pos.y, SIZE / 2, color);
    }
}

void Bot::draw_name() {
    const char* text = name.c_str();
    int font_size = 20;
    int x = glm::round(screen_position.x - MeasureText(text, font_size) / 2.0f);
    int y = glm::round(screen_position.y - 0.75f * SIZE);

    DrawText(text, x, y, font_size, WHITE);
}

void Bot::go(float delta) {
    delta = glm::clamp(delta, -10.0f, 10.0f);

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

void Bot::turn_gun(float delta) {
    delta = glm::clamp(delta, -PI / 10, PI / 10);
    gun_rotation += delta;
}