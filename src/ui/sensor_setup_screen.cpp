#include "sensor_setup_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include <cstdio>

namespace {
    Button backBtn{{50, 400, 150, 60}, "Back"};
    Button currentIdMinus{{150, 100, 60, 60}, "-"};
    Button currentIdMinus10{{220, 100, 60, 60}, "-10"};
    Button currentIdPlus10{{420, 100, 60, 60}, "+10"};
    Button currentIdPlus{{490, 100, 60, 60}, "+"};
    Button readBtn{{560, 100, 100, 60}, "Read"};
    Button newIdMinus{{150, 200, 60, 60}, "-"};
    Button newIdMinus10{{220, 200, 60, 60}, "-10"};
    Button newIdPlus10{{420, 200, 60, 60}, "+10"};
    Button newIdPlus{{490, 200, 60, 60}, "+"};
    Button assignBtn{{SCREEN_WIDTH/2 - 100, 320, 200, 60}, "Assign ID"};

    int setupCurrentId = 1;
    int setupNewId = 1;
    double setupSensorRoll = 0;
    double setupSensorPitch = 0;
    bool setupSensorConnected = false;
    const char *setupStatus = "";
}

ScreenResult handleSensorSetupInput(int tx, int ty, ExcavatorState *state) {
    if (backBtn.contains(tx, ty)) {
        return {Screen::MAIN, true};
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
        double roll, pitch;
        if (probe_sensor(state, setupCurrentId, &roll, &pitch)) {
            setupSensorRoll = roll;
            setupSensorPitch = pitch;
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

void renderSensorSetupScreen(SDL_Renderer *renderer) {
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 50};
    SDL_SetRenderDrawColor(renderer, 50, 50, 40, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    if (getFontMedium()) {
        drawTextCentered(renderer, getFontMedium(), 0, 0, SCREEN_WIDTH, 50, "SENSOR SETUP", ACCENT_COLOR);
    }

    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), 50, 115, "Current ID:");
    }
    currentIdMinus.draw(renderer);
    currentIdMinus10.draw(renderer);
    
    SDL_Rect currentIdBox = {290, 100, 120, 60};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &currentIdBox);
    SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
    SDL_RenderDrawRect(renderer, &currentIdBox);
    if (getFontLarge()) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", setupCurrentId);
        drawTextCentered(renderer, getFontLarge(), 290, 100, 120, 60, buf);
    }
    
    currentIdPlus10.draw(renderer);
    currentIdPlus.draw(renderer);
    readBtn.draw(renderer);

    if (getFontSmall()) {
        char buf[64];
        if (setupSensorConnected) {
            snprintf(buf, sizeof(buf), "Roll: %.1f  Pitch: %.1f", setupSensorRoll, setupSensorPitch);
            Color green = {0, 255, 0, 255};
            drawTextCentered(renderer, getFontSmall(), 670, 100, 120, 60, buf, green);
        } else {
            Color red = {255, 100, 100, 255};
            drawTextCentered(renderer, getFontSmall(), 670, 100, 120, 60, "NC", red);
        }
    }

    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), 50, 215, "New ID:");
    }
    newIdMinus.draw(renderer);
    newIdMinus10.draw(renderer);
    
    SDL_Rect newIdBox = {290, 200, 120, 60};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &newIdBox);
    SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
    SDL_RenderDrawRect(renderer, &newIdBox);
    if (getFontLarge()) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", setupNewId);
        drawTextCentered(renderer, getFontLarge(), 290, 200, 120, 60, buf);
    }
    
    newIdPlus10.draw(renderer);
    newIdPlus.draw(renderer);

    assignBtn.draw(renderer);

    if (getFontSmall()) {
        drawTextCentered(renderer, getFontSmall(), 0, 390, SCREEN_WIDTH, 30, setupStatus);
    }

    backBtn.draw(renderer);
}
