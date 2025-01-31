#include <sys/mman.h>
#include <stdio.h>
#include "goodtypes.h"
#include "freelist.h"

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

i32 main() {
    reuse_space();
    dont_reuse_space();
    return 0;
}
