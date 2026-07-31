#include "sensor_edit_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include "../config/config_file.h"
#include "../excavator/excavator.h"
#include <cstdio>

namespace {
    int editSensorIndex = 0;
    const char* statusMsg = "";
    
    // ID controls
    Button idMinus{{20, 120, 60, 60}, "-"};
    Button idMinus10{{90, 120, 80, 60}, "-10"};
    Button idPlus10{{310, 120, 80, 60}, "+10"};
    Button idPlus{{400, 120, 60, 60}, "+"};
    
    // Axis toggle
    Button axisX{{20, 230, 210, 60}, "X (Roll)"};
    Button axisY{{250, 230, 210, 60}, "Y (Pitch)"};
    
    // Inverted toggle
    Button invertedOn{{20, 310, 210, 60}, "Inverted"};
    Button invertedOff{{250, 310, 210, 60}, "Normal"};
    
    // Points down toggle
    Button pointsDownOn{{20, 390, 210, 60}, "Points Down"};
    Button pointsDownOff{{250, 390, 210, 60}, "Points Up"};
    
    // Offset controls
    Button offsetMinus{{20, 500, 60, 60}, "-1"};
    Button offsetMinusFine{{90, 500, 80, 60}, "-0.1"};
    Button offsetPlusFine{{310, 500, 80, 60}, "+0.1"};
    Button offsetPlus{{400, 500, 60, 60}, "+1"};
    
    // Length controls
    Button lengthMinus{{20, 610, 60, 60}, "-1"};
    Button lengthMinus10{{90, 610, 80, 60}, "-10"};
    Button lengthPlus10{{310, 610, 80, 60}, "+10"};
    Button lengthPlus{{400, 610, 60, 60}, "+1"};
    
    Button backBtn{{20, 720, 140, 60}, "Back"};
    Button saveBtn{{320, 720, 140, 60}, "Save"};
}

void setEditSensor(int index) {
    editSensorIndex = index;
    statusMsg = "";
}

int getEditSensor() {
    return editSensorIndex;
}

ScreenResult handleSensorEditInput(int tx, int ty, ExcavatorState *state, ExcavatorConfig *config) {
    SensorConfig &s = config->sensors[editSensorIndex];
    
    if (backBtn.contains(tx, ty)) {
        return {Screen::SENSOR_CONFIG, true};
    }
    
    if (saveBtn.contains(tx, ty)) {
        if (saveSensorConfig(config, CONFIG_FILE_PATH)) {
            statusMsg = "Saved!";
        } else {
            statusMsg = "Save failed";
        }
        return {Screen::SENSOR_EDIT, true};
    }
    
    // ID controls
    if (idMinus.contains(tx, ty)) {
        if (s.id > 1) s.id--;
        return {Screen::SENSOR_EDIT, true};
    }
    if (idMinus10.contains(tx, ty)) {
        s.id = (s.id > 10) ? s.id - 10 : 1;
        return {Screen::SENSOR_EDIT, true};
    }
    if (idPlus10.contains(tx, ty)) {
        s.id = (s.id < 237) ? s.id + 10 : 247;
        return {Screen::SENSOR_EDIT, true};
    }
    if (idPlus.contains(tx, ty)) {
        if (s.id < 247) s.id++;
        return {Screen::SENSOR_EDIT, true};
    }
    
    // Axis
    if (axisX.contains(tx, ty)) {
        s.axis = MountAxis::X;
        return {Screen::SENSOR_EDIT, true};
    }
    if (axisY.contains(tx, ty)) {
        s.axis = MountAxis::Y;
        return {Screen::SENSOR_EDIT, true};
    }
    
    // Inverted
    if (invertedOn.contains(tx, ty)) {
        s.inverted = true;
        return {Screen::SENSOR_EDIT, true};
    }
    if (invertedOff.contains(tx, ty)) {
        s.inverted = false;
        return {Screen::SENSOR_EDIT, true};
    }
    
    // Points down
    if (pointsDownOn.contains(tx, ty)) {
        s.points_down = true;
        return {Screen::SENSOR_EDIT, true};
    }
    if (pointsDownOff.contains(tx, ty)) {
        s.points_down = false;
        return {Screen::SENSOR_EDIT, true};
    }
    
    // Offset
    if (offsetMinus.contains(tx, ty)) {
        s.offset -= 1.0;
        return {Screen::SENSOR_EDIT, true};
    }
    if (offsetMinusFine.contains(tx, ty)) {
        s.offset -= 0.1;
        return {Screen::SENSOR_EDIT, true};
    }
    if (offsetPlusFine.contains(tx, ty)) {
        s.offset += 0.1;
        return {Screen::SENSOR_EDIT, true};
    }
    if (offsetPlus.contains(tx, ty)) {
        s.offset += 1.0;
        return {Screen::SENSOR_EDIT, true};
    }
    
    // Length
    if (lengthMinus.contains(tx, ty)) {
        if (s.length_mm > 0) s.length_mm--;
        return {Screen::SENSOR_EDIT, true};
    }
    if (lengthMinus10.contains(tx, ty)) {
        s.length_mm = (s.length_mm > 10) ? s.length_mm - 10 : 0;
        return {Screen::SENSOR_EDIT, true};
    }
    if (lengthPlus10.contains(tx, ty)) {
        s.length_mm += 10;
        return {Screen::SENSOR_EDIT, true};
    }
    if (lengthPlus.contains(tx, ty)) {
        s.length_mm++;
        return {Screen::SENSOR_EDIT, true};
    }
    
    return {Screen::SENSOR_EDIT, false};
}

void drawToggleButton(SDL_Renderer *renderer, const Button &btn, const char *label, bool active) {
    SDL_Rect rect = btn.rect;
    if (active) {
        SDL_SetRenderDrawColor(renderer, 60, 100, 60, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    }
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
    SDL_RenderDrawRect(renderer, &rect);
    if (getFontSmall()) {
        drawTextCentered(renderer, getFontSmall(), rect.x, rect.y, rect.w, rect.h, label);
    }
}

void renderSensorEditScreen(SDL_Renderer *renderer, ExcavatorState *state, ExcavatorConfig *config) {
    const SensorConfig &cfg = config->sensors[editSensorIndex];
    const Sensor &sensor = state->sensors[editSensorIndex];
    
    // Title with sensor name
    if (getFontSmall()) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Edit: %s", cfg.name);
        drawTextCentered(renderer, getFontSmall(), 0, 15, SCREEN_WIDTH, 30, buf);
    }
    
    // Live values
    if (getFontSmall()) {
        char buf[128];
        double angle = getSensorAngle(sensor, cfg);
        if (sensor.connected) {
            snprintf(buf, sizeof(buf), "Raw X:%.1f Y:%.1f -> %.1f°", sensor.roll, sensor.pitch, angle);
        } else {
            snprintf(buf, sizeof(buf), "Not connected");
        }
        Color c = sensor.connected ? Color{150, 255, 150, 255} : Color{255, 150, 150, 255};
        drawTextCentered(renderer, getFontSmall(), 0, 45, SCREEN_WIDTH, 30, buf, c);
    }
    
    // ID section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), 20, 95, "Modbus ID:");
    }
    idMinus.draw(renderer);
    idMinus10.draw(renderer);
    
    SDL_Rect idBox = {180, 120, 120, 60};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &idBox);
    SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
    SDL_RenderDrawRect(renderer, &idBox);
    if (getFontLarge()) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", cfg.id);
        drawTextCentered(renderer, getFontLarge(), 180, 120, 120, 60, buf);
    }
    
    idPlus10.draw(renderer);
    idPlus.draw(renderer);
    
    // Axis section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), 20, 205, "Axis:");
    }
    drawToggleButton(renderer, axisX, "X (Roll)", cfg.axis == MountAxis::X);
    drawToggleButton(renderer, axisY, "Y (Pitch)", cfg.axis == MountAxis::Y);
    
    // Inverted section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), 20, 285, "Direction:");
    }
    drawToggleButton(renderer, invertedOn, "Inverted", cfg.inverted);
    drawToggleButton(renderer, invertedOff, "Normal", !cfg.inverted);
    
    // Points down section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), 20, 365, "Zero angle:");
    }
    drawToggleButton(renderer, pointsDownOn, "Points Down", cfg.points_down);
    drawToggleButton(renderer, pointsDownOff, "Points Up", !cfg.points_down);
    
    // Offset section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), 20, 475, "Offset:");
    }
    offsetMinus.draw(renderer);
    offsetMinusFine.draw(renderer);
    
    SDL_Rect offsetBox = {180, 500, 120, 60};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &offsetBox);
    SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
    SDL_RenderDrawRect(renderer, &offsetBox);
    if (getFontLarge()) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.1f", cfg.offset);
        drawTextCentered(renderer, getFontLarge(), 180, 500, 120, 60, buf);
    }
    
    offsetPlusFine.draw(renderer);
    offsetPlus.draw(renderer);
    
    // Length section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), 20, 585, "Length (mm):");
    }
    lengthMinus.draw(renderer);
    lengthMinus10.draw(renderer);
    
    SDL_Rect lengthBox = {180, 610, 120, 60};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &lengthBox);
    SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
    SDL_RenderDrawRect(renderer, &lengthBox);
    if (getFontLarge()) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", cfg.length_mm);
        drawTextCentered(renderer, getFontLarge(), 180, 610, 120, 60, buf);
    }
    
    lengthPlus10.draw(renderer);
    lengthPlus.draw(renderer);
    
    // Status
    if (getFontSmall() && statusMsg[0]) {
        drawTextCentered(renderer, getFontSmall(), 160, 720, 160, 60, statusMsg);
    }
    
    backBtn.draw(renderer);
    saveBtn.draw(renderer);
}
