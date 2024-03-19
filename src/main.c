#include <stdio.h>
#include "cap.h"
#include "bfi.h"


#define BFI_VERSION "2.7.1"


int main(int argc, char *argv[]) {
    int err = 0, run = 0, x64nasm = 0;
    char *src = NULL, *out = NULL;
    BF_State *bf = NULL;
    Cap_t *cap = cap_init(argc, argv);

    (void)cap_register_flag(cap, NULL, "r", "Run a source file directly.");
    (void)cap_register_flag(cap, NULL, "asm", "Compile a source file.");
    (void)cap_register_flag(cap, NULL, "o", "Output file path to write compiler generated code.");

    err = cap_parse_args(cap);
    if (err) {
        fprintf(stderr,
                "BFI v" BFI_VERSION " - https://github.com/thehxdev/bfi\n"
                "Cap v" CAP_VERSION " - https://github.com/thehxdev/cap\n"
                "Usage:\n"
                "\tInterpreter: bfi -r <source-code-path>\n"
                "\tx86 ASM Compiler: bfi -asm <source-code-path> -o <output-file-path>\n"
                );
        goto exit;
    }


    if (cap_flag_provided(cap, NULL, "r")) {
        src  = cap_flag_getval(cap, NULL, "r");
        run  = 1;
    } else if (cap_flag_provided(cap, NULL, "asm")) {
        src = cap_flag_getval(cap, NULL, "asm");
        out = cap_flag_getval(cap, NULL, "o");
        x64nasm = 1;
    } else {
        BF_LOG_ERR("Invalid command-line arguments\n", NULL);
        BF_LOG_ERR("Stopping bfi...\n", NULL);
        goto exit;
    }

    bf = bf_init(src);
    if (!bf) {
        BF_LOG_ERR("Failed to initialize BFI\n", NULL);
        err = 1;
        goto exit;
    }

    if (run)
        err = bf_execute(&bf->tl, &bf->arr);
    else if (x64nasm)
        err = bf_compiler_x64nasm(&bf->tl, out);

exit:
    bf_deinit(&bf);
    cap_deinit(&cap);
    return err;
}
