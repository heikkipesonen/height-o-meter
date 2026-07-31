#pragma once

#include "config.h"

// Load sensor config from file, returns true if file existed
bool loadSensorConfig(ExcavatorConfig *config, const char *path);

// Save sensor config to file
bool saveSensorConfig(const ExcavatorConfig *config, const char *path);

// Default config file path
constexpr const char* CONFIG_FILE_PATH = "/home/rpi/sensor_config.txt";
