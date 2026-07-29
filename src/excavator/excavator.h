#pragma once

#include <atomic>

constexpr int NUM_SENSORS = 6;

// Sensor locations
enum SensorIndex {
    SENSOR_SUPERSTRUCTURE = 0,
    SENSOR_BOOM_A = 1,
    SENSOR_BOOM_B = 2,
    SENSOR_STICK = 3,
    SENSOR_TILT = 4,
    SENSOR_TEST = 5  // For setup screen probing
};

struct Sensor {
    int id = 0;                  // Modbus address
    double roll = 0;             // X axis (degrees)
    double pitch = 0;            // Y axis (degrees)
    double roll_offset = 0;      // Calibration offset
    double pitch_offset = 0;     // Calibration offset
    bool connected = false;      // Last read succeeded
};

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
    // Raw sensor data (updated by modbus thread)
    Sensor sensors[NUM_SENSORS];
    
    // Calculated position (legacy, keep for now)
    std::atomic<double> total_x{0};
    std::atomic<double> total_y{0};
    std::atomic<double> section_a_angle{0};
    std::atomic<double> section_b_angle{0};
    
    std::atomic<bool> running{true};
    std::atomic<bool> reconnect{false};  // Set to trigger modbus reconnect
    std::atomic<bool> paused{false};     // Pause polling for ID change
};

// Position calculations
double get_section_x(Section *section);
double get_section_y(Section *section);

// Sensor configuration
// Changes sensor's Modbus address. Connect only ONE sensor at a time.
// Returns 0 on success, -1 on failure.
int update_sensor_id(ExcavatorState *state, int current_id, int new_id);

// Probe a sensor by ID. Returns true if responds, fills roll/pitch.
bool probe_sensor(ExcavatorState *state, int id, double *roll, double *pitch);

// Modbus thread entry point
void excavator_thread(ExcavatorState *state, Section *a, Section *b);
