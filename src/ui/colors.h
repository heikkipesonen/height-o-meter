#pragma once

#include <SDL2/SDL.h>

struct Color {
    uint8_t r, g, b, a;
};

constexpr Color BG_COLOR = {25, 25, 35, 255};
constexpr Color BTN_COLOR = {50, 50, 65, 255};
constexpr Color ACCENT_COLOR = {100, 150, 255, 255};
constexpr Color TEXT_COLOR = {220, 220, 230, 255};
