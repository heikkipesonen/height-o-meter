#pragma once

#include <SDL3/SDL.h>
#include "ui.h"

enum class NumpadType {
    INTEGER,
    DECIMAL
};

// Set up numpad for editing a value
void openNumpad(Screen returnScreen, int *intTarget, double *doubleTarget, NumpadType type, const char *label);

// Get the screen to return to
Screen getNumpadReturnScreen();

ScreenResult handleNumpadInput(int tx, int ty);
void renderNumpadScreen(SDL_Renderer *renderer);
