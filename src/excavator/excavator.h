#pragma once

#include <atomic>

struct Section {
    int sensor_id = 0;
    int dist = 0;
    int base_offset_x = 0;
    int base_offset_y = 0;
    double value_x = 0;
    double value_y = 0;
    bool inv_x = false;
    bool inv_y = false;
};

struct ExcavatorState {
    std::atomic<double> total_x{0};
    std::atomic<double> total_y{0};
    std::atomic<double> section_a_angle{0};
    std::atomic<double> section_b_angle{0};
    std::atomic<bool> running{true};
};

// Position calculations
double get_section_x(Section *section);
double get_section_y(Section *section);

// Sensor configuration
// Changes sensor's Modbus address. Connect only ONE sensor at a time.
// Returns 0 on success, -1 on failure.
int update_sensor_id(int current_id, int new_id);

// Modbus thread entry point
void excavator_thread(ExcavatorState *state, Section *a, Section *b);
