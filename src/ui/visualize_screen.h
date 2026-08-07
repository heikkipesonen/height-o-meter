#pragma once

#include <SDL3/SDL.h>
#include "../excavator/excavator.h"
#include "../config/config.h"
#include "ui.h"

void renderVisualizeScreen(SDL_Renderer *renderer, ExcavatorState *state, const ExcavatorConfig *config);
ScreenResult handleVisualizeInput(int tx, int ty);
