#include "sensor_config_screen.h"
#include "sensor_edit_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include "../config/config_file.h"
#include "../excavator/excavator.h"
#include <cstdio>

namespace {
    // Sensor list buttons (tap to edit)
    Button sensorBtns[6] = {
        {{20, 50, 440, 100}, ""},
        {{20, 155, 440, 100}, ""},
        {{20, 260, 440, 100}, ""},
        {{20, 365, 440, 100}, ""},
        {{20, 470, 440, 100}, ""},
        {{20, 575, 440, 100}, ""},
    };
    
    Button backBtn{{20, 720, 140, 60}, "Back"};
    Button setupBtn{{170, 720, 140, 60}, "Setup"};
    Button saveBtn{{320, 720, 140, 60}, "Save"};
    
    const char* statusMsg = "";
}

ScreenResult handleSensorConfigInput(int tx, int ty, ExcavatorState *state, ExcavatorConfig *config) {
    if (backBtn.contains(tx, ty)) {
        return {Screen::MAIN, true};
    }
    
    if (setupBtn.contains(tx, ty)) {
        return {Screen::SENSOR_SETUP, true};
    }
    
    if (saveBtn.contains(tx, ty)) {
        if (saveSensorConfig(config, CONFIG_FILE_PATH)) {
            statusMsg = "Saved!";
        } else {
            statusMsg = "Save failed";
        }
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
    // Title
    if (getFontSmall()) {
        drawTextCentered(renderer, getFontSmall(), 0, 15, SCREEN_WIDTH, 30, "Sensors (tap to edit)");
    }
    
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
            // Line 1: Name, ID, status
            char buf[64];
            snprintf(buf, sizeof(buf), "%s (ID:%d)", cfg.name, cfg.id);
            drawText(renderer, getFontSmall(), rect.x + 10, rect.y + 20, buf);
            
            const char *status = sensor.connected ? "OK" : "NC";
            Color statusColor = sensor.connected ? Color{100, 200, 100, 255} : Color{200, 100, 100, 255};
            drawText(renderer, getFontSmall(), rect.x + rect.w - 50, rect.y + 20, status, statusColor);
            
            // Line 2: Raw X, Y values
            if (sensor.connected) {
                snprintf(buf, sizeof(buf), "X: %.1f   Y: %.1f", sensor.roll, sensor.pitch);
            } else {
                snprintf(buf, sizeof(buf), "X: --   Y: --");
            }
            drawText(renderer, getFontSmall(), rect.x + 10, rect.y + 55, buf);
        }
    }
    
    // Status message
    if (getFontSmall() && statusMsg[0]) {
        drawTextCentered(renderer, getFontSmall(), 0, 690, SCREEN_WIDTH, 25, statusMsg);
    }
    
    backBtn.draw(renderer);
    setupBtn.draw(renderer);
    saveBtn.draw(renderer);
}
