#include "main_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include "layout.h"
#include "../excavator/excavator.h"
#include <cstdio>
#include <cmath>

using namespace Layout;

namespace {
    Button visualizeBtn{{MARGIN, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "VIS"};
    Button sensorConfigBtn{{THIRD_CENTER_X, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "CFG"};
    Button zeroBtn{{THIRD_RIGHT_X, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "ZERO"};
    
    bool zeroSet = false;
    double zeroDepth = 0;
    double zeroReach = 0;
    double zeroSuperX = 0;
    double zeroSuperY = 0;
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
            zeroSuperX = 0;
            zeroSuperY = 0;
        } else {
            // Set zero
            zeroSet = true;
            zeroDepth = state->depth;
            zeroReach = state->reach;
            zeroSuperX = state->sensors[SENSOR_SUPERSTRUCTURE].x;
            zeroSuperY = state->sensors[SENSOR_SUPERSTRUCTURE].y;
        }
        return {Screen::MAIN, true};
    }
    return {Screen::MAIN, false};
}

void renderMainScreen(SDL_Renderer *renderer, ExcavatorState *state) {
    double depth = state->depth;
    double reach = state->reach;
    
    // Calculate rotation if zero is set
    double rotation = 0;
    if (zeroSet) {
        depth = state->depth - zeroDepth;
        reach = state->reach - zeroReach;
        
        double zeroAngle = atan2(zeroSuperX, zeroSuperY);
        double currentAngle = atan2(state->sensors[SENSOR_SUPERSTRUCTURE].x, 
                                    state->sensors[SENSOR_SUPERSTRUCTURE].y);
        rotation = (currentAngle - zeroAngle) * 180.0 / M_PI;
        while (rotation > 180) rotation -= 360;
        while (rotation < -180) rotation += 360;
    }
    
    // Circle parameters
    int centerX = SCREEN_WIDTH / 2;
    int centerY = 320;
    int radius = 200;
    int thickness = 16;
    
    // Draw rotation circle
    if (zeroSet) {
        // Draw thick circle outline
        for (int t = 0; t < thickness; t++) {
            int r = radius - t;
            // Gray background circle
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            for (int i = 0; i < 360; i++) {
                double angle = i * M_PI / 180.0;
                int x = centerX + (int)(cos(angle) * r);
                int y = centerY + (int)(sin(angle) * r);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
        
        // Draw rotation arc (from top, clockwise)
        // Top is -90 degrees in screen coords
        double startAngle = -90;
        double endAngle = -90 + rotation;
        if (rotation < 0) {
            double tmp = startAngle;
            startAngle = endAngle;
            endAngle = tmp;
        }
        
        // Highlight color
        SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
        for (int t = 0; t < thickness; t++) {
            int r = radius - t;
            for (double a = startAngle; (rotation >= 0 ? a <= endAngle : a <= endAngle); a += 0.5) {
                double angle = a * M_PI / 180.0;
                int x = centerX + (int)(cos(angle) * r);
                int y = centerY + (int)(sin(angle) * r);
                SDL_RenderDrawPoint(renderer, x, y);
                if (rotation < 0 && a >= startAngle) break;
            }
            // Proper arc drawing
            double step = 0.5;
            if (rotation >= 0) {
                for (double a = startAngle; a <= endAngle; a += step) {
                    double angle = a * M_PI / 180.0;
                    int x = centerX + (int)(cos(angle) * r);
                    int y = centerY + (int)(sin(angle) * r);
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            } else {
                for (double a = startAngle; a <= endAngle; a += step) {
                    double angle = a * M_PI / 180.0;
                    int x = centerX + (int)(cos(angle) * r);
                    int y = centerY + (int)(sin(angle) * r);
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
        
        // Draw marker at top (zero position)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect marker = {centerX - 3, centerY - radius - 8, 6, 16};
        SDL_RenderFillRect(renderer, &marker);
        
        // Draw rotation degrees text
        char rotStr[32];
        snprintf(rotStr, sizeof(rotStr), "%.0f°", rotation);
        if (getFontSmall()) {
            drawTextCentered(renderer, getFontSmall(), 0, centerY + radius + 10, SCREEN_WIDTH, 30, rotStr, {150, 150, 255, 255});
        }
    }
    
    int depthInt = (int)depth;
    bool goingUp = depthInt >= 0;
    
    // Depth triangle (inside circle, left side)
    int triX = centerX - 130;
    int triY = centerY - 60;
    int triSize = 30;
    SDL_SetRenderDrawColor(renderer, goingUp ? 0 : 255, goingUp ? 200 : 60, 60, 255);
    if (goingUp) {
        for (int y = 0; y < triSize; y++) {
            int halfWidth = y * triSize / triSize;
            SDL_RenderDrawLine(renderer, triX - halfWidth, triY + y, triX + halfWidth, triY + y);
        }
    } else {
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
        drawTextCentered(renderer, getFontHuge(), centerX - 140, centerY - 100, 280, 80, depthStr, depthColor);
    }
    
    // Divider line
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderDrawLine(renderer, centerX - 120, centerY, centerX + 120, centerY);
    
    // Reach triangle (inside circle, left side)
    bool reachPositive = reach >= 0;
    int reachTriX = centerX - 130;
    int reachTriY = centerY + 45;
    SDL_SetRenderDrawColor(renderer, reachPositive ? 0 : 255, reachPositive ? 200 : 60, 60, 255);
    if (reachPositive) {
        for (int x = 0; x < triSize; x++) {
            int halfHeight = (triSize - x) * triSize / triSize;
            SDL_RenderDrawLine(renderer, reachTriX + x, reachTriY - halfHeight, reachTriX + x, reachTriY + halfHeight);
        }
    } else {
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
        drawTextCentered(renderer, getFontHuge(), centerX - 140, centerY + 20, 280, 80, reachStr, reachColor);
    }
    
    // Debug: show superstructure X/Y at top
    double superX = state->sensors[SENSOR_SUPERSTRUCTURE].x;
    double superY = state->sensors[SENSOR_SUPERSTRUCTURE].y;
    char debugStr[64];
    snprintf(debugStr, sizeof(debugStr), "X:%.1f Y:%.1f", superX, superY);
    if (getFontSmall()) {
        drawTextCentered(renderer, getFontSmall(), 0, 10, SCREEN_WIDTH - 40, 30, debugStr, {100, 100, 100, 255});
    }
    
    // RELATIVE indicator below circle
    if (zeroSet) {
        if (getFontSmall()) {
            drawTextCentered(renderer, getFontSmall(), 0, centerY + radius + 35, SCREEN_WIDTH, 30, "RELATIVE", {255, 200, 0, 255});
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
