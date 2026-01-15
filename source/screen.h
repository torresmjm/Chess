#ifndef SCREEN_H
#define SCREEN_H

#include "raylib.h"
#include <stdio.h>
#include "board.h"

typedef enum ScreenToLoad {
    INTRO,
    TITLE,
    GAME,
} SCREEN;

void InitializeScreen();

void ChangeScreen(SCREEN newScreen);

SCREEN GetCurrentScreen();

void UpdateScreen();

void RenderScreen();

#endif