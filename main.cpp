#ifndef MAIN_CPP
#define MAIN_CPP
#include "csim.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <vector>


int main(int argc, char *argv[]) {
    int sim_time = 0;

    // Assign command-line arguments to variables
    int num_sets = atoi(argv[1]);
    std::cerr << argv[1] << "\n";
    std::cerr << num_sets << "\n";
    std::cerr << "!\n";
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
    Cache cache;
    cache.sets.resize(num_sets);
    for (int j = 0; j < num_sets; j++) {
      cache.sets[j].slots.resize(blocks_per_set);

      for(int i = 0; i < blocks_per_set; i++) {
        cache.sets[j].slots[i] = {0, false, 0, 0};
      }
    }
    std::cerr << num_sets << "\n";
    std::cerr << cache.sets.size() << "\n";
    Slot curr_slot = cache.sets[0].slots[0];
    std::cerr << "Seg fault on line above- never reached\n";

    // Determine if cache is direct-mapped
    bool direct_mapped = true;
    /*if (blocks_per_set == 1) {
      direct_mapped = true;
    } */

    // Setting up results variables:
    int total_loads = 0;
    int total_stores = 0;
    int load_hits = 0;
    int load_misses = 0;
    int store_hits = 0;
    int store_misses = 0;
    int total_cycles = 0;

    std::string curr_trace_line;

    // For each line in the input trace file
    while (std::getline(std::cin, curr_trace_line)) {
        // Stringstream for current part of line
        std::istringstream ss(curr_trace_line);

        std::string load_or_store;
        std::string memory_address;
        std::string ignored;
        ss >> load_or_store >> memory_address >> ignored;

        if (direct_mapped) {
          int num_offset_bits = log2(block_size);
          int num_index_bits = log2(num_sets);
          int num_tag_bits = 32 - num_offset_bits - num_index_bits;

          // Find the memory address's index
          uint32_t address_index = (stoi(memory_address) >> num_offset_bits) & (1 << num_index_bits - 1);
          // Find the memory address's tag
          uint32_t address_tag = (stoi(memory_address) >> num_offset_bits >> num_index_bits) & (1 << num_tag_bits - 1);
          std::cerr << memory_address << "\n";
          std::cerr << address_index << "\n";
          std::cerr << cache.sets.size() << "\n";
          Slot curr_slot = cache.sets[address_index].slots[0];
          std::cerr << "hit 1";
          if (load_or_store == "l") {  
            // If the current slot is valid and has the same tag as the memory address
            if (curr_slot.valid && (curr_slot.tag == address_tag)) {
              // The load is successful
              load_hits++;
              // Otherwise, it's a miss
            } else {
              load_misses++;
            }
            curr_slot.update_load_ts(sim_time);

          // If a store is being attempted
          } else {
              // If the current slot is valid and has the same tag as the memory address
            if (curr_slot.valid && (curr_slot.tag == address_tag)) {
              // The store is successful
              store_hits++;
              // Otherwise, it's a miss
            } else {
              store_misses++;
            }
          }

          // Update access time regardless of if a load or store happened
          curr_slot.update_access_ts(sim_time);
        }


      sim_time++;
    }
}

#endif