#include <chrono>
#include <iostream>
#include <modbus/modbus-rtu.h>
#include <modbus/modbus.h>
#include <ostream>
#include <thread>

#include "include/device-handler.h"
#include "include/machine.h"
#include "include/utils.h"

void clearScreenANSI() {
  std::cout << "\033[2J\033[1;1H"; // Clear screen and move cursor to top-left
}

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

  SensorData r_a = read_angle(ctx);
  x->value_x = r_a.x;
  x->value_y = r_a.y;
}

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
