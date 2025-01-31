#include <sys/mman.h>
#include <stdio.h>
#include "goodtypes.h"
#include "freelist.h"
#include "debug.h"

typedef struct {
    u8 a;
    f64 b;
} s;

void reuse_space() {
    printf("Reuse space example\n");
    s *a = malloc(sizeof(s));
    a->a = 1;
    a->b = 2.0;
    printf("(a) int: %d float: %f\n", a->a, a->b);
    free(a);
    s *b = malloc(sizeof(s));
    printf("(b) int: %d float: %f\n", b->a, b->b);
    printf("\n");
}

void dont_reuse_space() {
    printf("Don't reuse space example\n");
    s *a = malloc(sizeof(s));
    a->a = 1;
    a->b = 2.0;
    printf("(a) int: %d float: %f\n", a->a, a->b);
    s *b = malloc(sizeof(s));
    free(a);
    printf("(b) int: %d float: %f\n", b->a, b->b);
    printf("\n");
}

// Super simple fragmentation example
// Only works with Opt Level 0, compiler is smart
// enough to optimize this specific case away
void fragmenting() {
    printf("Fragmenting example\n");
    u64 start_memory = count_free_memory();
    for (u64 i = 0; i < 100000; i++) {
        if (i % 2 == 0) {
            s *a = malloc(8 * 128);
            free(a);
        }
        else {
            s *a = malloc(16 * 128);
            free(a);
        }
    }
    u64 end_memory = count_free_memory();
    u64 extra_mib = (end_memory - start_memory) / 1024 / 1024;
    printf("Extra memory used: %lu MiB\n", extra_mib);
    printf("Extra memory used bytes: %lu\n", end_memory - start_memory);
    printf("\n");
}

i32 main() {
    reuse_space();
    dont_reuse_space();
    fragmenting();
    return 0;
}
