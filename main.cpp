#ifndef MAIN_CPP
#define MAIN_CPP
#include "csim.h"
#include "csim.cpp"
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>

int main(int argc, char *argv[]) {
    Cache cache;

    // Assign command-line arguments to variables
    int num_sets = int(argv[0]);
    int blocks_per_set = int(argv[1]);
    int block_size = int(argv[2]);
    bool write_allocate = argv[3]== "write-allocate" ? true : false;  // Okay?
    bool write_back = argv[4]== "write-back" ? true : false;
    std::string eviction_policy = argv[5];

    // Check if parameters are invalid
    if(block_size < 4 || ceil(log2(block_size)) != floor(log2(block_size)) || ceil(log2(blocks_per_set)) != floor(log2(blocks_per_set)) ||
      ceil(log2(num_sets)) != floor(log2(num_sets)) || (write_back && !write_allocate)) {

        std::cerr << "Invalid configuration\n";
        return 1;
    }

    cache.sets.resize(num_sets);

    // Determine if cache is direct-mapped
    bool direct_mapped = false;
    if (num_sets == 1) {
      direct_mapped = true;
    }

    // Setting up variables for results:
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

        
    }
}

#endif