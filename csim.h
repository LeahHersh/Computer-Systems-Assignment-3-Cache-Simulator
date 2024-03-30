#ifndef CSIM_H
#define CSIM_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

struct Slot {
  int32_t tag;
  bool dirty;
  int load_ts;  // timestamp of last load
  int access_ts;  // timestamp of last store or load

  void update_load_ts(int new_load_ts) { load_ts = new_load_ts; }

  void update_access_ts(int new_access_ts) { access_ts = new_access_ts; }
};

struct Set {
  std::vector<Slot> slots;
};

struct Cache {
  std::vector<Set> sets;
};


#endif