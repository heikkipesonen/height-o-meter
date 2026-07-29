#pragma once

#include <SDL2/SDL.h>
#include "../excavator/excavator.h"
#include "ui.h"

void renderCalibrateScreen(SDL_Renderer *renderer, ExcavatorState *state);
ScreenResult handleCalibrateInput(int tx, int ty, ExcavatorState *state);
