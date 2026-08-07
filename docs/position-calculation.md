# Position Calculation

## Sensor Configuration

Each sensor has config values (in `config.cpp`):
- `axis` - which axis to use (X or Y)
- `inverted` - flip sign of angle
- `points_down` - segment points down at 0° (for Y calculation)
- `length_mm` - arm segment length
- `offset` - angle offset to apply

## Bucket Tip Position

Calculation in `excavator.cpp`:

```cpp
x += sin(angle) * length;
y += cos(angle) * length;  // or -= if points_down
```

Starting from boom pin position (rotated by superstructure tilt), each arm segment adds to x/y based on its absolute angle from vertical.

## Bucket Config

```cpp
struct BucketConfig {
    const char* name;
    int coupler_length_mm;  // stick end to tilt pin
    int bucket_length_mm;   // tilt pin to bucket edge
    int bucket_width_mm;    // for sideways tilt compensation
};
```

Curl/tilt sensor uses bucket config for length instead of sensor `length_mm`.

Tilt sensor uses:
- X axis (roll) for curl angle
- Y axis (pitch) for sideways tilt compensation

## Superstructure Rotation

The superstructure sensor measures tilt (X=roll, Y=pitch), not rotation directly. Rotation is calculated by comparing tilt vectors between stored and current position:

```cpp
double zeroAngle = atan2(storedX, storedY);
double currentAngle = atan2(currentX, currentY);
rotation = (currentAngle - zeroAngle) * 180.0 / M_PI;
```

This works because on uneven ground, the tilt vector points "downhill". When the superstructure rotates, the tilt vector rotates with it. The angle between stored and current tilt vectors gives rotation.

**Limitation:** On perfectly flat ground (X≈0, Y≈0), rotation cannot be detected.

## Known Issue: Boom B Angle

Height error grows when boom A and B pivot relative to each other.

Boom B visualization flips direction when passing 90° horizontal:
- Below 90°: cos() positive, Y goes up
- Above 90°: cos() negative, Y goes down with `y +=`, but boom B continues downward physically

The `points_down` flag doesn't fully solve this because boom B's world direction depends on boom A's angle.

Known positions:
- Both A and B at 90°: both horizontal, B continues forward from A (straight line)
- A at 90°, B at 30°: B points upward/backward from A's end (folded in)
