
### Report on Best Cache Configuration:

    Based on simulation results, fully associative caches with write-allocate write-back caches had the best performance. Whether the cache is more 
performative with a FIFO or LRU eviction policy was less obvious than the other variables, but LRU caches were more efficient more often. The total 
processor cycle count is the metric used to measure performance in this report, because the speed/efficiency of the cache is a direct result of the 
number of processor cycles used in the cache. However, it's important to mention that the number of processor cycles used is driven much more directly 
by cache misses than cache hits, since writing to and fetching from memory is significantly more expensive than writing to and loading from a cache 
(though the relationship between cache hits and misses is zero-sum). Smaller block sizes naturally also led to better cache performance, as the smaller
the size the less data needs to be fetched to/written back from the cache to main memory. The smallest block size possible is 4, so this size is most
performative.

    Allocating memory in the cache on write-misses is expensive, but decreases the likelihood of future cache misses, and the simulator's results suggest that  
the benefit of preventing misses by allocating/fetching blocks from main memory outweighs the costs of writing directly to memory on every write, as is 
necessary in no-write-allocate write-through caches. This seems to be true even though allocation is more expensive than singular writes to memory. Still, 
if the data had a higher proportion of write misses, it is hypothetically possible that the benefit of allocating memory that may be loaded later to the 
cache would be outweighted by the higher allocation costs, though if the gcc and swim trace files are representative of most processes this does not seem 
to be the case. Because write-allocate write-through caches incur the expenses of both allocating memory and writing to memory every write, they were not
as performative as the other legal configurations.

    While fully associative caches were most performative as far as efficiency goes, the hardware complexity involved in building in enough comparators to
find data that can be anywhere in a cache (as indexes are not used in fully associative caches) makes them unrealistic options. The performance of 8-way 
set-associative caches sometimes almost approached the efficiency of fully-associative caches, and are more realistic in terms of hardware. 4-way set 
associative caches were decently performative, though less than 8-way set associative caches, and would be even simpler to implement in hardware. 


### Experiment Results:

Sample results where only write policies are variable, and write-allocate write-back caches are more performative. While in some cases no-write-allocate
write-through caches outperformed write-allocate write-back caches, they very rarely did so by a large margin and were about as often less performative.
Results like these led me to consider write-allocate write-back caches to be more effective:

test case  169  ==> ./csim  256 4 16  write-allocate write-back  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 9341084

test case  170  ==> ./csim  256 4 16  write-allocate write-through  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 25126634

test case  171  ==> ./csim  256 4 16  no-write-allocate write-through  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 311613
Load misses: 6584
Store hits: 164819
Store misses: 32667
Total cycles: 22693813


The same configuration, except in an 8-way associative cache, then a fully associative cache. 
Because only the set association is variable, these results demonstrates the direct 
relationship between increasing set association and performance:

test case  217  ==> ./csim  256 8 16  write-allocate write-back  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 315261
Load misses: 2936
Store hits: 188513
Store misses: 8973
Total cycles: 8559547

test case  218  ==> ./csim  256 8 16  write-allocate write-through  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 315261
Load misses: 2936
Store hits: 188513
Store misses: 8973
Total cycles: 24836434

test case  219  ==> ./csim  256 8 16  no-write-allocate write-through  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 311937
Load misses: 6260
Store hits: 165155
Store misses: 32331
Total cycles: 22564537

test case  289  ==> ./csim  256 256 16  write-allocate write-back  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 315855
Load misses: 2342
Store hits: 188617
Store misses: 8869
Total cycles: 4997741

test case  290  ==> ./csim  256 256 16  write-allocate write-through  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 315855
Load misses: 2342
Store hits: 188617
Store misses: 8869
Total cycles: 24557724

test case  291  ==> ./csim  256 256 16  no-write-allocate write-through  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 312207
Load misses: 5990
Store hits: 165216
Store misses: 32270
Total cycles: 22456807


Results contrasting with test cases 217-219, where only the block size is variable, demonstrating
the inverse relationship between block size and performance:

test case  709  ==> ./csim  256 8 4  write-allocate write-back  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 311972
Load misses: 6225
Store hits: 169443
Store misses: 28043
Total cycles: 6755058

test case  710  ==> ./csim  256 8 4  write-allocate write-through  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 311972
Load misses: 6225
Store hits: 169443
Store misses: 28043
Total cycles: 23515415

test case  711  ==> ./csim  256 8 4  no-write-allocate write-through  lru  <  gcc.trace  <==
Total loads: 318197
Total stores: 197486
Load hits: 307972
Load misses: 10225
Store hits: 159652
Store misses: 37834
Total cycles: 21079072


Results demonstrating an example of where an LRU cache outperformed an otherwise identical FIFO cache
by a particularly high margin. Other cases usually had smaller disparities:

test case  1494  ==> ./csim  256 4 16  no-write-allocate write-through  lru  <  swim.trace  <==
Total loads: 220668
Total stores: 82525
Load hits: 218072
Load misses: 2596
Store hits: 58030
Store misses: 24495
Total cycles: 9508972

test case  1488  ==> ./csim  256 4 16  no-write-allocate write-through  fifo  <  swim.trace  <==
Total loads: 220668
Total stores: 82525
Load hits: 217968
Load misses: 2700
Store hits: 57994
Store misses: 24531
Total cycles: 9550468

