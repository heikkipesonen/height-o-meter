#pragma once

#include "config.h"
#include "../excavator/excavator.h"

// Stored position for comparison
struct StoredPosition {
    Sensor sensors[NUM_SENSORS];
    double depth = 0;
    double reach = 0;
    bool occupied = false;
};

// Load sensor config from file, returns true if file existed
bool loadSensorConfig(ExcavatorConfig *config, const char *path);

// Save sensor config to file
bool saveSensorConfig(const ExcavatorConfig *config, const char *path);

// Load stored positions from file
bool loadPositions(StoredPosition *positions, int maxPositions, int *selectedPosition, const char *path);

// Save stored positions to file
bool savePositions(const StoredPosition *positions, int maxPositions, int selectedPosition, const char *path);

// Default config file paths
constexpr const char* CONFIG_FILE_PATH = "/home/rpi/sensor_config.txt";
constexpr const char* POSITIONS_FILE_PATH = "/home/rpi/positions.txt";
