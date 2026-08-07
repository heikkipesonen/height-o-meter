#pragma once

#include <SDL3/SDL.h>
#include "../config/config.h"
#include "ui.h"

void renderBucketScreen(SDL_Renderer *renderer, ExcavatorConfig *config);
ScreenResult handleBucketInput(int tx, int ty, ExcavatorConfig *config);
