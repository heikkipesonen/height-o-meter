#pragma once

#include <SDL2/SDL.h>
#include "../excavator/excavator.h"
#include "ui.h"

void renderSensorRawScreen(SDL_Renderer *renderer, ExcavatorState *state);
ScreenResult handleSensorRawInput(int tx, int ty);
