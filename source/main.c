#include "raylib.h"

int main () {

    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(1920, 1080, "Chess");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("This is a borderless windowed mode!", 200, 280, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;

}