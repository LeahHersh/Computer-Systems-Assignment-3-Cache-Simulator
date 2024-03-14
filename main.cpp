#ifndef MAIN_CPP
#define MAIN_CPP
#include "csim.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <vector>


int find_curr_slot(Cache* cache, uint32_t index, int32_t tag, int* LRU_slot_index) {
  Set set = (*cache).sets[index];

  int oldest_access = set.slots[0].access_ts;
  int oldest_use = 0; 

  // Find slot with a matching tag or the oldest access date
  for (int i = 0; i < set.slots.size(); i++) {
    Slot* curr = &(set.slots[i]);

    if ((*curr).tag == tag) {
      return i;
    }

    // Keep track of which slot has the oldest access date
    if ((*curr).access_ts < oldest_access) { 
      oldest_use = i;
      oldest_access = (*curr).access_ts;
    }
  }

  *LRU_slot_index = oldest_use;
  return -1;
}


int main(int argc, char *argv[]) {
    int sim_time = 0;

    // Assign command-line arguments to variables
    int num_sets = atoi(argv[1]);
    int blocks_per_set = atoi(argv[2]);
    int block_size = atoi(argv[3]);
    bool write_allocate = argv[4]== "write-allocate" ? true : false;  // Okay?
    bool write_back = argv[5]== "write-back" ? true : false;
    std::string eviction_policy = argv[6];

    // Check if parameters are invalid
    if(block_size < 4 || ceil(log2(block_size)) != floor(log2(block_size)) || ceil(log2(blocks_per_set)) != floor(log2(blocks_per_set)) ||
      ceil(log2(num_sets)) != floor(log2(num_sets)) || (write_back && !write_allocate)) {

        std::cerr << "Invalid configuration\n";
        return 1;
    }

    // Initialize cache
    Cache* cache = new Cache();
    cache->sets.resize(num_sets);
    for (int j = 0; j < num_sets; j++) {
      cache->sets[j].slots.resize(blocks_per_set);

      for(int i = 0; i < blocks_per_set; i++) {
        (*cache).sets[j].slots[i] = *(new Slot {-1, false, 0, 0});
      }
    }

    // Setting up results variables:
    int total_loads = 0;
    int total_stores = 0;
    int load_hits = 0;
    int load_misses = 0;
    int store_hits = 0;
    int store_misses = 0;
    int total_cycles = 0;

    // Reading each line and adjusting results variables:
    std::string curr_trace_line;
    while (std::getline(std::cin, curr_trace_line)) {

        // Parse the current input line
        std::istringstream ss(curr_trace_line);
        std::string load_or_store;
        std::string memory_address;
        std::string ignored;
        ss >> load_or_store >> memory_address >> ignored;

        // Calculate memory address's number of offset, index, and tag bits 
        int num_offset_bits = log2(block_size);
        int num_index_bits = log2(num_sets);
        int num_tag_bits = 32 - num_offset_bits - num_index_bits;

        // Find the memory address's index
        uint32_t address_index = (stoi(memory_address) >> num_offset_bits) & ((1 << num_index_bits) - 1);
        // Find the memory address's tag
        int32_t address_tag = (stoi(memory_address) >> (num_offset_bits + num_index_bits)) & ((1 << num_tag_bits) - 1);

        int* LRU_chosen;
        std::cerr << "current line's index: " << address_index << std::endl;
        std::cerr << "current line's tag: " << address_tag << std::endl;
        std::cerr << address_tag << std::endl;
        // Find the slot being accessed
        Slot* curr_slot;
        int slot_index = find_curr_slot(cache, address_tag, address_index, LRU_chosen);
        // if the slot was in the cache
        if (slot_index != -1) {
          curr_slot = &(cache->sets[address_index].slots[slot_index]);
          (*curr_slot).tag = address_tag;
          (*curr_slot).valid = true;
        } else {
          curr_slot = &(cache->sets[address_index].slots[*LRU_chosen]);
          std::cerr << "Matching slot not found\n";
        }

        std::cerr << "Chosen slot's index: " << slot_index << std::endl;
        std::cerr << "Chosen slot's tag: " << (*curr_slot).tag << std::endl;
        std::cerr << (*curr_slot).tag << std::endl;

        // If a read is being attempted
        if (load_or_store == "l") {  
          // If the current slot is valid and has the same tag as the memory address
          if ((*curr_slot).valid && (*curr_slot).tag == address_tag) {
            // The load is successful
            load_hits++;

            // Otherwise, it's a miss
          } else {
            load_misses++;
            (*curr_slot).tag = address_tag;
            (*curr_slot).valid = true;
          }
          (*curr_slot).update_load_ts(sim_time);

        // If a store is being attempted
        } else {
            // If the current slot is valid and has the same tag as the memory address
            if ((*curr_slot).valid && ((*curr_slot).tag == address_tag)) {
              // The store is successful
              store_hits++;
              // Otherwise, it's a miss
            } else {
              store_misses++;
            }
          }

          // Update access time regardless of if a load or store happened
          (*curr_slot).update_access_ts(sim_time);

      sim_time++;
    }

    total_loads = load_hits + load_misses;
    total_stores = store_hits + store_misses;

    std::cerr<< "Total loads: " << total_loads << "\n";
    std::cerr<< "Total stores: " << total_stores << "\n";
    std::cerr<< "Load hits: " << load_hits << "\n";
    std::cerr<< "Load misses: " << load_misses << "\n";
    std::cerr<< "Store hits: " << store_hits << "\n";
    std::cerr<< "Store misses: " << store_misses << "\n";
    std::cerr<< "Total cycles: " << total_cycles << "\n";
}




#endif