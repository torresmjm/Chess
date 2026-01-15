#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"

typedef struct Button {
    Texture2D texture;
    Vector2 position;
    Rectangle hitbox;
    float scale;
} BUTTON;

void InitializeButton(BUTTON *button, const char *texturePath, Vector2 position);
bool IsButtonPressed(BUTTON *button);
bool IsButtonHover(BUTTON *button);
void UpdateButton(BUTTON *button);
void RenderButton(BUTTON *button);
void UnloadButton(BUTTON *button);

#endif