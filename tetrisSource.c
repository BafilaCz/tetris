#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#define ROWS 20
#define COLS 10
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000



typedef struct {
    SDL_Rect rect;
    const char* label;
    int hovered;
    int clicked;
    int active;
    SDL_Color activeColor;
} Button;


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



void sdl_draw_text(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, SDL_Rect location, const char* text){
    // Vykreslení textu se zadaným fontem a barvou do obrázku (surface)
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    // Převod surface na hardwarovou texturu
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Vykreslení obrázku
    SDL_RenderCopy(renderer, texture, NULL, &location);

    // Uvolnění textury a surface
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}


// int fce
int randomInt(int min, int max){
    return rand() % (max - min + 1) + min;
}
int blockIsFull(int r, int c, int grid[ROWS][COLS]){
        return grid[r][c] == 2;
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


// UI fce
Button createButton(int x, int y, int w, int h, const char* label){
    Button b;
    b.rect.x = x;
    b.rect.y = y;
    b.rect.w = w;
    b.rect.h = h;
    b.label = label;
    b.hovered = 0;
    b.clicked = 0;
    b.active = 0;
    b.activeColor = (SDL_Color){255, 200, 255, 255};
    return b;
}
void createButtonsUI(Button* buttons[], int count, TTF_Font* font){
// funkce vytvoří "UI" pro určitý počet buttonů a to tak, že rozdělí okno na "počet + 2" částí a do všech kromě první a poslední zarovná 1 button.

    int* widths  = malloc(sizeof(int) * count);
    int* heights = malloc(sizeof(int) * count);

    // 1. ziskání výšky a šíčky textů
    for (int i = 0; i < count; i++) {
        TTF_SizeText(font, buttons[i]->label, &widths[i], &heights[i]);
    }

    // 2. spočítáńi výšky jednoho segmentu
    float segmentHeight = (float)WINDOW_HEIGHT / (count + 2);

    // 3. určení pozice buttonů
    for (int i = 0; i < count; i++) {
        
        buttons[i]->rect.w = widths[i];
        buttons[i]->rect.h = heights[i];

        // X = střed
        buttons[i]->rect.x = WINDOW_WIDTH/2 - widths[i]/2;

        // Y = střed segmentu (i+1)
        float centerY = segmentHeight * (i + 1.5f);
        buttons[i]->rect.y = (int)(centerY - heights[i]/2);
    }

    free(widths);
    free(heights);
}
void handleButtonEvent(Button* b, SDL_Event* e) {
    if (e->type == SDL_MOUSEMOTION) {

        int mx = e->motion.x;
        int my = e->motion.y;

        b->hovered = (mx >= b->rect.x && mx <= b->rect.x + b->rect.w &&
                      my >= b->rect.y && my <= b->rect.y + b->rect.h);
    }
    else if (e->type == SDL_MOUSEBUTTONDOWN) {

        int mx = e->button.x;
        int my = e->button.y;

        b->hovered = (mx >= b->rect.x && mx <= b->rect.x + b->rect.w &&
                      my >= b->rect.y && my <= b->rect.y + b->rect.h);

        if (b->hovered) {
            b->clicked = 1;
        }
    }
}
void setActiveButton(Button* buttons[], int count, Button* selectedButton, SDL_Color activeColor) {
    for (int i = 0; i < count; i++) {
        if (buttons[i] == selectedButton) {
            buttons[i]->active = 1;
            buttons[i]->activeColor = activeColor;
        } else {
            buttons[i]->active = 0;
        }
    }
}
void renderButton(SDL_Renderer* renderer, TTF_Font* font, Button* b, SDL_Color color, SDL_Color colorHover) {

    SDL_Color finalColor = color;

    if (b->active) {
        finalColor = b->activeColor;
    }
    else if (b->hovered) {
        finalColor = colorHover;
    }

    sdl_draw_text(renderer, font, finalColor, b->rect, b->label);
}
void renderStateUI(SDL_Renderer* renderer, TTF_Font* font, Button* buttons[], int count, SDL_Color color, SDL_Color colorHover){
    for (int i = 0; i < count; i++) {
        renderButton(renderer, font, buttons[i], color, colorHover);
    }
}
// score fce
void loadHighscore(int* highscore){
    FILE* f = fopen("highscores.txt", "r");
    if (!f){
        *highscore = 0;
        return;
    }
    fscanf(f, "%d", highscore);
    fclose(f);
}

void saveHighscore(int score, int* highscore){
        *highscore = score;

        FILE* f = fopen("highscores.txt", "w");
        if (!f) return;

        fprintf(f, "%d\n", *highscore);
        fclose(f);
}


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
// main
int main() {

    GameState state = STATE_MENU;

    // reset seedu pro randomizaci
    srand(time(NULL));


    SDL_Init(SDL_INIT_VIDEO);
    // toto by mělo fixnout nějake problemy s renderovanim na nekterych pc
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    TTF_Init();
    // audio
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    int masterVolume = 64; // = 50 % (0–128)
    Mix_Music* song1 = Mix_LoadMUS("./original.mp3");
    Mix_Music* song2 = Mix_LoadMUS("./tetorisu.mp3");
    Mix_Music* song3 = Mix_LoadMUS("./alpha_remix.mp3");
    Mix_Music* songs[3] = {song1, song2, song3};
    Mix_Music* currentSong = NULL;
    Mix_VolumeMusic(masterVolume);
    Mix_Volume(-1, masterVolume); // -1 = všechny kanály

    int sfxVolume = 128; // = 100 % (0-128)
    Mix_Chunk* sfxMove = Mix_LoadWAV("./move.wav");
    Mix_Chunk* sfxRotate = Mix_LoadWAV("./rotate.wav");
    Mix_Chunk* sfxPlace = Mix_LoadWAV("./place.wav");
    Mix_Chunk* sfxScore = Mix_LoadWAV("./score.wav");
    Mix_Chunk* sfxGameOver = Mix_LoadWAV("./gameover.wav");
    Mix_VolumeChunk(sfxMove, sfxVolume);
    Mix_VolumeChunk(sfxRotate, sfxVolume);
    Mix_VolumeChunk(sfxPlace, sfxVolume);
    Mix_VolumeChunk(sfxScore, sfxVolume);
    Mix_VolumeChunk(sfxGameOver, sfxVolume);



    
    SDL_Window *window = SDL_CreateWindow(
        // name posX posY w h flags
        "Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // něco co by mělo zlepšit kvalitu textu
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    //

    TTF_Font* fontSmall  = TTF_OpenFont("./Arial.ttf", 20);
    TTF_Font* fontMedium = TTF_OpenFont("./Arial.ttf", 30);
    TTF_Font* fontLarge  = TTF_OpenFont("./Arial.ttf", 50);


    int score = 0;
    int highscore = 0;
    loadHighscore(&highscore);


    char scoreTextBuffer[64];
    char highscoreTextBuffer[64];
    char escTextBuffer[4];
    // barvy
    SDL_Color color1 = {255, 0, 0, 255};
    SDL_Color color2 = {0, 255, 0, 255};
    SDL_Color color3 = {0, 0, 255, 255};
    SDL_Color color4 = {255, 255, 0, 255};
    SDL_Color color5 = {255, 0, 255, 255};
    SDL_Color color6 = {0, 255, 255, 255};
    SDL_Color color7 = {140, 20, 255, 255};
    SDL_Color colorFull = {255, 255, 255, 255};
    SDL_Color colorText = {255, 255, 255, 255};
    SDL_Color colorGrid = {20, 20, 20, 255};
    SDL_Color colorBckgrnd = {0, 0, 0, 255};
    SDL_Color colorHover = {100, 100, 100, 255};



    SDL_Color colors[7] = {color1,color2,color3,color4,color5,color6,color7,};

    SDL_Rect scoreRect = {
        WINDOW_WIDTH - 200,   // X pozice (200 px od pravého okraje)
        20,                   // Y pozice
        150,                  // šířka
        40                    // výška
    };
    SDL_Rect highscoreRect = {
        WINDOW_WIDTH - 200,   // X pozice (200 px od pravého okraje)
        80,                   // Y pozice
        150,                  // šířka
        40                    // výška
    };
    
    SDL_Rect escRect = {
        20,   // X pozice (200 px od pravého okraje)
        20,                   // Y pozice
        60,                  // šířka
        40                    // výška
    };


    // vytvoření buttonů + jejich UI v konkretnich states
    // menu
    Button btnPlay = createButton(0,0,0,0,"PLAY");
    Button btnSettings = createButton(0,0,0,0,"SETTINGS");
    Button btnQuit = createButton(0,0,0,0,"QUIT");
    Button* menuButtons[] = {&btnPlay, &btnSettings, &btnQuit};
    int menuButtonCount = 3;
    createButtonsUI(menuButtons, menuButtonCount, fontLarge);
        // settings
        Button btnAudio = createButton(0,0,0,0,"AUDIO");
        Button btnDifficulty = createButton(0,0,0,0,"DIFFICULTY");
        Button btnBackSettings = createButton(0,0,0,0,"BACK");
        Button* settingsButtons[] = {&btnAudio, &btnDifficulty, &btnBackSettings};
        int settingsButtonCount = 3;
        createButtonsUI(settingsButtons, settingsButtonCount, fontLarge);
            // audio
            Button btnVolume = createButton(0,0,0,0,"VOLUME");
            Button btnMusic = createButton(0,0,0,0,"MUSIC");
            Button btnBackAudio = createButton(0,0,0,0,"BACK");
            Button* audioButtons[] = {&btnVolume, &btnMusic, &btnBackAudio};
            int audioButtonCount = 3;
            createButtonsUI(audioButtons, audioButtonCount, fontLarge);
                // volume    
                Button btnPlus = createButton(0,0,0,0,"+");
                Button btnVolumeTag = createButton(0,0,0,0,"VOLUME");
                Button btnMinus = createButton(0,0,0,0,"-");
                Button btnBackVolume = createButton(0,0,0,0,"BACK");
                Button* volumeButtons[] = {&btnPlus, &btnMinus, &btnBackVolume};
                int volumeButtonCount = 3;
                createButtonsUI(volumeButtons, volumeButtonCount, fontLarge);
                // music   
                Button btnSong1 = createButton(0,0,0,0,"Original Tetris Theme");
                Button btnSong2 = createButton(0,0,0,0,"Tetorisu");
                Button btnSong3 = createButton(0,0,0,0,"Alpha Remix");
                Button btnBackMusic = createButton(0,0,0,0,"BACK");
                Button* musicButtons[] = {&btnSong1, &btnSong2, &btnSong3, &btnBackMusic};
                int musicButtonCount = 4;
                createButtonsUI(musicButtons, musicButtonCount, fontLarge);
                setActiveButton(musicButtons, musicButtonCount, &btnSong1, color6);
                currentSong = songs[0];
            // difficulty
            Button btnEasy = createButton(0,0,0,0,"EASY");
            Button btnMedium = createButton(0,0,0,0,"MEDIUM");
            Button btnHard = createButton(0,0,0,0,"HARD");
            Button btnBackDifficulty = createButton(0,0,0,0,"BACK");
            Button* difficultyButtons[] = {&btnEasy, &btnMedium, &btnHard, &btnBackDifficulty};
            int difficultyButtonCount = 4;
            createButtonsUI(difficultyButtons, difficultyButtonCount, fontLarge);
            setActiveButton(difficultyButtons, difficultyButtonCount, &btnEasy, color2);
    //

    int running = 1;
    SDL_Event event;
    Uint32 lastTime = SDL_GetTicks();
    Uint32 lastFallTime = SDL_GetTicks();
    int fallDelay = 500; // pád každých 500 ms


    int grid[ROWS][COLS] = {0};
    int nextGrid[4][4] = {0};
    int nextGridMarginX = 40;
    int nextGridMarginY = 120;
    int cellSize = 40;
    int cellBorderSize = 2;

    int currentShapeIndex = randomInt(0,6);
    int nextShapeIndex = randomInt(0,6);
    while (nextShapeIndex == currentShapeIndex){
        nextShapeIndex = randomInt(0,6);
    }
    

    Tetromino t;
    t.row = 0;              // začátek nahoře    
    t.col = COLS/2 - 2;     // zhruba uprostřed
    t.active = 1;
    t.currentShapeIndex = currentShapeIndex;
    t.color = colors[currentShapeIndex];
    changeShape(&t, t.currentShapeIndex);
    drawTetromino(grid, &t);

    GameState prevState = state;


    // GAME LOOP
    while (running) {

        
        if (prevState == STATE_GAME && state != STATE_GAME) {
            Mix_HaltMusic();
        }
        prevState = state;

        // automatické padání tetromina
        if (state == STATE_GAME){
            Uint32 now = SDL_GetTicks();
            if (now - lastFallTime >= fallDelay) {
                moveTetromino(grid, &t, 'd', sfxMove);
                lastFallTime = now;

                if (!t.active) {
                    placeTetromino(grid, &t);
                    SDL_Delay(100);
                    Mix_PlayChannel(-1, sfxPlace, 0);

                    for (int i = 0; i < ROWS; i++) {
                        if (rowIsFull(i, grid)) {
                            moveAllDown(grid, i);
                            score++;
                            if (highscore < score)
                            {
                                highscore ++;
                            }
                            
                            Mix_PlayChannel(-1, sfxScore, 0);

                        }
                    }

                    respawnTetromino(grid, &t, nextShapeIndex, colors);

                    currentShapeIndex = nextShapeIndex;
                    nextShapeIndex = randomInt(0, 6);
                    while (nextShapeIndex == currentShapeIndex)
                        nextShapeIndex = randomInt(0, 6);

                    if (isGameOver(grid, &t)) {
                        saveHighscore(score, &highscore);
                        Mix_PlayChannel(-1, sfxGameOver, 0);

                        resetGame(grid, &t, &score, colors);
                    } else {
                        drawTetromino(grid, &t);
                    }
                }
            }
        }


        // EVENT LOOP
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                running = 0;
            }

            switch (state) {

            case STATE_MENU:
                for (int i = 0; i < menuButtonCount; i++) {
                    handleButtonEvent(menuButtons[i], &event);
                }

                if (btnPlay.clicked) {
                    state = STATE_GAME;
                    btnPlay.clicked = 0;
                    if (!Mix_PlayingMusic()){
                        Mix_PlayMusic(currentSong, -1);
                    }
                }

                if (btnSettings.clicked) {
                    state = STATE_SETTINGS;
                    btnSettings.clicked = 0;
                }

                if (btnQuit.clicked) {
                    running = 0;
                    btnQuit.clicked = 0;
                }
                break;

            case STATE_SETTINGS:
                for (int i = 0; i < settingsButtonCount; i++) {
                    handleButtonEvent(settingsButtons[i], &event);
                }

                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) { 
                    state = STATE_MENU;
                }

                if (btnAudio.clicked) {
                    state = STATE_AUDIO;
                    btnAudio.clicked = 0;
                }

                if (btnDifficulty.clicked) {
                    state = STATE_DIFFICULTY;
                    btnDifficulty.clicked = 0;
                }

                if (btnBackSettings.clicked) {
                    state = STATE_MENU;
                    btnBackSettings.clicked = 0;
                }
                break;

            case STATE_AUDIO:
                for (int i = 0; i < audioButtonCount; i++) {
                    handleButtonEvent(audioButtons[i], &event);
                }

                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) { 
                    state = STATE_SETTINGS;
                }

                if (btnVolume.clicked) {
                    state = STATE_VOLUME;
                    btnVolume.clicked = 0;
                }
                if (btnMusic.clicked) {
                    state = STATE_MUSIC;
                    btnMusic.clicked = 0;
                }

                if (btnBackAudio.clicked) {
                    state = STATE_SETTINGS;
                    btnBackAudio.clicked = 0;
                }
                break;

            case STATE_VOLUME:
                for (int i = 0; i < volumeButtonCount; i++) {
                    handleButtonEvent(volumeButtons[i], &event);
                }

                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) { 
                    state = STATE_AUDIO;
                }

            if (btnPlus.clicked) {
                masterVolume += 8;
                if (masterVolume > MIX_MAX_VOLUME)
                    masterVolume = MIX_MAX_VOLUME;

                Mix_VolumeMusic(masterVolume);
                Mix_Volume(-1, masterVolume);

                btnPlus.clicked = 0;
            }

            if (btnMinus.clicked) {
                masterVolume -= 8;
                if (masterVolume < 0)
                    masterVolume = 0;

                Mix_VolumeMusic(masterVolume);
                Mix_Volume(-1, masterVolume);

                btnMinus.clicked = 0;
            }

                if (btnBackVolume.clicked) {
                    state = STATE_AUDIO;
                    btnBackVolume.clicked = 0;
                }
                break;


            case STATE_MUSIC:
                for (int i = 0; i < musicButtonCount; i++) {
                    handleButtonEvent(musicButtons[i], &event);
                }

                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) { 
                    state = STATE_AUDIO;
                }

                if (btnSong1.clicked) {
                    setActiveButton(musicButtons, musicButtonCount, &btnSong1, color6);
                    printf("s1");
                    currentSong = songs[0];
                    btnSong1.clicked = 0;
                }
                if (btnSong2.clicked) {
                    setActiveButton(musicButtons, musicButtonCount, &btnSong2, color6);
                    printf("s2");
                    currentSong = songs[1];
                    btnSong2.clicked = 0;
                }

                if (btnSong3.clicked) {
                    setActiveButton(musicButtons, musicButtonCount, &btnSong3, color6);
                    printf("s3");
                    currentSong = songs[2];
                    btnSong3.clicked = 0;
                }
                if (btnBackMusic.clicked) {
                    state = STATE_AUDIO;
                    btnBackMusic.clicked = 0;
                }
                break;

            case STATE_DIFFICULTY:
                for (int i = 0; i < difficultyButtonCount; i++) {
                    handleButtonEvent(difficultyButtons[i], &event);
                }

                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) { 
                    state = STATE_SETTINGS;
                }

                if (btnEasy.clicked) {
                    setActiveButton(difficultyButtons, difficultyButtonCount, &btnEasy, color2);
                    fallDelay = 500;
                    btnEasy.clicked = 0;
                }
                if (btnMedium.clicked) {
                    setActiveButton(difficultyButtons, difficultyButtonCount, &btnMedium, color4);
                    fallDelay = 300;
                    btnMedium.clicked = 0;
                }

                if (btnHard.clicked) {
                    setActiveButton(difficultyButtons, difficultyButtonCount, &btnHard, color1);
                    fallDelay = 100;
                    btnHard.clicked = 0;
                }
                if (btnBackDifficulty.clicked) {
                    state = STATE_SETTINGS;
                    btnBackDifficulty.clicked = 0;
                }
                break;

            case STATE_GAME:
                if (event.type == SDL_KEYDOWN) {

                    // MOVE DOWN
                    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
                        moveTetromino(grid, &t, 'd', sfxMove);
                    }

                    // MOVE LEFT
                    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
                        moveTetromino(grid, &t, 'l', sfxMove);
                    }

                    // MOVE RIGHT
                    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
                        moveTetromino(grid, &t, 'r', sfxMove);
                    }

                    // ROTATE
                    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
                        if (event.key.repeat == 0){
                            rotateTetromino(grid, &t);
                            Mix_PlayChannel(-1, sfxRotate, 0);
                        }
                        

                    }

                    // ESCAPE -> RETURN TO MENU
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        saveHighscore(score, &highscore);
                        resetGame(grid, &t, &score, colors);
                        state = STATE_MENU;
                    }
                }
                break;

            case STATE_QUIT:
                running = 0;
                break;

            }
        }


        // DELTA TIME
        Uint32 currentTime = SDL_GetTicks();
        lastTime = currentTime;

        // RENDER
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (state == STATE_MENU) {
            renderStateUI(renderer, fontLarge, menuButtons, menuButtonCount, colorText, colorHover);
        }
        else if (state == STATE_SETTINGS) {
            renderStateUI(renderer, fontLarge, settingsButtons, settingsButtonCount, colorText, colorHover);
        }
        else if (state == STATE_AUDIO) {
            renderStateUI(renderer, fontLarge, audioButtons, audioButtonCount, colorText, colorHover);
        }
        else if (state == STATE_VOLUME) {
            renderStateUI(renderer, fontLarge, volumeButtons, volumeButtonCount, colorText, colorHover);

            // manualni pridani textu zobrazujiciho volume (0-16)
            char volText[32];
            sprintf(volText, "VOLUME: %d", masterVolume/8);
            SDL_Rect volRect;
            volRect.w = 300;
            volRect.h = 50;
            volRect.x = WINDOW_WIDTH/2 - volRect.w/2;
            volRect.y = WINDOW_HEIGHT/2 - 120; // nad plus/minus

            sdl_draw_text(renderer, fontLarge, colorText, volRect, volText);
        }

        else if (state == STATE_MUSIC) {
            renderStateUI(renderer, fontLarge, musicButtons, musicButtonCount, colorText, colorHover);
        }
        else if (state == STATE_DIFFICULTY) {
            renderStateUI(renderer, fontLarge, difficultyButtons, difficultyButtonCount, colorText, colorHover);
        }
        else if (state == STATE_GAME) {

            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {

                    if (grid[r][c] == 0)
                        SDL_SetRenderDrawColor(renderer, colorGrid.r, colorGrid.g, colorGrid.b, colorGrid.a);
                    else if (grid[r][c] == 1)
                        SDL_SetRenderDrawColor(renderer, t.color.r, t.color.g, t.color.b, t.color.a);
                    else if (grid[r][c] == 2)
                        SDL_SetRenderDrawColor(renderer, colorFull.r, colorFull.g, colorFull.b, colorFull.a);

                    SDL_Rect cell = {
                        c * cellSize + (WINDOW_WIDTH - cellSize * COLS) / 2,
                        r * cellSize + (WINDOW_HEIGHT - cellSize * ROWS),
                        cellSize - cellBorderSize,
                        cellSize - cellBorderSize
                    };
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {

                    if (SHAPES[nextShapeIndex][r][c] == 0)
                        SDL_SetRenderDrawColor(renderer, colorBckgrnd.r, colorBckgrnd.g, colorBckgrnd.b, colorBckgrnd.a);

                    if (SHAPES[nextShapeIndex][r][c] == 1)
                        SDL_SetRenderDrawColor(renderer, colors[nextShapeIndex].r, colors[nextShapeIndex].g, colors[nextShapeIndex].b, colors[nextShapeIndex].a);

                    SDL_Rect cell = {
                        c * cellSize + ((WINDOW_WIDTH - cellSize * 4) - nextGridMarginX),
                        r * cellSize + nextGridMarginY,
                        cellSize - cellBorderSize,
                        cellSize - cellBorderSize
                    };

                    SDL_RenderFillRect(renderer, &cell);
                }
            }

            sprintf(scoreTextBuffer, "Score: %d", score);
            sprintf(highscoreTextBuffer, "Best: %d", highscore);
            sprintf(escTextBuffer, "ESC");
            sdl_draw_text(renderer, fontSmall, colorText, scoreRect, scoreTextBuffer);
            sdl_draw_text(renderer, fontSmall, colorText, highscoreRect, highscoreTextBuffer);
            sdl_draw_text(renderer, fontSmall, colorText, escRect, escTextBuffer);
        }

        SDL_RenderPresent(renderer);
    }

    // Cleanup

    Mix_FreeChunk(sfxGameOver);
    Mix_FreeChunk(sfxScore);
    Mix_FreeChunk(sfxPlace);
    Mix_FreeChunk(sfxRotate);
    Mix_FreeChunk(sfxMove);
    Mix_FreeMusic(song3);
    Mix_FreeMusic(song2);
    Mix_FreeMusic(song1);
    TTF_CloseFont(fontLarge);
    TTF_CloseFont(fontMedium);
    TTF_CloseFont(fontSmall);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}
