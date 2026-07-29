#pragma once

#include <SDL2/SDL.h>
#include "../excavator/excavator.h"

// Screen dimensions
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 480;

enum class Screen { MAIN, CONFIG, CALIBRATE, SENSOR_SETUP, SENSOR_RAW };

// Screen handler result
struct ScreenResult {
    Screen nextScreen;
    bool handled;
};

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
};
