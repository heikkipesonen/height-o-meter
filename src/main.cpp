#include <thread>
#include "src/excavator/excavator.h"
#include "src/ui/ui.h"

int main(int argc, char *argv[]) {
    // Shared state
    ExcavatorState state;

    // Sections config
    Section a{0x50, 353, 0, 30, 0, 0, true, false};
    Section b{0x01, 353, 30, 40, 0, 0, false, false};

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
