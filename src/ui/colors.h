#pragma once

#include <SDL3/SDL.h>

struct Color {
    uint8_t r, g, b, a;
};

// Base colors - O&K theme
constexpr Color BG_COLOR = {47, 50, 52, 255};       // Dark gray (RAL 7021)
constexpr Color BTN_COLOR = {70, 73, 75, 255};      // Lighter gray for buttons
constexpr Color ACCENT_COLOR = {204, 6, 5, 255};    // RAL 3020 Traffic Red
constexpr Color TEXT_COLOR = {255, 255, 255, 255};  // White

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
constexpr Color LIST_ITEM_OK_COLOR = {60, 63, 65, 255};     // Dark gray, same as buttons
constexpr Color LIST_ITEM_ERROR_COLOR = {60, 63, 65, 255};  // Same - status shown by text color
constexpr Color LIST_ITEM_BORDER_COLOR = {90, 90, 90, 255};

// Visualization colors
constexpr Color GROUND_COLOR = {140, 120, 100, 255};
constexpr Color MACHINE_COLOR = {40, 160, 80, 255};
constexpr Color PIVOT_COLOR = {100, 100, 100, 255};

// Circle/arc colors
constexpr Color CIRCLE_BG_COLOR = {80, 80, 80, 255};
constexpr Color CIRCLE_ARC_COLOR = {255, 255, 255, 255};
constexpr Color DIVIDER_COLOR = {120, 120, 120, 255};
