#pragma once

#include <SDL3/SDL.h>
#include "../excavator/excavator.h"
#include "ui.h"

void renderMainScreen(SDL_Renderer *renderer, ExcavatorState *state);
ScreenResult handleMainInput(int tx, int ty, ExcavatorState *state);
void clearAllPositions();
