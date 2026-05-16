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

bool show_debug_window = false;

void init_imgui() {
	ImGui::CreateContext();
	float dpi = GetWindowScaleDPI().x;

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.IniFilename = nullptr;

	io.Fonts->AddFontFromFileTTF("Karla-Regular.ttf", 16.0f * dpi);

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

void draw_menu(Arena& arena, float dpi) {
	ImGui::SetNextWindowSize(ImVec2(500.0f * dpi, 0.0f), ImGuiCond_Always);
	ImGui::Begin("Bot Arena");

	ImGui::Text("Welcome to Bot Arena");
	ImGui::Text("Start a battle!");
	ImGui::Text("");

	if (ImGui::BeginTable("Bots table", 3, ImGuiTableFlags_Borders)) {
		ImGui::TableSetupColumn("Bot name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Health", ImGuiTableColumnFlags_WidthFixed, 50 * dpi);
		ImGui::TableSetupColumn("Remove", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 60 * dpi);
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
		NFD::UniquePathSet out_paths;
		if (NFD::OpenDialogMultiple(out_paths, (const nfdnfilteritem_t*)nullptr, 0, (const nfdnchar_t*) nullptr) == NFD_OKAY) {
			nfdpathsetsize_t num_paths;
			NFD::PathSet::Count(out_paths, num_paths);

			for (nfdpathsetsize_t i = 0; i < num_paths; ++i) {
				NFD::UniquePathSetPath path;
				NFD::PathSet::GetPath(out_paths, i, path);

				arena.add_lua_bot(path.get());
				std::cout << "Added lua bot at " << path.get() << '\n';
			}
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

	ImGui::SameLine();
	if (ImGui::Button("Debug")) {
		show_debug_window = true;
	}

	ImGui::End();
}

void draw_debug_window(Arena& arena) {
	if (!show_debug_window) return;
	ImGui::Begin("Bot Arena debug", &show_debug_window);

	ImGui::Text("SALUTUTUTUTUT");
	ImGui::Text(std::format("FPS: {:.2f}", 1.0 / GetFrameTime()).c_str());
	ImGui::SliderInt("TPS", &arena.tps, 1, 100);
	ImGui::Checkbox("Draw trails", &arena.draw_trail);
	ImGui::Checkbox("Draw radar", &arena.draw_radar);
	ImGui::Text("Tick: %d", arena.tick);
	ImGui::Text("Bullets alive: %d", arena.bullets.size());

	static TestBot test_bot("Test bot", &arena);
	static TestBot2 test_bot2("Test bot 2", &arena);

	if (ImGui::Button("Add test bot 1")) {
		test_bot.position = test_bot.prev_position = { 300, 300 };
		arena.bots.push_back(&test_bot);
	}

	ImGui::SameLine();
	if (ImGui::Button("Add test bot 2")) {
		test_bot2.position = test_bot2.prev_position = { 450, 300 };
		arena.bots.push_back(&test_bot2);
	}

	ImGui::End();
}

void parse_args(int argc, char* argv[], Arena& arena) {
	for (int i = 1; i < argc; i++) {
		arena.add_lua_bot(argv[i]);
	}
}

int main(int argc, char* argv[]) {
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(Arena::WIDTH, Arena::HEIGHT, "Bot Arena");
	init_imgui();
	NFD::Init();
	srand(time(0));

	Arena arena;
	parse_args(argc, argv, arena);
	
	while (!WindowShouldClose()) {
		BeginDrawing();
		
		ClearBackground(BLACK);
		arena.try_update_and_draw();

		rlDrawRenderBatchActive();

		render_imgui([&arena]() {
			draw_debug_window(arena);
			draw_menu(arena, GetWindowScaleDPI().x);
		});

		EndDrawing();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	NFD::Quit();
	CloseWindow();
}