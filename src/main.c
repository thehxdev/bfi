#include <stdio.h>
#include "bfi.h"


#define BF_VERSION "2.3.0"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        /* Check command-line arguments */
        fprintf(stderr, "Usage: bfi <source-file>\n");
        return 64;
    }

    /* Initialize BF_State and Deinit that at the
     * end of main function scope.
     * First command-line argument is BrainFuck
     * source code file path. */
    BF_State DEINIT_BF_END *bf = bf_init(argv[1]);

    /* BFI main loop. Execute tokens */
    bf_execute(&bf->tl, &bf->arr);
    return 0;
}
