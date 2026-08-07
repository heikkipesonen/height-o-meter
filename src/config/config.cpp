#include "config.h"

ExcavatorConfig getConfig() {
  ExcavatorConfig config;

  config.serial_port = "/dev/ttyUSB0";
  config.baud_rate = 9600;

  // Base height (ground to center pivot)
  config.base_height_mm = 30;

  // Center pivot to main boom pin
  config.pivot_offset_x_mm = 90; // Horizontal distance
  config.pivot_offset_y_mm = 41; // Vertical distance (boom pin is higher)

  // Sensor 0: Superstructure (cab rotation)
  config.sensors[0].id = 1;
  config.sensors[0].name = "Superstructure";
  config.sensors[0].axis = MountAxis::X;
  config.sensors[0].inverted = true;
  config.sensors[0].length_mm = 0;

  // Sensor 1: Boom A (main boom)
  config.sensors[1].id = 2;
  config.sensors[1].name = "Boom A";
  config.sensors[1].axis = MountAxis::X;
  config.sensors[1].inverted = false;
  config.sensors[1].offset = 90.0;
  config.sensors[1].length_mm = 180;

  // Sensor 2: Boom B (secondary boom)
  config.sensors[2].id = 3;
  config.sensors[2].name = "Boom B";
  config.sensors[2].axis = MountAxis::X;
  config.sensors[2].inverted = false;
  config.sensors[2].offset = 90.0;
  config.sensors[2].length_mm = 180;

  // Sensor 3: Stick
  config.sensors[3].id = 4;
  config.sensors[3].name = "Stick";
  config.sensors[3].axis = MountAxis::X;
  config.sensors[3].inverted = false;
  config.sensors[3].offset = 90.0 - 10.3;
  config.sensors[3].points_down = false;
  config.sensors[3].length_mm = 250;

  // Sensor 4: Coupler/bucket (curl angle on X, sideways tilt on Y)
  // length_mm = 0 here, effective length comes from active bucket
  config.sensors[4].id = 5;
  config.sensors[4].name = "Curl";
  config.sensors[4].axis = MountAxis::X;
  config.sensors[4].inverted = false;
  config.sensors[4].points_down = true;
  config.sensors[4].offset = 0.0;
  config.sensors[4].length_mm = 0;

  // Sensor 5: Test sensor (for setup)
  config.sensors[5].id = 80;
  config.sensors[5].name = "Test";
  config.sensors[5].axis = MountAxis::Y;
  config.sensors[5].inverted = false;
  config.sensors[5].length_mm = 0;

  // Buckets
  config.buckets.push_back({"Grading 1200mm", 27, 70, 70});
  config.active_bucket = 0;

  config.lean_x_max = 45;
  config.lean_y_max = 45;

  return config;
}
