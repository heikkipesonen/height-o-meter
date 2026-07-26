#include "include/device-handler.h"
#include <cerrno>
#include <iostream>
#include <modbus/modbus-rtu.h>
#include <modbus/modbus.h>

void open_connection(modbus_t *&ctx) {
  ctx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
  if (ctx == nullptr) {
    return;
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

SensorData read_angle(modbus_t *ctx) {
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
