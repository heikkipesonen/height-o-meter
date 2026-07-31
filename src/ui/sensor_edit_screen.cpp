#include "sensor_edit_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include "layout.h"
#include "../config/config_file.h"
#include "../excavator/excavator.h"
#include <cstdio>

using namespace Layout;

namespace {
    int editSensorIndex = 0;
    const char* statusMsg = "";
    
    // Numeric control layout helper
    constexpr int VALUE_BOX_WIDTH = 120;
    constexpr int VALUE_BOX_X = (SCREEN_WIDTH - VALUE_BOX_WIDTH) / 2;  // centered
    
    // ID controls - ROW1
    Button idMinus{{MARGIN, ROW1_Y, SMALL_BTN_WIDTH, BUTTON_HEIGHT}, "-"};
    Button idMinus10{{MARGIN + SMALL_BTN_WIDTH + GAP, ROW1_Y, MED_BTN_WIDTH, BUTTON_HEIGHT}, "-10"};
    Button idPlus10{{SCREEN_WIDTH - MARGIN - SMALL_BTN_WIDTH - GAP - MED_BTN_WIDTH, ROW1_Y, MED_BTN_WIDTH, BUTTON_HEIGHT}, "+10"};
    Button idPlus{{SCREEN_WIDTH - MARGIN - SMALL_BTN_WIDTH, ROW1_Y, SMALL_BTN_WIDTH, BUTTON_HEIGHT}, "+"};
    
    // Axis toggle - ROW2
    Button axisX{{MARGIN, ROW2_Y, HALF_WIDTH, BUTTON_HEIGHT}, "X (Roll)"};
    Button axisY{{RIGHT_HALF_X, ROW2_Y, HALF_WIDTH, BUTTON_HEIGHT}, "Y (Pitch)"};
    
    // Inverted toggle - ROW3
    Button invertedOn{{MARGIN, ROW3_Y, HALF_WIDTH, BUTTON_HEIGHT}, "Inverted"};
    Button invertedOff{{RIGHT_HALF_X, ROW3_Y, HALF_WIDTH, BUTTON_HEIGHT}, "Normal"};
    
    // Points down toggle - ROW4
    Button pointsDownOn{{MARGIN, ROW4_Y, HALF_WIDTH, BUTTON_HEIGHT}, "Points Down"};
    Button pointsDownOff{{RIGHT_HALF_X, ROW4_Y, HALF_WIDTH, BUTTON_HEIGHT}, "Points Up"};
    
    // Offset controls - ROW5
    Button offsetMinus{{MARGIN, ROW5_Y, SMALL_BTN_WIDTH, BUTTON_HEIGHT}, "-1"};
    Button offsetMinusFine{{MARGIN + SMALL_BTN_WIDTH + GAP, ROW5_Y, MED_BTN_WIDTH, BUTTON_HEIGHT}, "-0.1"};
    Button offsetPlusFine{{SCREEN_WIDTH - MARGIN - SMALL_BTN_WIDTH - GAP - MED_BTN_WIDTH, ROW5_Y, MED_BTN_WIDTH, BUTTON_HEIGHT}, "+0.1"};
    Button offsetPlus{{SCREEN_WIDTH - MARGIN - SMALL_BTN_WIDTH, ROW5_Y, SMALL_BTN_WIDTH, BUTTON_HEIGHT}, "+1"};
    
    // Length controls - ROW6
    Button lengthMinus{{MARGIN, ROW6_Y, SMALL_BTN_WIDTH, BUTTON_HEIGHT}, "-1"};
    Button lengthMinus10{{MARGIN + SMALL_BTN_WIDTH + GAP, ROW6_Y, MED_BTN_WIDTH, BUTTON_HEIGHT}, "-10"};
    Button lengthPlus10{{SCREEN_WIDTH - MARGIN - SMALL_BTN_WIDTH - GAP - MED_BTN_WIDTH, ROW6_Y, MED_BTN_WIDTH, BUTTON_HEIGHT}, "+10"};
    Button lengthPlus{{SCREEN_WIDTH - MARGIN - SMALL_BTN_WIDTH, ROW6_Y, SMALL_BTN_WIDTH, BUTTON_HEIGHT}, "+1"};
    
    Button backBtn{{MARGIN, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "Back"};
    Button saveBtn{{THIRD_RIGHT_X, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT}, "Save"};
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

static void drawValueBox(SDL_Renderer *renderer, int y, const char *value) {
    SDL_Rect box = {VALUE_BOX_X, y, VALUE_BOX_WIDTH, BUTTON_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
    SDL_RenderDrawRect(renderer, &box);
    if (getFontLarge()) {
        drawTextCentered(renderer, getFontLarge(), VALUE_BOX_X, y, VALUE_BOX_WIDTH, BUTTON_HEIGHT, value);
    }
}

void renderSensorEditScreen(SDL_Renderer *renderer, ExcavatorState *state, ExcavatorConfig *config) {
    const SensorConfig &cfg = config->sensors[editSensorIndex];
    const Sensor &sensor = state->sensors[editSensorIndex];
    
    // Title with sensor name
    if (getFontSmall()) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Edit: %s", cfg.name);
        drawTextCentered(renderer, getFontSmall(), 0, TITLE_Y, SCREEN_WIDTH, 30, buf);
    }
    
    // Live values
    if (getFontSmall()) {
        char buf[128];
        double angle = getSensorAngle(sensor, cfg);
        if (sensor.connected) {
            snprintf(buf, sizeof(buf), "Raw X:%.1f Y:%.1f -> %.1f", sensor.x, sensor.y, angle);
        } else {
            snprintf(buf, sizeof(buf), "Not connected");
        }
        Color c = sensor.connected ? Color{150, 255, 150, 255} : Color{255, 150, 150, 255};
        drawTextCentered(renderer, getFontSmall(), 0, TITLE_Y + 30, SCREEN_WIDTH, 30, buf, c);
    }
    
    // ID section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), MARGIN, ROW1_Y - 25, "Modbus ID:");
    }
    idMinus.draw(renderer);
    idMinus10.draw(renderer);
    char idBuf[8];
    snprintf(idBuf, sizeof(idBuf), "%d", cfg.id);
    drawValueBox(renderer, ROW1_Y, idBuf);
    idPlus10.draw(renderer);
    idPlus.draw(renderer);
    
    // Axis section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), MARGIN, ROW2_Y - 25, "Axis:");
    }
    drawToggleButton(renderer, axisX, "X (Roll)", cfg.axis == MountAxis::X);
    drawToggleButton(renderer, axisY, "Y (Pitch)", cfg.axis == MountAxis::Y);
    
    // Inverted section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), MARGIN, ROW3_Y - 25, "Direction:");
    }
    drawToggleButton(renderer, invertedOn, "Inverted", cfg.inverted);
    drawToggleButton(renderer, invertedOff, "Normal", !cfg.inverted);
    
    // Points down section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), MARGIN, ROW4_Y - 25, "Zero angle:");
    }
    drawToggleButton(renderer, pointsDownOn, "Points Down", cfg.points_down);
    drawToggleButton(renderer, pointsDownOff, "Points Up", !cfg.points_down);
    
    // Offset section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), MARGIN, ROW5_Y - 25, "Offset:");
    }
    offsetMinus.draw(renderer);
    offsetMinusFine.draw(renderer);
    char offsetBuf[16];
    snprintf(offsetBuf, sizeof(offsetBuf), "%.1f", cfg.offset);
    drawValueBox(renderer, ROW5_Y, offsetBuf);
    offsetPlusFine.draw(renderer);
    offsetPlus.draw(renderer);
    
    // Length section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), MARGIN, ROW6_Y - 25, "Length (mm):");
    }
    lengthMinus.draw(renderer);
    lengthMinus10.draw(renderer);
    char lengthBuf[16];
    snprintf(lengthBuf, sizeof(lengthBuf), "%d", cfg.length_mm);
    drawValueBox(renderer, ROW6_Y, lengthBuf);
    lengthPlus10.draw(renderer);
    lengthPlus.draw(renderer);
    
    // Status
    if (getFontSmall() && statusMsg[0]) {
        drawTextCentered(renderer, getFontSmall(), THIRD_CENTER_X, BOTTOM_Y, THIRD_WIDTH, BUTTON_HEIGHT, statusMsg);
    }
    
    backBtn.draw(renderer);
    saveBtn.draw(renderer);
}
