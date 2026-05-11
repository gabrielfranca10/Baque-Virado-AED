#include "raylib.h"

int main() {
    InitWindow(800, 450, "Raylib funcionando!");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Raylib OK!", 300, 200, 40, DARKBLUE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
