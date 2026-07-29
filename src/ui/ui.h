#pragma once

#include <SDL2/SDL.h>
#include "../excavator/excavator.h"

// Screen dimensions
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 480;

enum class Screen { MAIN, CONFIG, CALIBRATE, SENSOR_SETUP };

class UI {
public:
    UI();
    ~UI();

    bool init();
    void run(ExcavatorState *state);

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    Screen currentScreen = Screen::MAIN;

    void handleInput(ExcavatorState *state);
    void render(ExcavatorState *state);

    void renderMainScreen(ExcavatorState *state);
    void renderConfigScreen();
    void renderCalibrateScreen(ExcavatorState *state);
    void renderSensorSetupScreen();

    // Sensor setup state
    int setupCurrentId = 80;
    int setupNewId = 1;
    const char *setupStatus = "Connect one sensor";
};
