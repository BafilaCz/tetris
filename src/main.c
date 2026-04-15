#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../include/tetris.h"
#include "../include/ui.h"
#include "../include/utils.h"
#include "../include/score.h"

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
    Mix_Music* song1 = Mix_LoadMUS("../assets/original.mp3");
    Mix_Music* song2 = Mix_LoadMUS("../assets/tetorisu.mp3");
    Mix_Music* song3 = Mix_LoadMUS("../assets/alpha_remix.mp3");
    Mix_Music* songs[3] = {song1, song2, song3};
    Mix_Music* currentSong = NULL;
    Mix_VolumeMusic(masterVolume);
    Mix_Volume(-1, masterVolume); // -1 = všechny kanály

    int sfxVolume = 128; // = 100 % (0-128)
    Mix_Chunk* sfxMove = Mix_LoadWAV("../assets/move.wav");
    Mix_Chunk* sfxRotate = Mix_LoadWAV("../assets/rotate.wav");
    Mix_Chunk* sfxPlace = Mix_LoadWAV("../assets/place.wav");
    Mix_Chunk* sfxScore = Mix_LoadWAV("../assets/score.wav");
    Mix_Chunk* sfxGameOver = Mix_LoadWAV("../assets/gameover.wav");
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

    TTF_Font* fontSmall  = TTF_OpenFont("../assets/Arial.ttf", 20);
    TTF_Font* fontMedium = TTF_OpenFont("../assets/Arial.ttf", 30);
    TTF_Font* fontLarge  = TTF_OpenFont("../assets/Arial.ttf", 50);


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
    
    return 0;
}