#ifndef MAIN_CPP
#define MAIN_CPP
#include "csim.h"
#include "csim.cpp"
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>

int main(int argc, char *argv[])
{
    Cache cache;
    
    cache.sets.resize(int(argv[0]));
    int blocks_per_set = int(argv[1]);
    int block_size = int(argv[2]);
    bool write_allocate = argv[3]== "write-allocate" ? true : false;  // Okay?
    bool write_back = argv[4]== "write-back" ? true : false;
    std::string eviction_policy = argv[5];

    if(block_size < 4 || ceil(block_size) != floor(block_size) || ceil(blocks_per_set) != floor(blocks_per_set) ||
      (write_back && !write_allocate)) {

        std::cerr << "Invalid configuration\n";
        return 1;
    }


}

#endif