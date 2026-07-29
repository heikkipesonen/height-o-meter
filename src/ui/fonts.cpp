#include "fonts.h"
#include <cstdio>
#include <cstring>

namespace {
    TTF_Font *fontLarge = nullptr;
    TTF_Font *fontMedium = nullptr;
    TTF_Font *fontSmall = nullptr;
}

TTF_Font* getFontLarge() { return fontLarge; }
TTF_Font* getFontMedium() { return fontMedium; }
TTF_Font* getFontSmall() { return fontSmall; }

bool initFonts() {
    if (TTF_Init() < 0) {
        printf("TTF init failed: %s\n", TTF_GetError());
        return false;
    }

    const char *fontPaths[] = {
        "/usr/share/fonts/TTF/DejaVuSans.ttf",              // Arch
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Debian
        "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf", // Fedora
        "/usr/share/fonts/TTF/Roboto-Regular.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        nullptr
    };

    const char *foundPath = nullptr;
    for (int i = 0; fontPaths[i] != nullptr; i++) {
        FILE *f = fopen(fontPaths[i], "r");
        if (f) {
            fclose(f);
            foundPath = fontPaths[i];
            break;
        }
    }

    if (!foundPath) {
        printf("No font found. Install dejavu fonts.\n");
        return false;
    }

    fontLarge = TTF_OpenFont(foundPath, 48);
    fontMedium = TTF_OpenFont(foundPath, 28);
    fontSmall = TTF_OpenFont(foundPath, 18);

    return fontLarge && fontMedium && fontSmall;
}

void closeFonts() {
    if (fontLarge) TTF_CloseFont(fontLarge);
    if (fontMedium) TTF_CloseFont(fontMedium);
    if (fontSmall) TTF_CloseFont(fontSmall);
    TTF_Quit();
}

void drawText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, const char *text, Color color) {
    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, sdlColor);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dest);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void drawTextCentered(SDL_Renderer *renderer, TTF_Font *font, int x, int y, int w, int h, const char *text, Color color) {
    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, sdlColor);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest = {
        x + (w - surface->w) / 2,
        y + (h - surface->h) / 2,
        surface->w,
        surface->h
    };
    SDL_RenderCopy(renderer, texture, nullptr, &dest);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void drawValueBox(SDL_Renderer *renderer, int x, int y, const char *label, double value, int width, int height) {
    SDL_Rect box = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, ACCENT_COLOR.r, ACCENT_COLOR.g, ACCENT_COLOR.b, ACCENT_COLOR.a);
    SDL_RenderDrawRect(renderer, &box);

    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), x + 10, y + 5, label, ACCENT_COLOR);
    }

    char valueStr[32];
    snprintf(valueStr, sizeof(valueStr), "%.1f", value);
    if (getFontLarge()) {
        drawTextCentered(renderer, getFontLarge(), x, y + 30, width, height - 30, valueStr);
    }
}
