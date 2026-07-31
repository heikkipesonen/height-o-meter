#include <cstdio>
#include <cstdint>
#include <modbus/modbus.h>
#include <modbus/modbus-rtu.h>

int main() {
    modbus_t *ctx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
    if (!ctx) {
        fprintf(stderr, "Failed to create modbus context\n");
        return 1;
    }
    modbus_connect(ctx);
    modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);

    const char* names[] = {"Superstructure", "Boom A", "Boom B", "Stick", "Curl/Tilt"};
    int ids[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        modbus_set_slave(ctx, ids[i]);
        uint16_t regs[2];
        int rc = modbus_read_registers(ctx, 61, 2, regs);
        if (rc == 2) {
            int16_t x_raw = (int16_t)regs[0];
            int16_t y_raw = (int16_t)regs[1];
            double x = (x_raw / 32768.0) * 180.0;
            double y = (y_raw / 32768.0) * 180.0;
            printf("%s (id %d): X=%.1f Y=%.1f\n", names[i], ids[i], x, y);
        } else {
            printf("%s (id %d): not connected\n", names[i], ids[i]);
        }
    }

    modbus_close(ctx);
    modbus_free(ctx);
    return 0;
}
