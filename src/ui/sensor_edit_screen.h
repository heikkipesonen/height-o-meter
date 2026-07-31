#pragma once

#include <SDL2/SDL.h>
#include "ui.h"
#include "../excavator/excavator.h"
#include "../config/config.h"

void setEditSensor(int index);
int getEditSensor();
ScreenResult handleSensorEditInput(int tx, int ty, ExcavatorState *state, ExcavatorConfig *config);
void renderSensorEditScreen(SDL_Renderer *renderer, ExcavatorState *state, ExcavatorConfig *config);
