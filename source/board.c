#include "board.h"

static TILES chessboard [BOARD_SIZE][BOARD_SIZE];
static PIECE pieces[32];
static int pieceCount = 0;

static PIECE* selectedPiece = NULL;
static int selectedRow = -1;
static int selectedColumn = -1;

// Basically initializes the chessboard tiles and their colors
void InitializeChessboard() {
    int boardPixelSize = BOARD_SIZE * TILE_SIZE;
    int startX = (1920 - boardPixelSize) / 2;
    int startY = (1080 - boardPixelSize) / 2;

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int column = 0; column < BOARD_SIZE; column++) {
            
            chessboard[row][column].position.x = startX + column * TILE_SIZE;
            chessboard[row][column].position.y = startY + row * TILE_SIZE;

            if ((row + column) % 2 == 0) {
                chessboard[row][column].color = 0;
            } else {
                chessboard[row][column].color = 1;
            }
            
            chessboard[row][column].occupiedBy = -1;
            chessboard[row][column].isPressed = false;
            chessboard[row][column].isAllowed = false; 
        }
    }
}

// Renders the chessboard and highlights pressed and allowed tiles
void PrintChessboard() {

    // We check each slot in array and set as a so called "tiles" that contains based on the struct in board.h
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int column = 0; column < BOARD_SIZE; column++) {

            Color tileColor;
            if (chessboard[row][column].isPressed) {
                tileColor = BLUE;
            } else {
                tileColor = (chessboard[row][column].color == 0) ? LIGHTGRAY : DARKGRAY;
            }

            DrawRectangle(
                chessboard[row][column].position.x,
                chessboard[row][column].position.y,
                TILE_SIZE,
                TILE_SIZE,
                tileColor
            );
            
            // Check and render allowed moves
            if (chessboard[row][column].isAllowed) {
                Vector2 center = {
                    chessboard[row][column].position.x + TILE_SIZE / 2.0f,
                    chessboard[row][column].position.y + TILE_SIZE / 2.0f
                };
                
                // Case 1: Empty tile, draw a filled circle
                if (chessboard[row][column].occupiedBy == -1) {
                    float radius = TILE_SIZE / 6.0f;
                    DrawCircleV(center, radius, Fade(WHITE, 0.7f));
                }
                
                // Case 2: Occupied tile, draw a ring
                else {
                    float outerRadius = TILE_SIZE / 2.5f;
                    float innerRadius = outerRadius - 5.0f;
                    DrawRing(center, innerRadius, outerRadius, 0, 360, 36, Fade(WHITE, 0.7f));
                }
            }
        }
    }
}

// Helpers to place pieces on the board
void PlacePiece(int row, int column, int color, PIECETYPE type) {
    pieces[pieceCount].position = chessboard[row][column].position;
    pieces[pieceCount].color = color;
    pieces[pieceCount].type = type;
    chessboard[row][column].occupiedBy = pieceCount;
    pieceCount++;
}

// Places all pieces in their starting positions
void PlaceStartingPieces() {
    PlacePiece(0, 0, 1, ROOK);
    PlacePiece(0, 1, 1, KNIGHT);
    PlacePiece(0, 2, 1, BISHOP);
    PlacePiece(0, 3, 1, QUEEN);
    PlacePiece(0, 4, 1, KING);
    PlacePiece(0, 5, 1, BISHOP);
    PlacePiece(0, 6, 1, KNIGHT);
    PlacePiece(0, 7, 1, ROOK);
    
    for (int col = 0; col < BOARD_SIZE; col++) {
        PlacePiece(1, col, 1, PAWN);
    }
    
    for (int col = 0; col < BOARD_SIZE; col++) {
        PlacePiece(6, col, 0, PAWN);
    }
    
    PlacePiece(7, 0, 0, ROOK);
    PlacePiece(7, 1, 0, KNIGHT);
    PlacePiece(7, 2, 0, BISHOP);
    PlacePiece(7, 3, 0, QUEEN);
    PlacePiece(7, 4, 0, KING);
    PlacePiece(7, 5, 0, BISHOP);
    PlacePiece(7, 6, 0, KNIGHT);
    PlacePiece(7, 7, 0, ROOK);
}

// So we can call the variable selectedPiece from other files since its encapsulated here
PIECE* GetSelectedPiece() {
    return selectedPiece;
}

// Renders all pieces on the board, with the selected piece following the mouse
void RenderPieces(Vector2 mouseGamePos) {
    for (int i = 0; i < pieceCount; i++) {

        Color pieceColor = (pieces[i].color == 0) ? WHITE : BLACK;

        Vector2 drawPos = pieces[i].position;

        if (&pieces[i] == selectedPiece) {
            drawPos.x = mouseGamePos.x - TILE_SIZE / 2;
            drawPos.y = mouseGamePos.y - TILE_SIZE / 2;
        }

        int x = drawPos.x + TILE_SIZE / 2 - 12;
        int y = drawPos.y + TILE_SIZE / 2 - 25;

        switch (pieces[i].type) {
            case PAWN:   DrawText("P", x, y, 50, pieceColor); break;
            case ROOK:   DrawText("R", x, y, 50, pieceColor); break;
            case KNIGHT: DrawText("N", x, y, 50, pieceColor); break;
            case BISHOP: DrawText("B", x, y, 50, pieceColor); break;
            case QUEEN:  DrawText("Q", x, y, 50, pieceColor); break;
            case KING:   DrawText("K", x, y, 50, pieceColor); break;
        }
    }
}

// Handles mouse clicks to select/deselect pieces and move them
void MovePiece(Vector2 mousePos) {
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int column = 0; column < BOARD_SIZE; column++) {
            TILES *tile = &chessboard[row][column];
            Rectangle tileRect = {
                tile->position.x,
                tile->position.y,
                TILE_SIZE,
                TILE_SIZE
            };
            
            if (!CheckCollisionPointRec(mousePos, tileRect))
                continue;
            
            // If a piece is selected and we click an allowed move, move the piece
            if (selectedPiece != NULL && tile -> isAllowed) {

                // If there's a piece on the target tile, capture it
                if (tile -> occupiedBy != -1) {
                    PIECE *capturedPiece = &pieces[tile -> occupiedBy];
                    capturedPiece -> position.x = -1000;
                    capturedPiece -> position.y = -1000;

                }
                
                // Clear the old tile
                chessboard[selectedRow][selectedColumn].occupiedBy = -1;
                
                // Move the piece to the new position
                selectedPiece->position = tile->position;
                tile->occupiedBy = selectedPiece - pieces;
                
                // Clear selection and allowed moves
                for (int r = 0; r < BOARD_SIZE; r++) {
                    for (int c = 0; c < BOARD_SIZE; c++) {
                        chessboard[r][c].isPressed = false;
                        chessboard[r][c].isAllowed = false;
                    }
                }
                
                selectedPiece = NULL;
                selectedRow = -1;
                selectedColumn = -1;
                return;
            }
            
            // Clicked on an empty tile - clear all selections
            if (tile->occupiedBy == -1) {
                for (int r = 0; r < BOARD_SIZE; r++)
                    for (int c = 0; c < BOARD_SIZE; c++) {
                        chessboard[r][c].isPressed = false;
                        chessboard[r][c].isAllowed = false;
                    }
                selectedPiece = NULL;
                selectedRow = -1;
                selectedColumn = -1;
                return;
            }
            
            PIECE *piece = &pieces[tile->occupiedBy];
            
            // Clicking the same piece - deselect it
            if (selectedPiece == piece) {
                selectedPiece = NULL;
                selectedRow = -1;
                selectedColumn = -1;
                tile->isPressed = false;
                for (int r = 0; r < BOARD_SIZE; r++)
                    for (int c = 0; c < BOARD_SIZE; c++)
                        chessboard[r][c].isAllowed = false;
            } 
            // Clicking a different piece while one is already selected - deselect only
            else if (selectedPiece != NULL) {
                for (int r = 0; r < BOARD_SIZE; r++)
                    for (int c = 0; c < BOARD_SIZE; c++) {
                        chessboard[r][c].isPressed = false;
                        chessboard[r][c].isAllowed = false;
                    }
                selectedPiece = NULL;
                selectedRow = -1;
                selectedColumn = -1;
            }
            // No piece selected - select this one
            else {
                tile->isPressed = true;
                selectedPiece = piece;
                selectedRow = row;
                selectedColumn = column;
                CheckAllowedMoves();
            }
            return;
        }
    }
}

void CheckAllowedMoves() {

    // Clear all previous allowed moves
    for (int row = 0; row < BOARD_SIZE; row++)
        for (int column = 0; column < BOARD_SIZE; column++)
            chessboard[row][column].isAllowed = false;
    
    if (selectedPiece == NULL) return;
    
    int row = selectedRow;
    int column = selectedColumn;
    
    switch (selectedPiece->type) {
        case PAWN: {

            int direction = (selectedPiece->color == 1) ? 1 : -1;
            int startRow = (selectedPiece->color == 1) ? 1 : 6;
            
            // Move forward one square
            if (row + direction >= 0 && row + direction < BOARD_SIZE) {
                if (chessboard[row + direction][column].occupiedBy == -1) {
                    chessboard[row + direction][column].isAllowed = true;
                    
                    // Check if in starting position to move two squares
                    if (row == startRow && chessboard[row + 2 * direction][column].occupiedBy == -1) {
                        chessboard[row + 2 * direction][column].isAllowed = true;
                    }
                }
            }
            
            // Capture diagonally
            for (int dc = -1; dc <= 1; dc += 2) {
                int newRow = row + direction;
                int newColumn = column + dc;
                if (newRow >= 0 && newRow < BOARD_SIZE && newColumn >= 0 && newColumn < BOARD_SIZE) {
                    int targetPiece = chessboard[newRow][newColumn].occupiedBy;
                    if (targetPiece != -1 && pieces[targetPiece].color != selectedPiece->color) {
                        chessboard[newRow][newColumn].isAllowed = true;
                    }
                }
            }
            break;
        }
        
        case ROOK: {
            // Horizontal and vertical directions
            int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
            for (int d = 0; d < 4; d++) {
                for (int i = 1; i < BOARD_SIZE; i++) {
                    int newRow = row + directions[d][0] * i;
                    int newCol = column + directions[d][1] * i;
                    
                    if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE)
                        break;
                    
                    int targetPiece = chessboard[newRow][newCol].occupiedBy;
                    if (targetPiece == -1) {
                        chessboard[newRow][newCol].isAllowed = true;
                    } else {
                        if (pieces[targetPiece].color != selectedPiece->color)
                            chessboard[newRow][newCol].isAllowed = true;
                        break;
                    }
                }
            }
            break;
        }
        
        case KNIGHT: {
            int moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
            for (int i = 0; i < 8; i++) {
                int newRow = row + moves[i][0];
                int newCol = column + moves[i][1];
                
                if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE) {
                    int targetPiece = chessboard[newRow][newCol].occupiedBy;
                    if (targetPiece == -1 || pieces[targetPiece].color != selectedPiece->color)
                        chessboard[newRow][newCol].isAllowed = true;
                }
            }
            break;
        }
        
        case BISHOP: {
            // Diagonal directions
            int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
            for (int d = 0; d < 4; d++) {
                for (int i = 1; i < BOARD_SIZE; i++) {
                    int newRow = row + directions[d][0] * i;
                    int newCol = column + directions[d][1] * i;
                    
                    if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE)
                        break;
                    
                    int targetPiece = chessboard[newRow][newCol].occupiedBy;
                    if (targetPiece == -1) {
                        chessboard[newRow][newCol].isAllowed = true;
                    } else {
                        if (pieces[targetPiece].color != selectedPiece->color)
                            chessboard[newRow][newCol].isAllowed = true;
                        break;
                    }
                }
            }
            break;
        }
        
        case QUEEN: {
            // All 8 directions (rook + bishop)
            int directions[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
            for (int d = 0; d < 8; d++) {
                for (int i = 1; i < BOARD_SIZE; i++) {
                    int newRow = row + directions[d][0] * i;
                    int newColumn = column + directions[d][1] * i;
                    
                    if (newRow < 0 || newRow >= BOARD_SIZE || newColumn < 0 || newColumn >= BOARD_SIZE)
                        break;
                    
                    int targetPiece = chessboard[newRow][newColumn].occupiedBy;
                    if (targetPiece == -1) {
                        chessboard[newRow][newColumn].isAllowed = true;
                    } else {
                        if (pieces[targetPiece].color != selectedPiece->color)
                            chessboard[newRow][newColumn].isAllowed = true;
                        break;
                    }
                }
            }
            break;
        }
        
        case KING: {
            // All 8 directions, one square only
            int directions[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
            for (int d = 0; d < 8; d++) {
                int newRow = row + directions[d][0];
                int newCol = column + directions[d][1];
                
                if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE) {
                    int targetPiece = chessboard[newRow][newCol].occupiedBy;
                    if (targetPiece == -1 || pieces[targetPiece].color != selectedPiece->color)
                        chessboard[newRow][newCol].isAllowed = true;
                }
            }
            break;
        }
    }
}

void UpdatePiecePosition(Vector2 mousePosition) {
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    if (selectedPiece == NULL) return;
    
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            TILES *tile = &chessboard[row][col];
            
            // Check if we clicked on this tile
            Rectangle tileHitBox = {
                tile->position.x,
                tile->position.y,
                TILE_SIZE,
                TILE_SIZE
            };
            
            if (!CheckCollisionPointRec(mousePosition, tileHitBox))
                continue;
            
            // Check if this tile is an allowed move
            if (!tile -> isAllowed)
                return;
            
            // If there's a piece on the target tile, capture it
            if (tile->occupiedBy != -1) {
                PIECE *capturedPiece = &pieces[tile->occupiedBy];
                capturedPiece->position.x = -1000; // Move off screen
                capturedPiece->position.y = -1000;
            }
            
            // Clear the old tile
            chessboard[selectedRow][selectedColumn].occupiedBy = -1;
            
            // Move the piece to the new position
            selectedPiece->position = tile->position;
            tile->occupiedBy = selectedPiece - pieces; // Get the piece index
            
            // Clear selection and allowed moves
            for (int r = 0; r < BOARD_SIZE; r++) {
                for (int c = 0; c < BOARD_SIZE; c++) {
                    chessboard[r][c].isPressed = false;
                    chessboard[r][c].isAllowed = false;
                }
            }
            
            selectedPiece = NULL;
            selectedRow = -1;
            selectedColumn = -1;
            
            return;
        }
    }
}

void UnloadChessboard() {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            chessboard[row][col].occupiedBy = -1;
            chessboard[row][col].color = (row + col) % 2;
            chessboard[row][col].position.x = 0;
            chessboard[row][col].position.y = 0;
        }
    }

    for (int i = 0; i < 32; i++) {
        pieces[i].color = 0;
        pieces[i].type = PAWN;
        pieces[i].position.x = 0;
        pieces[i].position.y = 0;
    }

    pieceCount = 0;
}