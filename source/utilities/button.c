#include "button.h"

void InitializeButton(BUTTON *button, const char *texturePath, Vector2 position){
    button -> texture = LoadTexture(texturePath);
    button -> position = position;
}

void RenderButton(BUTTON button) {
    DrawTexture(button.texture, button.position.x, button.position.y, WHITE);
}

void IsButtonPressed(BUTTON button) {
    Rectangle buttonHitbox = {button.position.x, button.position.y, button.texture.width, button.texture.height};
    if (CheckCollisionPointRec(GetMousePosition(), buttonHitbox)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        }
    }
}

void UnloadButton(BUTTON *button){
    UnloadTexture(button -> texture);
}

