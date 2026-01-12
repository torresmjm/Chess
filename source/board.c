#include "board.h"

bool IsSquareUnderAttack(int row, int col, int byColor);
int FindKing(int color);
bool IsMoveLegal(int fromRow, int fromCol, int toRow, int toCol);
bool HasLegalMoves(int color);
void UpdateCheckStatus();
void CheckAllowedMoves();

static TILES chessboard [BOARD_SIZE][BOARD_SIZE];
static PIECE pieces[32];
static int pieceCount = 0;

static PIECE* selectedPiece = NULL;
static int selectedRow = -1;
static int selectedColumn = -1;

static int currentTurn = 0;

static int lastMoveFromRow = -1;
static int lastMoveFromColumn = -1;
static int lastMoveToRow = -1;
static int lastMoveToColumn = -1;

static bool whiteKingInCheck = false;
static bool blackKingInCheck = false;
static bool isCheckmate = false;
static int winner = -1;

int GetCurrentTurn() {
    return currentTurn;
}

PIECE* GetSelectedPiece() {
    return selectedPiece;
}

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

void RenderChessboard() {

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int column = 0; column < BOARD_SIZE; column++) {

            Color tileColor;

            if (chessboard[row][column].isPressed) {
                tileColor = BLUE;
            } else if ((row == lastMoveFromRow && column == lastMoveFromColumn)) {
                tileColor = (Color){244, 196, 48, 255};
 
            } else if ((row == lastMoveToRow && column == lastMoveToColumn)) {
                tileColor = (Color){255, 244, 79, 255};
            } else if ((whiteKingInCheck || blackKingInCheck) && chessboard[row][column].occupiedBy == FindKing(currentTurn)) {
                tileColor = RED;
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
    pieces[pieceCount].hasMoved = false;
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
    
    for (int column = 0; column < BOARD_SIZE; column++) {
        PlacePiece(1, column, 1, PAWN);
    }
    
    for (int column = 0; column < BOARD_SIZE; column++) {
        PlacePiece(6, column, 0, PAWN);
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

int FindKing(int color) {
    for (int i = 0; i < pieceCount; i++) {
        if (pieces[i].type == KING && 
            pieces[i].color == color && 
            pieces[i].position.x >= 0) {
            return i;
        }
    }
    return -1;
}

bool IsMoveLegal(int fromRow, int fromCol, int toRow, int toCol) {
    // Simulate the move
    int movingPieceIndex = chessboard[fromRow][fromCol].occupiedBy;
    int capturedPieceIndex = chessboard[toRow][toCol].occupiedBy;
    
    PIECE *movingPiece = &pieces[movingPieceIndex];
    Vector2 originalPos = movingPiece->position;
    Vector2 capturedOriginalPos = {-1, -1};
    
    // Temporarily make the move
    chessboard[fromRow][fromCol].occupiedBy = -1;
    movingPiece->position = chessboard[toRow][toCol].position;
    
    if (capturedPieceIndex != -1) {
        capturedOriginalPos = pieces[capturedPieceIndex].position;
        pieces[capturedPieceIndex].position.x = -1000;
        pieces[capturedPieceIndex].position.y = -1000;
    }
    
    chessboard[toRow][toCol].occupiedBy = movingPieceIndex;
    
    // Find our king
    int kingIndex = FindKing(movingPiece->color);
    int kingRow = -1, kingCol = -1;
    
    // Find king's position on board
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (chessboard[r][c].occupiedBy == kingIndex) {
                kingRow = r;
                kingCol = c;
                break;
            }
        }
        if (kingRow != -1) break;
    }
    
    // Check if king is under attack
    int enemyColor = (movingPiece->color == 0) ? 1 : 0;
    bool isLegal = !IsSquareUnderAttack(kingRow, kingCol, enemyColor);
    
    // Undo the move
    chessboard[toRow][toCol].occupiedBy = capturedPieceIndex;
    chessboard[fromRow][fromCol].occupiedBy = movingPieceIndex;
    movingPiece->position = originalPos;
    
    if (capturedPieceIndex != -1) {
        pieces[capturedPieceIndex].position = capturedOriginalPos;
    }
    
    return isLegal;
}

bool HasLegalMoves(int color) {
    for (int fromRow = 0; fromRow < BOARD_SIZE; fromRow++) {
        for (int fromCol = 0; fromCol < BOARD_SIZE; fromCol++) {
            int pieceIndex = chessboard[fromRow][fromCol].occupiedBy;
            if (pieceIndex == -1) continue;
            if (pieces[pieceIndex].color != color) continue;
            
            // Check all possible destination squares
            for (int toRow = 0; toRow < BOARD_SIZE; toRow++) {
                for (int toCol = 0; toCol < BOARD_SIZE; toCol++) {
                    // Temporarily set this piece as selected to check moves
                    PIECE *tempPiece = selectedPiece;
                    int tempRow = selectedRow;
                    int tempCol = selectedColumn;
                    
                    selectedPiece = &pieces[pieceIndex];
                    selectedRow = fromRow;
                    selectedColumn = fromCol;
                    
                    // Clear and recalculate allowed moves
                    for (int r = 0; r < BOARD_SIZE; r++)
                        for (int c = 0; c < BOARD_SIZE; c++)
                            chessboard[r][c].isAllowed = false;
                    
                    CheckAllowedMoves();
                    
                    // If this square is allowed and the move is legal
                    if (chessboard[toRow][toCol].isAllowed && 
                        IsMoveLegal(fromRow, fromCol, toRow, toCol)) {
                        
                        // Restore selection
                        selectedPiece = tempPiece;
                        selectedRow = tempRow;
                        selectedColumn = tempCol;

                        for (int r = 0; r < BOARD_SIZE; r++)
                            for (int c = 0; c < BOARD_SIZE; c++)
                                chessboard[r][c].isAllowed = false;

                        return true;
                    }
                    
                    // Restore selection
                    selectedPiece = tempPiece;
                    selectedRow = tempRow;
                    selectedColumn = tempCol;
                }
            }
        }
    }

    for (int r = 0; r < BOARD_SIZE; r++)
        for (int c = 0; c < BOARD_SIZE; c++)
            chessboard[r][c].isAllowed = false;
            
    return false;
}

void UpdateCheckStatus() {
    // Check if white king is in check
    int whiteKingIndex = FindKing(0);
    if (whiteKingIndex != -1) {
        int kingRow = -1, kingCol = -1;
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (chessboard[r][c].occupiedBy == whiteKingIndex) {
                    kingRow = r;
                    kingCol = c;
                    break;
                }
            }
            if (kingRow != -1) break;
        }
        whiteKingInCheck = IsSquareUnderAttack(kingRow, kingCol, 1);
    }
    
    // Check if black king is in check
    int blackKingIndex = FindKing(1);
    if (blackKingIndex != -1) {
        int kingRow = -1, kingCol = -1;
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (chessboard[r][c].occupiedBy == blackKingIndex) {
                    kingRow = r;
                    kingCol = c;
                    break;
                }
            }
            if (kingRow != -1) break;
        }
        blackKingInCheck = IsSquareUnderAttack(kingRow, kingCol, 0);
    }
    
    // Check for checkmate
    if (whiteKingInCheck && !HasLegalMoves(0)) {
        isCheckmate = true;
        winner = 1; // Black wins
    } else if (blackKingInCheck && !HasLegalMoves(1)) {
        isCheckmate = true;
        winner = 0; // White wins
    } else {
        isCheckmate = false;
        winner = -1;
    }
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

    if (isCheckmate) return;
    
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
                
                // Check if this is a castling move
                if (selectedPiece->type == KING && abs(column - selectedColumn) == 2) {
                    
                    // Determine if kingside or queenside
                    if (column > selectedColumn) {
                        // Kingside castling
                        int rookIndex = chessboard[row][selectedColumn + 3].occupiedBy;
                        PIECE *rook = &pieces[rookIndex];
                        
                        // Move rook
                        chessboard[row][selectedColumn + 3].occupiedBy = -1;
                        rook->position = chessboard[row][selectedColumn + 1].position;
                        chessboard[row][selectedColumn + 1].occupiedBy = rookIndex;
                        rook->hasMoved = true;
                    } else {
                        // Queenside castling
                        int rookIndex = chessboard[row][selectedColumn - 4].occupiedBy;
                        PIECE *rook = &pieces[rookIndex];
                        
                        // Move rook
                        chessboard[row][selectedColumn - 4].occupiedBy = -1;
                        rook->position = chessboard[row][selectedColumn - 1].position;
                        chessboard[row][selectedColumn - 1].occupiedBy = rookIndex;
                        rook->hasMoved = true;
                    }
                }

                // Move the piece to the new position
                selectedPiece->position = tile->position;
                tile->occupiedBy = selectedPiece - pieces;
                selectedPiece->hasMoved = true;  // Mark piece as moved
                
                lastMoveFromColumn = selectedColumn;
                lastMoveFromRow = selectedRow;
                lastMoveToColumn = column;
                lastMoveToRow = row;

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

                currentTurn = (currentTurn == 0) ? 1 : 0;

                UpdateCheckStatus();

                // Prevent further moves if checkmate
                if (isCheckmate) {
                    return;
                }

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

            if (piece->color != currentTurn) {
                return;
            }
            
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
                for (int row = 0; row < BOARD_SIZE; row++)
                    for (int column = 0; column < BOARD_SIZE; column++) {
                        chessboard[row][column].isPressed = false;
                        chessboard[row][column].isAllowed = false;
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

bool IsSquareUnderAttack(int row, int column, int byColor) {
    // Check if square (row, column) is under attack by pieces of color 'byColor'

    for (int i = 0; i < pieceCount; i++) {
        if (pieces[i].color != byColor) continue;
        if (pieces[i].position.x < 0) continue; // Skip captured pieces
        
        // Find the piece's position on the board
        int pieceRow = -1, pieceCol = -1;
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (chessboard[r][c].occupiedBy == i) {
                    pieceRow = r;
                    pieceCol = c;
                    break;
                }
            }
            if (pieceRow != -1) break;
        }
        
        if (pieceRow == -1) continue;
        
        // Check if this piece can attack the target square
        switch (pieces[i].type) {
            case PAWN: {
                int direction = (pieces[i].color == 1) ? 1 : -1;
                if (pieceRow + direction == row && 
                    (pieceCol - 1 == column || pieceCol + 1 == column)) {
                    return true;
                }
                break;
            }
            
            case KNIGHT: {
                int moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, 
                                   {1, -2}, {1, 2}, {2, -1}, {2, 1}};
                for (int m = 0; m < 8; m++) {
                    if (pieceRow + moves[m][0] == row && pieceCol + moves[m][1] == column) {
                        return true;
                    }
                }
                break;
            }
            
            case BISHOP: {
                int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
                for (int d = 0; d < 4; d++) {
                    for (int dist = 1; dist < BOARD_SIZE; dist++) {
                        int newRow = pieceRow + directions[d][0] * dist;
                        int newCol = pieceCol + directions[d][1] * dist;
                        
                        if (newRow < 0 || newRow >= BOARD_SIZE || 
                            newCol < 0 || newCol >= BOARD_SIZE) break;
                        
                        if (newRow == row && newCol == column) return true;
                        if (chessboard[newRow][newCol].occupiedBy != -1) break;
                    }
                }
                break;
            }
            
            case ROOK: {
                int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
                for (int d = 0; d < 4; d++) {
                    for (int dist = 1; dist < BOARD_SIZE; dist++) {
                        int newRow = pieceRow + directions[d][0] * dist;
                        int newCol = pieceCol + directions[d][1] * dist;
                        
                        if (newRow < 0 || newRow >= BOARD_SIZE || 
                            newCol < 0 || newCol >= BOARD_SIZE) break;
                        
                        if (newRow == row && newCol == column) return true;
                        if (chessboard[newRow][newCol].occupiedBy != -1) break;
                    }
                }
                break;
            }
            
            case QUEEN: {
                int directions[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, 
                                        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
                for (int d = 0; d < 8; d++) {
                    for (int dist = 1; dist < BOARD_SIZE; dist++) {
                        int newRow = pieceRow + directions[d][0] * dist;
                        int newCol = pieceCol + directions[d][1] * dist;
                        
                        if (newRow < 0 || newRow >= BOARD_SIZE || 
                            newCol < 0 || newCol >= BOARD_SIZE) break;
                        
                        if (newRow == row && newCol == column) return true;
                        if (chessboard[newRow][newCol].occupiedBy != -1) break;
                    }
                }
                break;
            }
            
            case KING: {
                int directions[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, 
                                        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
                for (int d = 0; d < 8; d++) {
                    if (pieceRow + directions[d][0] == row && 
                        pieceCol + directions[d][1] == column) {
                        return true;
                    }
                }
                break;
            }
        }
    }
    
    return false;
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
            // Normal king moves (existing code)
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
            
            // CASTLING LOGIC - Add this section
            if (!selectedPiece->hasMoved) {
                int enemyColor = (selectedPiece->color == 0) ? 1 : 0;
                
                // Check if king is currently in check
                if (!IsSquareUnderAttack(row, column, enemyColor)) {
                    
                    // Kingside castling (right)
                    if (column + 3 < BOARD_SIZE) {
                        int rookIndex = chessboard[row][column + 3].occupiedBy;
                        if (rookIndex != -1 && 
                            pieces[rookIndex].type == ROOK && 
                            pieces[rookIndex].color == selectedPiece->color &&
                            !pieces[rookIndex].hasMoved) {
                            
                            // Check if squares between are empty
                            if (chessboard[row][column + 1].occupiedBy == -1 &&
                                chessboard[row][column + 2].occupiedBy == -1) {
                                
                                // Check if king passes through or lands on attacked square
                                if (!IsSquareUnderAttack(row, column + 1, enemyColor) &&
                                    !IsSquareUnderAttack(row, column + 2, enemyColor)) {
                                    chessboard[row][column + 2].isAllowed = true;
                                }
                            }
                        }
                    }
                    
                    // Queenside castling (left)
                    if (column - 4 >= 0) {
                        int rookIndex = chessboard[row][column - 4].occupiedBy;
                        if (rookIndex != -1 && 
                            pieces[rookIndex].type == ROOK && 
                            pieces[rookIndex].color == selectedPiece->color &&
                            !pieces[rookIndex].hasMoved) {
                            
                            // Check if squares between are empty
                            if (chessboard[row][column - 1].occupiedBy == -1 &&
                                chessboard[row][column - 2].occupiedBy == -1 &&
                                chessboard[row][column - 3].occupiedBy == -1) {
                                
                                // Check if king passes through or lands on attacked square
                                if (!IsSquareUnderAttack(row, column - 1, enemyColor) &&
                                    !IsSquareUnderAttack(row, column - 2, enemyColor)) {
                                    chessboard[row][column - 2].isAllowed = true;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
    }

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (chessboard[r][c].isAllowed) {
                if (!IsMoveLegal(selectedRow, selectedColumn, r, c)) {
                    chessboard[r][c].isAllowed = false;
                }
            }
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
        pieces[i].hasMoved = false;
    }

    pieceCount = 0;
    currentTurn = 0; 
    selectedPiece = NULL;
    selectedRow = -1;
    selectedColumn = -1;

    lastMoveFromRow = -1;
    lastMoveFromColumn = -1;
    lastMoveToRow = -1;
    lastMoveToColumn = -1;
    
    whiteKingInCheck = false;
    blackKingInCheck = false;
    isCheckmate = false;
    winner = -1;
}