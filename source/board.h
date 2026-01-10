#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include <stdlib.h> 
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
    bool hasMoved;
} PIECE;

/* 
    Manage the chessboard state 
*/
void InitializeChessboard();
void RenderChessboard();
void UnloadChessboard();

/*
    Manage piece
*/
void PlacePiece(int row, int column, int color, PIECETYPE type);
void PlaceStartingPieces();

/*
    Manage piece movements, game rules, interactions and rendering
*/

void MovePiece(Vector2 mousePos);
void RenderPieces(Vector2 mouseGamePos);
void CheckAllowedMoves();
void UpdatePiecePosition(Vector2 mousePos);

PIECE* GetSelectedPiece();
int GetCurrentTurn();

bool IsSquareUnderAttack(int row, int column, int attackingColor);

#endif // BOARD_H