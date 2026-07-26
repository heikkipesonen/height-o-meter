#pragma once

struct Section {
  int sensor_id;
  int dist = 0;
  int base_offset_x = 0;
  int base_offset_y = 0;
  double value_x = 0;
  double value_y = 0;
  bool inv_x = false;
  bool inv_y = false;
};

struct Bucket {
  int id;
  int width;
  int height;
};

struct Excavator {
  Section superstructure;
  Section boom_a;
  Section boom_b;
  Section boom_c;
  Section coupler;
  Bucket bucket;
};
