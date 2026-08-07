#pragma once

#include <SDL3/SDL.h>
#include "../excavator/excavator.h"
#include "ui.h"

void renderSensorSetupScreen(SDL_Renderer *renderer);
ScreenResult handleSensorSetupInput(int tx, int ty, ExcavatorState *state);
