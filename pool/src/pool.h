#include <stddef.h>

typedef struct PoolAllocator PoolAllocator;
typedef struct Chunk Chunk;

PoolAllocator* pool_create(size_t element_size, size_t num_elements);

void* pool_alloc(PoolAllocator* pool);

void pool_free(PoolAllocator* pool, void* ptr);
