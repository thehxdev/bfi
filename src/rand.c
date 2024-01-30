#include <stdlib.h>
#include "rand.h"


/* generate a random number in range of `low` and `high`.
 * I know this is wierd but works well :) */
long __bf_gen_rand_long(const long low, const long high) {
    long num;
    void *ptr = malloc(1);
    num = ((long)ptr + (((long)rand() % high) + low)) % high;
    free(ptr);
    return num;
}
