#pragma once

#include <SDL2/SDL.h>
#include "../excavator/excavator.h"
#include "../config/config.h"
#include "layout.h"

// Physical display dimensions
constexpr int DISPLAY_WIDTH = 800;
constexpr int DISPLAY_HEIGHT = 480;

enum class Screen { MAIN, SENSOR_SETUP, VISUALIZE, SENSOR_CONFIG, SENSOR_EDIT };

// Screen handler result
struct ScreenResult {
    Screen nextScreen;
    bool handled;
};

class UI {
public:
    UI(ExcavatorConfig *config);
    ~UI();

    bool init();
    void run(ExcavatorState *state);

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *renderTarget = nullptr;
    Screen currentScreen = Screen::MAIN;
    ExcavatorConfig *config = nullptr;

    void handleInput(ExcavatorState *state);
    void render(ExcavatorState *state);
};
