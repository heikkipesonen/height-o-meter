#include <SDL2/SDL.h>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <thread>

#include "include/device-handler.h"
#include "include/machine.h"
#include "include/utils.h"

// Screen dimensions (adjust for your touchscreen)
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 480;

// Colors
struct Color {
    uint8_t r, g, b, a;
};

constexpr Color BG_COLOR = {30, 30, 30, 255};
constexpr Color TEXT_COLOR = {220, 220, 220, 255};
constexpr Color ACCENT_COLOR = {80, 140, 200, 255};
constexpr Color BTN_COLOR = {60, 60, 80, 255};
constexpr Color BTN_HOVER_COLOR = {80, 80, 120, 255};

// UI State
enum class Screen { MAIN, CONFIG, CALIBRATE };

std::atomic<bool> running{true};
std::atomic<Screen> currentScreen{Screen::MAIN};

// Shared sensor data
std::atomic<double> total_x{0};
std::atomic<double> total_y{0};
std::atomic<double> section_a_angle{0};
std::atomic<double> section_b_angle{0};

double get_section_x(Section *section) {
    int multiplier = section->inv_x ? -1 : 1;
    double pos_x =
        (get_x_of(section->value_x, section->dist) + section->base_offset_x) *
        multiplier;
    return pos_x;
}

double get_section_y(Section *section) {
    int multiplier = section->inv_y ? -1 : 1;
    double pos_y =
        (get_y_of(section->value_x, section->dist) + section->base_offset_y) *
        multiplier;
    return pos_y;
}

void update_section(modbus_t *ctx, Section *x) {
    modbus_set_slave(ctx, x->sensor_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    SensorData r_a = read_angle(ctx);
    x->value_x = r_a.x;
    x->value_y = r_a.y;
}

// Modbus polling thread
void modbusThread(Section *a, Section *b) {
    modbus_t *ctx = nullptr;
    open_connection(ctx);

    while (running) {
        update_section(ctx, a);
        update_section(ctx, b);

        total_x = get_section_x(a) + get_section_x(b);
        total_y = get_section_y(a) + get_section_y(b);
        section_a_angle = a->value_x;
        section_b_angle = b->value_x;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    cleanup(ctx);
}

// Simple button
struct Button {
    SDL_Rect rect;
    const char *label;
    bool hovered = false;

    bool contains(int x, int y) {
        return x >= rect.x && x <= rect.x + rect.w &&
               y >= rect.y && y <= rect.y + rect.h;
    }

    void draw(SDL_Renderer *renderer) {
        Color c = hovered ? BTN_HOVER_COLOR : BTN_COLOR;
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(renderer, &rect);

        // Border
        SDL_SetRenderDrawColor(renderer, ACCENT_COLOR.r, ACCENT_COLOR.g, ACCENT_COLOR.b, ACCENT_COLOR.a);
        SDL_RenderDrawRect(renderer, &rect);
    }
};

void drawText(SDL_Renderer *renderer, int x, int y, const char *text, int scale = 2) {
    // Simple placeholder - in real app use SDL_ttf
    // For now just draw a rectangle where text would be
    int len = strlen(text);
    SDL_Rect r = {x, y, len * 8 * scale, 12 * scale};
    SDL_SetRenderDrawColor(renderer, TEXT_COLOR.r, TEXT_COLOR.g, TEXT_COLOR.b, TEXT_COLOR.a);
    SDL_RenderDrawRect(renderer, &r);
}

void drawNumber(SDL_Renderer *renderer, int x, int y, double value, int width = 200, int height = 80) {
    // Draw box with number (placeholder visualization)
    SDL_Rect box = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, ACCENT_COLOR.r, ACCENT_COLOR.g, ACCENT_COLOR.b, ACCENT_COLOR.a);
    SDL_RenderDrawRect(renderer, &box);

    // Draw bar representing value (simple visualization)
    int barWidth = (int)(abs(value) * 2) % width;
    SDL_Rect bar = {x + 5, y + height/2 - 10, barWidth, 20};
    SDL_SetRenderDrawColor(renderer, ACCENT_COLOR.r, ACCENT_COLOR.g, ACCENT_COLOR.b, ACCENT_COLOR.a);
    SDL_RenderFillRect(renderer, &bar);
}

void renderMainScreen(SDL_Renderer *renderer, Button &configBtn, Button &calibrateBtn) {
    // Title area
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 60};
    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderFillRect(renderer, &titleBar);

    // Position display
    drawNumber(renderer, 50, 100, total_x, 300, 120);
    drawNumber(renderer, 450, 100, total_y, 300, 120);

    // Labels (placeholder)
    SDL_Rect labelX = {50, 80, 20, 16};
    SDL_Rect labelY = {450, 80, 20, 16};
    SDL_SetRenderDrawColor(renderer, TEXT_COLOR.r, TEXT_COLOR.g, TEXT_COLOR.b, TEXT_COLOR.a);
    SDL_RenderFillRect(renderer, &labelX);
    SDL_RenderFillRect(renderer, &labelY);

    // Section angles
    drawNumber(renderer, 50, 280, section_a_angle, 200, 60);
    drawNumber(renderer, 300, 280, section_b_angle, 200, 60);

    // Buttons
    configBtn.draw(renderer);
    calibrateBtn.draw(renderer);
}

void renderConfigScreen(SDL_Renderer *renderer, Button &backBtn) {
    // Title
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 60};
    SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
    SDL_RenderFillRect(renderer, &titleBar);

    // Config options (placeholder boxes)
    for (int i = 0; i < 4; i++) {
        SDL_Rect option = {50, 100 + i * 80, SCREEN_WIDTH - 100, 60};
        SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
        SDL_RenderFillRect(renderer, &option);
        SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
        SDL_RenderDrawRect(renderer, &option);
    }

    backBtn.draw(renderer);
}

void renderCalibrateScreen(SDL_Renderer *renderer, Button &backBtn, Button &zeroBtn) {
    // Title
    SDL_Rect titleBar = {0, 0, SCREEN_WIDTH, 60};
    SDL_SetRenderDrawColor(renderer, 40, 50, 40, 255);
    SDL_RenderFillRect(renderer, &titleBar);

    // Current readings
    drawNumber(renderer, SCREEN_WIDTH/2 - 150, 150, total_x, 300, 100);
    drawNumber(renderer, SCREEN_WIDTH/2 - 150, 280, total_y, 300, 100);

    backBtn.draw(renderer);
    zeroBtn.draw(renderer);
}

int main(int argc, char *argv[]) {
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Height-O-Meter",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    // Sections
    Section a{0x50, 353, 0, 30, 0, 0, true, false};
    Section b{0x01, 353, 30, 40, 0, 0, false, false};

    // Start Modbus thread
    std::thread modbus(modbusThread, &a, &b);

    // Buttons
    Button configBtn{{50, 400, 150, 60}, "Config"};
    Button calibrateBtn{{220, 400, 150, 60}, "Calibrate"};
    Button backBtn{{50, 400, 150, 60}, "Back"};
    Button zeroBtn{{SCREEN_WIDTH/2 - 75, 400, 150, 60}, "Zero"};

    // Main loop
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }

            int mx = 0, my = 0;

            // Handle both mouse and touch
            if (e.type == SDL_MOUSEMOTION) {
                mx = e.motion.x;
                my = e.motion.y;
                configBtn.hovered = configBtn.contains(mx, my);
                calibrateBtn.hovered = calibrateBtn.contains(mx, my);
                backBtn.hovered = backBtn.contains(mx, my);
                zeroBtn.hovered = zeroBtn.contains(mx, my);
            }

            if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_FINGERDOWN) {
                if (e.type == SDL_FINGERDOWN) {
                    mx = (int)(e.tfinger.x * SCREEN_WIDTH);
                    my = (int)(e.tfinger.y * SCREEN_HEIGHT);
                } else {
                    mx = e.button.x;
                    my = e.button.y;
                }

                Screen screen = currentScreen.load();

                if (screen == Screen::MAIN) {
                    if (configBtn.contains(mx, my)) {
                        currentScreen = Screen::CONFIG;
                    } else if (calibrateBtn.contains(mx, my)) {
                        currentScreen = Screen::CALIBRATE;
                    }
                } else if (screen == Screen::CONFIG) {
                    if (backBtn.contains(mx, my)) {
                        currentScreen = Screen::MAIN;
                    }
                } else if (screen == Screen::CALIBRATE) {
                    if (backBtn.contains(mx, my)) {
                        currentScreen = Screen::MAIN;
                    } else if (zeroBtn.contains(mx, my)) {
                        // TODO: implement zero calibration
                    }
                }
            }

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_q) {
                    running = false;
                } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    currentScreen = Screen::MAIN;
                }
            }
        }

        // Clear
        SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
        SDL_RenderClear(renderer);

        // Render current screen
        Screen screen = currentScreen.load();
        switch (screen) {
            case Screen::MAIN:
                renderMainScreen(renderer, configBtn, calibrateBtn);
                break;
            case Screen::CONFIG:
                renderConfigScreen(renderer, backBtn);
                break;
            case Screen::CALIBRATE:
                renderCalibrateScreen(renderer, backBtn, zeroBtn);
                break;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);  // ~60fps
    }

    modbus.join();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
