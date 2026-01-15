#include "screen.h"
#include "menu.h"

static SCREEN currentScreen = INTRO;
static bool gameStart = false;

void InitializeScreen(){
    currentScreen = INTRO;
}

void ChangeScreen(SCREEN newScreen) {
    currentScreen = newScreen;
}

SCREEN GetCurrentScreen() {
    return currentScreen;
}

void UpdateScreen() {
    switch (currentScreen)
    {
        case INTRO:
            if (IsKeyPressed(KEY_ENTER))
            {
                ChangeScreen(TITLE);
            }
            break;

        case TITLE:
            break;

        case GAME:
            if (GetSelectedPiece())
                SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            else
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);

            if (!gameStart)
            {
                InitializeChessboard();
                PlaceStartingPieces();
                gameStart = true;
            }

            MovePiece(GetMousePosition());

            if (IsKeyPressed(KEY_ENTER))
            {
                UnloadChessboard();
                gameStart = false;
                ChangeScreen(INTRO);
            }
            break;
    }
}

void RenderScreen(){
    switch (currentScreen) {
        case INTRO:{
            DrawText("INTRO SCREEN", 120, 20, 80, LIGHTGRAY);
            
        } break;
        case TITLE:{
            DrawText("TITLE SCREEN", 120, 20, 80, LIGHTGRAY);
            RenderMenu();
        } break;
        case GAME:{
            RenderChessboard();
            RenderPieces(GetMousePosition());
        } break;
        default: break;
    }
}