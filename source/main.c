#include "raylib.h"
#include <stdio.h>
#include "board.h"
#include "screen.h"
#include <math.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

int main () {

    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_RESIZABLE);
    InitWindow(1920, 1080, "Chess");

    SCREEN screenToLoad = INTRO;
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        BeginDrawing();
            ClearBackground(RAYWHITE);
            ChangeScreen(&screenToLoad);
            RenderScreen(&screenToLoad);
        EndDrawing();
    }

    CloseWindow();

    return 0;

}