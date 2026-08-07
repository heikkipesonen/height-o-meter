#pragma once

#include <vector>

// Sensor mounting orientation
enum class MountAxis {
    X,  // Use X axis
    Y   // Use Y axis
};

struct SensorConfig {
    int id = 0;                     // Modbus address
    const char* name = "";          // Display name
    MountAxis axis = MountAxis::Y;  // Which axis to use
    bool inverted = false;          // Flip angle sign
    bool points_down = false;       // True if 0° means pointing down
    double offset = 0.0;            // Angle offset to apply
    int length_mm = 0;              // Arm segment length in mm
};

struct BucketConfig {
    const char* name = "";          // Bucket name
    int coupler_length_mm = 0;      // Stick end to tilt pin
    int bucket_length_mm = 0;       // Tilt pin to bucket edge
    int bucket_width_mm = 0;        // Bucket width for tilt calc
};

struct ExcavatorConfig {
    const char* serial_port = "/dev/ttyUSB0";
    int baud_rate = 9600;
    
    // Base height (ground to center pivot)
    int base_height_mm = 0;
    
    // Pivot point offset (center pivot to boom pin)
    int pivot_offset_x_mm = 0;      // Horizontal distance
    int pivot_offset_y_mm = 0;      // Vertical distance (positive = up)
    
    // Lean indicator range (degrees)
    double lean_x_max = 15.0;       // X bar (roll) range ±
    double lean_y_max = 15.0;       // Y bar (pitch) range ±
    
    SensorConfig sensors[6];
    
    // Bucket configurations
    std::vector<BucketConfig> buckets;
    int active_bucket = 0;
};

// Returns the config - edit config.cpp to change values
ExcavatorConfig getConfig();
