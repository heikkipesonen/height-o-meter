#pragma once

#include <atomic>
#include "src/config/config.h"

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

struct ExcavatorState {
    // Raw sensor data (updated by modbus thread)
    Sensor sensors[NUM_SENSORS];
    
    // Calculated bucket tip position relative to ground
    std::atomic<double> depth{0};    // Vertical (mm, negative = below ground)
    std::atomic<double> reach{0};    // Horizontal from pivot (mm)
    
    std::atomic<bool> running{true};
    std::atomic<bool> reconnect{false};  // Set to trigger modbus reconnect
    std::atomic<bool> paused{false};     // Pause polling for ID change
};

// Sensor configuration
// Changes sensor's Modbus address. Connect only ONE sensor at a time.
// Returns 0 on success, -1 on failure.
int update_sensor_id(ExcavatorState *state, int current_id, int new_id);

// Probe a sensor by ID. Returns true if responds, fills roll/pitch.
bool probe_sensor(ExcavatorState *state, int id, double *roll, double *pitch);

// Modbus thread entry point
void excavator_thread(ExcavatorState *state, const ExcavatorConfig *config);
