#  Contiguous Allocator
Designed a contiguous memory allocator library that supports different allocation techniques. The allocator will be used as a library that exposes interface to allocate, deallocate and manage memory. The allocator needs to know the total size of memory assumed in this project and the memory allocation algorithm to be used. 

THe _size variable indicates the contiguous memory chunk size that is assumed for the rest of the program. Any requests for allocation and deallocation requests will be served from this contiguous chunk. This memory chunk is allocated using malloc() function. The allocation_algorithm is an enum variable which will determine the algorithm used for allocation in the rest of the program:

- FIRST_FIT - satisfies the allocation request from the first available memory block (from left) that is at least as large as the requested size. 
- BEST_FIT - satisfies the allocation request from the available memory block that at least as large as the requested size and that results in the smallest remainder fragment.
- WORST_FIT - satisfies the allocation request from the available memory block that at least as large as the requested size and that results in the largest remainder fragment.

This allocator maintains meta-data (pointer to the block and size of block) about the allocated and free blocks so that they can be used for faster allocation and compaction. This meta-data is implemented in the form of two linked lists (allocated blocks and free blocks). When a block gets allocated (using kalloc), its meta-data is inserted to the list of allocated blocks. Similarly when a block gets freed (using kfree), its meta-data is inserted to the list of free blocks. The free list never maintains contiguous free blocks, i.e., if two blocks, one of size m and other of size n, are consecutive in the memory chunk, they must become a combined block of size m + n. This combination must happen when kfree is called.

Since contiguous allocation results in fragmentation, the allocator must support a compaction API as shown below
Compaction will be performed by grouping the allocated memory blocks in the beginning of the memory chunk and combining the free memory at the end of the memory chunk (as shown in Figure 3). This process will require you to manipulate the allocated and free meta-data lists. The process of compaction must be in-place, which means that you must not declare extra memory chunk to perform compaction. This can be done by going through the allocated list in a sorted manner and copying the contents of allocated blocks on free blocks from left to right.

As compaction relocates data, all the pointer addresses in the driver program must also be updated. Hence, the API accepts _before and _after arrays of void* pointers (hence they are void**). The relocation logic will insert the previous address and new address of each relocated block in _before and _after. You can assume that _before and _after arrays supplied by the driver program are large enough (i.e., you don't need to worry about allocation of these two arrays). The return value is an integer which is the total number of pointers inserted in the _before/_after array. This way, the calling program can perform pointer adjustment like this
Allocated size = <X>
Allocated chunks = <X>
Free size = <X>
Free chunks = <X>
Largest free chunk size = <X>
Smallest free chunk size = <X>

In order to avoid memory leaks after using your contiguous allocator, you need to implement a function that will release any dynamically allocated memory in your contiguous allocator
