#include <raylib.h>
#include <format>
#include "bot.h"
#include "arena.h"
#include <iostream>

struct TestBot : Bot {
	using Bot::Bot;

	void update() {
		go(10);
		turn(glm::radians(5.0));
	}
};

int main() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(900, 600, "Bot Arena");

	TestBot bot({ 100, 100 });
	Arena arena;
	arena.bots.push_back(&bot);
	
	while (!WindowShouldClose()) {
		BeginDrawing();
		
		ClearBackground(RAYWHITE);
		DrawText(std::format("SALUT! FPS: {}", 1.0 / GetFrameTime()).c_str(), 100, 200, 20, BLACK);
		arena.try_update_and_draw();

		EndDrawing();
	}
}