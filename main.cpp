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

    // Determine cache's mapping
    std::string mapping;
    if (blocks_per_set == 1) {
      mapping = "direct_mapped";
    } else if (num_sets == 1) {
      mapping = "fully_associative";
    } else {
      mapping = "set_associative";
    }

    // Setting up results variables:
    int total_loads = 0;
    int total_stores = 0;
    int load_hits = 0;
    int load_misses = 0;
    int store_hits = 0;
    int store_misses = 0;
    int total_cycles = 0;

    std::string curr_trace_line;

    if (std::cin.fail()) {
        std::cerr << "Failed to read from input stream." << std::endl;
    }

    // For each line in the input trace file
    while (std::getline(std::cin, curr_trace_line)) {
        std::cerr << "line: " << curr_trace_line << "\n";

        // Stringstream for current part of line
        std::istringstream ss(curr_trace_line);

        std::string load_or_store;
        std::string memory_address;
        std::string ignored;
        ss >> load_or_store >> memory_address >> ignored;

        if (mapping == "direct_mapped") {
          int num_offset_bits = log2(block_size);
          int num_index_bits = log2(num_sets);
          int num_tag_bits = 32 - num_offset_bits - num_index_bits;

          // Find the memory address's index
          uint32_t address_index = (stoi(memory_address) >> num_offset_bits) & (1 << num_index_bits - 1);
          // Find the memory address's tag
          uint32_t address_tag = (stoi(memory_address) >> num_offset_bits >> num_index_bits) & (1 << num_tag_bits - 1);

          Slot curr_slot = cache.sets[address_index].slots[0];
          if (load_or_store == "l") {  
            // If the current slot is valid and has the same tag as the memory address
            if (curr_slot.valid && (curr_slot.tag == address_tag)) {
              // The load is successful
              load_hits++;
              // Otherwise, it's a miss
            } else {
              load_misses++;
              curr_slot.valid = true;
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

        else if (mapping == "set_associative") {

        }


      sim_time++;
    }

    total_loads = load_hits + load_misses;
    total_stores = store_hits + store_misses;

    std::cerr<< "Total loads: " << total_loads << "/n";
    std::cerr<< "Total stores: " << total_stores << "/n";
    std::cerr<< "Load hits: " << total_loads << "/n";
    std::cerr<< "Load misses: " << total_stores << "/n";
    std::cerr<< "Store hits: " << total_stores << "/n";
    std::cerr<< "Store misses: " << total_loads << "/n";
    std::cerr<< "Total cycles: " << total_stores << "/n";
}

#endif