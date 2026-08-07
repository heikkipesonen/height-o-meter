#include "bucket_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include "layout.h"
#include <cstdio>

using namespace Layout;

namespace {
    constexpr int MAX_BUCKETS = 8;
    constexpr int ROW_H = 70;
    constexpr int START_Y = 30;
    
    Button bucketBtns[MAX_BUCKETS];
    Button backBtn{{MARGIN, BOTTOM_Y, HALF_WIDTH, BUTTON_HEIGHT}, "Back"};
}

ScreenResult handleBucketInput(int tx, int ty, ExcavatorConfig *config) {
    if (backBtn.contains(tx, ty)) {
        return {Screen::MAIN, true};
    }
    
    for (size_t i = 0; i < config->buckets.size() && i < MAX_BUCKETS; i++) {
        if (bucketBtns[i].contains(tx, ty)) {
            config->active_bucket = i;
            return {Screen::MAIN, true};
        }
    }
    
    return {Screen::BUCKET, false};
}

void renderBucketScreen(SDL_Renderer *renderer, ExcavatorConfig *config) {
    // Bucket list
    for (size_t i = 0; i < config->buckets.size() && i < MAX_BUCKETS; i++) {
        int y = START_Y + i * (ROW_H + GAP);
        
        bucketBtns[i].rect = {MARGIN, y, CONTENT_WIDTH, ROW_H};
        
        SDL_FRect rowBg = {(float)MARGIN, (float)y, (float)CONTENT_WIDTH, (float)ROW_H};
        
        // Highlight active bucket
        if ((int)i == config->active_bucket) {
            SDL_SetRenderDrawColor(renderer, ACCENT_COLOR.r, ACCENT_COLOR.g, ACCENT_COLOR.b, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, BTN_COLOR.r, BTN_COLOR.g, BTN_COLOR.b, 255);
        }
        SDL_RenderFillRect(renderer, &rowBg);
        
        if (getFontButton()) {
            drawTextCentered(renderer, getFontButton(), MARGIN, y, CONTENT_WIDTH, ROW_H, 
                           config->buckets[i].name, TEXT_COLOR);
        }
    }
    
    backBtn.draw(renderer);
}
