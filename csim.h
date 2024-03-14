#ifndef CSIM_H
#define CSIM_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

struct Slot {
  int32_t tag;
  bool valid; 
  int load_ts;  // ts of last load
  int access_ts;  // ts of last store or load

  void update_load_ts(int new_load_ts) { load_ts = new_load_ts; }

  void update_access_ts(int new_access_ts) { access_ts = new_access_ts; }
};

struct Set {
  std::vector<Slot> slots;
  // might need to add a map later, std::map<uint32_t, Slot *> index;
};

struct Cache {
  std::vector<Set> sets;
};

// Helper function called either if there's a read-miss or write-miss in write-allocate
void write_allocate_miss();

Slot* find_curr_slot(Cache* cache, uint32_t index, int32_t tag);

#endif