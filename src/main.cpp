#include <raylib.h>
#include <format>
#include "bot.h"
#include "arena.h"
#include "luabot.h"
#include <iostream>
#include <rlImGui.h>
#include <imgui.h>

struct TestBot : Bot {
	void update() {
		go(10);
		turn(glm::radians(5.0));
	}
};
struct TestBot2 : Bot {
	void update() {
		go(10);
		turn(glm::radians(5.0));
	}
};

int main() {
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(900, 600, "Bot Arena");
	rlImGuiSetup(true);

	TestBot test_bot;
	test_bot.position = { 300, 300 };
	TestBot2 test_bot2;
	test_bot2.position = { 450, 300 };
	LuaBot lua_bot("luabots/test.lua");

	Arena arena;
	arena.bots.push_back(&test_bot);
	arena.bots.push_back(&lua_bot);
	arena.bots.push_back(&test_bot2);
	arena.init();
	
	while (!WindowShouldClose()) {
		BeginDrawing();
		
		ClearBackground(BLACK);
		DrawText(std::format("SALUT! FPS: {}", 1.0 / GetFrameTime()).c_str(), 100, 200, 20, RED);
		arena.try_update_and_draw();

		rlImGuiBegin();

		ImGui::Begin("Bot Arena");

		ImGui::Text("SALUTUTUTUTUT");
		ImGui::Text(std::format("FPS: {:.2f}", 1.0 / GetFrameTime()).c_str());

		ImGui::End();

		rlImGuiEnd();

		EndDrawing();
	}

	rlImGuiShutdown();
	CloseWindow();
}