#include "raylib.h"
#include <stdio.h>
#include "board.h"

typedef enum ScreenToLoad {
    INTRO,
    TITLE,
    GAME,
} SCREEN;

void InitializeScreen();

void ChangeScreen(SCREEN *currentScreen);

void RenderScreen(SCREEN *currentScreen);

void UnloadScreen();