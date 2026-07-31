#include "visualize_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include "layout.h"
#include "src/excavator/excavator.h"
#include <cmath>

using namespace Layout;

namespace {
    Button backBtn{{MARGIN, BOTTOM_Y, HALF_WIDTH, BUTTON_HEIGHT}, "Back"};
    double toRadians(double deg) { return deg * M_PI / 180.0; }
}

ScreenResult handleVisualizeInput(int tx, int ty) {
    if (backBtn.contains(tx, ty)) {
        return {Screen::MAIN, true};
    }
    return {Screen::VISUALIZE, false};
}

void renderVisualizeScreen(SDL_Renderer *renderer, ExcavatorState *state, const ExcavatorConfig *config) {
    // Drawing area - full screen without title
    int drawAreaY = 10;
    int drawAreaH = 550;
    int groundY = drawAreaY + drawAreaH - 50;
    
    float scale = 0.7f;
    
    // Pivot point near left edge
    int pivotX = 40;
    int pivotY = groundY;
    
    // Draw ground line
    SDL_SetRenderDrawColor(renderer, 100, 80, 60, 255);
    SDL_RenderDrawLine(renderer, 0, groundY, SCREEN_WIDTH, groundY);
    
    // Get superstructure tilt (machine lean)
    const Sensor &superSensor = state->sensors[SENSOR_SUPERSTRUCTURE];
    const SensorConfig &superCfg = config->sensors[SENSOR_SUPERSTRUCTURE];
    double superTilt = 0;
    if (superSensor.connected) {
        superTilt = getSensorAngle(superSensor, superCfg);
    }
    double superRad = toRadians(superTilt);
    
    // Draw superstructure box (rotated by tilt) - from pivot to boom pin
    SDL_SetRenderDrawColor(renderer, 0, 180, 80, 255);
    
    // Box size based on boom pin offset
    float boxW = config->pivot_offset_x_mm * 2;
    float boxH = config->base_height_mm + config->pivot_offset_y_mm;
    
    // Box corners relative to pivot (bottom center at pivot)
    float corners[4][2] = {
        {-boxW/2.0f, 0},
        {boxW/2.0f, 0},
        {boxW/2.0f, boxH},
        {-boxW/2.0f, boxH}
    };
    
    // Rotate and draw box
    for (int i = 0; i < 4; i++) {
        int next = (i + 1) % 4;
        float x1 = corners[i][0] * std::cos(superRad) - corners[i][1] * std::sin(superRad);
        float y1 = corners[i][0] * std::sin(superRad) + corners[i][1] * std::cos(superRad);
        float x2 = corners[next][0] * std::cos(superRad) - corners[next][1] * std::sin(superRad);
        float y2 = corners[next][0] * std::sin(superRad) + corners[next][1] * std::cos(superRad);
        
        SDL_RenderDrawLine(renderer, 
            pivotX + (int)(x1 * scale), pivotY - (int)(y1 * scale),
            pivotX + (int)(x2 * scale), pivotY - (int)(y2 * scale));
    }
    
    // Start position (boom pin) - rotated by superstructure tilt
    float baseX = config->pivot_offset_x_mm;
    float baseY = config->base_height_mm + config->pivot_offset_y_mm;
    
    // Rotate base position by superstructure tilt
    float x = baseX * std::cos(superRad) - baseY * std::sin(superRad);
    float y = baseX * std::sin(superRad) + baseY * std::cos(superRad);
    
    int prevScreenX = pivotX + (int)(x * scale);
    int prevScreenY = pivotY - (int)(y * scale);
    
    // Draw pivot to boom pin
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawLine(renderer, pivotX, pivotY, prevScreenX, prevScreenY);
    
    // Colors for each segment
    SDL_Color colors[] = {
        {0, 0, 0, 255},       // 0: superstructure (not drawn as segment)
        {255, 100, 100, 255}, // 1: boom A - red
        {255, 200, 100, 255}, // 2: boom B - orange
        {255, 255, 100, 255}, // 3: stick - yellow
        {100, 200, 255, 255}, // 4: tilt/bucket - blue
    };
    
    // Draw each segment
    for (int i = SENSOR_BOOM_A; i <= SENSOR_TILT; i++) {
        const Sensor &sensor = state->sensors[i];
        const SensorConfig &cfg = config->sensors[i];
        
        if (!sensor.connected || cfg.length_mm == 0) continue;
        
        double angle_deg = getSensorAngle(sensor, cfg);
        double rad = toRadians(angle_deg);
        
        x += std::sin(rad) * cfg.length_mm;
        
        if (cfg.points_down) {
            y -= std::cos(rad) * cfg.length_mm;
        } else {
            y += std::cos(rad) * cfg.length_mm;
        }
        
        int screenX = pivotX + (int)(x * scale);
        int screenY = pivotY - (int)(y * scale);
        
        SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, 255);
        SDL_RenderDrawLine(renderer, prevScreenX, prevScreenY, screenX, screenY);
        SDL_RenderDrawLine(renderer, prevScreenX+1, prevScreenY, screenX+1, screenY);
        SDL_RenderDrawLine(renderer, prevScreenX, prevScreenY+1, screenX, screenY+1);
        
        prevScreenX = screenX;
        prevScreenY = screenY;
    }
    
    // Bucket tip marker
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect tip = {prevScreenX - 4, prevScreenY - 4, 8, 8};
    SDL_RenderFillRect(renderer, &tip);
    
    // Debug info below visualization
    if (getFontSmall()) {
        char buf[64];
        const Sensor &sSuper = state->sensors[SENSOR_SUPERSTRUCTURE];
        const Sensor &sA = state->sensors[SENSOR_BOOM_A];
        const Sensor &sB = state->sensors[SENSOR_BOOM_B];
        const Sensor &sS = state->sensors[SENSOR_STICK];
        const Sensor &sT = state->sensors[SENSOR_TILT];
        
        int textY = drawAreaY + drawAreaH + 20;
        
        snprintf(buf, sizeof(buf), "Super: %.1f", superTilt);
        drawTextCentered(renderer, getFontSmall(), 0, textY, SCREEN_WIDTH/2, 25, buf);
        
        snprintf(buf, sizeof(buf), "A: %.1f  B: %.1f", getSensorAngle(sA, config->sensors[SENSOR_BOOM_A]), 
                 getSensorAngle(sB, config->sensors[SENSOR_BOOM_B]));
        drawTextCentered(renderer, getFontSmall(), SCREEN_WIDTH/2, textY, SCREEN_WIDTH/2, 25, buf);
        
        snprintf(buf, sizeof(buf), "Stick: %.1f  Tilt: %.1f", getSensorAngle(sS, config->sensors[SENSOR_STICK]),
                 getSensorAngle(sT, config->sensors[SENSOR_TILT]));
        drawTextCentered(renderer, getFontSmall(), 0, textY + 25, SCREEN_WIDTH, 25, buf);
        
        snprintf(buf, sizeof(buf), "Depth: %d   Reach: %d", (int)state->depth, (int)state->reach);
        drawTextCentered(renderer, getFontSmall(), 0, textY + 55, SCREEN_WIDTH, 25, buf);
    }
    
    backBtn.draw(renderer);
}
