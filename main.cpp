#ifndef MAIN_CPP
#define MAIN_CPP
#include "csim.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <vector>


int main(int, char *argv[]) {
    int sim_time = 0;

    // Assign command-line arguments to variables:
    int num_sets = atoi(argv[1]);
    int blocks_per_set = atoi(argv[2]);
    int block_size = atoi(argv[3]);
    bool write_allocate = std::string(argv[4]) == "write-allocate" ? true : false;
    bool write_back = std::string(argv[5]) == "write-back" ? true : false;
    std::string eviction_policy = std::string(argv[6]);

    // Check if the input parameters are invalid:
    if(block_size < 4 || !((block_size & (block_size - 1)) == 0) || 
      !((num_sets & (num_sets - 1)) == 0) || (write_back && !write_allocate)) {

        std::cerr << "Invalid configuration" << std::endl;
        return 1;
    }

    // Create cache
    Cache* cache = new Cache();
    // Initialize the cache's set-associativity and slots
    cache->sets.resize(num_sets);
    
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

        // Parse the current input line:
        std::istringstream ss(curr_trace_line);
        std::string load_or_store;
        std::string memory_address;
        std::string ignored;
        ss >> load_or_store >> memory_address >> ignored;

        // Calculate the memory address's index and tag:
        uint32_t address_index = 0;
        uint32_t address_tag = 0;
        calculate_address_index_and_tag(memory_address, &address_index, &address_tag, block_size, num_sets);
        
        // int that will be updated to the empty/evicted slot's index if no slot with a matching tag is found:
        int LRU_chosen_index = 0;
        int FIFO_chosen_index = 0;

        // Find the slot being accessed. If a slot with a matching tag is found, this slot will always be chosen:
        Slot* curr_slot;
        int slot_index;

        if (eviction_policy == "lru") { 
          slot_index = choose_slot_LRU(cache, address_index, address_tag, &LRU_chosen_index);
        } else {
          slot_index = choose_slot_FIFO(cache, address_index, address_tag, &FIFO_chosen_index);
        }
        
        bool block_in_cache = false;

        // if the block was in the cache, set curr_slot to the block's spot and set block_in_cache to true
        if (slot_index != -1) {
          curr_slot = &(cache->sets[address_index].slots[slot_index]);
          block_in_cache = true;

        // Otherwise, access the slot favored by LRU or FIFO respectively depending on the cache
        } else {
          curr_slot = eviction_policy == "lru" ? &(cache->sets[address_index].slots[LRU_chosen_index]) : &(cache->sets[address_index].slots[FIFO_chosen_index]);
        }
        
        /* Start of load or store */

        // If a read is being attempted
        if (load_or_store == "l") {  

          // If the current slot has the same tag as the memory address requested by the CPU 
          if (block_in_cache) {
            // The load is successful
            load_hits++;
            total_cycles++;

            // Otherwise, it's a miss
          } else {
            load_misses++;
            fetch_block_to_cache(curr_slot, address_tag, block_size, &total_cycles, sim_time);

            // Write the block being evicted to main memory if the block was dirty and the cache is write-back
            if (write_back && (*curr_slot).dirty) {
              (*curr_slot).dirty = false;
              total_cycles += (25 * block_size);
            }
          }

          // Update the access time regardless of if a hit or a miss happened
          curr_slot->update_access_ts(sim_time);

        // If a write is being attempted
        } else {
            // If the current slot has the same tag as the memory address requested by the CPU 
            if (block_in_cache) {
              // The store is successful, and the bit becomes dirty if it wasn't already (in write-backs).
              store_hits++;

              if (write_back) { 
                (*curr_slot).dirty = true; 
                total_cycles++;
              }

              // Because the slot was hit/accessed, its access time needs to be updated
              curr_slot->update_access_ts(sim_time);

              // Otherwise, it's a miss 
            } else {
              store_misses++;

              if (write_allocate) {

                // Write the block being evicted to main memory if the block was dirty and the cache is write-back
                if (write_back && (*curr_slot).dirty) {
                  total_cycles += (25 * block_size);
                }

                // Fetch the requested block from main memory, then write to it, making it dirty. Update its access time.
                fetch_block_to_cache(curr_slot, address_tag, block_size, &total_cycles, sim_time);
                total_cycles++;
                (*curr_slot).dirty = true;  
                curr_slot->update_access_ts(sim_time);
              }
            }

            // If the cache is write-through, it writes to main memory regardless of if a hit or a miss happened
            if (!write_back) {
              total_cycles += 100;
            }
          }

      // Update simulation time
      sim_time++;
    }

    // Calculate total loads and stores
    total_loads = load_hits + load_misses;
    total_stores = store_hits + store_misses;

    // Print all results values
    print_results(total_loads, total_stores, load_hits, load_misses, store_hits, store_misses, total_cycles);
}


#endif