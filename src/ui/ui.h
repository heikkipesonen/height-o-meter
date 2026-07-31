#pragma once

#include <SDL2/SDL.h>
#include "../excavator/excavator.h"
#include "../config/config.h"

// Screen dimensions (logical, portrait)
constexpr int SCREEN_WIDTH = 480;
constexpr int SCREEN_HEIGHT = 800;

// Physical display dimensions
constexpr int DISPLAY_WIDTH = 800;
constexpr int DISPLAY_HEIGHT = 480;

enum class Screen { MAIN, CONFIG, CALIBRATE, SENSOR_SETUP, SENSOR_RAW, VISUALIZE };

// Screen handler result
struct ScreenResult {
    Screen nextScreen;
    bool handled;
};

class UI {
public:
    UI(const ExcavatorConfig *config);
    ~UI();

    bool init();
    void run(ExcavatorState *state);

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *renderTarget = nullptr;
    Screen currentScreen = Screen::MAIN;
    const ExcavatorConfig *config = nullptr;

    void handleInput(ExcavatorState *state);
    void render(ExcavatorState *state);
};
