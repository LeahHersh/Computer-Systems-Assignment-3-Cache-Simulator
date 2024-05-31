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

void initialize_cache(Cache* cache, int num_sets, int blocks_per_set);

int choose_slot_LRU(Cache* cache, uint32_t index, int32_t tag, int* LRU_slot_index);

int choose_slot_FIFO(Cache* cache, uint32_t index, int32_t tag, int* FIFO_slot_index);

void fetch_block_to_cache(Slot* destination, int new_tag, int block_size, int* CPU_cycles, int curr_time);

void calculate_address_index_and_tag(std::string memory_address, uint32_t* address_index, uint32_t* address_tag, int block_size, 
int num_sets);

void print_results(int total_loads, int total_stores, int load_hits, int load_misses, int store_hits, int store_misses, int total_cycles);

#endif