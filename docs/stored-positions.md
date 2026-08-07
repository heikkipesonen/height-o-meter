# Stored Positions Feature

## Overview
Store multiple excavator positions in memory and compare current position to any of them.

## Data Structure
```cpp
struct StoredPosition {
    Sensor sensors[NUM_SENSORS];
};

std::vector<StoredPosition> stored_positions;  // max 6 slots (A-F)
int compare_to = -1;  // -1 = none selected, 0-5 = index
```

No need to store calculated depth/reach - calculate on demand from stored sensor state.

## UI Layout
Top of main screen, horizontal buttons:
```
[A] [B] [C] [D] [E] [F]
```

- 6 buttons fit in 480px width (60px each + 10px gaps)
- Empty slots shown dimmed/outlined
- Selected slot highlighted
- Stored (but not selected) slots shown normal

## Interaction
- **Tap slot (A-F)**: Select for comparison, display shows deviation from that position
- **Tap selected slot again**: Deselect, show absolute values
- **Press ZERO button**: Store current position into selected slot
  - If nothing selected, store into first empty slot
  - Overwrites if slot already has data

## Display
- When comparing: show depth/reach as deviation from stored position
- Rotation deviation calculated same as current zero logic
- Label shows which slot comparing to (e.g. "vs A")

## Labels
Display from index: `char label = 'A' + index`
- 0 → A, 1 → B, etc.
