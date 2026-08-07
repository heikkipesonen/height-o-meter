#pragma once

#include <SDL3/SDL.h>
#include "colors.h"
#include "fonts.h"

struct ButtonStyle {
    Color bg = BTN_COLOR;
    Color text = TEXT_COLOR;
};

struct Button {
    SDL_Rect rect;
    const char *label;
    ButtonStyle style;

    bool contains(int x, int y) const {
        return x >= rect.x && x <= rect.x + rect.w &&
               y >= rect.y && y <= rect.y + rect.h;
    }

    void draw(SDL_Renderer *renderer) const {
        SDL_SetRenderDrawColor(renderer, style.bg.r, style.bg.g, style.bg.b, style.bg.a);
        SDL_FRect frect = {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h};
        SDL_RenderFillRect(renderer, &frect);

        if (getFontButton()) {
            drawTextCentered(renderer, getFontButton(), rect.x, rect.y, rect.w, rect.h, label, style.text);
        }
    }
};
