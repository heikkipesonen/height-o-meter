#include "excavator.h"
#include <cmath>
#include <chrono>
#include <thread>
#include <modbus/modbus.h>
#include <modbus/modbus-rtu.h>
#include <cerrno>
#include <cstdio>

namespace {

constexpr double PI = 3.14159265358979323846;

double toRadians(double deg) {
    return deg * (PI / 180.0);
}

double get_y_of(double angle, int dist) {
    return std::sin(toRadians(angle)) * dist;
}

double get_x_of(double angle, int dist) {
    return std::cos(toRadians(angle)) * dist;
}

struct SensorData {
    double x;
    double y;
};

void open_connection(modbus_t *&ctx) {
    ctx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
    if (ctx == nullptr) {
        return;
    }
    modbus_connect(ctx);
    modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
}

void cleanup(modbus_t *&ctx) {
    if (ctx) {
        modbus_close(ctx);
        modbus_free(ctx);
    }
}

SensorData read_angle(modbus_t *ctx) {
    uint16_t tab_reg[2];
    SensorData data{0, 0};

    int rc = modbus_read_registers(ctx, 61, 2, tab_reg);
    if (rc == -1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return data;
    }

    int16_t roll_raw = static_cast<int16_t>(tab_reg[0]);
    int16_t pitch_raw = static_cast<int16_t>(tab_reg[1]);

    data.x = (roll_raw / 32768.0) * 180.0;
    data.y = (pitch_raw / 32768.0) * 180.0;

    return data;
}

void update_section(modbus_t *ctx, Section *s) {
    modbus_set_slave(ctx, s->sensor_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    SensorData data = read_angle(ctx);
    s->value_x = data.x;
    s->value_y = data.y;
}

} // anonymous namespace

double get_section_x(Section *section) {
    int multiplier = section->inv_x ? -1 : 1;
    return (get_x_of(section->value_x, section->dist) + section->base_offset_x) * multiplier;
}

double get_section_y(Section *section) {
    int multiplier = section->inv_y ? -1 : 1;
    return (get_y_of(section->value_x, section->dist) + section->base_offset_y) * multiplier;
}

void excavator_thread(ExcavatorState *state, Section *a, Section *b) {
    modbus_t *ctx = nullptr;
    open_connection(ctx);

    while (state->running) {
        update_section(ctx, a);
        update_section(ctx, b);

        state->total_x = get_section_x(a) + get_section_x(b);
        state->total_y = get_section_y(a) + get_section_y(b);
        state->section_a_angle = a->value_x;
        state->section_b_angle = b->value_x;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    cleanup(ctx);
}
