#pragma once

#include <SDL2/SDL.h>

struct Color {
    uint8_t r, g, b, a;
};

// Base colors
constexpr Color BG_COLOR = {25, 25, 35, 255};
constexpr Color BTN_COLOR = {50, 50, 65, 255};
constexpr Color ACCENT_COLOR = {100, 150, 255, 255};
constexpr Color TEXT_COLOR = {220, 220, 230, 255};

// Status colors
constexpr Color SUCCESS_COLOR = {100, 200, 100, 255};
constexpr Color ERROR_COLOR = {200, 100, 100, 255};

// UI element colors
constexpr Color BORDER_COLOR = {80, 80, 100, 255};
constexpr Color INPUT_BG_COLOR = {50, 50, 60, 255};
constexpr Color INACTIVE_COLOR = {30, 30, 40, 255};
constexpr Color DIMMED_TEXT_COLOR = {60, 60, 70, 255};

// Toggle button colors
constexpr Color TOGGLE_ON_COLOR = {60, 100, 60, 255};
constexpr Color TOGGLE_OFF_COLOR = {50, 50, 60, 255};

// List item backgrounds
constexpr Color LIST_ITEM_OK_COLOR = {40, 50, 40, 255};
constexpr Color LIST_ITEM_ERROR_COLOR = {50, 40, 40, 255};
constexpr Color LIST_ITEM_BORDER_COLOR = {60, 60, 70, 255};

// Visualization colors
constexpr Color GROUND_COLOR = {100, 80, 60, 255};
constexpr Color MACHINE_COLOR = {0, 180, 80, 255};
constexpr Color PIVOT_COLOR = {100, 100, 100, 255};

// Circle/arc colors
constexpr Color CIRCLE_BG_COLOR = {50, 50, 50, 255};
constexpr Color DIVIDER_COLOR = {80, 80, 80, 255};
