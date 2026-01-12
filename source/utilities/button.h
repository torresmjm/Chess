#include "raylib.h"

typedef struct Button {
    Texture2D texture;
    Vector2 position;
} BUTTON;

void InitializeButton(BUTTON *button, const char *texturePath, Vector2 position);
void RenderButton(BUTTON button);
void UnloadButton(BUTTON *button);
