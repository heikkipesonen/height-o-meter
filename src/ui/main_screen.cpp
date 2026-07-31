#include "main_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include "layout.h"
#include <cstdio>

using namespace Layout;

namespace {
    Button visualizeBtn{{MARGIN, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "VIS"};
    Button sensorConfigBtn{{THIRD_CENTER_X, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "CFG"};
    Button zeroBtn{{THIRD_RIGHT_X, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "ZERO"};
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
    
    // Depth value (convert mm to cm)
    int depthCm = depth / 10;
    char depthStr[32];
    snprintf(depthStr, sizeof(depthStr), "%d", depthCm < 0 ? -depthCm : depthCm);
    if (getFontHuge()) {
        drawTextCentered(renderer, getFontHuge(), 0, 280, SCREEN_WIDTH, 120, depthStr, {255, 255, 255, 255});
    }
    
    // Divider
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderDrawLine(renderer, 40, 440, SCREEN_WIDTH - 40, 440);
    
    // Reach value (convert mm to cm)
    int reachCm = (int)state->reach / 10;
    char reachStr[32];
    snprintf(reachStr, sizeof(reachStr), "%d", reachCm);
    if (getFontHuge()) {
        drawTextCentered(renderer, getFontHuge(), 0, 490, SCREEN_WIDTH, 120, reachStr, {255, 255, 255, 255});
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
