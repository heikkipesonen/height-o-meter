#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <modbus/modbus-rtu.h>
#include <modbus/modbus.h>
#include <thread>

struct boom_joint {
  int sensor_id;
  int dist;
};

struct SensorData {
  double x;
  double y;
};

void clearScreenANSI() {
  std::cout << "\033[2J\033[1;1H"; // Clear screen and move cursor to top-left
}

void open_connection(modbus_t *&ctx) {
  ctx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
  if (ctx == nullptr) {
    std::cerr << "fart";
  }

  modbus_connect(ctx);
  modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
}

void cleanup(modbus_t *&ctx) {
  modbus_close(ctx);
  modbus_free(ctx);
}

SensorData read_device(modbus_t *ctx, int id) {
  uint16_t tab_reg[2];
  SensorData data{0, 0};

  modbus_set_slave(ctx, id);
  int rc = modbus_read_registers(ctx, 61, 2, tab_reg);
  if (rc == -1) {
    fprintf(stderr, "%s\n", modbus_strerror(errno));
  }
  std::cout << "::" << rc;
  // Extract raw values
  int16_t roll_raw = static_cast<int16_t>(tab_reg[0]);
  int16_t pitch_raw = static_cast<int16_t>(tab_reg[1]);

  data.x = (roll_raw / 32768.0) * 180.0;
  data.y = (pitch_raw / 32768.0) * 180.0;

  return data;
}

double toRadians(double deg) { return deg * (3.14159265358979323846 / 180); }

double get_y_of(double angle, int dist) {
  return std::sin(toRadians(angle)) * dist;
}
double get_x_of(double angle, int dist) {
  return std::cos(toRadians(angle)) * dist;
}

int main() {
  modbus_t *ctx = nullptr;
  open_connection(ctx);

  int counter = 0;
  while (counter < 100) {
    clearScreenANSI();

    boom_joint a{80, 1000};

    SensorData reading = read_device(ctx, a.sensor_id);
    double y = get_y_of(reading.x, a.dist);

    std::cout << "boom y:" << y << "\n";

    std::cout << "Count: " << counter << "\n";
    std::cout << "Roll: " << reading.x << "°, Pitch: " << reading.y << "°"
              << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    counter++;
  }
  cleanup(ctx);
  return 0;
}
