# Agent Development Guide

## Rules

- Deploy only when changes are complete and tested locally if possible
- Do exactly what is asked, no more
- One change at a time, test, then commit if working
- When reverting, be careful not to lose working code
- No guessing - ask for sensor readings and actual measurements
- Do not build locally - macOS lacks libmodbus and SDL2
- Use existing UI components (Button, colors, layout constants) instead of custom drawing

## Deploy

```bash
rsync -av --checksum --exclude 'build/' --exclude '.git/' ~/Documents/height-o-meter/src/ rpi@192.168.4.200:~/height-o-meter/src/ && ssh rpi@192.168.4.200 "cd ~/height-o-meter/build && make -j4 && sudo systemctl restart heightmatic"
```

To sync CMakeLists.txt as well:
```bash
rsync -av ~/Documents/height-o-meter/CMakeLists.txt rpi@192.168.4.200:~/height-o-meter/
```

## Architecture

### Display
- Physical: 800×480 landscape DSI touchscreen
- Logical: 480×800 portrait (rotated via `display_lcd_rotate=1`)
- SDL kmsdrm doesn't respect rotation - app renders with coordinates transformed

### Sensors
Witmotion industrial angle sensors on RS485 Modbus:
- SENSOR_SUPERSTRUCTURE (id 1) - cab rotation
- SENSOR_BOOM_A (id 2) - main boom section
- SENSOR_BOOM_B (id 3) - secondary boom section
- SENSOR_STICK (id 4) - stick/arm
- SENSOR_TILT (id 5) - bucket tilt
- SENSOR_TEST (id 80) - for setup/probing

Sensor readings:
- X axis = roll
- Y axis = pitch
- 0° = vertical, 90° = horizontal

### Position Calculation

Config per sensor (in config.cpp):
- `axis` - which axis to use (X or Y)
- `inverted` - flip sign of angle
- `points_down` - segment points down at 0° (for Y calculation)
- `length_mm` - segment length

Current calculation (excavator.cpp):
```cpp
x += sin(angle) * length;
y += cos(angle) * length;  // or -= if points_down
```

### Bucket Config
```cpp
struct BucketConfig {
    const char* name;
    int coupler_length_mm;  // stick end to tilt pin
    int bucket_length_mm;   // tilt pin to bucket edge
    int bucket_width_mm;    // for sideways tilt compensation
};
```

Tilt sensor uses:
- X axis (roll) for curl angle
- Y axis (pitch) for sideways tilt compensation

## Current Issue: Boom B Angle

Problem: Height error grows when boom A and B pivot relative to each other.

Boom B visualization flips direction when passing 90° horizontal:
- Below 90°: cos() positive, Y goes up
- Above 90°: cos() negative, Y goes down with `y +=`, but boom B continues downward physically

The `points_down` flag doesn't fully solve this because boom B's world direction depends on boom A's angle.

Known positions:
- Both A and B at 90°: both horizontal, B continues forward from A (straight line)
- A at 90°, B at 30°: B points upward/backward from A's end (folded in)

## Key Files

- `src/config/config.h` - Config structs
- `src/config/config.cpp` - Sensor and bucket configuration
- `src/excavator/excavator.cpp` - Position calculation, sensor reading
- `src/excavator/excavator.h` - State structs, sensor enums
- `src/ui/ui.cpp` - Main UI loop, screen switching
- `src/ui/layout.h` - Layout constants (margins, gaps, row heights, screen dimensions)
- `src/ui/visualize_screen.cpp` - Arm visualization for debugging
- `src/ui/main_screen.cpp` - Main display with depth/reach (values shown in cm)

## UI Layout System

All screens use `src/ui/layout.h` for consistent spacing:
```cpp
namespace Layout {
    constexpr int SCREEN_WIDTH = 480;
    constexpr int SCREEN_HEIGHT = 800;
    constexpr int MARGIN = 20;
    constexpr int GAP = 10;
    constexpr int CONTENT_WIDTH = 440;  // SCREEN_WIDTH - 2*MARGIN
    constexpr int BUTTON_HEIGHT = 60;
    constexpr int ROW_HEIGHT = 70;      // BUTTON_HEIGHT + GAP
    constexpr int HALF_WIDTH = 215;     // (CONTENT_WIDTH - GAP) / 2
    constexpr int BOTTOM_Y = 720;
    // ROW1_Y through ROW8_Y for standard row positions
}
```

Add `using namespace Layout;` at top of screen files.

## Gotchas

- Helper functions like `drawValueBox` must be `static` if defined in multiple .cpp files (linker error otherwise)
- Main screen shows depth/reach in **cm** (divided by 10 from internal mm values)
- Sensor angles stored in `state->sensors[i].roll` (X) and `.pitch` (Y) - processed angle from `getSensorAngle()`
