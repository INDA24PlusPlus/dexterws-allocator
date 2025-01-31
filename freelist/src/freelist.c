#include <string.h>
#include <sys/mman.h>
#include "goodtypes.h"

// Magic number to check integrity of the header
const u64 MAGIC = 0x1234567890ABCDEF;

// Header of a used block
typedef struct UsedHeader{
    u64 magic;
} UsedHeader;

// Header of a free block
// Double linked list structure
typedef struct FreeHeader{
    struct Header *next;
    struct Header *prev;
} FreeHeader;

// Header of a block
typedef struct Header {
    u64 size;
    union {
        UsedHeader used;
        FreeHeader free;
    };
} Header;

typedef struct Allocator {
    Header *head;
} Allocator;

Allocator allocator;

// Rounds up to multiple of alignment
static inline u64 align(u64 size, u64 alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

// Rounds up to multiple of 8
u64 align_size(u64 size) {
    return align(size, 8);
}

// Rounds up to multiple of 4096
u64 align_chunk(u64 size) {
    return align(size, 4096);
}

// Request a new chunk from the OS
void* new_chunk(u64 size) {
    size = align_chunk(size);
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == (void *)-1) {
        return NULL;
    }
    Header *header = (Header *)ptr;
    header->size = size - sizeof(Header);
    header->free.next = NULL;
    header->free.prev = NULL;
    return (void *)((u64)ptr + sizeof(Header));
}

Header *header_from_ptr(void *ptr) {
    return (Header *)((u64)ptr - sizeof(Header));
}

void *ptr_from_header(Header *header) {
    return (void *)((u64)header + sizeof(Header));
}

// Pushes a block to the free list
void push_free(void *ptr) {
    Header *header = header_from_ptr(ptr);
    header->free.next = allocator.head;
    header->free.prev = NULL;

    if (allocator.head != NULL) {
        allocator.head->free.prev = header;
    }
    allocator.head = header;
}

void *malloc(u64 size) {
    size = align_size(size + sizeof(Header));
    Header *prev = NULL;
    Header *current = allocator.head;
    while (current != NULL) {
        if (current->size >= size) {
            // If the chunk is too large, split it into two chunks
            if (current->size >= size + sizeof(Header) + 8) {
                Header *new = (Header *)((u64)current + size);
                new->size = current->size - size;
                new->free.next = current->free.next;
                new->free.prev = current->free.prev;
                if (prev == NULL) {
                    allocator.head = new;
                } else {
                    prev->free.next = new;
                }
                current->size = size;
            }
            if (prev == NULL) {
                allocator.head = current->free.next;
            } else {
                prev->free.next = current->free.next;
            }
            current->free.next = NULL;
            current->free.prev = NULL;
            current->used.magic = MAGIC;
            void *ptr = ptr_from_header(current);
            return ptr;
        }
        prev = current;
        current = current->free.next;
    }

    // If no chunk is large enough, allocate a new chunk and try again
    void *ptr = new_chunk(size);
    if (ptr == NULL) {
        return NULL;
    }
    push_free(ptr);
    return malloc(size);
}

void free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    Header *header = (Header *)((u64)ptr - sizeof(Header));
    // Check integrity of the header
    if (header->used.magic != MAGIC) {
        return;
    }
    header->used.magic = 0;
    push_free(ptr);
}

void *realloc(void *ptr, u64 size) {
    if (ptr == NULL) {
        return NULL;
    }
    Header *header = header_from_ptr(ptr);
    // Check integrity of the header
    if (header->used.magic != MAGIC) {
        return NULL;
    }
    // If the new size is smaller, just return the same block
    if (header->size >= size) {
        return ptr;
    }

    // We allocate a new block and copy the old data, then free the old block
    void *new = malloc(size);
    memcpy(new, ptr, header->size);
    free(ptr);
    return new;
}

u64 count_free_memory() {
    u64 count = 0;
    Header *current = allocator.head;
    while (current != NULL) {
        count += current->size;
        current = current->free.next;
    }
    return count;
}
