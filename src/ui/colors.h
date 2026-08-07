#pragma once

#include <SDL3/SDL.h>

struct Color {
    uint8_t r, g, b, a;
};

// Base colors
constexpr Color BG_COLOR = {160, 160, 160, 255};
constexpr Color BTN_COLOR = {140, 140, 140, 255};
constexpr Color ACCENT_COLOR = {80, 80, 80, 255};
constexpr Color TEXT_COLOR = {20, 20, 20, 255};

// Status colors
constexpr Color SUCCESS_COLOR = {40, 160, 40, 255};
constexpr Color ERROR_COLOR = {200, 60, 60, 255};

// UI element colors
constexpr Color BORDER_COLOR = {120, 120, 120, 255};
constexpr Color INPUT_BG_COLOR = {220, 220, 220, 255};
constexpr Color INACTIVE_COLOR = {180, 180, 180, 255};
constexpr Color DIMMED_TEXT_COLOR = {140, 140, 140, 255};

// Toggle button colors
constexpr Color TOGGLE_ON_COLOR = {120, 180, 120, 255};
constexpr Color TOGGLE_OFF_COLOR = {180, 180, 180, 255};

// List item backgrounds
constexpr Color LIST_ITEM_OK_COLOR = {180, 210, 180, 255};
constexpr Color LIST_ITEM_ERROR_COLOR = {210, 180, 180, 255};
constexpr Color LIST_ITEM_BORDER_COLOR = {150, 150, 150, 255};

// Visualization colors
constexpr Color GROUND_COLOR = {140, 120, 100, 255};
constexpr Color MACHINE_COLOR = {40, 160, 80, 255};
constexpr Color PIVOT_COLOR = {100, 100, 100, 255};

// Circle/arc colors
constexpr Color CIRCLE_BG_COLOR = {80, 80, 80, 255};
constexpr Color CIRCLE_ARC_COLOR = {255, 255, 255, 255};
constexpr Color DIVIDER_COLOR = {120, 120, 120, 255};
