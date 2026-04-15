#ifndef TETRIS_H
#define TETRIS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define ROWS 20
#define COLS 10
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000


extern int SHAPES[7][4][4];

typedef struct { 
    int shape[4][4];    // matice 4x4
    int row;            // aktuální řádek v gridu 
    int col;            // aktuální sloupec v gridu 
    int active;         // stav aktivity 1/0    
    int currentShapeIndex; // index momentalniho tvatu z pole SHAPES
    SDL_Color color; // barva
} Tetromino;

typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_SETTINGS,
    STATE_AUDIO,
    STATE_VOLUME,
    STATE_MUSIC,
    STATE_DIFFICULTY,
    STATE_QUIT
} GameState;

void applyWallKick(Tetromino *t, int rotated[4][4]);
void shiftAfterRotation(Tetromino *t);
void changeShape(Tetromino *t, int shapeIndex);
void changeBlockState(int r, int c, int grid[ROWS][COLS], int state);
void deleteTetromino(int grid[ROWS][COLS], Tetromino *t);
void drawTetromino(int grid[ROWS][COLS], Tetromino *t);
void moveAllDown(int grid[ROWS][COLS], int lastRow);
void moveTetromino(int grid[ROWS][COLS], Tetromino *t, char direction, Mix_Chunk* sfx);
void placeTetromino(int grid[ROWS][COLS], Tetromino *t);
void rotateTetromino(int grid[ROWS][COLS], Tetromino *t);
void respawnTetromino(int grid[ROWS][COLS], Tetromino *t, int randomShapeIndex, SDL_Color colors[]);
void resetGame(int grid[ROWS][COLS], Tetromino *t, int *score, SDL_Color colors[]);

int canMoveLeft(int grid[ROWS][COLS], Tetromino *t);
int canMoveRight(int grid[ROWS][COLS], Tetromino *t);
int canMoveDown(int grid[ROWS][COLS], Tetromino *t);
int leftmost(Tetromino *t);
int rightmost(Tetromino *t);
int canRotate(int grid[ROWS][COLS], Tetromino *t, int rotated[4][4]);
int isGameOver(int grid[ROWS][COLS], Tetromino *t);
int rowIsFull(int r, int grid[ROWS][COLS]);
int blockIsFull(int r, int c, int grid[ROWS][COLS]);

#endif