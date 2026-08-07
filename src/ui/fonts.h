#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "colors.h"

// Font access
TTF_Font* getFontHuge();
TTF_Font* getFontLarge();
TTF_Font* getFontMedium();
TTF_Font* getFontSmall();
TTF_Font* getFontButton();

bool initFonts();
void closeFonts();

// Drawing helpers
void drawText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, const char *text, Color color = TEXT_COLOR);
void drawTextCentered(SDL_Renderer *renderer, TTF_Font *font, int x, int y, int w, int h, const char *text, Color color = TEXT_COLOR);
