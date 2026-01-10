#include "screen.h"

void InitializeScreen(){

}

void ChangeScreen(SCREEN *currentScreen) {
    switch (*currentScreen) {
        case INTRO:{
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
            {
                *currentScreen = TITLE;
            }
        } break;
        case TITLE:{
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
            {
                *currentScreen = GAME;
            }   
        } break;
        case GAME:{
            if (GetSelectedPiece())
                SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            else
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);

            static bool gameStart = false;
            if (!gameStart) {
                InitializeChessboard();
                PlaceStartingPieces();
                gameStart = true;
            }

            MovePiece(GetMousePosition());

            if (IsKeyPressed(KEY_ENTER))
            {
                UnloadChessboard();
                *currentScreen = INTRO;
                gameStart = false;
            }

        } break;
        default: break;
    }
}

void RenderScreen(SCREEN *currentScreen){
    switch (*currentScreen) {
        case INTRO:{
            DrawText("INTRO SCREEN", 120, 20, 80, LIGHTGRAY);
            
        } break;
        case TITLE:{
            DrawText("TITLE SCREEN", 120, 20, 80, LIGHTGRAY);
        } break;
        case GAME:{
            RenderChessboard();
            RenderPieces(GetMousePosition());
        } break;
        default: break;
    }
}

void UnloadScreen() {

}