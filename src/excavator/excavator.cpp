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

struct SensorData {
    double x;
    double y;
    bool valid = false;
};

void open_connection(modbus_t *&ctx, const char* port, int baud) {
    ctx = modbus_new_rtu(port, baud, 'N', 8, 1);
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
        data.valid = false;
        return data;
    }

    int16_t roll_raw = static_cast<int16_t>(tab_reg[0]);
    int16_t pitch_raw = static_cast<int16_t>(tab_reg[1]);

    data.x = (roll_raw / 32768.0) * 180.0;
    data.y = (pitch_raw / 32768.0) * 180.0;
    data.valid = true;

    return data;
}

bool read_sensor(modbus_t *ctx, Sensor *sensor) {
    if (sensor->id == 0) return false;
    
    modbus_set_slave(ctx, sensor->id);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    SensorData data = read_angle(ctx);
    if (data.valid) {
        sensor->roll = data.x - sensor->roll_offset;
        sensor->pitch = data.y - sensor->pitch_offset;
        sensor->connected = true;
        return true;
    }
    sensor->connected = false;
    return false;
}

int update_device_id(modbus_t *ctx, int newId) {
    int unlock = modbus_write_register(ctx, 0x69, 0xB588);
    if (unlock == -1) {
        fprintf(stderr, "Unlock failed: %s\n", modbus_strerror(errno));
        return -1;
    }

    int update = modbus_write_register(ctx, 0x1A, newId);
    if (update == -1) {
        fprintf(stderr, "Write ID failed: %s\n", modbus_strerror(errno));
        return -1;
    }

    modbus_set_slave(ctx, newId);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    int save = modbus_write_register(ctx, 0x00, 0x00);
    if (save == -1) {
        fprintf(stderr, "Save failed: %s\n", modbus_strerror(errno));
        return -1;
    }

    return 0;
}

// Get the angle to use from sensor based on config
double getSensorAngle(const Sensor &sensor, const SensorConfig &cfg) {
    double angle = (cfg.axis == MountAxis::X) ? sensor.roll : sensor.pitch;
    return cfg.inverted ? -angle : angle;
}

// Calculate position from all arm segments
// Sensors read absolute angle from vertical (0° = straight up/down depending on mount)
// Positive angle = tilted away from cab
void calculatePosition(ExcavatorState *state, const ExcavatorConfig *config) {
    // Start at boom pin position
    double x = config->pivot_offset_x_mm;
    double y = config->base_height_mm + config->pivot_offset_y_mm;
    
    // Each sensor reads absolute angle from vertical (gravity-referenced)
    for (int i = SENSOR_BOOM_A; i <= SENSOR_TILT; i++) {
        const Sensor &sensor = state->sensors[i];
        const SensorConfig &cfg = config->sensors[i];
        
        if (!sensor.connected || cfg.length_mm == 0) continue;
        
        double angle_deg = getSensorAngle(sensor, cfg);
        double rad = toRadians(angle_deg);
        
        // Horizontal: always sin(angle) * length
        x += std::sin(rad) * cfg.length_mm;
        
        // Vertical: cos(angle) * length, but subtract if segment points down at 0°
        if (cfg.points_down) {
            y -= std::cos(rad) * cfg.length_mm;
        } else {
            y += std::cos(rad) * cfg.length_mm;
        }
    }
    
    state->reach = x;
    state->depth = y;
}

} // anonymous namespace

void excavator_thread(ExcavatorState *state, const ExcavatorConfig *config) {
    modbus_t *ctx = nullptr;
    int consecutive_failures = 0;
    
    open_connection(ctx, config->serial_port, config->baud_rate);

    while (state->running) {
        // Paused for ID change - close connection and wait
        if (state->paused) {
            if (ctx != nullptr) {
                cleanup(ctx);
                ctx = nullptr;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // Manual reconnect requested
        if (state->reconnect) {
            cleanup(ctx);
            ctx = nullptr;
            state->reconnect = false;
        }
        
        // Reconnect if connection lost
        if (ctx == nullptr) {
            open_connection(ctx, config->serial_port, config->baud_rate);
            if (ctx == nullptr) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }
            consecutive_failures = 0;
        }
        
        // Read sensors (skip test sensor at index 5)
        int failures = 0;
        for (int i = 0; i < NUM_SENSORS - 1; i++) {
            if (!read_sensor(ctx, &state->sensors[i])) {
                failures++;
            }
        }
        
        // If all reads fail repeatedly, try USB reset then reconnect
        if (failures == NUM_SENSORS - 1) {
            consecutive_failures++;
            if (consecutive_failures > 10) {
                cleanup(ctx);
                ctx = nullptr;
                
                // Try USB device reset
                FILE *f = fopen("/sys/bus/usb/devices/1-1.3/authorized", "w");
                if (f) {
                    fprintf(f, "0");
                    fclose(f);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    f = fopen("/sys/bus/usb/devices/1-1.3/authorized", "w");
                    if (f) {
                        fprintf(f, "1");
                        fclose(f);
                        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                    }
                }
                
                consecutive_failures = 0;
            }
        } else {
            consecutive_failures = 0;
        }
        
        // Calculate bucket tip position
        calculatePosition(state, config);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    cleanup(ctx);
}

bool probe_sensor(ExcavatorState *state, int id, double *roll, double *pitch) {
    // Pause the polling thread and wait for it to release the port
    state->paused = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    modbus_t *ctx = nullptr;
    open_connection(ctx, "/dev/ttyUSB0", 9600);
    
    if (ctx == nullptr) {
        state->paused = false;
        return false;
    }

    modbus_set_slave(ctx, id);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    modbus_set_response_timeout(ctx, 0, 200000); // 200ms timeout
    
    SensorData data = read_angle(ctx);
    
    cleanup(ctx);
    
    // Resume polling
    state->paused = false;
    
    if (data.valid) {
        *roll = data.x;
        *pitch = data.y;
        return true;
    }
    return false;
}

int update_sensor_id(ExcavatorState *state, int current_id, int new_id) {
    // Pause the polling thread and wait for it to release the port
    state->paused = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    modbus_t *ctx = nullptr;
    open_connection(ctx, "/dev/ttyUSB0", 9600);
    
    if (ctx == nullptr) {
        fprintf(stderr, "Failed to open connection\n");
        state->paused = false;
        return -1;
    }

    modbus_set_slave(ctx, current_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    int result = update_device_id(ctx, new_id);
    
    cleanup(ctx);
    
    // Resume polling
    state->paused = false;
    return result;
}
