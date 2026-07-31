#include "ui.h"
#include "fonts.h"
#include "colors.h"
#include "main_screen.h"
#include "sensor_setup_screen.h"
#include "visualize_screen.h"
#include "sensor_config_screen.h"
#include "sensor_edit_screen.h"
#include <cstdio>

UI::UI(ExcavatorConfig *config) : config(config) {}

UI::~UI() {
    closeFonts();
    if (renderTarget) SDL_DestroyTexture(renderTarget);
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
        DISPLAY_WIDTH, DISPLAY_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE);
    }

    if (!renderer) {
        return false;
    }

    renderTarget = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        SCREEN_WIDTH, SCREEN_HEIGHT
    );

    if (!renderTarget) {
        printf("Render target creation failed: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void UI::handleInput(ExcavatorState *state) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            state->running = false;
        }

        if (e.type == SDL_FINGERDOWN) {
            // Window is 800x480, logical is 480x800 rotated 90° CW
            // Physical top-left -> logical bottom-left
            int tx = (int)(e.tfinger.y * SCREEN_WIDTH);
            int ty = (int)((1.0f - e.tfinger.x) * SCREEN_HEIGHT);

            ScreenResult result = {currentScreen, false};

            switch (currentScreen) {
                case Screen::MAIN:
                    result = handleMainInput(tx, ty, state);
                    break;
                case Screen::SENSOR_SETUP:
                    result = handleSensorSetupInput(tx, ty, state);
                    break;
                case Screen::VISUALIZE:
                    result = handleVisualizeInput(tx, ty);
                    break;
                case Screen::SENSOR_CONFIG:
                    result = handleSensorConfigInput(tx, ty, state, config);
                    break;
                case Screen::SENSOR_EDIT:
                    result = handleSensorEditInput(tx, ty, state, config);
                    break;
            }

            if (result.handled) {
                currentScreen = result.nextScreen;
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

void UI::render(ExcavatorState *state) {
    // Render to texture
    SDL_SetRenderTarget(renderer, renderTarget);
    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
    SDL_RenderClear(renderer);

    switch (currentScreen) {
        case Screen::MAIN:
            renderMainScreen(renderer, state);
            break;
        case Screen::SENSOR_SETUP:
            renderSensorSetupScreen(renderer);
            break;
        case Screen::VISUALIZE:
            renderVisualizeScreen(renderer, state, config);
            break;
        case Screen::SENSOR_CONFIG:
            renderSensorConfigScreen(renderer, state, config);
            break;
        case Screen::SENSOR_EDIT:
            renderSensorEditScreen(renderer, state, config);
            break;
    }

    // Blit texture to screen (rotated 90° clockwise)
    // destRect defines where texture is placed, then rotated around center
    // We want 480x800 texture to end up filling 800x480 screen after 90° rotation
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_Rect destRect = {(DISPLAY_WIDTH - SCREEN_WIDTH) / 2, (DISPLAY_HEIGHT - SCREEN_HEIGHT) / 2, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Point center = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    SDL_RenderCopyEx(renderer, renderTarget, nullptr, &destRect, 90.0, &center, SDL_FLIP_NONE);
    SDL_RenderPresent(renderer);
}

void UI::run(ExcavatorState *state) {
    while (state->running) {
        handleInput(state);
        render(state);
        SDL_Delay(16);
    }
}
