# 5-Sensor Excavator Position System

## Problem Statement
Calculate the 3D world-relative position of the bucket tip using 5 dual-axis sensors, with slope compensation and a user-settable zero reference point.

## Requirements
- 5 sensors: superstructure, boom A, boom B, stick, tilt hitch (each with X and Y axis readings in degrees)
- World-relative XYZ output (compensated for machine tilt)
- **Centimeter accuracy** (display in cm, config in mm for precision)
- Configurable arm segment lengths via UI
- User-settable zero reference point
- Display XYZ position relative to zero

## Sensor Setup
| # | Location | Axis X | Axis Y |
|---|----------|--------|--------|
| 1 | Superstructure | Slew rotation | Machine tilt |
| 2 | Boom A | Angle | Sideways tilt |
| 3 | Boom B | Angle | Sideways tilt |
| 4 | Stick | Angle | Sideways tilt |
| 5 | Tilt hitch | Curl angle | Side tilt |

## Sensor Configuration (per sensor)
Each sensor has mounting calibration:
- **Modbus ID**: Sensor address
- **Offset X**: Mounting angle offset for X axis (degrees) - subtracted from raw reading
- **Offset Y**: Mounting angle offset for Y axis (degrees) - subtracted from raw reading
- **Invert X**: Flip X axis sign (bool)
- **Invert Y**: Flip Y axis sign (bool)

Calibration process: Position arm segment horizontal/level, read raw sensor value, store as offset.

## Superstructure Configuration
- **Slew center to boom pivot X**: Forward offset from slew axis to boom pivot (mm)
- **Slew center to boom pivot Y**: Side offset if boom isn't centered (mm)
- **Slew center to boom pivot Z**: Height offset (mm)

## Slew Detection (dual method)
Slew angle is calculated from superstructure sensor using two methods:

**Method 1: Tilt vector rotation (primary)**
- On a slope, ground tilt appears as roll/pitch on superstructure sensor
- When cab rotates, tilt "rotates" through roll and pitch axes
- Slew angle = `atan2(roll, pitch)` relative to zero reference
- Ground slope magnitude = `sqrt(roll² + pitch²)`
- Works well on slopes, unreliable when slope < ~0.5°

**Method 2: Compass/Yaw (fallback)**
- Slew angle = `current_yaw - zero_yaw`
- Works on flat ground
- Subject to drift and magnetic interference from steel

**Combined approach:**
- Store both tilt vector and yaw when setting zero point
- Use tilt method when slope > 0.5° threshold
- Fall back to compass when machine is nearly level
- Optional: warn user if methods diverge significantly (magnetic interference)

**Reference:**
- When user sets zero point, system stores:
  - Tilt vector (roll/pitch) for tilt-based slew
  - Yaw reading for compass-based slew
- Subsequent slew calculated from both, primary method selected by slope magnitude

**Limitation:** On perfectly flat ground with magnetic interference, slew detection may be inaccurate. Re-zero after significant slewing in these conditions.

## Bucket Configuration
Each bucket includes coupler and bucket dimensions (coupler may vary between buckets):
- **Coupler length**: Distance from stick pin to tilt pivot (mm)
- **Bucket length**: Distance from tilt pivot to bucket cutting edge (mm)
- **Bucket width**: Width of bucket (mm) - used for sideways tilt compensation
- **Bucket name**: User-friendly label (e.g., "600mm ditching", "1200mm grading")

The curl/tilt sensor (sensor 5) uses:
- X axis for curl angle (bucket tips forward/back)
- Y axis for sideways tilt compensation (lowest corner calculation)

Multiple bucket profiles stored in `config.buckets` vector, `active_bucket` index selects current.

Example buckets:
| Name | Coupler | Bucket | Width |
|------|---------|--------|-------|
| Grading 1200mm | 27 | 70 | 70 |
| Ditching 600mm | 27 | 45 | 60 |

## Kinematics Chain
```
World Frame
    │
    ▼ (superstructure tilt compensation)
Superstructure ──slew rotation──►
    │
    ▼
Boom A pivot ──angle + tilt──►
    │
    ▼
Boom B pivot ──angle + tilt──►
    │
    ▼
Stick pivot ──angle + tilt──►
    │
    ▼
Coupler pivot ──angle + side pivot──►
    │
    ▼
Bucket Tip (XYZ)
    │
    ▼ (subtract zero point)
Display Position
```

## Repository
`~/Documents/height-o-meter`

## Current Structure
```
src/
├── main.cpp              # Entry point
├── config/
│   ├── config.h          # ExcavatorConfig, SensorConfig, BucketConfig structs
│   ├── config.cpp        # Default config values
│   ├── config_file.h     # Save/load config functions
│   └── config_file.cpp   # Config file persistence
├── excavator/
│   ├── excavator.h       # Sensor, ExcavatorState, sensor enums
│   └── excavator.cpp     # Modbus polling, position calculation
└── ui/
    ├── ui.h              # UI class, Screen enum, ScreenResult
    ├── ui.cpp            # Main UI loop, screen switching
    ├── layout.h          # Layout constants (margins, rows, dimensions)
    ├── fonts.h           # Font access, drawing helpers
    ├── fonts.cpp         # Font loading, text rendering
    ├── colors.h          # Color constants
    ├── button.h          # Button struct with hit detection
    ├── main_screen.cpp/h         # Main display (depth/reach)
    ├── visualize_screen.cpp/h    # Arm visualization for debugging
    ├── sensor_config_screen.cpp/h # Sensor list
    ├── sensor_edit_screen.cpp/h   # Edit individual sensor
    └── sensor_setup_screen.cpp/h  # Change Modbus IDs
```

## Task Breakdown

### Task 1: Extend sensor and state structures
- **Objective**: Update `excavator.h` with 5 sensors and 3D state
- Add `Sensor` struct with: `id`, `length`, `offset_x/y/z`, `angle_x`, `angle_y`, axis inversion flags
- Add `MachineConfig` struct with all 5 sensors and their physical dimensions
- Extend `ExcavatorState` with: `pos_x`, `pos_y`, `pos_z`, `zero_x`, `zero_y`, `zero_z`, individual sensor angles
- **Test**: Compiles, existing code still works with stub values
- **Demo**: Program runs, displays zeros for new fields

### Task 2: Implement 3D rotation math
- **Objective**: Create rotation utilities for forward kinematics
- Add `math_utils.h/cpp` in `src/excavator/` with: `Vec3` struct, rotation matrix functions, `rotateX()`, `rotateY()`, `rotateZ()`, `applyRotation()`
- Implement rotation composition for dual-axis sensors
- **Test**: Unit test rotating known vectors (e.g., rotate [1,0,0] by 90° around Z = [0,1,0])
- **Demo**: Test output printed showing correct rotations

### Task 3: Implement forward kinematics chain
- **Objective**: Calculate bucket tip XYZ from all sensor angles
- Create `calculate_bucket_position()` function that chains all 5 sensors
- Start from superstructure, apply each joint's rotation + translation
- Apply superstructure tilt first to get world-relative frame
- **Test**: With all angles at 0°, bucket tip should be at sum of all segment lengths along boom axis
- **Demo**: Display shows plausible XYZ values when angles hardcoded

### Task 4: Update Modbus polling for 5 sensors
- **Objective**: Read all 5 sensors over Modbus
- Update `excavator_thread()` to iterate through sensor array
- Read both X and Y registers for each sensor
- Handle connection errors gracefully (continue with last known values)
- **Test**: With sensors connected, values update in state
- **Demo**: UI shows live angle readings from all 5 sensors

### Task 5: Add configuration persistence ✓ DONE
- **Objective**: Save/load machine dimensions and bucket profiles
- Implemented `config_file.h/cpp` with `loadSensorConfig()` / `saveSensorConfig()`
- Stores sensor config: id, axis, inverted, points_down, offset, length_mm
- Config saved to `/home/rpi/sensor_config.txt`
- Load on startup, save via UI buttons
- **Status**: Complete. Bucket profiles not yet persisted (only sensor config).

### Task 6: Update UI for configuration (partial)
- **Objective**: Config screen to edit all segment dimensions and buckets
- **Done**: Sensor config screen lists all sensors with live values, tap to edit
- **Done**: Sensor edit screen with +/- buttons for ID, axis toggle, inverted toggle, points_down toggle, offset, length
- **TODO**: Bucket management UI (list buckets, add/edit/delete, select active)
- **Test**: Can modify and save sensor parameters ✓
- **Demo**: Edit sensor config via touch, save persists

### Task 7: Implement zero reference point, slope modes, and sensor calibration
- **Objective**: User sets reference point/slope, calibrate sensor mounting offsets

**Zero/slope reference modes:**

- **Mode 1: Single point (level reference)**
  - Add "Set Point A" button on main screen
  - Store current world XYZ as reference
  - Also store tilt vector and yaw as slew = 0° reference
  - Display shows deviation from horizontal plane at A

- **Mode 2: Two-point (auto grade)**
  - "Set Point A" then "Set Point B"
  - Both points captured in world coordinates (tilt-compensated)
  - System calculates grade automatically from A to B
  - Slope extends as a **plane** (not just a line) - same grade sideways
  - Display shows deviation from slope plane

- **Mode 3: Point + manual grade**
  - Set point A, enter grade manually (% or °)
  - Optionally set point B for direction only
  - If no B: slope direction is away from machine (at time of A)
  - If B set: slope goes from A toward B at manual grade
  - Slope extends as a plane perpendicular to grade direction

**Display math:**
```
slope_plane defined by:
- Point A (origin)
- Grade direction (toward B or away from machine)
- Grade angle (auto-calculated or manual)
- Plane extends sideways at same grade

For current bucket position P (full XYZ from kinematics + slew):
- Calculate plane Z at bucket's XY position
- deviation = P.z - plane_z_at(P.x, P.y)
```

**Sensor calibration:**
  - Calibrate screen shows each sensor's raw and corrected values
  - "Set Level" button per sensor: stores current raw reading as mounting offset
  - User positions each arm segment level/horizontal, presses button

- **Test**: Set zero, move boom, position shows deviation; set two-point slope, verify grade; slew and verify still on plane; calibrate sensor, offset applied
- **Demo**: Set A and B points, see calculated grade, bucket follows slope line

### Task 8: Update main screen display
- **Objective**: Show XYZ position and key angles
- Display: X (forward/back), Y (left/right), Z (up/down) relative to zero/slope
- Show superstructure slew angle (calculated from tilt)
- Show current grade (if two-point or manual set)
- Show color indication when near grade (green when within tolerance)
- **Test**: Values update smoothly as boom moves
- **Demo**: Full working display with live sensor data
