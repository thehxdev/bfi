#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bfi.h"


#define BF_VERSION "2.0.0"


int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc != 2) {
        fprintf(stderr, "Usage: bfi <source-file>\n");
        return 64;
    }

    BF_State DEINIT_BF_END *bf = bf_init(argv[1]);
    bf_execute(bf);
    return 0;
}
