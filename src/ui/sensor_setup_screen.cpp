#include "sensor_setup_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include "layout.h"
#include <cstdio>

using namespace Layout;

namespace {
    constexpr int VALUE_BOX_WIDTH = 120;
    constexpr int VALUE_BOX_X = (SCREEN_WIDTH - VALUE_BOX_WIDTH) / 2;
    
    // Current ID row - ROW1
    Button currentIdMinus{{MARGIN, ROW1_Y, SMALL_BTN_WIDTH, BUTTON_HEIGHT}, "-"};
    Button currentIdMinus10{{MARGIN + SMALL_BTN_WIDTH + GAP, ROW1_Y, MED_BTN_WIDTH, BUTTON_HEIGHT}, "-10"};
    Button currentIdPlus10{{SCREEN_WIDTH - MARGIN - SMALL_BTN_WIDTH - GAP - MED_BTN_WIDTH, ROW1_Y, MED_BTN_WIDTH, BUTTON_HEIGHT}, "+10"};
    Button currentIdPlus{{SCREEN_WIDTH - MARGIN - SMALL_BTN_WIDTH, ROW1_Y, SMALL_BTN_WIDTH, BUTTON_HEIGHT}, "+"};
    
    // Read button and status - ROW2
    Button readBtn{{MARGIN, ROW2_Y, HALF_WIDTH, BUTTON_HEIGHT}, "Read"};
    
    // New ID row - ROW4
    Button newIdMinus{{MARGIN, ROW4_Y, SMALL_BTN_WIDTH, BUTTON_HEIGHT}, "-"};
    Button newIdMinus10{{MARGIN + SMALL_BTN_WIDTH + GAP, ROW4_Y, MED_BTN_WIDTH, BUTTON_HEIGHT}, "-10"};
    Button newIdPlus10{{SCREEN_WIDTH - MARGIN - SMALL_BTN_WIDTH - GAP - MED_BTN_WIDTH, ROW4_Y, MED_BTN_WIDTH, BUTTON_HEIGHT}, "+10"};
    Button newIdPlus{{SCREEN_WIDTH - MARGIN - SMALL_BTN_WIDTH, ROW4_Y, SMALL_BTN_WIDTH, BUTTON_HEIGHT}, "+"};
    
    // Assign button - ROW5
    Button assignBtn{{MARGIN, ROW5_Y, CONTENT_WIDTH, BUTTON_HEIGHT}, "Assign ID"};
    
    Button backBtn{{MARGIN, BOTTOM_Y, HALF_WIDTH, BUTTON_HEIGHT}, "Back"};

    int setupCurrentId = 1;
    int setupNewId = 1;
    double setupSensorX = 0;
    double setupSensorY = 0;
    bool setupSensorConnected = false;
    const char *setupStatus = "";
}

ScreenResult handleSensorSetupInput(int tx, int ty, ExcavatorState *state) {
    if (backBtn.contains(tx, ty)) {
        return {Screen::SENSOR_CONFIG, true};
    } else if (currentIdMinus.contains(tx, ty)) {
        if (setupCurrentId > 1) setupCurrentId--;
        return {Screen::SENSOR_SETUP, true};
    } else if (currentIdMinus10.contains(tx, ty)) {
        setupCurrentId = (setupCurrentId > 10) ? setupCurrentId - 10 : 1;
        return {Screen::SENSOR_SETUP, true};
    } else if (currentIdPlus.contains(tx, ty)) {
        if (setupCurrentId < 247) setupCurrentId++;
        return {Screen::SENSOR_SETUP, true};
    } else if (currentIdPlus10.contains(tx, ty)) {
        setupCurrentId = (setupCurrentId < 237) ? setupCurrentId + 10 : 247;
        return {Screen::SENSOR_SETUP, true};
    } else if (readBtn.contains(tx, ty)) {
        double x, y;
        if (probe_sensor(state, setupCurrentId, &x, &y)) {
            setupSensorX = x;
            setupSensorY = y;
            setupSensorConnected = true;
        } else {
            setupSensorConnected = false;
        }
        return {Screen::SENSOR_SETUP, true};
    } else if (newIdMinus.contains(tx, ty)) {
        if (setupNewId > 1) setupNewId--;
        return {Screen::SENSOR_SETUP, true};
    } else if (newIdMinus10.contains(tx, ty)) {
        setupNewId = (setupNewId > 10) ? setupNewId - 10 : 1;
        return {Screen::SENSOR_SETUP, true};
    } else if (newIdPlus.contains(tx, ty)) {
        if (setupNewId < 247) setupNewId++;
        return {Screen::SENSOR_SETUP, true};
    } else if (newIdPlus10.contains(tx, ty)) {
        setupNewId = (setupNewId < 237) ? setupNewId + 10 : 247;
        return {Screen::SENSOR_SETUP, true};
    } else if (assignBtn.contains(tx, ty)) {
        int result = update_sensor_id(state, setupCurrentId, setupNewId);
        if (result == 0) {
            setupStatus = "OK! Power cycle sensor";
            setupCurrentId = setupNewId;
        } else {
            setupStatus = "Failed - check connection";
        }
        return {Screen::SENSOR_SETUP, true};
    }
    return {Screen::SENSOR_SETUP, false};
}

static void drawValueBox(SDL_Renderer *renderer, int y, const char *value) {
    SDL_FRect box = {(float)VALUE_BOX_X, (float)y, (float)VALUE_BOX_WIDTH, (float)BUTTON_HEIGHT};
    SDL_SetRenderDrawColor(renderer, INPUT_BG_COLOR.r, INPUT_BG_COLOR.g, INPUT_BG_COLOR.b, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, 255);
    SDL_RenderRect(renderer, &box);
    if (getFontLarge()) {
        drawTextCentered(renderer, getFontLarge(), VALUE_BOX_X, y, VALUE_BOX_WIDTH, BUTTON_HEIGHT, value);
    }
}

void renderSensorSetupScreen(SDL_Renderer *renderer) {
    // Title
    if (getFontSmall()) {
        drawTextCentered(renderer, getFontSmall(), 0, TITLE_Y, SCREEN_WIDTH, 30, "Sensor ID Setup");
    }
    
    // Current ID section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), MARGIN, ROW1_Y - 25, "Current ID:");
    }
    
    currentIdMinus.draw(renderer);
    currentIdMinus10.draw(renderer);
    char currentIdBuf[8];
    snprintf(currentIdBuf, sizeof(currentIdBuf), "%d", setupCurrentId);
    drawValueBox(renderer, ROW1_Y, currentIdBuf);
    currentIdPlus10.draw(renderer);
    currentIdPlus.draw(renderer);
    
    // Read button and sensor status
    readBtn.draw(renderer);
    
    if (getFontSmall()) {
        char buf[64];
        if (setupSensorConnected) {
            snprintf(buf, sizeof(buf), "X:%.1f Y:%.1f", setupSensorX, setupSensorY);
            Color green = {0, 255, 0, 255};
            drawText(renderer, getFontSmall(), RIGHT_HALF_X, ROW2_Y + 15, buf, green);
        } else {
            Color red = {255, 100, 100, 255};
            drawText(renderer, getFontSmall(), RIGHT_HALF_X, ROW2_Y + 15, "Not connected", red);
        }
    }

    // New ID section
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), MARGIN, ROW4_Y - 25, "New ID:");
    }
    
    newIdMinus.draw(renderer);
    newIdMinus10.draw(renderer);
    char newIdBuf[8];
    snprintf(newIdBuf, sizeof(newIdBuf), "%d", setupNewId);
    drawValueBox(renderer, ROW4_Y, newIdBuf);
    newIdPlus10.draw(renderer);
    newIdPlus.draw(renderer);

    assignBtn.draw(renderer);

    // Status message
    if (getFontSmall()) {
        drawTextCentered(renderer, getFontSmall(), 0, ROW6_Y, SCREEN_WIDTH, 30, setupStatus);
    }

    backBtn.draw(renderer);
}
