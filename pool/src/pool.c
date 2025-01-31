#include "alloca.h"
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct Chunk {
    struct Chunk* next;
} Chunk;

typedef struct PoolAllocator {
    size_t element_size;
    Chunk* free_list;
    void* data;
} PoolAllocator;

size_t align_chunk(size_t size) {
    return (size + 4095) & ~4095;
}

PoolAllocator* pool_create(size_t element_size, size_t num_elements) {
    size_t alloc_size = align_chunk(element_size * num_elements);
    void* data = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // We allocate the pool allocator on the stack
    PoolAllocator* pool = alloca(sizeof(PoolAllocator));
    pool->element_size = element_size;
    pool->free_list = NULL;
    pool->data = data;
    // Initialize the free list
    for (size_t i = 0; i < num_elements; i++) {
        Chunk* chunk = (Chunk*)((char*)data + element_size * i);
        chunk->next = pool->free_list;
        pool->free_list = chunk;
    }
    return pool;
}

void* pool_alloc(PoolAllocator* pool) {
    // Return NULL if no memory left
    if (pool->free_list == NULL) {
        return NULL;
    }
    Chunk* chunk = pool->free_list;
    pool->free_list = chunk->next;
    return chunk;
}

void pool_free(PoolAllocator* pool, void* ptr) {
    Chunk* chunk = (Chunk*)ptr;
    chunk->next = pool->free_list;
    pool->free_list = chunk;
}
