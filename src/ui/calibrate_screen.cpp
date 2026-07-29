#include "calibrate_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include <cstdio>

namespace {
    Button backBtn{{50, 400, 150, 60}, "Back"};
    Button zeroBtn{{SCREEN_WIDTH/2 - 75, 400, 150, 60}, "Zero"};
}

ScreenResult handleCalibrateInput(int tx, int ty, ExcavatorState *state) {
    if (backBtn.contains(tx, ty)) {
        return {Screen::MAIN, true};
    } else if (zeroBtn.contains(tx, ty)) {
        // TODO: implement zero calibration
        return {Screen::CALIBRATE, true};
    }
    return {Screen::CALIBRATE, false};
}

void drawValueBox(SDL_Renderer *renderer, int x, int y, const char *label, double value, int w, int h) {
    SDL_Rect box = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, 40, 40, 55, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
    SDL_RenderDrawRect(renderer, &box);

    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), x + 10, y + 8, label, ACCENT_COLOR);
    }
    if (getFontLarge()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.0f", value);
        drawTextCentered(renderer, getFontLarge(), x, y + 30, w, h - 30, buf);
    }
}

void renderCalibrateScreen(SDL_Renderer *renderer, ExcavatorState *state) {
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 50};
    SDL_SetRenderDrawColor(renderer, 40, 50, 40, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    if (getFontMedium()) {
        drawTextCentered(renderer, getFontMedium(), 0, 0, SCREEN_WIDTH, 50, "CALIBRATE", ACCENT_COLOR);
    }

    if (getFontSmall()) {
        drawTextCentered(renderer, getFontSmall(), 0, 70, SCREEN_WIDTH, 30, "Position boom horizontally and press Zero");
    }

    drawValueBox(renderer, SCREEN_WIDTH/2 - 160, 120, "Reach", state->reach, 320, 110);
    drawValueBox(renderer, SCREEN_WIDTH/2 - 160, 250, "Depth", state->depth, 320, 110);

    backBtn.draw(renderer);
    zeroBtn.draw(renderer);
}
