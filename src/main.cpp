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
#include <functional>
#include <nfd.hpp>

struct TestBot : Bot {
	using Bot::Bot;

	void update() {
		go(10);
		turn(glm::radians(5.0));
	}
};
struct TestBot2 : Bot {
	using Bot::Bot;

	void update() {
		go(10);
		turn(glm::radians(5.0));
	}
};

void init_imgui() {
	ImGui::CreateContext();
	float dpi = GetWindowScaleDPI().x;

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::GetIO().IniFilename = nullptr;
	ImGui::GetIO().FontGlobalScale = dpi;

	ImGui::GetStyle().ScaleAllSizes(dpi);
	std::cout << "Display scale DPI: " << dpi << '\n';

	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init();
}

void render_imgui(std::function<void()> fn) {
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

void draw_menu(Arena& arena) {
	ImGui::SetNextWindowSize(ImVec2(500.0f, 0.0f), ImGuiCond_Always);
	ImGui::Begin("Bot Arena");

	ImGui::Text("Welcome to Bot Arena");
	ImGui::Text("Start a battle!");
	ImGui::Text("");

	if (ImGui::BeginTable("Bots table", 3, ImGuiTableFlags_Borders)) {
		ImGui::TableSetupColumn("Bot name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Health", ImGuiTableColumnFlags_WidthFixed, 50);
		ImGui::TableSetupColumn("Remove", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 50);
		ImGui::TableHeadersRow();

		int bot_to_delete = -1;

		for (int i = 0; i < arena.bots.size(); i++) {
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::Text(arena.bots[i]->name.c_str());

			ImGui::TableNextColumn();
			ImGui::Text(std::format("{:.2f}", arena.bots[i]->health).c_str());

			ImGui::TableNextColumn();
			if (ImGui::Button(std::format("Remove##{}", i).c_str())) {
				bot_to_delete = i;
			}
		}

		if (bot_to_delete != -1) {
			arena.bots.erase(arena.bots.begin() + bot_to_delete);
		}

		ImGui::EndTable();
	}

	if (ImGui::Button("Add bot")) {
		NFD::UniquePathU8 out_path;
		if (NFD::OpenDialog(out_path) == NFD_OKAY) {
			arena.add_lua_bot(out_path.get());
			std::cout << "Added lua bot at " << out_path.get() << '\n';
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Resume")) {
		arena.resume();
	}

	ImGui::SameLine();
	if (ImGui::Button("Pause")) {
		arena.pause();
	}

	ImGui::SameLine();
	if (ImGui::Button("Start")) {
		arena.start();
	}

	ImGui::End();
}

int main() {
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(Arena::WIDTH, Arena::HEIGHT, "Bot Arena");
	init_imgui();
	NFD::Init();
	srand(time(0));

	TestBot test_bot("Test bot");
	test_bot.position = test_bot.prev_position = { 300, 300 };
	TestBot2 test_bot2("Test bot 2");
	test_bot2.position = test_bot2.prev_position = { 450, 300 };

	Arena arena;
	arena.bots.push_back(&test_bot);
	arena.bots.push_back(&test_bot2);
	
	while (!WindowShouldClose()) {
		BeginDrawing();
		
		ClearBackground(BLACK);
		arena.try_update_and_draw();

		rlDrawRenderBatchActive();

		render_imgui([&arena]() {
			ImGui::Begin("Bot Arena debug");

			ImGui::Text("SALUTUTUTUTUT");
			ImGui::Text(std::format("FPS: {:.2f}", 1.0 / GetFrameTime()).c_str());
			ImGui::Checkbox("Draw trails", &arena.draw_trail);

			ImGui::End();

			draw_menu(arena);
		});

		EndDrawing();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	NFD::Quit();
	CloseWindow();
}