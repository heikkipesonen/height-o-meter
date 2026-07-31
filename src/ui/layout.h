#pragma once

namespace Layout {
    // Screen dimensions (portrait, rotated)
    constexpr int SCREEN_WIDTH = 480;
    constexpr int SCREEN_HEIGHT = 800;
    
    // Margins and spacing
    constexpr int MARGIN = 20;
    constexpr int GAP = 10;
    
    // Content area
    constexpr int CONTENT_WIDTH = SCREEN_WIDTH - 2 * MARGIN;  // 440
    constexpr int CONTENT_X = MARGIN;
    
    // Button dimensions
    constexpr int BUTTON_HEIGHT = 60;
    constexpr int ROW_HEIGHT = BUTTON_HEIGHT + GAP;  // 70
    
    // Half-width buttons (side by side)
    constexpr int HALF_WIDTH = (CONTENT_WIDTH - GAP) / 2;  // 215
    constexpr int RIGHT_HALF_X = MARGIN + HALF_WIDTH + GAP;  // 245
    
    // Third-width buttons
    constexpr int THIRD_WIDTH = (CONTENT_WIDTH - 2 * GAP) / 3;  // 140
    constexpr int THIRD_CENTER_X = MARGIN + THIRD_WIDTH + GAP;
    constexpr int THIRD_RIGHT_X = MARGIN + 2 * (THIRD_WIDTH + GAP);
    
    // Standard rows from top
    constexpr int TITLE_Y = 20;
    constexpr int ROW1_Y = 100;
    constexpr int ROW2_Y = ROW1_Y + ROW_HEIGHT;
    constexpr int ROW3_Y = ROW2_Y + ROW_HEIGHT;
    constexpr int ROW4_Y = ROW3_Y + ROW_HEIGHT;
    constexpr int ROW5_Y = ROW4_Y + ROW_HEIGHT;
    constexpr int ROW6_Y = ROW5_Y + ROW_HEIGHT;
    constexpr int ROW7_Y = ROW6_Y + ROW_HEIGHT;
    constexpr int ROW8_Y = ROW7_Y + ROW_HEIGHT;
    
    // Bottom button row
    constexpr int BOTTOM_Y = 720;
    
    // Small +/- buttons for numeric controls
    constexpr int SMALL_BTN_WIDTH = 60;
    constexpr int MED_BTN_WIDTH = 80;
}
