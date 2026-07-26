#pragma once

#include <cerrno>
#include <iostream>
#include <modbus/modbus-rtu.h>
#include <modbus/modbus.h>

struct SensorData {
  double x;
  double y;
};

void open_connection(modbus_t *&ctx);
void cleanup(modbus_t *&ctx);
int update_device_id(modbus_t *ctx, int newId);
SensorData read_angle(modbus_t *ctx);
