#include <stdio.h>
#include "cap.h"
#include "bfi.h"


#define BFI_VERSION "2.6.0"


int main(int argc, char *argv[]) {
    int err = 0, run = 0, compile = 0;
    char *src, *out;
    Cap_RawArgs_t args;
    BF_State *bf = NULL;
    Cap_t *cap = cap_init(argc, argv);

    (void)cap_register_subcmd(cap, "run", "Run a source file directly.");
    (void)cap_register_subcmd(cap, "compile", "Compile a source file.");
    (void)cap_register_flag(cap, "compile", "o", "Output file to store generated code.");
    (void)cap_register_flag(cap, "compile", "s", "Source file path.");

    err = cap_parse_args(cap);
    if (err) {
        fprintf(stderr,
                "BFI v" BFI_VERSION " - https://github.com/thehxdev/bfi\n"
                "Usage:\n"
                "\tInterpreter: bfi run <source-code-path>\n"
                "\tCompiler: bfi compile -s <source-code-path> -o <output-asm-path>\n");
        goto exit;
    }


    if (cap_subcmd_provided(cap, "run")) {
        args = cap_subcmd_rawargs(cap, "run");
        src  = args.argv[0];
        run  = 1;
    } else if (cap_subcmd_provided(cap, "compile")) {
        src = cap_flag_getval(cap, "compile", "s");
        out = cap_flag_getval(cap, "compile", "o");
        compile = 1;
    } else {
        BF_LOG_ERR("Invalid sub-command\n", NULL);
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
