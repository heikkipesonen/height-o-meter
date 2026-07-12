#include <cmath>
#include <cstdint>
#include <iostream>
#include <modbus/modbus.h>

struct boom_joint {
  int sensor_id;
  int dist;
  double angle_x;
  double angle_y;
};

void clearScreenANSI() {
  std::cout << "\033[2J\033[1;1H"; // Clear screen and move cursor to top-left
}

void open_connection(modbus_t *&ctx) {
  ctx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
  if (ctx == nullptr) {
    std::cerr << "fart";
  }
}

void cleanup(modbus_t *&ctx) {
  modbus_close(ctx);
  modbus_free(ctx);
}

struct SensorData {
  double x;
  double y;
  double z;
};

SensorData read_device(modbus_t *ctx, int id) {
  uint16_t tab_reg[3];

  modbus_set_slave(ctx, id);
  modbus_read_registers(ctx, 60, 3, tab_reg);

  // Extract raw values
  int16_t roll_raw = static_cast<int16_t>(tab_reg[0]);
  int16_t pitch_raw = static_cast<int16_t>(tab_reg[1]);
  int16_t yaw_raw = static_cast<int16_t>(tab_reg[2]);

  SensorData data;
  data.x = (roll_raw / 32768.0) * 180.0;
  data.y = (pitch_raw / 32768.0) * 180.0;
  data.z = (yaw_raw / 32768.0) * 180.0;

  return data;
}

double toRadians(double deg) { return deg * (3.14159265358979323846 / 180); }

double get_y_of(double angle, int dist) {
  return std::sin(toRadians(angle)) * dist;
}
double get_x_of(double angle, int dist) {
  return std::cos(toRadians(angle)) * dist;
}

double get_boom_joint_y(boom_joint *&joint) {
  return get_y_of(joint->angle_y, joint->dist);
}

int main() {
  modbus_t *ctx = nullptr;
  open_connection(ctx);

  boom_joint a;
  a.dist = 1000;
  a.sensor_id = 80;
  a.angle_x = 0;
  a.angle_y = 0;

  SensorData reading = read_device(ctx, 80);

  std::cout << "fart" << reading.x << "\n";
  std::cout << "fart" << reading.y;

  cleanup(ctx);
  return 0;
}
