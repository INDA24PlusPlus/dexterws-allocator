#include "pool.h"
#include <stdio.h>

int main() {
    PoolAllocator* pool = pool_create(sizeof(int), 10);
    int* a = pool_alloc(pool);
    *a = 42;
    printf("%d\n", *a);
    pool_free(pool, a);
    return 0;
}
