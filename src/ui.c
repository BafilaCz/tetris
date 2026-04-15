#include "../include/ui.h"
#include <stdlib.h>

// UI fce
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