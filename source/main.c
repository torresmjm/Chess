#include "raylib.h"
#include <stdio.h>
#include "board.h"
#include "screen.h"
#include "menu.h"
#include <math.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

int main () {

    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_RESIZABLE);
    InitWindow(1920, 1080, "Chess");

    InitializeScreen();
    InitializeMenu();
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        UpdateMenu();
        UpdateScreen();

        BeginDrawing();
            ClearBackground(RAYWHITE);
            RenderScreen();
        EndDrawing();
    }

    CloseWindow();

    return 0;

}