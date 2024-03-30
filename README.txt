
### Milestone 1 Implementation Details:

Only csim and a main file is part of the executable.




### Milestone 2 Implementation Details:

The simulator's cycle count isn't always accurate, and I think write-throughs might not be handled correctly.




### Milestone 3 Implementation Details:

1. This submission used late time going into March 30th, when ugrad machines were down, so the experiment data mentioned in the cache report come 
from results posted on Courselore that were not produced by this simulator. While the results I'm using in the report clash slightly with the data 
posted by Daniel Jin on the csim results thread, they are all that I have access to which allow me to compare cache configurations while controlling 
for all variables. I would be happy to rewrite the report when the ugrad machines come back up if that would be possible.

2. When there is a cache miss, invalid/empty slots are identified by their load and access times being the same as their timestamps at initialization, 
-1, in this simulator, which is why its slots do not have a valid field. 



### Report on Best Cache Configuration:

Based on simulation results, fully associative caches and no-write-allocate write-through caches had the best performance. Whether the cache is more 
performative with a FIFO or LRU eviction policy was dependent on the dataset used, varying between the gcc and swim memory trace files. The total 
processor cycle count is the metric used to measure performance in this report, because the speed/efficiency of the cache is a direct result of the 
number of processor cycles used in the cache. However, it's important to mention that the number of processor cycles used is driven much more directly 
by cache misses than cache hits, since writing to and fetching from memory is significantly more expensive than writing to and loading from a cache 
(though the relationship between cache hits and misses is zero-sum).

While allocating memory in the cache on write-misses decreases the likelihood of future cache misses, the simulator's results suggest that the cost 
of allocating/fetching blocks from main memory on write misses outweighs the benefit of preventing the more common writes to memory necessary in 
no-write-allocate write-through caches. Allocating memory is expensive, and more expensive than singular writes to memory. However, if a data set 
had a higher proportion of loads, it is possible that the benefit of allocating memory that may be loaded later would lead to write-allocate caches
being more performative. Also, while fully associative caches are most performative as far as efficiency goes, the hardware complexity involved in 
making fully associative caches make them unrealistic options. The performance of 8-way and even 4-way set-associative caches approaches the efficiency
of fully-associative caches, and are more realistic in terms of hardware (4-way set associative caches more than 8-way). 

Simulation Experiment Results Used for the Report and What Each Shows:

