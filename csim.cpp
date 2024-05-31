#include "csim.h"
#include "csim.h"
#include <stdio.h>
#include <iostream>
#include <string>



/* Initialize the cache settings. */
void initialize_cache(Cache* cache, int num_sets, int blocks_per_set) {
  for (int j = 0; j < num_sets; j++) {
      // Each slot is initialized with an invalid tag, to clean/invalid, and as unaccessed/never loaded
      cache->sets[j].slots.resize(blocks_per_set, {-1, false, -1, -1});
  }
}

/* 
  Return the index of the slot whose tag matches the current tag if it's found, 
  or assign the slot with the oldest access time to the LRU_slot_index. Note that 
  if a slot with a matching tag is not found, the first empty slot available will be 
  returned, as an empty slot's access time is always zero. 
*/
int choose_slot_LRU(Cache* cache, uint32_t index, int32_t tag, int* LRU_slot_index) {
  Set set = (*cache).sets[index];

  // Set up variables for keeping track of which slot has been accessed least recently
  int oldest_access = set.slots[0].access_ts;
  int oldest_use_index = 0; 

  // Find slot with a matching tag or the oldest access date
  int num_slots = set.slots.size();
  for (int i = 0; i < num_slots; i++) {
    Slot* curr = &(set.slots[i]);

    // If a slot with a matching tag was found, return the index of the slot with that tag
    if ((*curr).tag == tag) {
      return i;
    }

    // Update which slot has the oldest access date if curr is older
    if ((*curr).access_ts < oldest_access) { 
      oldest_use_index = i;
      oldest_access = (*curr).access_ts;
    }
  }
  // Assign the LRU-chosen slot, so that it can be written to or evicted
  *LRU_slot_index = oldest_use_index;

  // Indicate that no slots with a matching tag were found in the set
  return -1;
}

/* 
  Return the index of the slot whose tag matches the current tag if it's found, 
  or assign the slot with the oldest load time to the FIFO_slot_index. Note that 
  if a slot with a matching tag is not found, the first empty slot available will be 
  returned, as an empty slot's load time is always zero. 
*/
int choose_slot_FIFO(Cache* cache, uint32_t index, int32_t tag, int* FIFO_slot_index) {
  Set set = (*cache).sets[index];

  // Set up variables for keeping track of which slot has been loaded least recently
  int oldest_load = set.slots[0].load_ts;
  int oldest_load_index = 0; 

  // Find slot with a matching tag or the oldest load date
  int num_slots = set.slots.size();
  for (int i = 0; i < num_slots; i++) {
    Slot* curr = &(set.slots[i]);

    // If a slot with a matching tag was found, return the index of the slot with that tag
    if ((*curr).tag == tag) {
      return i;
    }

    // Update which slot has the oldest load date if curr is "older"
    if ((*curr).load_ts < oldest_load) { 
      oldest_load_index = i;
      oldest_load = (*curr).load_ts;
    }
  }
  // Assign the FIFO-chosen slot, so that it can be written to or evicted
  *FIFO_slot_index = oldest_load_index;

  // Indicate that no slots with a matching tag were found in the set
  return -1;
}

/* Simulate bringing a block from main memory into a cache slot. */
void fetch_block_to_cache(Slot* destination, int new_tag, int block_size, int* CPU_cycles, int curr_time) { 
  (*destination).tag = new_tag;
  (*destination).load_ts = curr_time;
  (*CPU_cycles) += (25 * block_size);
}

/* Find an input memory address's index and tag, and assign them to the appropriate pointers' values */
void calculate_address_index_and_tag(std::string memory_address, uint32_t* address_index, uint32_t* address_tag, int block_size, 
int num_sets) {

  // Calculate memory address's number of offset, index, and tag bits 
  int num_offset_bits = log2(block_size);
  int num_index_bits = log2(num_sets);
  int num_tag_bits = 32 - num_offset_bits - num_index_bits;

  // Find the memory address's index
  (*address_index) = (stoul(memory_address, nullptr, 0) >> num_offset_bits) & ((1 << num_index_bits) - 1);
  // Find the memory address's tag
  (*address_tag) = (stoul(memory_address, nullptr, 0) >> (num_offset_bits + num_index_bits)) & ((1 << num_tag_bits) - 1);
}

void print_results(int total_loads, int total_stores, int load_hits, int load_misses,
                   int store_hits, int store_misses, int total_cycles) {

    // Print all results values
    std::cout << "Total loads: " << total_loads << "\n";
    std::cout << "Total stores: " << total_stores << "\n";
    std::cout << "Load hits: " << load_hits << "\n";
    std::cout << "Load misses: " << load_misses << "\n";
    std::cout << "Store hits: " << store_hits << "\n";
    std::cout << "Store misses: " << store_misses << "\n";
    std::cout << "Total cycles: " << total_cycles << "\n";
}