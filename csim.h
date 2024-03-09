#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <vector>


struct Slot {
  uint32_t tag;
  bool valid = false; 
  uint32_t load_ts;  // Timestamp in simulation time/count of loads and stores
  uint32_t access_ts;
};

struct Set {
  std::vector<Slot> slots;
  // might need to add a map later, std: :map<uint32_t, Slot *> index;
};

struct Cache {
  std::vector<Set> sets;
};