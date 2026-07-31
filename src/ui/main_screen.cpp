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
    
    bool zeroSet = false;
    double zeroDepth = 0;
    double zeroReach = 0;
}

ScreenResult handleMainInput(int tx, int ty, ExcavatorState *state) {
    if (visualizeBtn.contains(tx, ty)) {
        return {Screen::VISUALIZE, true};
    } else if (sensorConfigBtn.contains(tx, ty)) {
        return {Screen::SENSOR_CONFIG, true};
    } else if (zeroBtn.contains(tx, ty)) {
        if (zeroSet) {
            // Clear zero
            zeroSet = false;
            zeroDepth = 0;
            zeroReach = 0;
        } else {
            // Set zero
            zeroSet = true;
            zeroDepth = state->depth;
            zeroReach = state->reach;
        }
        return {Screen::MAIN, true};
    }
    return {Screen::MAIN, false};
}

void renderMainScreen(SDL_Renderer *renderer, ExcavatorState *state) {
    double depth = state->depth;
    double reach = state->reach;
    
    // Apply zero offset if set
    if (zeroSet) {
        depth = state->depth - zeroDepth;
        reach = state->reach - zeroReach;
    }
    
    int depthInt = (int)depth;
    bool goingUp = depthInt >= 0;
    
    // Depth triangle
    int triX = 60;
    int triY = 220;
    int triSize = 40;
    SDL_SetRenderDrawColor(renderer, goingUp ? 0 : 255, goingUp ? 200 : 60, 60, 255);
    if (goingUp) {
        // Up triangle
        for (int y = 0; y < triSize; y++) {
            int halfWidth = y * triSize / triSize;
            SDL_RenderDrawLine(renderer, triX - halfWidth, triY + y, triX + halfWidth, triY + y);
        }
    } else {
        // Down triangle
        for (int y = 0; y < triSize; y++) {
            int halfWidth = (triSize - y) * triSize / triSize;
            SDL_RenderDrawLine(renderer, triX - halfWidth, triY + y, triX + halfWidth, triY + y);
        }
    }
    
    // Depth value (convert mm to cm)
    int depthCm = depthInt / 10;
    char depthStr[32];
    snprintf(depthStr, sizeof(depthStr), "%d", depthCm < 0 ? -depthCm : depthCm);
    if (getFontHuge()) {
        Color depthColor = goingUp ? Color{0, 200, 60, 255} : Color{255, 60, 60, 255};
        drawTextCentered(renderer, getFontHuge(), 80, 180, SCREEN_WIDTH - 80, 120, depthStr, depthColor);
    }
    
    // Divider
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderDrawLine(renderer, 40, 340, SCREEN_WIDTH - 40, 340);
    
    // Reach triangle
    bool reachPositive = reach >= 0;
    int reachTriX = 60;
    int reachTriY = 420;
    SDL_SetRenderDrawColor(renderer, reachPositive ? 0 : 255, reachPositive ? 200 : 60, 60, 255);
    if (reachPositive) {
        // Right triangle (away)
        for (int x = 0; x < triSize; x++) {
            int halfHeight = (triSize - x) * triSize / triSize;
            SDL_RenderDrawLine(renderer, reachTriX + x, reachTriY - halfHeight, reachTriX + x, reachTriY + halfHeight);
        }
    } else {
        // Left triangle (toward)
        for (int x = 0; x < triSize; x++) {
            int halfHeight = x * triSize / triSize;
            SDL_RenderDrawLine(renderer, reachTriX + x, reachTriY - halfHeight, reachTriX + x, reachTriY + halfHeight);
        }
    }
    
    // Reach value (convert mm to cm)
    int reachCm = (int)reach / 10;
    char reachStr[32];
    snprintf(reachStr, sizeof(reachStr), "%d", reachCm < 0 ? -reachCm : reachCm);
    if (getFontHuge()) {
        Color reachColor = reachPositive ? Color{0, 200, 60, 255} : Color{255, 60, 60, 255};
        drawTextCentered(renderer, getFontHuge(), 80, 380, SCREEN_WIDTH - 80, 120, reachStr, reachColor);
    }
    
    // Zero indicator
    if (zeroSet) {
        if (getFontSmall()) {
            drawTextCentered(renderer, getFontSmall(), 0, 520, SCREEN_WIDTH, 30, "RELATIVE", {255, 200, 0, 255});
        }
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
    
    // Change button text based on state
    zeroBtn.label = zeroSet ? "CLR" : "ZERO";
    zeroBtn.draw(renderer);
}
