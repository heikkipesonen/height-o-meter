#include "numpad_screen.h"
#include "button.h"
#include "fonts.h"
#include "colors.h"
#include "layout.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace Layout;

namespace {
    constexpr int MAX_INPUT_LEN = 12;
    constexpr int KEY_GAP = 10;
    constexpr int KEY_SIZE = (CONTENT_WIDTH - 2 * KEY_GAP) / 3;  // ~140px each
    
    char inputBuffer[MAX_INPUT_LEN + 1] = "";
    Screen returnScreen = Screen::MAIN;
    int *intTarget = nullptr;
    double *doubleTarget = nullptr;
    NumpadType numpadType = NumpadType::INTEGER;
    const char *inputLabel = "";
    bool hasDecimal = false;
    bool isNegative = false;
    
    // Keyboard layout: 3x4 grid + controls
    // 1 2 3
    // 4 5 6
    // 7 8 9
    // . 0 <-
    constexpr int GRID_START_X = MARGIN;
    constexpr int GRID_START_Y = 130;
    
    Button keyBtns[12];
    const char* keyLabels[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "0", "<"};
    
    // +/- and CLR use half width, same row as bottom of keypad
    Button negBtn{{MARGIN, BOTTOM_Y - BUTTON_HEIGHT - GAP, HALF_WIDTH, BUTTON_HEIGHT}, "+/-"};
    Button clearBtn{{RIGHT_HALF_X, BOTTOM_Y - BUTTON_HEIGHT - GAP, HALF_WIDTH, BUTTON_HEIGHT}, "CLR"};
    Button cancelBtn{{MARGIN, BOTTOM_Y, HALF_WIDTH, BUTTON_HEIGHT}, "Cancel"};
    Button okBtn{{RIGHT_HALF_X, BOTTOM_Y, HALF_WIDTH, BUTTON_HEIGHT}, "OK"};
    
    bool keysInitialized = false;
    
    void initKeys() {
        if (keysInitialized) return;
        // Calculate key height to fit between input box and +/-/CLR row
        // Available: GRID_START_Y to (BOTTOM_Y - BUTTON_HEIGHT - GAP - GAP) = 130 to 640 = 510
        // 4 rows with 3 gaps: 510 = 4*h + 3*10 → h = 120
        int keyH = (BOTTOM_Y - BUTTON_HEIGHT - GAP - GAP - GRID_START_Y - 3 * KEY_GAP) / 4;
        
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 3; col++) {
                int idx = row * 3 + col;
                int x = GRID_START_X + col * (KEY_SIZE + KEY_GAP);
                int y = GRID_START_Y + row * (keyH + KEY_GAP);
                keyBtns[idx] = {{x, y, KEY_SIZE, keyH}, keyLabels[idx]};
            }
        }
        keysInitialized = true;
    }
    
    void appendChar(char c) {
        int len = strlen(inputBuffer);
        if (len < MAX_INPUT_LEN) {
            inputBuffer[len] = c;
            inputBuffer[len + 1] = '\0';
        }
    }
    
    void backspace() {
        int len = strlen(inputBuffer);
        if (len > 0) {
            if (inputBuffer[len - 1] == '.') hasDecimal = false;
            inputBuffer[len - 1] = '\0';
        }
    }
    
    void applyValue() {
        double val = atof(inputBuffer);
        if (isNegative) val = -val;
        
        if (intTarget) {
            *intTarget = (int)val;
        }
        if (doubleTarget) {
            *doubleTarget = val;
        }
    }
}

void openNumpad(Screen retScreen, int *intTgt, double *doubleTgt, NumpadType type, const char *label) {
    returnScreen = retScreen;
    intTarget = intTgt;
    doubleTarget = doubleTgt;
    numpadType = type;
    inputLabel = label;
    
    // Initialize with current value
    inputBuffer[0] = '\0';
    hasDecimal = false;
    isNegative = false;
    
    if (intTgt && *intTgt != 0) {
        int val = *intTgt;
        if (val < 0) {
            isNegative = true;
            val = -val;
        }
        snprintf(inputBuffer, sizeof(inputBuffer), "%d", val);
    } else if (doubleTgt && *doubleTgt != 0) {
        double val = *doubleTgt;
        if (val < 0) {
            isNegative = true;
            val = -val;
        }
        snprintf(inputBuffer, sizeof(inputBuffer), "%.1f", val);
        hasDecimal = (strchr(inputBuffer, '.') != nullptr);
    }
}

Screen getNumpadReturnScreen() {
    return returnScreen;
}

ScreenResult handleNumpadInput(int tx, int ty) {
    initKeys();
    
    if (cancelBtn.contains(tx, ty)) {
        return {returnScreen, true};
    }
    
    if (okBtn.contains(tx, ty)) {
        applyValue();
        return {returnScreen, true};
    }
    
    if (negBtn.contains(tx, ty)) {
        isNegative = !isNegative;
        return {Screen::NUMPAD, true};
    }
    
    if (clearBtn.contains(tx, ty)) {
        inputBuffer[0] = '\0';
        hasDecimal = false;
        return {Screen::NUMPAD, true};
    }
    
    // Number keys
    for (int i = 0; i < 12; i++) {
        if (keyBtns[i].contains(tx, ty)) {
            if (i == 9) {  // Decimal point
                if (numpadType == NumpadType::DECIMAL && !hasDecimal) {
                    if (inputBuffer[0] == '\0') appendChar('0');
                    appendChar('.');
                    hasDecimal = true;
                }
            } else if (i == 11) {  // Backspace
                backspace();
            } else {
                // Digit
                char digit = (i == 10) ? '0' : ('1' + i);
                appendChar(digit);
            }
            return {Screen::NUMPAD, true};
        }
    }
    
    return {Screen::NUMPAD, false};
}

void renderNumpadScreen(SDL_Renderer *renderer) {
    initKeys();
    
    // Value display - full width, use large font instead of huge
    char displayBuf[MAX_INPUT_LEN + 2];
    if (isNegative && inputBuffer[0] != '\0') {
        snprintf(displayBuf, sizeof(displayBuf), "-%s", inputBuffer);
    } else {
        snprintf(displayBuf, sizeof(displayBuf), "%s", inputBuffer[0] ? inputBuffer : "0");
    }
    
    SDL_FRect valueBox = {(float)MARGIN, 10, (float)CONTENT_WIDTH, 110};
    SDL_SetRenderDrawColor(renderer, BTN_COLOR.r, BTN_COLOR.g, BTN_COLOR.b, 255);
    SDL_RenderFillRect(renderer, &valueBox);
    
    if (getFontHuge()) {
        drawTextCentered(renderer, getFontHuge(), MARGIN, 10, CONTENT_WIDTH, 110, displayBuf);
    }
    
    // Number keys
    for (int i = 0; i < 12; i++) {
        // Dim decimal point for integer mode
        if (i == 9 && numpadType == NumpadType::INTEGER) {
            SDL_FRect frect = {(float)keyBtns[i].rect.x, (float)keyBtns[i].rect.y, 
                              (float)keyBtns[i].rect.w, (float)keyBtns[i].rect.h};
            SDL_SetRenderDrawColor(renderer, INACTIVE_COLOR.r, INACTIVE_COLOR.g, INACTIVE_COLOR.b, 255);
            SDL_RenderFillRect(renderer, &frect);
            if (getFontLarge()) {
                drawTextCentered(renderer, getFontLarge(), keyBtns[i].rect.x, keyBtns[i].rect.y,
                               keyBtns[i].rect.w, keyBtns[i].rect.h, keyLabels[i], DIMMED_TEXT_COLOR);
            }
        } else if (i == 11) {  // Backspace - red
            SDL_FRect frect = {(float)keyBtns[i].rect.x, (float)keyBtns[i].rect.y, 
                              (float)keyBtns[i].rect.w, (float)keyBtns[i].rect.h};
            SDL_SetRenderDrawColor(renderer, ACCENT_COLOR.r, ACCENT_COLOR.g, ACCENT_COLOR.b, 255);
            SDL_RenderFillRect(renderer, &frect);
            if (getFontLarge()) {
                drawTextCentered(renderer, getFontLarge(), keyBtns[i].rect.x, keyBtns[i].rect.y,
                               keyBtns[i].rect.w, keyBtns[i].rect.h, keyLabels[i]);
            }
        } else {
            SDL_FRect frect = {(float)keyBtns[i].rect.x, (float)keyBtns[i].rect.y, 
                              (float)keyBtns[i].rect.w, (float)keyBtns[i].rect.h};
            SDL_SetRenderDrawColor(renderer, BTN_COLOR.r, BTN_COLOR.g, BTN_COLOR.b, 255);
            SDL_RenderFillRect(renderer, &frect);
            if (getFontLarge()) {
                drawTextCentered(renderer, getFontLarge(), keyBtns[i].rect.x, keyBtns[i].rect.y,
                               keyBtns[i].rect.w, keyBtns[i].rect.h, keyLabels[i]);
            }
        }
    }
    
    negBtn.draw(renderer);
    clearBtn.draw(renderer);
    cancelBtn.draw(renderer);
    okBtn.draw(renderer);
}
