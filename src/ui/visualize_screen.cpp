#include "visualize_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include <cmath>

namespace {
    Button backBtn{{20, 720, 210, 60}, "Back"};
    
    double toRadians(double deg) { return deg * M_PI / 180.0; }
}

ScreenResult handleVisualizeInput(int tx, int ty) {
    if (backBtn.contains(tx, ty)) {
        return {Screen::MAIN, true};
    }
    return {Screen::VISUALIZE, false};
}

void renderVisualizeScreen(SDL_Renderer *renderer, ExcavatorState *state, const ExcavatorConfig *config) {
    // Title
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 50};
    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    if (getFontMedium()) {
        drawTextCentered(renderer, getFontMedium(), 0, 0, SCREEN_WIDTH, 50, "ARM POSITION", ACCENT_COLOR);
    }
    
    // Drawing area
    int drawAreaY = 60;
    int drawAreaH = 600;
    int groundY = drawAreaY + drawAreaH - 50;  // Ground line near bottom
    
    // Scale: fit the arm in the drawing area
    // Max reach ~700mm, max height ~700mm for this model
    float scale = 0.7f;  // pixels per mm
    
    // Pivot point on screen (left side, at ground level)
    int pivotX = 80;
    int pivotY = groundY;
    
    // Draw ground line
    SDL_SetRenderDrawColor(renderer, 100, 80, 60, 255);
    SDL_RenderDrawLine(renderer, 0, groundY, SCREEN_WIDTH, groundY);
    
    // Start position (boom pin)
    float x = config->pivot_offset_x_mm;
    float y = config->base_height_mm + config->pivot_offset_y_mm;
    
    int prevScreenX = pivotX + (int)(x * scale);
    int prevScreenY = pivotY - (int)(y * scale);
    
    // Draw pivot to boom pin
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawLine(renderer, pivotX, pivotY, prevScreenX, prevScreenY);
    
    // Colors for each segment
    SDL_Color colors[] = {
        {0, 0, 0, 255},       // 0: superstructure (not drawn)
        {255, 100, 100, 255}, // 1: boom A - red
        {255, 200, 100, 255}, // 2: boom B - orange
        {255, 255, 100, 255}, // 3: stick - yellow
        {100, 200, 255, 255}, // 4: tilt/bucket - blue
    };
    
    // Draw each segment - same logic as calculatePosition
    for (int i = SENSOR_BOOM_A; i <= SENSOR_TILT; i++) {
        const Sensor &sensor = state->sensors[i];
        const SensorConfig &cfg = config->sensors[i];
        
        if (!sensor.connected || cfg.length_mm == 0) continue;
        
        // Get angle (same logic as position calculation)
        double angle_deg = (cfg.axis == MountAxis::X) ? sensor.roll : sensor.pitch;
        if (cfg.inverted) angle_deg = -angle_deg;
        double rad = toRadians(angle_deg);
        
        // Horizontal: always sin(angle) * length
        x += std::sin(rad) * cfg.length_mm;
        
        // Vertical: cos(angle) * length, but subtract if segment points down at 0°
        if (cfg.points_down) {
            y -= std::cos(rad) * cfg.length_mm;
        } else {
            y += std::cos(rad) * cfg.length_mm;
        }
        
        int screenX = pivotX + (int)(x * scale);
        int screenY = pivotY - (int)(y * scale);
        
        // Draw segment
        SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, 255);
        SDL_RenderDrawLine(renderer, prevScreenX, prevScreenY, screenX, screenY);
        // Draw thicker
        SDL_RenderDrawLine(renderer, prevScreenX+1, prevScreenY, screenX+1, screenY);
        SDL_RenderDrawLine(renderer, prevScreenX, prevScreenY+1, screenX, screenY+1);
        
        prevScreenX = screenX;
        prevScreenY = screenY;
    }
    
    // Draw bucket tip marker
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect tip = {prevScreenX - 4, prevScreenY - 4, 8, 8};
    SDL_RenderFillRect(renderer, &tip);
    
    // Show depth/reach values
    if (getFontSmall()) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Depth: %d  Reach: %d", (int)state->depth, (int)state->reach);
        drawTextCentered(renderer, getFontSmall(), 0, drawAreaY + drawAreaH + 10, SCREEN_WIDTH, 30, buf);
    }
    
    backBtn.draw(renderer);
}
