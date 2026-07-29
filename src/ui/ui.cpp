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

        if (getFontMedium()) {
            drawTextCentered(renderer, getFontMedium(), rect.x, rect.y, rect.w, rect.h, label);
        }
    }
};

// Main screen buttons (bottom row)
Button calibrateBtn{{10, 410, 190, 60}, "CAL"};
Button sensorSetupBtn{{210, 410, 190, 60}, "SETUP"};
Button sensorRawBtn{{410, 410, 190, 60}, "RAW"};
Button reloadBtn{{610, 410, 180, 60}, "ZERO"};
Button backBtn{{50, 400, 150, 60}, "Back"};
Button zeroBtn{{SCREEN_WIDTH/2 - 75, 400, 150, 60}, "Zero"};

// Sensor setup buttons
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
                    if (calibrateBtn.contains(tx, ty)) {
                        currentScreen = Screen::CALIBRATE;
                    } else if (sensorSetupBtn.contains(tx, ty)) {
                        currentScreen = Screen::SENSOR_SETUP;
                    } else if (sensorRawBtn.contains(tx, ty)) {
                        currentScreen = Screen::SENSOR_RAW;
                    } else if (reloadBtn.contains(tx, ty)) {
                        // TODO: zero calibration
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
                    } else if (currentIdMinus10.contains(tx, ty)) {
                        setupCurrentId = (setupCurrentId > 10) ? setupCurrentId - 10 : 1;
                    } else if (currentIdPlus.contains(tx, ty)) {
                        if (setupCurrentId < 247) setupCurrentId++;
                    } else if (currentIdPlus10.contains(tx, ty)) {
                        setupCurrentId = (setupCurrentId < 237) ? setupCurrentId + 10 : 247;
                    } else if (readBtn.contains(tx, ty)) {
                        double roll, pitch;
                        if (probe_sensor(state, setupCurrentId, &roll, &pitch)) {
                            setupSensorRoll = roll;
                            setupSensorPitch = pitch;
                            setupSensorConnected = true;
                        } else {
                            setupSensorConnected = false;
                        }
                    } else if (newIdMinus.contains(tx, ty)) {
                        if (setupNewId > 1) setupNewId--;
                    } else if (newIdMinus10.contains(tx, ty)) {
                        setupNewId = (setupNewId > 10) ? setupNewId - 10 : 1;
                    } else if (newIdPlus.contains(tx, ty)) {
                        if (setupNewId < 247) setupNewId++;
                    } else if (newIdPlus10.contains(tx, ty)) {
                        setupNewId = (setupNewId < 237) ? setupNewId + 10 : 247;
                    } else if (assignBtn.contains(tx, ty)) {
                        int result = update_sensor_id(state, setupCurrentId, setupNewId);
                        if (result == 0) {
                            setupStatus = "OK! Power cycle sensor";
                            setupCurrentId = setupNewId;
                        } else {
                            setupStatus = "Failed - check connection";
                        }
                    }
                    break;
                case Screen::SENSOR_RAW:
                    if (backBtn.contains(tx, ty)) {
                        currentScreen = Screen::MAIN;
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
    // DEPTH section (top half)
    int depth = (int)state->total_y;
    bool goingUp = depth >= 0;
    
    // Center everything
    int centerX = SCREEN_WIDTH / 2;
    
    // Draw arrow centered with value
    int arrowCenterY = 100;
    int arrowX = centerX - 180;
    SDL_SetRenderDrawColor(renderer, goingUp ? 0 : 255, goingUp ? 200 : 60, 60, 255);
    
    if (goingUp) {
        // Up arrow
        SDL_Rect arrowBody = {arrowX + 15, arrowCenterY, 50, 60};
        SDL_RenderFillRect(renderer, &arrowBody);
        for (int y = 0; y < 60; y++) {
            int halfWidth = 55 - y;
            SDL_RenderDrawLine(renderer, arrowX + 40 - halfWidth, arrowCenterY - y, 
                              arrowX + 40 + halfWidth, arrowCenterY - y);
        }
    } else {
        // Down arrow
        SDL_Rect arrowBody = {arrowX + 15, arrowCenterY - 60, 50, 60};
        SDL_RenderFillRect(renderer, &arrowBody);
        for (int y = 0; y < 60; y++) {
            int halfWidth = 55 - y;
            SDL_RenderDrawLine(renderer, arrowX + 40 - halfWidth, arrowCenterY + y, 
                              arrowX + 40 + halfWidth, arrowCenterY + y);
        }
    }
    
    // Depth value
    char depthStr[32];
    snprintf(depthStr, sizeof(depthStr), "%d", depth < 0 ? -depth : depth);
    if (getFontHuge()) {
        drawText(renderer, getFontHuge(), arrowX + 100, 35, depthStr, {255, 255, 255, 255});
    }
    
    // Separator line
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderDrawLine(renderer, 40, 200, SCREEN_WIDTH - 40, 200);
    
    // REACH section (centered)
    int reach = (int)state->total_x;
    char reachStr[32];
    snprintf(reachStr, sizeof(reachStr), "%d", reach);
    if (getFontHuge()) {
        drawTextCentered(renderer, getFontHuge(), 0, 220, SCREEN_WIDTH, 150, reachStr, {255, 255, 255, 255});
    }
    
    // Sensor status indicator
    bool allConnected = true;
    for (int i = 0; i < NUM_SENSORS; i++) {
        if (state->sensors[i].id != 0 && !state->sensors[i].connected) {
            allConnected = false;
            break;
        }
    }
    SDL_SetRenderDrawColor(renderer, allConnected ? 0 : 255, allConnected ? 200 : 0, 0, 255);
    SDL_Rect statusDot = {SCREEN_WIDTH - 30, 10, 20, 20};
    SDL_RenderFillRect(renderer, &statusDot);

    // Bottom buttons
    calibrateBtn.draw(renderer);
    sensorSetupBtn.draw(renderer);
    sensorRawBtn.draw(renderer);
    reloadBtn.draw(renderer);
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
        case Screen::SENSOR_RAW:
            renderSensorRawScreen(state);
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
    currentIdMinus10.draw(renderer);
    
    // Current ID value box
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

    // Live data for current ID
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

    // New ID row
    if (getFontSmall()) {
        drawText(renderer, getFontSmall(), 50, 215, "New ID:");
    }
    newIdMinus.draw(renderer);
    newIdMinus10.draw(renderer);
    
    // New ID value box
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
    newIdPlus.draw(renderer);;

    // Assign button
    assignBtn.draw(renderer);

    // Status text
    if (getFontSmall()) {
        drawTextCentered(renderer, getFontSmall(), 0, 390, SCREEN_WIDTH, 30, setupStatus);
    }

    backBtn.draw(renderer);
}

void UI::renderSensorRawScreen(ExcavatorState *state) {
    // Title bar
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 50};
    SDL_SetRenderDrawColor(renderer, 40, 50, 50, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    if (getFontMedium()) {
        drawTextCentered(renderer, getFontMedium(), 0, 0, SCREEN_WIDTH, 50, "RAW SENSOR DATA", ACCENT_COLOR);
    }

    const char *sensorNames[] = {
        "Superstructure",
        "Boom A",
        "Boom B", 
        "Stick",
        "Tilt Hitch",
        "Test"
    };

    int rowHeight = 65;
    int startY = 60;

    for (int i = 0; i < NUM_SENSORS; i++) {
        Sensor *s = &state->sensors[i];
        int y = startY + i * rowHeight;

        // Background
        SDL_Rect row = {10, y, SCREEN_WIDTH - 20, rowHeight - 5};
        if (s->connected) {
            SDL_SetRenderDrawColor(renderer, 40, 50, 40, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
        }
        SDL_RenderFillRect(renderer, &row);
        SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
        SDL_RenderDrawRect(renderer, &row);

        if (getFontSmall()) {
            // Sensor name and ID
            char nameBuf[32];
            snprintf(nameBuf, sizeof(nameBuf), "%s (ID:%d)", sensorNames[i], s->id);
            drawText(renderer, getFontSmall(), 20, y + 8, nameBuf);

            // Roll/Pitch values
            char rollBuf[32], pitchBuf[32];
            if (s->connected) {
                snprintf(rollBuf, sizeof(rollBuf), "Roll: %.1f", s->roll);
                snprintf(pitchBuf, sizeof(pitchBuf), "Pitch: %.1f", s->pitch);
            } else {
                snprintf(rollBuf, sizeof(rollBuf), "Roll: --");
                snprintf(pitchBuf, sizeof(pitchBuf), "Pitch: --");
            }
            drawText(renderer, getFontSmall(), 300, y + 8, rollBuf);
            drawText(renderer, getFontSmall(), 500, y + 8, pitchBuf);

            // Status
            const char *status = s->connected ? "OK" : "NC";
            Color statusColor = s->connected ? Color{100, 200, 100, 255} : Color{200, 100, 100, 255};
            drawText(renderer, getFontSmall(), 700, y + 8, status, statusColor);
        }
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
