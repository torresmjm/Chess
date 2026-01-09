#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include "raylib.h"

#define TILE_SIZE 100
#define BOARD_SIZE 8

typedef struct TileState {
    int color;
    int occupiedBy;
    Vector2 position;
    bool isPressed;
    bool isAllowed;
} TILES;

typedef enum PieceType {
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING
} PIECETYPE;

typedef struct Piece {
    Vector2 position;
    Texture2D texture;
    int color;
    PIECETYPE type;
} PIECE;

void InitializeChessboard();
void PrintChessboard();

PIECE* GetSelectedPiece();

int GetCurrentTurn();
void PlacePiece(int row, int column, int color, PIECETYPE type);
void PlaceStartingPieces();
void RenderPieces(Vector2 mouseGamePos);
void MovePiece(Vector2 mousePos);
void CheckAllowedMoves();
void UpdatePiecePosition(Vector2 mousePos);
void UnloadChessboard();

#endif // BOARD_H