#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <vector>


struct Slot {
  uint32_t tag;
  bool valid;
  uint32_t load_ts, access_ts;
};

struct Set {
  std::vector<Slot> slots;
};

struct Cache {
  std::vector<Set> sets;
};