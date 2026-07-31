#include <thread>
#include "src/excavator/excavator.h"
#include "src/config/config.h"
#include "src/config/config_file.h"
#include "src/ui/ui.h"

int main(int argc, char *argv[]) {
    // Load config (compiled defaults)
    ExcavatorConfig config = getConfig();
    
    // Override with saved config file if exists
    loadSensorConfig(&config, CONFIG_FILE_PATH);

    // Shared state
    ExcavatorState state;

    // Initialize sensor IDs from config
    for (int i = 0; i < NUM_SENSORS; i++) {
        state.sensors[i].id = config.sensors[i].id;
    }

    // Start excavator thread
    std::thread excavator(excavator_thread, &state, &config);

    // Run UI (blocks until quit)
    UI ui(&config);
    if (ui.init()) {
        ui.run(&state);
    }

    // Cleanup
    state.running = false;
    excavator.join();

    return 0;
}
