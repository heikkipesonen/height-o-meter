#include "main_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include <cstdio>

namespace {
    Button calibrateBtn{{10, 410, 190, 60}, "CAL"};
    Button sensorSetupBtn{{210, 410, 190, 60}, "SETUP"};
    Button sensorRawBtn{{410, 410, 190, 60}, "RAW"};
    Button zeroBtn{{610, 410, 180, 60}, "ZERO"};
}

ScreenResult handleMainInput(int tx, int ty, ExcavatorState *state) {
    if (calibrateBtn.contains(tx, ty)) {
        return {Screen::CALIBRATE, true};
    } else if (sensorSetupBtn.contains(tx, ty)) {
        return {Screen::SENSOR_SETUP, true};
    } else if (sensorRawBtn.contains(tx, ty)) {
        return {Screen::SENSOR_RAW, true};
    } else if (zeroBtn.contains(tx, ty)) {
        // TODO: zero calibration
        return {Screen::MAIN, true};
    }
    return {Screen::MAIN, false};
}

void renderMainScreen(SDL_Renderer *renderer, ExcavatorState *state) {
    int depth = (int)state->depth;
    bool goingUp = depth >= 0;
    
    int centerX = SCREEN_WIDTH / 2;
    int arrowCenterY = 100;
    int arrowX = centerX - 180;
    
    SDL_SetRenderDrawColor(renderer, goingUp ? 0 : 255, goingUp ? 200 : 60, 60, 255);
    
    if (goingUp) {
        SDL_Rect arrowBody = {arrowX + 15, arrowCenterY, 50, 60};
        SDL_RenderFillRect(renderer, &arrowBody);
        for (int y = 0; y < 60; y++) {
            int halfWidth = 55 - y;
            SDL_RenderDrawLine(renderer, arrowX + 40 - halfWidth, arrowCenterY - y, 
                              arrowX + 40 + halfWidth, arrowCenterY - y);
        }
    } else {
        SDL_Rect arrowBody = {arrowX + 15, arrowCenterY - 60, 50, 60};
        SDL_RenderFillRect(renderer, &arrowBody);
        for (int y = 0; y < 60; y++) {
            int halfWidth = 55 - y;
            SDL_RenderDrawLine(renderer, arrowX + 40 - halfWidth, arrowCenterY + y, 
                              arrowX + 40 + halfWidth, arrowCenterY + y);
        }
    }
    
    char depthStr[32];
    snprintf(depthStr, sizeof(depthStr), "%d", depth < 0 ? -depth : depth);
    if (getFontHuge()) {
        drawText(renderer, getFontHuge(), arrowX + 100, 35, depthStr, {255, 255, 255, 255});
    }
    
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderDrawLine(renderer, 40, 200, SCREEN_WIDTH - 40, 200);
    
    int reach = (int)state->reach;
    char reachStr[32];
    snprintf(reachStr, sizeof(reachStr), "%d", reach);
    if (getFontHuge()) {
        drawTextCentered(renderer, getFontHuge(), 0, 220, SCREEN_WIDTH, 150, reachStr, {255, 255, 255, 255});
    }
    
    bool allConnected = true;
    for (int i = 0; i < NUM_SENSORS - 1; i++) {  // Skip test sensor
        if (state->sensors[i].id != 0 && !state->sensors[i].connected) {
            allConnected = false;
            break;
        }
    }
    SDL_SetRenderDrawColor(renderer, allConnected ? 0 : 255, allConnected ? 200 : 0, 0, 255);
    SDL_Rect statusDot = {SCREEN_WIDTH - 30, 10, 20, 20};
    SDL_RenderFillRect(renderer, &statusDot);

    calibrateBtn.draw(renderer);
    sensorSetupBtn.draw(renderer);
    sensorRawBtn.draw(renderer);
    zeroBtn.draw(renderer);
}
