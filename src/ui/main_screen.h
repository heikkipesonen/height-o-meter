#pragma once

#include <SDL3/SDL.h>
#include "../excavator/excavator.h"
#include "../config/config.h"
#include "ui.h"

void renderMainScreen(SDL_Renderer *renderer, ExcavatorState *state, const ExcavatorConfig *config);
ScreenResult handleMainInput(int tx, int ty, ExcavatorState *state);
void clearAllPositions();
