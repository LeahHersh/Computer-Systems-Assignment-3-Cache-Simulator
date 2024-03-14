#ifndef CSIM_CPP
#define CSIM_CPP
#include "csim.h"
#endif

Slot find_curr_slot(Cache cache, uint32_t index, int32_t tag) {
  Set set = cache.sets[index];

  int oldest_access = set.slots[0].access_ts;
  Slot oldest_use = set.slots[0];

  // Find slot with a matching tag or oldest access date
  for (int i = 0; i < set.slots.size(); i++) {
    Slot curr = set.slots[i];

    if (curr.tag == tag) {
      return curr;
    }

    // Keep track of which slot has the oldest access date
    if (curr.access_ts < oldest_access) { 
      oldest_use = curr;
      oldest_access = curr.access_ts;
    }
  }

  // Evict block used least recently
  return oldest_use;
}