# ArenaAllocatorAssignment
A memory arena allocator that supports First Fit, Best Fit, Worst Fit and Next Fit algorithms.
The memory allocator implement the following functions: 
int mavalloc_init( size_t size, enum ALGORITHM algorithm ) -  This function will use malloc to allocate a pool of memory that is size bytes big.
void *mavalloc_alloc( size_t size ) - This function will allocate size bytes from your preallocated memory arena using the heap allocation algorithm that was specified during mavalloc_init. This function returns a pointer to the memory on success and NULL on failure.
void mavalloc_free( void * pointer ) - This function will free the block pointed to by the pointer back to your preallocated memory arena. This function returns no value. If there are two consecutive blocks free then combine (coalesce) them.
void mavalloc_destroy( ) - This function will free the allocated arena and empty the linked list.
int mavalloc_size( ) - This function will return the number of nodes in the memory area.
