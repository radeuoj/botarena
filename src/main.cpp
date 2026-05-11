#include <raylib.h>
#include <format>
#include "bot.h"
#include "arena.h"
#include "luabot.h"
#include <iostream>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <rlgl.h>

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

void init_imgui() {
	ImGui::CreateContext();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::GetIO().IniFilename = nullptr;

	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init();
}

void render_imgui(void (*fn)()) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	fn();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	GLFWwindow* mainCtx = glfwGetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	glfwMakeContextCurrent(mainCtx);
}

int main() {
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(900, 600, "Bot Arena");
	init_imgui();

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

		rlDrawRenderBatchActive();

		render_imgui([]() {
			ImGui::Begin("Bot Arena");

			ImGui::Text("SALUTUTUTUTUT");
			ImGui::Text(std::format("FPS: {:.2f}", 1.0 / GetFrameTime()).c_str());

			ImGui::End();
		});

		EndDrawing();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	CloseWindow();
}