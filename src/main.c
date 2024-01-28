#include <stdio.h>
#include "bfi.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: bfi <source-file>\n");
        return 64;
    }

    BF_State DEINIT_BF_END *bf = bf_init(argv[1]);
    bf_execute(bf);
    return 0;
}
