#include "bot.h"
#include <raylib.h>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <iostream>
#include "arena.h"

// Declarăm variabile globale statice pentru texturi în interiorul acestui fișier
static Texture2D body_texture;
static Texture2D gun_texture;
static bool textures_loaded = false;

Bot::Bot(std::string name, Arena* arena) : name(name), arena(arena) {
    position = prev_position = screen_position = { 0, 0 };
    rotation = prev_rotation = screen_rotation = 0;
    gun_rotation = prev_gun_rotation = screen_gun_rotation = 0;
    radar_rotation = prev_radar_rotation = screen_radar_rotation = 0;
    health = 100.0f;
    last_shoot_tick = 0;
    radar_hit = false;

    // Încărcăm texturile tale o singură dată, la crearea primului robot
    if (!textures_loaded) {
        body_texture = LoadTexture("src/bot_skins/robot1.png");
        gun_texture = LoadTexture("src/bot_skins/arma1.png");
        textures_loaded = true;
    }
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

void Bot::draw() {
    if (body_texture.id > 0 && gun_texture.id > 0) {

        // 1. DESENARE CORP ROBOT (Rămâne neschimbat, centrat perfect)
        Rectangle sourceRecBody = { 0.0f, 0.0f, (float)body_texture.width, (float)body_texture.height };
        Rectangle destRecBody = { screen_position.x, screen_position.y, SIZE, SIZE };
        Vector2 originBody = { SIZE / 2.0f, SIZE / 2.0f };

        DrawTexturePro(body_texture, sourceRecBody, destRecBody, originBody, glm::degrees(screen_rotation), WHITE);

        // 2. DESENARE ARMA (Pentru textură de 64x64 unde arma începe de la jumătate)
        Rectangle sourceRecGun = { 0.0f, 0.0f, (float)gun_texture.width, (float)gun_texture.height };

        // Desenăm arma exact pe poziția robotului, la dimensiunea ei nativă (SIZE x SIZE)
        Rectangle destRecGun = { screen_position.x, screen_position.y, SIZE, SIZE };

        // --- CALIBRARE FINĂ AICI ---
        // SIZE / 2.0f (adică 32) înseamnă centrul imaginii. 
        // Deoarece desenul tău începe de la jumătate, dacă setăm valoarea pe X mai mică, 
        // imaginea va fi "împinsă" în față.
        // Încearcă valori între 0.0f și 16.0f (de exemplu, SIZE / 4.0f înseamnă 16 pixeli) 
        // până când se aliniază perfect cu marginea robotului.
        float punctAncoraX = 0.0f;

        Vector2 originGun = { punctAncoraX, SIZE / 2.0f };

        float total_gun_angle = glm::degrees(screen_rotation + screen_gun_rotation);
        DrawTexturePro(gun_texture, sourceRecGun, destRecGun, originGun, total_gun_angle, WHITE);

    }
    else {
        // Fallback în caz că nu se găsesc pozele
        DrawRectanglePro({ screen_position.x, screen_position.y, SIZE, SIZE }, { SIZE / 2, SIZE / 2 }, glm::degrees(screen_rotation), BLUE);
        float gun_w = 40.0f; float gun_h = 10.0f;
        DrawRectanglePro({ screen_position.x, screen_position.y, gun_w, gun_h }, { 0, gun_h / 2 }, glm::degrees(screen_rotation + screen_gun_rotation), BLACK);
    }
}

void Bot::draw_radar() {
    float radar_dist = 2000.0f;
    float total_radar_rotation = screen_rotation + screen_radar_rotation;
    glm::vec2 radar_dir(glm::cos(total_radar_rotation), glm::sin(total_radar_rotation));

    Color color = GREEN;
    if (radar_hit) {
        color = RED;
    }
    color.a = 50;

    DrawLineEx(
        raylib_vec_from_glm_vec(screen_position),
        raylib_vec_from_glm_vec(screen_position + radar_dir * radar_dist),
        3.0f,
        color
    );
}

void Bot::draw_name() {
    DrawText(name.c_str(), screen_position.x - SIZE / 2, screen_position.y - SIZE / 2 - 20, 16, WHITE);

    DrawRectangle(screen_position.x - SIZE / 2, screen_position.y - SIZE / 2 - 5, SIZE, 4, RED);
    DrawRectangle(screen_position.x - SIZE / 2, screen_position.y - SIZE / 2 - 5, SIZE * (health / 100.0f), 4, GREEN);
}

void Bot::draw_trail() {
    for (size_t i = 0; i < trail.size(); i++) {
        Color color = BLUE;
        color.a = (float)i / trail.size() * 50;
        DrawRectanglePro(
            { trail[i].pos.x, trail[i].pos.y, SIZE, SIZE },
            { SIZE / 2, SIZE / 2 },
            glm::degrees(trail[i].rotation),
            color
        );
    }
}

void Bot::go(float delta) {
    delta = glm::clamp(delta, -8.0f, 8.0f);

    if (arena->tick % 2 == 0) {
        trail.push_back({ position, rotation });
        if (trail.size() > MAX_TRAIL_SIZE) {
            trail.pop_front();
        }
    }

    glm::vec2 direction(glm::cos(rotation), glm::sin(rotation));
    position += direction * delta;

    float halfSize = SIZE / 2.0f;

    if (position.x < halfSize) {
        position.x = halfSize;
    }
    else if (position.x > GetScreenWidth() - halfSize) {
        position.x = GetScreenWidth() - halfSize;
    }

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

void Bot::turn_radar(float delta) {
    delta = glm::clamp(delta, -PI / 4, PI / 4);
    radar_rotation += delta;
}

void Bot::shoot() {
    if (arena->tick - last_shoot_tick >= SHOOT_DELTA_TICKS) {
        last_shoot_tick = arena->tick;
        float total_gun_rotation = rotation + gun_rotation;
        glm::vec2 direction(glm::cos(total_gun_rotation), glm::sin(total_gun_rotation));
        arena->bullets.push_back(Bullet(position, direction, this));
    }
}