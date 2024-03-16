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

  // Set up variables for keeping track of which slot has been accessed least recently
  int oldest_access = set.slots[0].access_ts;
  int oldest_use_index = 0; 
  int num_slots = set.slots.size();

  // Find slot with a matching tag or the oldest access date
  for (int i = 0; i < num_slots; i++) {
    Slot* curr = &(set.slots[i]);

    // If a slot with a matching tag was found, return that tag
    if ((*curr).tag == tag) {
      return i;
    }

    // Update which slot has the oldest access date if curr is "older"
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


int main(int, char *argv[]) {
    int sim_time = 0;

    // Assign command-line arguments to variables
    int num_sets = atoi(argv[1]);
    int blocks_per_set = atoi(argv[2]);
    int block_size = atoi(argv[3]);
    bool write_allocate = std::string(argv[4]) == "write-allocate" ? true : false;
    bool write_back = std::string(argv[5]) == "write-back" ? true : false;
    std::string eviction_policy = std::string(argv[6]);

    // Check if the input parameters are invalid
    if(block_size < 4 || !((block_size & (block_size - 1)) == 0) || 
      !((num_sets & (num_sets - 1)) == 0) || (write_back && !write_allocate)) {

        std::cerr << "Invalid configuration" << std::endl;
        return 1;
    }

    // Initialize the cache:
    Cache* cache = new Cache();
    cache->sets.resize(num_sets);
    for (int j = 0; j < num_sets; j++) {
      // Each slot is initialized with an invalid tag, to clean/invalid, and as unaccessed/never loaded
      cache->sets[j].slots.resize(blocks_per_set, {-1, false, false, 0, 0});
    }

    // Set up results variables:
    int total_loads = 0;
    int total_stores = 0;
    int load_hits = 0;
    int load_misses = 0;
    int store_hits = 0;
    int store_misses = 0;
    int total_cycles = 0;

    // Read and act on each line of the trace file:
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
        uint32_t address_index = (stoul(memory_address, nullptr, 0) >> num_offset_bits) & ((1 << num_index_bits) - 1);
        // Find the memory address's tag
        int32_t address_tag = (stoul(memory_address, nullptr, 0) >> (num_offset_bits + num_index_bits)) & ((1 << num_tag_bits) - 1);

        // int that will be updated to the least-recently-accessed slot's index if no match is found
        int LRU_chosen_index = 0;

        // Find the slot being accessed
        Slot* curr_slot;
        int slot_index = find_curr_slot(cache, address_index, address_tag, &LRU_chosen_index);
        bool block_in_cache = false;

        // if the block was in the cache, set curr_slot to the block's spot and set block_in_cache to true
        if (slot_index != -1) {
          curr_slot = &(cache->sets[address_index].slots[slot_index]);
          block_in_cache = true;

        // Otherwise, evict the slot favored by LRU
        } else {
          curr_slot = &(cache->sets[address_index].slots[LRU_chosen_index]);
        }

        // Update the slot's tag and its validity depending on write policies
        if (load_or_store == "l" || write_allocate) {
          (*curr_slot).tag = address_tag;
          (*curr_slot).valid = true;

          // On a read or a write in write-allocate, fetch the line to the cache
          if (load_or_store == "l" || (load_or_store == "s" && write_allocate)) {
            total_cycles += (25 * blocks_per_set);
          }
        }
        
        /* Start of load or store */

        // If a read is being attempted
        if (load_or_store == "l") {  


          // If the current slot is valid and had the same tag as the memory address's tag
          if (block_in_cache) {
            // The load is successful
            load_hits++;

            // Otherwise, it's a miss
          } else {
            load_misses++;
            total_cycles += (25 * blocks_per_set);
          }
          
          // Regardless of if there was a hit or miss, update the slot's access time and "update the block"
          (*curr_slot).update_load_ts(sim_time);
          total_cycles += 1;

        // If a write is being attempted
        } else {
            // If the current slot is valid and has the same tag as the memory address
            if (block_in_cache) {
              // The store is successful, and the bit becomes dirty if it wasn't already (in write-backs)
              store_hits++;
              if (write_back) { (*curr_slot).dirty = true; }

              // Otherwise, it's a miss 
            } else {
              store_misses++;

              // Because an eviction may have taken place, a write-back to memory might be necessary
              if (write_back && (*curr_slot).dirty) {
                (*curr_slot).dirty = false;
                total_cycles += (25 * blocks_per_set);
              }

              // If the cache is write-allocate, it retrieves the new block from main memory before the store
              if (write_allocate) {
                total_cycles += (25 * blocks_per_set);
              } 
              
              // If the cache is write-through, it writes to main memory as well as the cache
              if (!write_back) {
                total_cycles += (25 * blocks_per_set);
              }

            }
            // Add cycle for a write to the cache regardless of if a hit or miss happened
            total_cycles++;
          }

      // Update slot access time and simulation time regardless of if a load or store happened
      (*curr_slot).update_access_ts(sim_time);
      sim_time++;
    }

    // Calculate total loads and stores
    total_loads = load_hits + load_misses;
    total_stores = store_hits + store_misses;

    // Print all results values
    std::cout << "Total loads: " << total_loads << "\n";
    std::cout << "Total stores: " << total_stores << "\n";
    std::cout << "Load hits: " << load_hits << "\n";
    std::cout << "Load misses: " << load_misses << "\n";
    std::cout << "Store hits: " << store_hits << "\n";
    std::cout << "Store misses: " << store_misses << "\n";
    std::cout << "Total cycles: " << total_cycles << "\n";
}


#endif
