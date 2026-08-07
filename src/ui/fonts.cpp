#include "fonts.h"
#include <cstdio>
#include <cstring>

namespace {
    TTF_Font *fontHuge = nullptr;
    TTF_Font *fontLarge = nullptr;
    TTF_Font *fontMedium = nullptr;
    TTF_Font *fontSmall = nullptr;
    TTF_Font *fontButton = nullptr;
}

TTF_Font* getFontHuge() { return fontHuge; }
TTF_Font* getFontLarge() { return fontLarge; }
TTF_Font* getFontMedium() { return fontMedium; }
TTF_Font* getFontSmall() { return fontSmall; }
TTF_Font* getFontButton() { return fontButton; }

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

    const char *boldFontPaths[] = {
        "/usr/share/fonts/TTF/DejaVuSans-Bold.ttf",              // Arch
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",  // Debian
        "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans-Bold.ttf", // Fedora
        "/usr/share/fonts/TTF/Roboto-Bold.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf",
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

    const char *foundBoldPath = nullptr;
    for (int i = 0; boldFontPaths[i] != nullptr; i++) {
        FILE *f = fopen(boldFontPaths[i], "r");
        if (f) {
            fclose(f);
            foundBoldPath = boldFontPaths[i];
            break;
        }
    }

    fontHuge = TTF_OpenFont(foundBoldPath ? foundBoldPath : foundPath, 120);
    fontLarge = TTF_OpenFont(foundPath, 48);
    fontMedium = TTF_OpenFont(foundPath, 28);
    fontSmall = TTF_OpenFont(foundPath, 18);
    fontButton = TTF_OpenFont(foundBoldPath ? foundBoldPath : foundPath, 28);

    return fontHuge && fontLarge && fontMedium && fontSmall && fontButton;
}

void closeFonts() {
    if (fontHuge) TTF_CloseFont(fontHuge);
    if (fontLarge) TTF_CloseFont(fontLarge);
    if (fontMedium) TTF_CloseFont(fontMedium);
    if (fontSmall) TTF_CloseFont(fontSmall);
    if (fontButton) TTF_CloseFont(fontButton);
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
