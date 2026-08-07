#include "main_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include "layout.h"
#include "../excavator/excavator.h"
#include "../config/config_file.h"
#include <cstdio>
#include <cmath>

using namespace Layout;

namespace {
    // Stored positions
    constexpr int MAX_POSITIONS = 4;
    constexpr int POS_BTN_WIDTH = (CONTENT_WIDTH - 3 * GAP) / 4;  // 102
    constexpr int POS_BTN_HEIGHT = 60;
    constexpr int POS_BTN_Y = 10;
    
    StoredPosition storedPositions[MAX_POSITIONS];
    int selectedPosition = -1;  // -1 = none selected
    bool positionsLoaded = false;
    
    // Position slot buttons (A-F)
    Button positionBtns[MAX_POSITIONS];
    bool positionBtnsInitialized = false;
    
    void initPositionButtons() {
        if (positionBtnsInitialized) return;
        
        // Load saved positions on first init
        if (!positionsLoaded) {
            loadPositions(storedPositions, MAX_POSITIONS, &selectedPosition, POSITIONS_FILE_PATH);
            positionsLoaded = true;
        }
        
        int totalWidth = MAX_POSITIONS * POS_BTN_WIDTH + (MAX_POSITIONS - 1) * GAP;
        int startX = (SCREEN_WIDTH - totalWidth) / 2;
        for (int i = 0; i < MAX_POSITIONS; i++) {
            char label[2] = {(char)('A' + i), '\0'};
            positionBtns[i] = {{startX + i * (POS_BTN_WIDTH + GAP), POS_BTN_Y, POS_BTN_WIDTH, POS_BTN_HEIGHT}, ""};
        }
        positionBtnsInitialized = true;
    }
    
    Button visualizeBtn{{MARGIN, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "VIS"};
    Button sensorConfigBtn{{THIRD_CENTER_X, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "CFG"};
    Button zeroBtn{{THIRD_RIGHT_X, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "ZERO"};
    Button bucketBtn{{MARGIN, BOTTOM_Y - BUTTON_HEIGHT - GAP, CONTENT_WIDTH, BUTTON_HEIGHT}, ""};
}

void clearAllPositions() {
    for (int i = 0; i < MAX_POSITIONS; i++) {
        storedPositions[i].occupied = false;
    }
    selectedPosition = -1;
    savePositions(storedPositions, MAX_POSITIONS, selectedPosition, POSITIONS_FILE_PATH);
}

ScreenResult handleMainInput(int tx, int ty, ExcavatorState *state, ExcavatorConfig *config) {
    initPositionButtons();
    
    // Position slot buttons
    for (int i = 0; i < MAX_POSITIONS; i++) {
        if (positionBtns[i].contains(tx, ty)) {
            if (selectedPosition == i) {
                // Tap selected again to deselect
                selectedPosition = -1;
            } else {
                selectedPosition = i;
            }
            savePositions(storedPositions, MAX_POSITIONS, selectedPosition, POSITIONS_FILE_PATH);
            return {Screen::MAIN, true};
        }
    }
    
    if (bucketBtn.contains(tx, ty)) {
        return {Screen::BUCKET, true};
    }
    
    if (visualizeBtn.contains(tx, ty)) {
        return {Screen::VISUALIZE, true};
    } else if (sensorConfigBtn.contains(tx, ty)) {
        return {Screen::SENSOR_CONFIG, true};
    } else if (zeroBtn.contains(tx, ty)) {
        if (selectedPosition >= 0) {
            // Store current position into selected slot
            StoredPosition &pos = storedPositions[selectedPosition];
            for (int i = 0; i < NUM_SENSORS; i++) {
                pos.sensors[i] = state->sensors[i];
            }
            pos.depth = state->depth;
            pos.reach = state->reach;
            pos.occupied = true;
        } else {
            // No slot selected - store into first empty slot
            for (int i = 0; i < MAX_POSITIONS; i++) {
                if (!storedPositions[i].occupied) {
                    StoredPosition &pos = storedPositions[i];
                    for (int j = 0; j < NUM_SENSORS; j++) {
                        pos.sensors[j] = state->sensors[j];
                    }
                    pos.depth = state->depth;
                    pos.reach = state->reach;
                    pos.occupied = true;
                    selectedPosition = i;
                    break;
                }
            }
        }
        savePositions(storedPositions, MAX_POSITIONS, selectedPosition, POSITIONS_FILE_PATH);
        return {Screen::MAIN, true};
    }
    return {Screen::MAIN, false};
}

void renderMainScreen(SDL_Renderer *renderer, ExcavatorState *state, const ExcavatorConfig *config) {
    initPositionButtons();
    
    // Draw position slot buttons
    for (int i = 0; i < MAX_POSITIONS; i++) {
        SDL_Rect rect = positionBtns[i].rect;
        SDL_FRect frect = {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h};
        
        // Background color based on state
        if (selectedPosition == i) {
            // Selected - highlight
            SDL_SetRenderDrawColor(renderer, ACCENT_COLOR.r, ACCENT_COLOR.g, ACCENT_COLOR.b, 255);
            SDL_RenderFillRect(renderer, &frect);
        } else if (storedPositions[i].occupied) {
            // Occupied - standard button color
            SDL_SetRenderDrawColor(renderer, BTN_COLOR.r, BTN_COLOR.g, BTN_COLOR.b, BTN_COLOR.a);
            SDL_RenderFillRect(renderer, &frect);
        } else {
            // Empty slot - dimmed
            SDL_SetRenderDrawColor(renderer, INACTIVE_COLOR.r, INACTIVE_COLOR.g, INACTIVE_COLOR.b, 255);
            SDL_RenderFillRect(renderer, &frect);
        }
        
        // Label
        char label[2] = {(char)('A' + i), '\0'};
        if (getFontButton()) {
            Color labelColor = storedPositions[i].occupied ? TEXT_COLOR : DIMMED_TEXT_COLOR;
            if (selectedPosition == i) labelColor = {255, 255, 255, 255};
            drawTextCentered(renderer, getFontButton(), rect.x, rect.y, rect.w, rect.h, label, labelColor);
        }
    }
    
    double depth = state->depth;
    double reach = state->reach;
    
    // Calculate deviation if comparing to stored position
    double rotation = 0;
    bool zeroSet = (selectedPosition >= 0 && storedPositions[selectedPosition].occupied);
    
    if (zeroSet) {
        const StoredPosition &stored = storedPositions[selectedPosition];
        
        // Calculate stored depth/reach (would need config, for now use stored sensor values directly)
        // For now, compare superstructure rotation
        double zeroSuperX = stored.sensors[SENSOR_SUPERSTRUCTURE].x;
        double zeroSuperY = stored.sensors[SENSOR_SUPERSTRUCTURE].y;
        
        double zeroAngle = atan2(zeroSuperX, zeroSuperY);
        double currentAngle = atan2(state->sensors[SENSOR_SUPERSTRUCTURE].x, 
                                    state->sensors[SENSOR_SUPERSTRUCTURE].y);
        rotation = (currentAngle - zeroAngle) * 180.0 / M_PI;
        while (rotation > 180) rotation -= 360;
        while (rotation < -180) rotation += 360;
        
        // Depth/reach deviation from stored values
        depth = state->depth - stored.depth;
        reach = state->reach - stored.reach;
    }
    
    // Circle parameters
    int centerX = SCREEN_WIDTH / 2;
    int centerY = 320;
    int radius = 200;
    int bgThickness = 24;
    int arcThickness = 16;
    
    // Draw rotation arc indicator
    if (zeroSet) {
        // Background circle outline
        SDL_SetRenderDrawColor(renderer, CIRCLE_BG_COLOR.r, CIRCLE_BG_COLOR.g, CIRCLE_BG_COLOR.b, 255);
        for (double i = 0; i < 360; i += 0.1) {
            double angle = i * M_PI / 180.0;
            for (int t = 0; t < bgThickness; t++) {
                int r = radius - t;
                int x = centerX + (int)(cos(angle) * r);
                int y = centerY + (int)(sin(angle) * r);
                SDL_RenderPoint(renderer, x, y);
            }
        }
        
        // Arc indicator
        double startAngle = -90;
        double endAngle = -90 + rotation;
        if (rotation < 0) {
            double tmp = startAngle;
            startAngle = endAngle;
            endAngle = tmp;
        }
        
        SDL_SetRenderDrawColor(renderer, CIRCLE_ARC_COLOR.r, CIRCLE_ARC_COLOR.g, CIRCLE_ARC_COLOR.b, 255);
        for (double a = startAngle; a <= endAngle; a += 0.1) {
            double angle = a * M_PI / 180.0;
            for (int t = 0; t < arcThickness; t++) {
                int r = radius - (bgThickness - arcThickness) / 2 - t;
                int x = centerX + (int)(cos(angle) * r);
                int y = centerY + (int)(sin(angle) * r);
                SDL_RenderPoint(renderer, x, y);
            }
        }
    }
    
    // Superstructure lean indicators
    const Sensor &superSensor = state->sensors[SENSOR_SUPERSTRUCTURE];
    const SensorConfig &superCfg = config->sensors[SENSOR_SUPERSTRUCTURE];
    if (superSensor.connected) {
        int barLength = radius * 2;  // Same as circle diameter
        int barThickness = bgThickness;  // Same as circle
        int indicatorSize = 20;
        
        // Get X and Y angles - config axis is used for Y bar, other axis for X bar
        double xAngle = (superCfg.axis == MountAxis::X) ? superSensor.y : superSensor.x;
        double yAngle = (superCfg.axis == MountAxis::X) ? superSensor.x : superSensor.y;
        if (superCfg.inverted) {
            xAngle = -xAngle;
            yAngle = -yAngle;
        }
        
        // X bar (horizontal below circle)
        int xBarX = centerX - barLength / 2;
        int xBarY = centerY + radius + 20;
        
        // Background bar
        SDL_SetRenderDrawColor(renderer, CIRCLE_BG_COLOR.r, CIRCLE_BG_COLOR.g, CIRCLE_BG_COLOR.b, 255);
        SDL_FRect xBarBg = {(float)xBarX, (float)xBarY, (float)barLength, (float)barThickness};
        SDL_RenderFillRect(renderer, &xBarBg);
        
        // Indicator position (clamp to bar range)
        double xPos = xAngle / config->lean_x_max;
        if (xPos < -1) xPos = -1;
        if (xPos > 1) xPos = 1;
        int xIndicatorX = centerX + (int)(xPos * (barLength / 2 - indicatorSize / 2)) - indicatorSize / 2;
        
        SDL_SetRenderDrawColor(renderer, CIRCLE_ARC_COLOR.r, CIRCLE_ARC_COLOR.g, CIRCLE_ARC_COLOR.b, 255);
        SDL_FRect xIndicator = {(float)xIndicatorX, (float)xBarY, (float)indicatorSize, (float)barThickness};
        SDL_RenderFillRect(renderer, &xIndicator);
        
        // Y bar (vertical on left of circle)
        int yBarX = centerX - radius - 20 - barThickness;
        int yBarY = centerY - barLength / 2;
        
        // Background bar
        SDL_SetRenderDrawColor(renderer, CIRCLE_BG_COLOR.r, CIRCLE_BG_COLOR.g, CIRCLE_BG_COLOR.b, 255);
        SDL_FRect yBarBg = {(float)yBarX, (float)yBarY, (float)barThickness, (float)barLength};
        SDL_RenderFillRect(renderer, &yBarBg);
        
        // Indicator position
        double yPos = yAngle / config->lean_y_max;
        if (yPos < -1) yPos = -1;
        if (yPos > 1) yPos = 1;
        int yIndicatorY = centerY + (int)(yPos * (barLength / 2 - indicatorSize / 2)) - indicatorSize / 2;
        
        SDL_SetRenderDrawColor(renderer, CIRCLE_ARC_COLOR.r, CIRCLE_ARC_COLOR.g, CIRCLE_ARC_COLOR.b, 255);
        SDL_FRect yIndicator = {(float)yBarX, (float)yIndicatorY, (float)barThickness, (float)indicatorSize};
        SDL_RenderFillRect(renderer, &yIndicator);
    }
    
    int depthInt = (int)depth;
    
    // Depth value (convert mm to cm)
    int depthCm = depthInt / 10;
    char depthStr[32];
    snprintf(depthStr, sizeof(depthStr), "%+d", depthCm);
    if (getFontHuge()) {
        drawTextCentered(renderer, getFontHuge(), centerX - 140, centerY - 100, 280, 80, depthStr, TEXT_COLOR);
    }
    
    // Divider line
    SDL_SetRenderDrawColor(renderer, DIVIDER_COLOR.r, DIVIDER_COLOR.g, DIVIDER_COLOR.b, 255);
    SDL_RenderLine(renderer, centerX - 120, centerY, centerX + 120, centerY);
    
    // Reach value (convert mm to cm)
    int reachCm = (int)reach / 10;
    char reachStr[32];
    snprintf(reachStr, sizeof(reachStr), "%+d", reachCm);
    if (getFontHuge()) {
        drawTextCentered(renderer, getFontHuge(), centerX - 140, centerY + 20, 280, 80, reachStr, TEXT_COLOR);
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
    SDL_FRect statusDot = {(float)(SCREEN_WIDTH - 30), 10.0f, 20.0f, 20.0f};
    SDL_RenderFillRect(renderer, &statusDot);

    // Bucket button
    if (!config->buckets.empty()) {
        bucketBtn.label = config->buckets[config->active_bucket].name;
    }
    bucketBtn.draw(renderer);

    visualizeBtn.draw(renderer);
    sensorConfigBtn.draw(renderer);
    
    // Change button text based on state
    zeroBtn.label = "+";
    zeroBtn.draw(renderer);
}
