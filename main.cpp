#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <modbus/modbus-rtu.h>
#include <modbus/modbus.h>
#include <ostream>
#include <thread>

double toRadians(double deg) { return deg * (3.14159265358979323846 / 180); }

double get_y_of(double angle, int dist) {
  return std::sin(toRadians(angle)) * dist;
}
double get_x_of(double angle, int dist) {
  return std::cos(toRadians(angle)) * dist;
}

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

int update_device_id(modbus_t *ctx, int newId) {
  // first write code to unlock register to enable writing
  int unlock = modbus_write_register(ctx, 0x69, 0xB588);
  if (unlock == -1) {
    fprintf(stderr, "%s\n", modbus_strerror(errno));
    return -1;
  }

  // 0x1A slave_id register for witmotion sinat-485
  int update = modbus_write_register(ctx, 0x1A, newId);
  if (update == -1) {
    fprintf(stderr, "%s\n", modbus_strerror(errno));
    return -1;
  }

  // id of device updated already, dunno if this is even needed
  modbus_set_slave(ctx, newId);

  int save = modbus_write_register(ctx, 0x0000, 0x0000);
  if (save == -1) {
    fprintf(stderr, "%s\n", modbus_strerror(errno));
    return -1;
  }

  return 0;
}

struct SensorData {
  double x;
  double y;
};

SensorData readAngle(modbus_t *ctx) {
  uint16_t tab_reg[2];
  SensorData data{0, 0};

  // x & y rotation is stored register 61 & 62
  int rc = modbus_read_registers(ctx, 61, 2, tab_reg);

  if (rc == -1) {
    fprintf(stderr, "%s\n", modbus_strerror(errno));
  }

  int16_t roll_raw = static_cast<int16_t>(tab_reg[0]);
  int16_t pitch_raw = static_cast<int16_t>(tab_reg[1]);

  data.x = (roll_raw / 32768.0) * 180.0;
  data.y = (pitch_raw / 32768.0) * 180.0;

  return data;
}

struct Section {
  int sensor_id;
  int dist = 0;
  int base_offset_x = 0;
  int base_offset_y = 0;
  double value_x = 0;
  double value_y = 0;
  bool inv_x = false;
  bool inv_y = false;
};

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

  SensorData r_a = readAngle(ctx);
  x->value_x = r_a.x;
  x->value_y = r_a.y;
}

struct Excavator {
  Section superstructure;
  Section boom_a;
  Section boom_b;
  Section boom_c;
  Section coupler;
};

int main() {
  modbus_t *ctx = nullptr;
  open_connection(ctx);

  Section a{0x50, 353, 0, 30, 0, 0, true};
  Section b{0x01, 353, 30, 40, 0, 0, false};

  while (true) {
    clearScreenANSI();

    update_section(ctx, &a);
    update_section(ctx, &b);

    int total_x = get_section_x(&a) + get_section_x(&b);
    int total_y = get_section_y(&a) + get_section_y(&b);

    std::cout << "X: " << total_x << " Y: " << total_y << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  };

  cleanup(ctx);
  return 0;
}
