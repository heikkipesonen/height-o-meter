#include "ui.h"
#include "fonts.h"
#include <cstdio>

namespace {

struct Button {
    SDL_Rect rect;
    const char *label;

    bool contains(int x, int y) const {
        return x >= rect.x && x <= rect.x + rect.w &&
               y >= rect.y && y <= rect.y + rect.h;
    }

    void draw(SDL_Renderer *renderer) const {
        SDL_SetRenderDrawColor(renderer, BTN_COLOR.r, BTN_COLOR.g, BTN_COLOR.b, BTN_COLOR.a);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, ACCENT_COLOR.r, ACCENT_COLOR.g, ACCENT_COLOR.b, ACCENT_COLOR.a);
        SDL_RenderDrawRect(renderer, &rect);

        if (getFontMedium()) {
            drawTextCentered(renderer, getFontMedium(), rect.x, rect.y, rect.w, rect.h, label);
        }
    }
};

// Button definitions
Button configBtn{{50, 400, 150, 60}, "Config"};
Button calibrateBtn{{220, 400, 150, 60}, "Calibrate"};
Button sensorSetupBtn{{390, 400, 150, 60}, "Sensors"};
Button backBtn{{50, 400, 150, 60}, "Back"};
Button zeroBtn{{SCREEN_WIDTH/2 - 75, 400, 150, 60}, "Zero"};

// Sensor setup buttons
Button currentIdMinus{{200, 100, 60, 60}, "-"};
Button currentIdPlus{{380, 100, 60, 60}, "+"};
Button newIdMinus{{200, 200, 60, 60}, "-"};
Button newIdPlus{{380, 200, 60, 60}, "+"};
Button assignBtn{{SCREEN_WIDTH/2 - 100, 320, 200, 60}, "Assign ID"};

} // anonymous namespace

UI::UI() {}

UI::~UI() {
    closeFonts();
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool UI::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init failed: %s\n", SDL_GetError());
        return false;
    }

    if (!initFonts()) {
        printf("Warning: fonts not loaded, text will not display.\n");
    }

    window = SDL_CreateWindow(
        "Height-O-Meter",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    return renderer != nullptr;
}

void UI::handleInput(ExcavatorState *state) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            state->running = false;
        }

        if (e.type == SDL_FINGERDOWN) {
            int tx = (int)(e.tfinger.x * SCREEN_WIDTH);
            int ty = (int)(e.tfinger.y * SCREEN_HEIGHT);

            switch (currentScreen) {
                case Screen::MAIN:
                    if (configBtn.contains(tx, ty)) {
                        currentScreen = Screen::CONFIG;
                    } else if (calibrateBtn.contains(tx, ty)) {
                        currentScreen = Screen::CALIBRATE;
                    } else if (sensorSetupBtn.contains(tx, ty)) {
                        currentScreen = Screen::SENSOR_SETUP;
                    }
                    break;
                case Screen::CONFIG:
                    if (backBtn.contains(tx, ty)) {
                        currentScreen = Screen::MAIN;
                    }
                    break;
                case Screen::CALIBRATE:
                    if (backBtn.contains(tx, ty)) {
                        currentScreen = Screen::MAIN;
                    } else if (zeroBtn.contains(tx, ty)) {
                        // TODO: implement zero calibration
                    }
                    break;
                case Screen::SENSOR_SETUP:
                    if (backBtn.contains(tx, ty)) {
                        currentScreen = Screen::MAIN;
                    } else if (currentIdMinus.contains(tx, ty)) {
                        if (setupCurrentId > 1) setupCurrentId--;
                    } else if (currentIdPlus.contains(tx, ty)) {
                        if (setupCurrentId < 247) setupCurrentId++;
                    } else if (newIdMinus.contains(tx, ty)) {
                        if (setupNewId > 1) setupNewId--;
                    } else if (newIdPlus.contains(tx, ty)) {
                        if (setupNewId < 247) setupNewId++;
                    } else if (assignBtn.contains(tx, ty)) {
                        int result = update_sensor_id(setupCurrentId, setupNewId);
                        if (result == 0) {
                            setupStatus = "OK! Power cycle sensor";
                            setupCurrentId = setupNewId;
                        } else {
                            setupStatus = "Failed - check connection";
                        }
                    }
                    break;
            }
        }

        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_q) {
                state->running = false;
            } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                currentScreen = Screen::MAIN;
            }
        }
    }
}

void UI::renderMainScreen(ExcavatorState *state) {
    // Title bar
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 50};
    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    if (getFontMedium()) {
        drawTextCentered(renderer, getFontMedium(), 0, 0, SCREEN_WIDTH, 50, "HEIGHT-O-METER", ACCENT_COLOR);
    }

    // Position display
    drawValueBox(renderer, 50, 80, "X Position (mm)", state->total_x, 340, 140);
    drawValueBox(renderer, 410, 80, "Y Position (mm)", state->total_y, 340, 140);

    // Section angles
    drawValueBox(renderer, 50, 250, "Section A", state->section_a_angle, 220, 100);
    drawValueBox(renderer, 290, 250, "Section B", state->section_b_angle, 220, 100);

    configBtn.draw(renderer);
    calibrateBtn.draw(renderer);
    sensorSetupBtn.draw(renderer);
}

void UI::renderConfigScreen() {
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 50};
    SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    if (getFontMedium()) {
        drawTextCentered(renderer, getFontMedium(), 0, 0, SCREEN_WIDTH, 50, "CONFIGURATION", ACCENT_COLOR);
    }

    const char *options[] = {"Section A Distance", "Section A Offset", "Section B Distance", "Section B Offset"};
    for (int i = 0; i < 4; i++) {
        SDL_Rect option = {50, 70 + i * 70, SCREEN_WIDTH - 100, 55};
        SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
        SDL_RenderFillRect(renderer, &option);
        SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
        SDL_RenderDrawRect(renderer, &option);

        if (getFontSmall()) {
            drawText(renderer, getFontSmall(), option.x + 15, option.y + 18, options[i]);
        }
    }

    backBtn.draw(renderer);
}

void UI::renderCalibrateScreen(ExcavatorState *state) {
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 50};
    SDL_SetRenderDrawColor(renderer, 40, 50, 40, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    if (getFontMedium()) {
        drawTextCentered(renderer, getFontMedium(), 0, 0, SCREEN_WIDTH, 50, "CALIBRATE", ACCENT_COLOR);
    }

    if (getFontSmall()) {
        drawTextCentered(renderer, getFontSmall(), 0, 70, SCREEN_WIDTH, 30, "Position boom horizontally and press Zero");
    }

    drawValueBox(renderer, SCREEN_WIDTH/2 - 160, 120, "X Position", state->total_x, 320, 110);
    drawValueBox(renderer, SCREEN_WIDTH/2 - 160, 250, "Y Position", state->total_y, 320, 110);

    backBtn.draw(renderer);
    zeroBtn.draw(renderer);
}

void UI::render(ExcavatorState *state) {
    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
    SDL_RenderClear(renderer);

    switch (currentScreen) {
        case Screen::MAIN:
            renderMainScreen(state);
            break;
        case Screen::CONFIG:
            renderConfigScreen();
            break;
        case Screen::CALIBRATE:
            renderCalibrateScreen(state);
            break;
        case Screen::SENSOR_SETUP:
            renderSensorSetupScreen();
            break;
    }

    SDL_RenderPresent(renderer);
}

void UI::renderSensorSetupScreen() {
    // Title bar
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 50};
    SDL_SetRenderDrawColor(renderer, 50, 50, 40, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    if (getFontMedium()) {
        drawTextCentered(renderer, getFontMedium(), 0, 0, SCREEN_WIDTH, 50, "SENSOR SETUP", ACCENT_COLOR);
    }

    // Current ID row
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), 50, 115, "Current ID:");
    }
    currentIdMinus.draw(renderer);
    
    // Current ID value box
    SDL_Rect currentIdBox = {270, 100, 100, 60};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &currentIdBox);
    SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
    SDL_RenderDrawRect(renderer, &currentIdBox);
    if (getFontLarge()) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", setupCurrentId);
        drawTextCentered(renderer, getFontLarge(), 270, 100, 100, 60, buf);
    }
    
    currentIdPlus.draw(renderer);

    // New ID row
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), 50, 215, "New ID:");
    }
    newIdMinus.draw(renderer);
    
    // New ID value box
    SDL_Rect newIdBox = {270, 200, 100, 60};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &newIdBox);
    SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
    SDL_RenderDrawRect(renderer, &newIdBox);
    if (getFontLarge()) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", setupNewId);
        drawTextCentered(renderer, getFontLarge(), 270, 200, 100, 60, buf);
    }
    
    newIdPlus.draw(renderer);

    // Assign button
    assignBtn.draw(renderer);

    // Status text
    if (getFontSmall()) {
        drawTextCentered(renderer, getFontSmall(), 0, 390, SCREEN_WIDTH, 30, setupStatus);
    }

    backBtn.draw(renderer);
}

void UI::run(ExcavatorState *state) {
    while (state->running) {
        handleInput(state);
        render(state);
        SDL_Delay(16);
    }
}
