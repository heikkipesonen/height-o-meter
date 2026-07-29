#include <thread>
#include "src/excavator/excavator.h"
#include "src/ui/ui.h"

int main(int argc, char *argv[]) {
    // Shared state
    ExcavatorState state;

    // Initialize sensor IDs (change these after configuring sensors)
    state.sensors[SENSOR_SUPERSTRUCTURE].id = 1;
    state.sensors[SENSOR_BOOM_A].id = 2;
    state.sensors[SENSOR_BOOM_B].id = 3;
    state.sensors[SENSOR_STICK].id = 4;
    state.sensors[SENSOR_TILT].id = 5;
    state.sensors[SENSOR_TEST].id = 80;  // Test sensor for setup screen

    // Sections config (legacy, for backward compat)
    Section a{0x02, 353, 0, 30, 0, 0, true, false};  // Boom A
    Section b{0x03, 353, 30, 40, 0, 0, false, false}; // Boom B

    // Start excavator thread
    std::thread excavator(excavator_thread, &state, &a, &b);

    // Run UI (blocks until quit)
    UI ui;
    if (ui.init()) {
        ui.run(&state);
    }

    // Cleanup
    state.running = false;
    excavator.join();

    return 0;
}
