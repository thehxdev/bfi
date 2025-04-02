#include <stdio.h>

/* BFI */
#include "bfi.h"
#include "log.h"
#include "interpreter.h"
#include "compiler.h"


#define BFI_VERSION "3.2.0"

void usage(void);


int main(int argc, char *argv[]) {
    int err = 0, run = 0, x64asm = 0;
    char *src = NULL, *out = NULL;
    BF_State bf = {0};

    if (argc == 2) {
        run = 1;
    } else if (argc == 3) {
        x64asm = 1;
        out = argv[2];
    } else {
        usage();
        goto exit;
    }
    src = argv[1];

    bf = bf_init(src);
    if (!bf.tl.tokens) {
        BF_LOG_ERR("Failed to initialize BFI\n", NULL);
        err = 1;
        goto exit;
    }

    if (run)
        err = bf_execute(bf.tl.tokens);
    else if (x64asm)
        err = bf_compiler_x64gcc(bf.tl.tokens, out);

exit:
    bf_deinit(&bf);
    return err;
}


void usage(void) {
    fprintf(stderr,
            "BFI v" BFI_VERSION " - https://github.com/thehxdev/bfi\n"
            "Usage:\n"
            "\tInterpreter: bfi <source-code-path>\n"
            "\tx86 ASM Compiler: bfi <source-code-path> <output-file-path>\n"
            );
}
