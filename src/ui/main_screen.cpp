#include "main_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include <cstdio>

namespace {
    Button visualizeBtn{{10, 720, 150, 70}, "VIS"};
    Button sensorConfigBtn{{170, 720, 150, 70}, "CFG"};
    Button zeroBtn{{330, 720, 140, 70}, "ZERO"};
}

ScreenResult handleMainInput(int tx, int ty, ExcavatorState *state) {
    if (visualizeBtn.contains(tx, ty)) {
        return {Screen::VISUALIZE, true};
    } else if (sensorConfigBtn.contains(tx, ty)) {
        return {Screen::SENSOR_CONFIG, true};
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
    int arrowCenterY = 180;
    int arrowX = centerX - 40;
    
    SDL_SetRenderDrawColor(renderer, goingUp ? 0 : 255, goingUp ? 200 : 60, 60, 255);
    
    // Draw arrow
    if (goingUp) {
        SDL_Rect arrowBody = {arrowX + 15, arrowCenterY, 50, 80};
        SDL_RenderFillRect(renderer, &arrowBody);
        for (int y = 0; y < 80; y++) {
            int halfWidth = 60 - y;
            if (halfWidth > 0) {
                SDL_RenderDrawLine(renderer, arrowX + 40 - halfWidth, arrowCenterY - y, 
                                  arrowX + 40 + halfWidth, arrowCenterY - y);
            }
        }
    } else {
        SDL_Rect arrowBody = {arrowX + 15, arrowCenterY - 80, 50, 80};
        SDL_RenderFillRect(renderer, &arrowBody);
        for (int y = 0; y < 80; y++) {
            int halfWidth = 60 - y;
            if (halfWidth > 0) {
                SDL_RenderDrawLine(renderer, arrowX + 40 - halfWidth, arrowCenterY + y, 
                                  arrowX + 40 + halfWidth, arrowCenterY + y);
            }
        }
    }
    
    // Depth value
    char depthStr[32];
    snprintf(depthStr, sizeof(depthStr), "%d", depth < 0 ? -depth : depth);
    if (getFontHuge()) {
        drawTextCentered(renderer, getFontHuge(), 0, 280, SCREEN_WIDTH, 120, depthStr, {255, 255, 255, 255});
    }
    
    // Divider
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderDrawLine(renderer, 40, 420, SCREEN_WIDTH - 40, 420);
    
    // Reach label and value
    if (getFontSmall()) {
        drawTextCentered(renderer, getFontSmall(), 0, 440, SCREEN_WIDTH, 30, "REACH", {150, 150, 150, 255});
    }
    int reach = (int)state->reach;
    char reachStr[32];
    snprintf(reachStr, sizeof(reachStr), "%d", reach);
    if (getFontHuge()) {
        drawTextCentered(renderer, getFontHuge(), 0, 470, SCREEN_WIDTH, 120, reachStr, {255, 255, 255, 255});
    }
    
    // Status dot
    bool allConnected = true;
    for (int i = 0; i < NUM_SENSORS - 1; i++) {
        if (state->sensors[i].id != 0 && !state->sensors[i].connected) {
            allConnected = false;
            break;
        }
    }
    SDL_SetRenderDrawColor(renderer, allConnected ? 0 : 255, allConnected ? 200 : 0, 0, 255);
    SDL_Rect statusDot = {SCREEN_WIDTH - 30, 10, 20, 20};
    SDL_RenderFillRect(renderer, &statusDot);

    visualizeBtn.draw(renderer);
    sensorConfigBtn.draw(renderer);
    zeroBtn.draw(renderer);
}
