#include <stdio.h>
#include "cap.h"
#include "bfi.h"


#define BFI_VERSION "2.6.0"


int main(int argc, char *argv[]) {
    int err = 0, run = 0, compile = 0;
    char *src, *out;
    BF_State *bf = NULL;
    Cap_t *cap = cap_init(argc, argv);

    (void)cap_register_flag(cap, NULL, "r", "Run a source file directly.");
    (void)cap_register_flag(cap, NULL, "c", "Compile a source file.");
    (void)cap_register_flag(cap, NULL, "o", "Output file path to write compiler generated code.");

    err = cap_parse_args(cap);
    if (err) {
        fprintf(stderr,
                "BFI v" BFI_VERSION " - https://github.com/thehxdev/bfi\n"
                "Usage:\n"
                "\tInterpreter: bfi -r <source-code-path>\n"
                "\tCompiler: bfi -c <source-code-path> -o <output-file-path>\n");
        goto exit;
    }


    if (cap_flag_provided(cap, NULL, "r")) {
        src  = cap_flag_getval(cap, NULL, "r");
        run  = 1;
    } else if (cap_flag_provided(cap, NULL, "c")) {
        src = cap_flag_getval(cap, NULL, "c");
        out = cap_flag_getval(cap, NULL, "o");
        compile = 1;
    } else {
        BF_LOG_ERR("Invalid flags\n", NULL);
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
    else if (compile)
        err = bf_compiler_x64nasm(&bf->tl, out);

exit:
    bf_deinit(&bf);
    cap_deinit(&cap);
    return err;
}
