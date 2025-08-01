#include <stdio.h>

/* BFI */
#include "scanner.h"
#include "compiler.h"

/* general purpose register in x64 asm */
#define BF_GEN_REG "r15"


int bf_compiler_x64gcc(BF_Token *tlp, const char *out_path) {
    FILE *fp;
    size_t i;
    register BF_Token *tks = tlp, t = *tks, *m_t;

    if (out_path) {
        fp = fopen(out_path, "w");
        if (!fp) {
            fprintf(stderr, "[ERROR] Could not open output file: ");
            perror(NULL);
            return 1;
        }
    } else
        fp = stdout;

    fprintf(fp,
            ".equ\tBF_ARR_LEN, 0x10000\n\n"
            "\t.text\n"
            "\t.globl main\n"
            "\t.type main, @function\n"
            "main:\n\t"
            "push\t%%rbp\n\t"
            "movq\t%%rsp, %%rbp\n\t"
            "subq\t$BF_ARR_LEN, %%rsp\n\t"
            "movq\t%%rsp, %%r15\n\n\t"
            );

    while (1) {
        switch (t.op) {
            case CMD_INC_DP: {
                fprintf(fp, "\taddq\t$%u, %%"BF_GEN_REG"\n", t.repeat);
            }
            break;

            case CMD_DEC_DP: {
                fprintf(fp, "\tsubq\t$%u, %%"BF_GEN_REG"\n", t.repeat);
            }
            break;

            case CMD_INC_VAL: {
                fprintf(fp, "\taddb\t$%u, (%%"BF_GEN_REG")\n", t.repeat);
            }
            break;

            case CMD_DEC_VAL: {
               fprintf(fp, "\tsubb\t$%u, (%%"BF_GEN_REG")\n", t.repeat);
            }
            break;

            case CMD_OUTPUT: {
                for (i = 0; i < t.repeat; i++) {
                    fprintf(fp,
                            "\tmovb\t(%%"BF_GEN_REG"), %%dil\n"
                            "\tcallq\tputchar\n");
                }
            }
            break;

            case CMD_INPUT: {
                for (i = 0; i < t.repeat; i++) {
                    fprintf(fp,
                            "\tcallq\tgetchar\n"
                            "\tmovb\t(%%"BF_GEN_REG"), %%al\n");
                }
            }
            break;

            case CMD_JUMP_F: {
                m_t = &tlp[t.jmp_idx];
                fprintf(fp,
                        ".L%u:\n"
                        "\tcmpb\t$0, (%%"BF_GEN_REG")\n"
                        "\tje\t\t.L%u\n",
                        m_t->jmp_idx,
                        t.jmp_idx);
            }
            break;

            case CMD_JUMP_B: {
                m_t = &tlp[t.jmp_idx];
                fprintf(fp,
                        "\tcmpb\t$0, (%%"BF_GEN_REG")\n"
                        "\tjne\t\t.L%u\n"
                        ".L%u:\n",
                        t.jmp_idx,
                        m_t->jmp_idx);
            }
            break;

            case CMD_EXIT:
                goto exit;
        } /* end switch(t->op) */
        t = *(++tks);
    } /* end while (t) */

exit:
    fprintf(fp,
            "_exit:\n\t"
            "leave\n\t"
            "xorq\t%%rax, %%rax\n\t"
            "retq\n"
            ".section .note.GNU-stack,\"\",@progbits\n"
            );

    fclose(fp);
    return 0;
}
