#include <stdio.h>
#include "bfi.h"


#define BFI_VERSION "2.4.2"


int main(int argc, char *argv[]) {
    int err = 0;
    BF_State *bf = NULL;

    if (argc > 3 || argc < 2) {
        /* Check command-line arguments */
        fprintf(stderr,
                "BFI v" BFI_VERSION " - https://github.com/thehxdev/bfi\n"
                "Usage:\n"
                "\tInterpreter: bfi <source-code-path>\n"
                "\tCompiler: bfi <source-code-path> <output-asm-path>\n");
        err = 1;
        goto exit;
    }

    /* Initialize BF_State and Deinit that at the
     * end of main function scope.
     * First command-line argument is BrainFuck
     * source code file path. */
    bf = bf_init(argv[1]);
    if (!bf) {
        BF_LOG_ERR("Failed to initialize BFI");
        err = 1;
        goto exit;
    }

    if (argc == 2)
        /* BFI main loop. Execute tokens */
        err = bf_execute(&bf->tl, &bf->arr);
    else if (argc == 3)
        /* Compile BF source code to x64 assembly */
        err = bf_compiler_x64nasm(&bf->tl, argv[2]);

    if (err)
        goto exit;

exit:
    bf_deinit(&bf);
    return err;
}
