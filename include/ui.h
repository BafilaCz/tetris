#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "tetris.h"

typedef struct {
    SDL_Rect rect;
    const char* label;
    int hovered;
    int clicked;
    int active;
    SDL_Color activeColor;
} Button;

void sdl_draw_text(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, SDL_Rect location, const char* text);
Button createButton(int x, int y, int w, int h, const char* label);
void createButtonsUI(Button* buttons[], int count, TTF_Font* font);
void handleButtonEvent(Button* b, SDL_Event* e);
void setActiveButton(Button* buttons[], int count, Button* selectedButton, SDL_Color activeColor);
void renderButton(SDL_Renderer* renderer, TTF_Font* font, Button* b, SDL_Color color, SDL_Color colorHover);
void renderStateUI(SDL_Renderer* renderer, TTF_Font* font, Button* buttons[], int count, SDL_Color color, SDL_Color colorHover);

#endif