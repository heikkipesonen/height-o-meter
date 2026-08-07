#include "sensor_config_screen.h"
#include "sensor_edit_screen.h"
#include "main_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include "layout.h"
#include "../config/config_file.h"
#include "../excavator/excavator.h"
#include <cstdio>

using namespace Layout;

namespace {
    constexpr int LIST_ITEM_HEIGHT = 60;
    constexpr int LIST_START_Y = 20;
    
    // Sensor list buttons (tap to edit)
    Button sensorBtns[6] = {
        {{MARGIN, LIST_START_Y + 0 * (LIST_ITEM_HEIGHT + GAP), CONTENT_WIDTH, LIST_ITEM_HEIGHT}, ""},
        {{MARGIN, LIST_START_Y + 1 * (LIST_ITEM_HEIGHT + GAP), CONTENT_WIDTH, LIST_ITEM_HEIGHT}, ""},
        {{MARGIN, LIST_START_Y + 2 * (LIST_ITEM_HEIGHT + GAP), CONTENT_WIDTH, LIST_ITEM_HEIGHT}, ""},
        {{MARGIN, LIST_START_Y + 3 * (LIST_ITEM_HEIGHT + GAP), CONTENT_WIDTH, LIST_ITEM_HEIGHT}, ""},
        {{MARGIN, LIST_START_Y + 4 * (LIST_ITEM_HEIGHT + GAP), CONTENT_WIDTH, LIST_ITEM_HEIGHT}, ""},
        {{MARGIN, LIST_START_Y + 5 * (LIST_ITEM_HEIGHT + GAP), CONTENT_WIDTH, LIST_ITEM_HEIGHT}, ""},
    };
    
    Button backBtn{{MARGIN, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "Back"};
    Button clearPosBtn{{THIRD_CENTER_X, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "Clr Pos"};
    Button setupBtn{{THIRD_RIGHT_X, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "Setup"};
    
    const char* statusMsg = "";
}

ScreenResult handleSensorConfigInput(int tx, int ty, ExcavatorState *state, ExcavatorConfig *config) {
    if (backBtn.contains(tx, ty)) {
        return {Screen::MAIN, true};
    }
    
    if (setupBtn.contains(tx, ty)) {
        return {Screen::SENSOR_SETUP, true};
    }
    
    if (clearPosBtn.contains(tx, ty)) {
        clearAllPositions();
        return {Screen::SENSOR_CONFIG, true};
    }
    
    // Sensor selection - go to edit screen
    for (int i = 0; i < 6; i++) {
        if (sensorBtns[i].contains(tx, ty)) {
            setEditSensor(i);
            return {Screen::SENSOR_EDIT, true};
        }
    }
    
    return {Screen::SENSOR_CONFIG, false};
}

void renderSensorConfigScreen(SDL_Renderer *renderer, ExcavatorState *state, ExcavatorConfig *config) {
    // Table columns
    int colName = MARGIN;
    int colID = 150;
    int colX = 220;
    int colY = 300;
    int colStatus = 410;
    int rowH = 70;
    int startY = 30;
    
    // Header
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), colName, startY, "Name");
        drawText(renderer, getFontSmall(), colID, startY, "ID");
        drawText(renderer, getFontSmall(), colX, startY, "X");
        drawText(renderer, getFontSmall(), colY, startY, "Y");
        drawText(renderer, getFontSmall(), colStatus, startY, "Status");
    }
    
    // Sensor rows
    for (int i = 0; i < 6; i++) {
        const SensorConfig &cfg = config->sensors[i];
        const Sensor &sensor = state->sensors[i];
        
        int y = startY + 30 + i * rowH;
        
        // Update button positions for tap detection
        sensorBtns[i].rect = {MARGIN, y - 10, CONTENT_WIDTH, rowH};
        
        // Row background
        SDL_FRect rowBg = {(float)MARGIN, (float)(y - 10), (float)CONTENT_WIDTH, (float)rowH};
        SDL_SetRenderDrawColor(renderer, LIST_ITEM_OK_COLOR.r, LIST_ITEM_OK_COLOR.g, LIST_ITEM_OK_COLOR.b, 255);
        SDL_RenderFillRect(renderer, &rowBg);
        
        if (getFontSmall()) {
            char buf[32];
            
            // Name (shortened for superstructure)
            const char *name = cfg.name;
            if (i == 0) name = "Super";
            drawText(renderer, getFontSmall(), colName, y, name);
            
            // ID
            snprintf(buf, sizeof(buf), "%d", cfg.id);
            drawText(renderer, getFontSmall(), colID, y, buf);
            
            // X/Y values
            if (sensor.connected) {
                snprintf(buf, sizeof(buf), "%+.1f", sensor.x);
                drawText(renderer, getFontSmall(), colX, y, buf);
                snprintf(buf, sizeof(buf), "%+.1f", sensor.y);
                drawText(renderer, getFontSmall(), colY, y, buf);
            } else {
                drawText(renderer, getFontSmall(), colX, y, "--");
                drawText(renderer, getFontSmall(), colY, y, "--");
            }
            
            // Status
            const char *status = sensor.connected ? "OK" : "NC";
            Color statusColor = sensor.connected ? SUCCESS_COLOR : ERROR_COLOR;
            drawText(renderer, getFontSmall(), colStatus, y, status, statusColor);
        }
    }
    
    backBtn.draw(renderer);
    clearPosBtn.draw(renderer);
    setupBtn.draw(renderer);
}
