#pragma once

#include <SDL3/SDL.h>
#include "colors.h"
#include "fonts.h"

struct Button {
    SDL_Rect rect;
    const char *label;

    bool contains(int x, int y) const {
        return x >= rect.x && x <= rect.x + rect.w &&
               y >= rect.y && y <= rect.y + rect.h;
    }

    void draw(SDL_Renderer *renderer) const {
        SDL_SetRenderDrawColor(renderer, BTN_COLOR.r, BTN_COLOR.g, BTN_COLOR.b, BTN_COLOR.a);
        SDL_FRect frect = {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h};
        SDL_RenderFillRect(renderer, &frect);

        if (getFontButton()) {
            drawTextCentered(renderer, getFontButton(), rect.x, rect.y, rect.w, rect.h, label, {255, 255, 255, 255});
        }
    }
};
