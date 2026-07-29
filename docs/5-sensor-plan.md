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

## Slew Detection (from tilt change)
Slew angle is calculated from superstructure tilt sensor, not measured directly.

**How it works:**
- On a slope, the ground tilt appears as roll/pitch on the superstructure sensor
- When the cab rotates (slews), the tilt "rotates" through roll and pitch axes
- Slew angle = `atan2(roll, pitch)` relative to zero position
- Ground slope magnitude = `sqrt(roll² + pitch²)` (constant while machine stationary)

**Reference:**
- When user sets zero point, system also stores the tilt vector (roll/pitch) as slew = 0° reference
- Subsequent slew calculated as rotation from that reference tilt vector

**Limitation:** Only works on slopes. On flat ground, slew doesn't matter anyway - bucket height is unaffected by cab rotation when machine is level.

## Tilt Hitch Configuration
The tilt hitch connects stick to bucket with two degrees of freedom:
- **Stick pin to tilt pivot X**: Forward distance from stick pin to tilt pivot (mm)
- **Stick pin to tilt pivot Z**: Vertical offset from stick pin to tilt pivot (mm) - typically below
- **Tilt pivot to bucket pin**: Distance from tilt pivot to bucket pin (mm)
- Curl (X axis): bucket tips forward/back
- Side tilt (Y axis): bucket tips left/right

## Bucket Configuration
Different buckets attach to the same coupler but have different dimensions:
- **Pin to edge distance**: Distance from bucket pin to bucket cutting edge (mm)
- **Bucket name**: User-friendly label (e.g., "600mm ditching", "1200mm grading")

Multiple bucket profiles stored, user selects active bucket from UI.

Example buckets:
| Name | Pin to Edge |
|------|-------------|
| 600mm ditching | 450 |
| 1200mm grading | 650 |
| 300mm trenching | 380 |

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
├── excavator/
│   ├── excavator.h       # Section, ExcavatorState (currently 2 sensors)
│   └── excavator.cpp     # Modbus, sensor reading, position math
└── ui/
    ├── ui.h              # UI class
    ├── ui.cpp            # Screens, input handling, rendering
    ├── fonts.h           # Font access, drawing helpers
    └── fonts.cpp         # Font loading, text rendering
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

### Task 5: Add configuration persistence
- **Objective**: Save/load machine dimensions and bucket profiles
- Create `config.h/cpp` in `src/` with save/load functions (simple text file)
- Store: segment lengths, offsets, sensor IDs, axis inversions
- Store bucket profiles: name, pin-to-edge distance
- Store active bucket selection
- Load on startup, save when user modifies config
- **Test**: Change config, restart, values persist
- **Demo**: Modify a length in config screen, restart app, value retained

### Task 6: Update UI for configuration
- **Objective**: Config screen to edit all segment dimensions and buckets
- Add input fields for each segment: length, X/Y/Z offset, sensor ID
- Add bucket management: list buckets, add/edit/delete, select active
- Add +/- buttons for touch-friendly input
- Display current values, highlight when modified
- **Test**: Can modify and save all parameters
- **Demo**: Add new bucket "800mm grading" with 520mm pin-to-edge

### Task 7: Implement zero reference point, slope modes, and sensor calibration
- **Objective**: User sets reference point/slope, calibrate sensor mounting offsets

**Zero/slope reference modes:**

- **Mode 1: Single point (level reference)**
  - Add "Set Point A" button on main screen
  - Store current world XYZ as reference
  - Also store tilt vector (roll/pitch) as slew = 0° reference
  - Display shows deviation from horizontal plane at A

- **Mode 2: Two-point (auto grade)**
  - "Set Point A" then "Set Point B"
  - Both points captured in world coordinates (tilt-compensated)
  - System calculates grade automatically from A to B
  - Display shows deviation from slope line

- **Mode 3: Point + manual grade**
  - Set point A, enter grade manually (% or °)
  - Optionally set point B for direction only
  - If no B: slope direction is away from machine (at time of A)
  - If B set: slope goes from A toward B at manual grade

**Display math:**
```
slope_vector = B - A (or derived from manual grade + direction)
For current bucket position P:
- Project P onto slope line
- deviation = P.z - interpolated_z_on_slope
```

**Sensor calibration:**
  - Calibrate screen shows each sensor's raw and corrected values
  - "Set Level" button per sensor: stores current raw reading as mounting offset
  - User positions each arm segment level/horizontal, presses button

- **Test**: Set zero, move boom, position shows deviation; set two-point slope, verify grade; calibrate sensor, offset applied
- **Demo**: Set A and B points, see calculated grade, bucket follows slope line

### Task 8: Update main screen display
- **Objective**: Show XYZ position and key angles
- Display: X (forward/back), Y (left/right), Z (up/down) relative to zero/slope
- Show superstructure slew angle (calculated from tilt)
- Show current grade (if two-point or manual set)
- Show color indication when near grade (green when within tolerance)
- **Test**: Values update smoothly as boom moves
- **Demo**: Full working display with live sensor data
