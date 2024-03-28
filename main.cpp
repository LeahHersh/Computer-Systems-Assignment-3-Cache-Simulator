#ifndef MAIN_CPP
#define MAIN_CPP
#include "csim.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <vector>


/* Helper methods */

/* Return the index of the slot/assign the slot that would be chosen in an cache that uses LRU */
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


/* Return the index of the slot/assign the slot that would be chosen in an cache that uses FIFO */
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


/* Simulate bringing a block from main memory into a cache slot */
void fetch_block_to_cache(Slot* destination, int new_tag, int block_size, int* CPU_cycles) { 
  (*destination).tag = new_tag;
  (*destination).valid = true;
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

        // Calculate the memory address's index and tag:
        uint32_t address_index = 0;
        uint32_t address_tag = 0;
        calculate_address_index_and_tag(memory_address, &address_index, &address_tag, block_size, num_sets);
        
        // int that will be updated to the least-recently-accessed slot's index if no match is found
        int LRU_chosen_index = 0;
        int FIFO_chosen_index = 0;

        // Find the slot being accessed
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

        // Otherwise, access the slot favored by LRU or FIFO respectively
        } else {
          curr_slot = eviction_policy == "lru" ? &(cache->sets[address_index].slots[LRU_chosen_index]) : &(cache->sets[address_index].slots[FIFO_chosen_index]);
        }

        // On a read miss or on a write miss in a write-allocate cache, fetch the requested block from main memory
        if (slot_index == -1 && (load_or_store == "l" || write_allocate)) {
          fetch_block_to_cache(curr_slot, address_tag, block_size, &total_cycles);

          // On a write miss, set the bit to dirty in a write-back cache 
          if (load_or_store == "w" && write_back) { curr_slot->dirty = true; }
        }
        
        /* Start of load or store */

        // If a read is being attempted
        if (load_or_store == "l") {  

          // If the current slot had the same tag as the memory address's tag
          if (block_in_cache) {
            // The load is successful
            load_hits++;

            // Otherwise, it's a miss
          } else {
            load_misses++;
          }
          
          // Regardless of if there was a hit or miss, update the slot's access time and "load the block"
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
                total_cycles += (25 * block_size);
              }
            }

            // Add cycle for a write to the cache regardless of if a hit or miss happened
            total_cycles++;

            // If the cache is write-through, it writes to main memory as well as the cache
            if (!write_back) {
              total_cycles += (25 * block_size);
            }
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