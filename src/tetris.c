#include "../include/tetris.h"
#include <string.h>

int SHAPES[7][4][4] = {
    // O
    {
        {0,0,0,0},
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0}
    },
    // I
    {
        {0,0,0,0},
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0}
    },
    // T
    {
        {0,0,0,0},
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0}
    },
    // Z
    {
        {0,0,0,0},
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0}
    },
    // S
    {
        {0,0,0,0},
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0}
    },
    // L
    {
        {0,0,0,0},
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0}
    },
    // J
    {
        {0,0,0,0},
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0}
    }
};

// tetris fce
void applyWallKick(Tetromino *t, int rotated[4][4]) {
    int left = 4;
    int right = 0;

    // najde levý a pravý okraj otočeného tvaru
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (rotated[r][c] == 1) {
                if (c < left) left = c;
                if (c > right) right = c;
            }
        }
    }

    // posun doleva
    if (t->col + left < 0) {
        t->col = -left;
    }

    // posun doprava
    if (t->col + right >= COLS) {
        t->col = COLS - 1 - right;
    }
}

void shiftAfterRotation(Tetromino *t) {
    int left = leftmost(t);
    int right = rightmost(t);

    // posun doleva
    if (t->col + left < 0) {
        t->col = -left;
    }

    // posun doprava
    if (t->col + right >= COLS) {
        t->col = COLS - 1 - right;
    }
}

void changeShape(Tetromino *t, int shapeIndex){
    if (shapeIndex < (sizeof(SHAPES)/sizeof(SHAPES[0]))){
        memcpy(t->shape, SHAPES[shapeIndex], sizeof(SHAPES[0]));
    }
    
     
}

void changeBlockState(int r, int c, int grid[ROWS][COLS], int state){
    grid[r][c] = state;
}

void deleteTetromino(int grid[ROWS][COLS], Tetromino *t){
    // smazat staré pozice tetromina v gridu
    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++) {
            if (t->shape[r][c] == 1) {
                int gridRow = t->row + r;
                int gridCol = t->col + c;
                changeBlockState(gridRow, gridCol, grid, 0);
            }
        }
    }
}

void drawTetromino(int grid[ROWS][COLS], Tetromino *t) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (t->shape[r][c] == 1) {
                int gridRow  = t->row + r;
                int gridCol = t->col + c;
                if (gridRow >= 0 && gridRow < ROWS && gridCol >= 0 && gridCol < COLS){
                    grid[gridRow][gridCol] = 1;
                }

            }
        }
    }
}

void moveAllDown(int grid[ROWS][COLS], int lastRow){
    // posune všechny řádky nad lastRow o 1 dolů
    for (int r = lastRow; r > 0; r--){
        for (int c = 0; c < COLS; c++){
            // zkopíruje stav z řádku nad tímto
            if(blockIsFull(r-1, c, grid)){
                changeBlockState(r, c, grid, 2);
            } 
            else{
                changeBlockState(r, c, grid, 0);
            }
        }
    }
    // vymazani horniho řádku
    for (int c = 0; c < COLS; c++){
        changeBlockState(0, c, grid, 0);
    }
}

void moveTetromino(int grid[ROWS][COLS], Tetromino *t, char direction, Mix_Chunk* sfx) {

    int oldRow = t->row;
    int oldCol = t->col;

    if (direction == 'd') {
        if (!canMoveDown(grid, t)) {
            t->active = 0;
            return;
        }
    }
    deleteTetromino(grid, t);

    if (direction == 'd') {
        t->row += 1;
    }
    else if (direction == 'l') {
        if (canMoveLeft(grid, t)) t->col -= 1;
    }
    else if (direction == 'r') {
        if (canMoveRight(grid, t)) t->col += 1;
    }

    // zvuk se prehraje jen kdyz se tetromino realně pohne (zabrani to loopujicimu se zvuku [drrrrrrrrrrrrrr])
    if (t->row != oldRow || t->col != oldCol) {
        Mix_PlayChannel(-1, sfx, 0);
    }

    drawTetromino(grid, t);
}

void placeTetromino(int grid[ROWS][COLS], Tetromino *t){
    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++){
            if (t->shape[r][c] == 1){
                int gridRow = t->row + r;
                int gridCol = t->col + c;
                changeBlockState(gridRow, gridCol, grid, 2);
            }
        }
    }
}

void rotateTetromino(int grid[ROWS][COLS], Tetromino *t) {
    int rotated[4][4] = {0};

    // vytvoření otočené verze
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            rotated[c][3 - r] = t->shape[r][c];

    // mazání staré pozice z gridu
    deleteTetromino(grid, t);

    // wall kick
    applyWallKick(t, rotated);

    // kontrola kolize otočeného tvaru
    if (!canRotate(grid, t, rotated)) {
        // rotace nejde -> nastaví starý tvar zpět do gridu
        drawTetromino(grid, t);
        return;
    }

    //uložení otočené matice do tvaru
    memcpy(t->shape, rotated, sizeof(rotated));

    // vykreslení nové pozice
    drawTetromino(grid, t);
}

void respawnTetromino(int grid[ROWS][COLS], Tetromino *t, int randomShapeIndex, SDL_Color colors[]){
    t->row = -1;
    t->col = COLS/2 - 2;
    t->active = 1;
    t->currentShapeIndex = randomShapeIndex;
    if (t->currentShapeIndex >= 7){
        t->currentShapeIndex = 0;
    }
        t->color = colors[t->currentShapeIndex];
    changeShape(t, t->currentShapeIndex);
}

void resetGame(int grid[ROWS][COLS], Tetromino *t, int *score, SDL_Color colors[]) {
    // vymazat grid
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            grid[r][c] = 0;
        }
    }

    // reset skóre
    *score = 0;

    // reset tetromina
    respawnTetromino(grid, t, t->currentShapeIndex, colors);
    changeShape(t, t->currentShapeIndex);
    drawTetromino(grid, t);
}

// int fce
int canMoveLeft(int grid[ROWS][COLS], Tetromino *t){
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (t->shape[r][c] == 1) {
                int newCol = t->col + c - 1;
                int newRow = t->row + r;

                // mimo hranici vlevo
                if (newCol < 0) {
                    return 0;
                }
                // kolize s plným blokem
                if (blockIsFull(newRow, newCol, grid)){
                    return 0;
                }
            }
        }
    }
    return 1;
}

int canMoveRight(int grid[ROWS][COLS], Tetromino *t){
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (t->shape[r][c] == 1) {
                int newCol = t->col + c + 1;
                int newRow = t->row + r;

                if (newCol >= COLS) {
                    return 0;
                }

                if (blockIsFull(newRow, newCol, grid)) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

int canMoveDown(int grid[ROWS][COLS], Tetromino *t){
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (t->shape[r][c] == 1) {
                int newCol = t->col + c;
                int newRow = t->row + r + 1;

                if (newRow >= ROWS) {
                    return 0;
                }

                if (blockIsFull(newRow, newCol, grid)) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

int leftmost(Tetromino *t){
    int min = 4;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (t->shape[r][c] == 1 && c < min) {
                min = c;
            }
        }
    }
    return min;
}

int rightmost(Tetromino *t){
    int max = 0;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (t->shape[r][c] == 1 && c > max) {
                max = c;
            }
        }
    }
    return max;
}

int canRotate(int grid[ROWS][COLS], Tetromino *t, int rotated[4][4]){
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (rotated[r][c] == 1) {
                int newRow = t->row + r;
                int newCol = t->col + c;

                // mimo grid
                if (newRow < 0 || newRow >= ROWS || newCol < 0 || newCol >= COLS)
                    return 0;

                // kolize s plným blokem
                if (grid[newRow][newCol] == 2)
                    return 0;
            }
        }
    }
    return 1;
}

int isGameOver(int grid[ROWS][COLS], Tetromino *t) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (t->shape[r][c] == 1) {
                int gr = t->row + r;
                int gc = t->col + c;

                if (gr >= 0 && grid[gr][gc] == 2) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int rowIsFull(int r, int grid[ROWS][COLS]){

        for (int c = 0; c < COLS; c++)
        {
            if (grid[r][c] != 2){
                return 0;
            }
        }
        return 1;
}

int blockIsFull(int r, int c, int grid[ROWS][COLS]){
        return grid[r][c] == 2;
}