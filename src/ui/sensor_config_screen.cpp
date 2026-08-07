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
    // Sensor list
    for (int i = 0; i < 6; i++) {
        const SensorConfig &cfg = config->sensors[i];
        const Sensor &sensor = state->sensors[i];
        
        SDL_Rect rect = sensorBtns[i].rect;
        
        // Background color based on connection
        if (sensor.connected) {
            SDL_SetRenderDrawColor(renderer, 40, 50, 40, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
        }
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
        SDL_RenderDrawRect(renderer, &rect);
        
        if (getFontSmall()) {
            // Single line: Name ID  X:val Y:val  status
            char buf[64];
            snprintf(buf, sizeof(buf), "%s (ID:%d)", cfg.name, cfg.id);
            drawText(renderer, getFontSmall(), rect.x + GAP, rect.y + 20, buf);
            
            if (sensor.connected) {
                snprintf(buf, sizeof(buf), "X:%.1f Y:%.1f", sensor.x, sensor.y);
                drawText(renderer, getFontSmall(), rect.x + 200, rect.y + 20, buf);
            }
            
            const char *status = sensor.connected ? "OK" : "NC";
            Color statusColor = sensor.connected ? Color{100, 200, 100, 255} : Color{200, 100, 100, 255};
            drawText(renderer, getFontSmall(), rect.x + rect.w - 35, rect.y + 20, status, statusColor);
        }
    }
    
    // Status message
    if (getFontSmall() && statusMsg[0]) {
        drawTextCentered(renderer, getFontSmall(), 0, BOTTOM_Y - 30, SCREEN_WIDTH, 25, statusMsg);
    }
    
    backBtn.draw(renderer);
    clearPosBtn.draw(renderer);
    setupBtn.draw(renderer);
}
