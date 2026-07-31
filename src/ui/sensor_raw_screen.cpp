#include "sensor_raw_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include <cstdio>

namespace {
    Button backBtn{{20, 720, 210, 60}, "Back"};
}

ScreenResult handleSensorRawInput(int tx, int ty) {
    if (backBtn.contains(tx, ty)) {
        return {Screen::MAIN, true};
    }
    return {Screen::SENSOR_RAW, false};
}

void renderSensorRawScreen(SDL_Renderer *renderer, ExcavatorState *state) {
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 50};
    SDL_SetRenderDrawColor(renderer, 40, 50, 50, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    if (getFontMedium()) {
        drawTextCentered(renderer, getFontMedium(), 0, 0, SCREEN_WIDTH, 50, "RAW SENSOR DATA", ACCENT_COLOR);
    }

    const char *sensorNames[] = {
        "Superstructure",
        "Boom A",
        "Boom B", 
        "Stick",
        "Tilt",
        "Test"
    };

    int rowHeight = 105;
    int startY = 60;

    for (int i = 0; i < NUM_SENSORS; i++) {
        Sensor *s = &state->sensors[i];
        int y = startY + i * rowHeight;

        SDL_Rect row = {10, y, SCREEN_WIDTH - 20, rowHeight - 5};
        if (s->connected) {
            SDL_SetRenderDrawColor(renderer, 40, 50, 40, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
        }
        SDL_RenderFillRect(renderer, &row);
        SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
        SDL_RenderDrawRect(renderer, &row);

        if (getFontSmall()) {
            // Name and ID on first line
            char nameBuf[32];
            snprintf(nameBuf, sizeof(nameBuf), "%s (ID:%d)", sensorNames[i], s->id);
            drawText(renderer, getFontSmall(), 20, y + 10, nameBuf);

            // Status on right of first line
            const char *status = s->connected ? "OK" : "NC";
            Color statusColor = s->connected ? Color{100, 200, 100, 255} : Color{200, 100, 100, 255};
            drawText(renderer, getFontSmall(), SCREEN_WIDTH - 60, y + 10, status, statusColor);

            // Roll and Pitch on second line
            char xBuf[32], yBuf[32];
            if (s->connected) {
                snprintf(xBuf, sizeof(xBuf), "X: %.1f", s->roll);
                snprintf(yBuf, sizeof(yBuf), "Y: %.1f", s->pitch);
            } else {
                snprintf(xBuf, sizeof(xBuf), "X: --");
                snprintf(yBuf, sizeof(yBuf), "Y: --");
            }
            drawText(renderer, getFontSmall(), 20, y + 55, xBuf);
            drawText(renderer, getFontSmall(), 220, y + 55, yBuf);
        }
    }

    backBtn.draw(renderer);
}
