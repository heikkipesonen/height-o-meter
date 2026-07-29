#include "sensor_raw_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include <cstdio>

namespace {
    Button backBtn{{50, 400, 150, 60}, "Back"};
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
        "Tilt Hitch",
        "Test"
    };

    int rowHeight = 55;
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
            char nameBuf[32];
            snprintf(nameBuf, sizeof(nameBuf), "%s (ID:%d)", sensorNames[i], s->id);
            drawText(renderer, getFontSmall(), 20, y + 8, nameBuf);

            char rollBuf[32], pitchBuf[32];
            if (s->connected) {
                snprintf(rollBuf, sizeof(rollBuf), "Roll: %.1f", s->roll);
                snprintf(pitchBuf, sizeof(pitchBuf), "Pitch: %.1f", s->pitch);
            } else {
                snprintf(rollBuf, sizeof(rollBuf), "Roll: --");
                snprintf(pitchBuf, sizeof(pitchBuf), "Pitch: --");
            }
            drawText(renderer, getFontSmall(), 300, y + 8, rollBuf);
            drawText(renderer, getFontSmall(), 500, y + 8, pitchBuf);

            const char *status = s->connected ? "OK" : "NC";
            Color statusColor = s->connected ? Color{100, 200, 100, 255} : Color{200, 100, 100, 255};
            drawText(renderer, getFontSmall(), 700, y + 8, status, statusColor);
        }
    }

    backBtn.draw(renderer);
}
