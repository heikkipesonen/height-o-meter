#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct Color {
    uint8_t r, g, b, a;
};

constexpr Color BG_COLOR = {30, 30, 30, 255};
constexpr Color TEXT_COLOR = {220, 220, 220, 255};
constexpr Color ACCENT_COLOR = {80, 140, 200, 255};
constexpr Color BTN_COLOR = {60, 60, 80, 255};

// Font access
TTF_Font* getFontLarge();
TTF_Font* getFontMedium();
TTF_Font* getFontSmall();

bool initFonts();
void closeFonts();

// Drawing helpers
void drawText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, const char *text, Color color = TEXT_COLOR);
void drawTextCentered(SDL_Renderer *renderer, TTF_Font *font, int x, int y, int w, int h, const char *text, Color color = TEXT_COLOR);
void drawValueBox(SDL_Renderer *renderer, int x, int y, const char *label, double value, int width = 300, int height = 120);
