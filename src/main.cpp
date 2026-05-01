#include <raylib.h>

int main() {
	SetConfigFlags(ConfigFlags::FLAG_WINDOW_RESIZABLE);
	InitWindow(900, 600, "Bot Arena");

	while (!WindowShouldClose()) {
		BeginDrawing();
		
		ClearBackground(RAYWHITE);
		DrawText("SALUT!", 100, 200, 20, BLACK);

		EndDrawing();
	}
}