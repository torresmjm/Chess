#include "button.h"

void InitializeButton(BUTTON *button, const char *texturePath, Vector2 position){
    button -> texture = LoadTexture(texturePath);
    button -> position = position;
    button -> scale = 4.0f;
    button -> hitbox = (Rectangle){position.x, position.y, button -> texture.width * button -> scale, button -> texture.height * button -> scale};
}

bool IsButtonPressed(BUTTON *button) {
    if (CheckCollisionPointRec(GetMousePosition(), button -> hitbox)) {
        return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }
    return false;
}

bool IsButtonHover(BUTTON *button) {
    return CheckCollisionPointRec(GetMousePosition(), button -> hitbox);
}

void UpdateButton(BUTTON *button){
    float targetScale = IsButtonHover(button) ? 4.1f : 4.0f;

    button -> scale += (targetScale - button -> scale) * 0.2f;

    button -> hitbox.width  = button -> texture.width  * button -> scale;
    button -> hitbox.height = button -> texture.height * button -> scale;

    float targetOffset = IsButtonPressed(button) ? 12 : 0;

    button -> position.x += (targetOffset - (button -> position.x - button -> hitbox.x)) * 0.2f;
    button -> position.y += (targetOffset - (button -> position.y - button -> hitbox.y)) * 0.2f;
}

void RenderButton(BUTTON *button) {
    DrawTextureEx(button -> texture, button -> position, 0.0f, button -> scale, WHITE);
}

void UnloadButton(BUTTON *button){
    UnloadTexture(button -> texture);
}

