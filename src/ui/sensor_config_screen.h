#pragma once

#include <SDL2/SDL.h>
#include "ui.h"
#include "../excavator/excavator.h"
#include "../config/config.h"

ScreenResult handleSensorConfigInput(int tx, int ty, ExcavatorState *state, ExcavatorConfig *config);
void renderSensorConfigScreen(SDL_Renderer *renderer, ExcavatorState *state, ExcavatorConfig *config);
