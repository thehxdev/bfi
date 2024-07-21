#include <stdio.h>

/* BFI */
#include "scanner.h"
#include "compiler.h"

/* general purpose register in x64 asm */
#define BF_GEN_REG "r15"


int bf_compiler_x64gcc(BF_TokenList *tlp, const char *out_path) {
    FILE *fp;
    size_t i;
    register BF_Token *tks = tlp->tokens, t = *tks, *m_t;

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
            ".text\n"
            ".globl main\n\n"
            "main:\n\t"
            "push\t%%rbp\n\t"
            "movq\t%%rsp, %%rbp\n\t"
            "subq\t$8, %%rsp\n\n\t"
            "movq\t$BF_ARR_LEN, %%rdi\n\t"
            "movq\t$1, %%rsi\n\t"
            "callq\tcalloc\n\t"
            "movq\t%%rax, -8(%%rbp)\n\t"
            "cmpq\t$0, %%rax\n\t"
            "je\t\t_exit\n\t"
            "movq\t%%rax, %%"BF_GEN_REG"\n\n"
            );

    while (1) {
        if (t.repeat == 0)
            break;
        switch (t.op) {
            case CMD_INC_DP: {
                if (t.repeat == 1)
                    fprintf(fp, "\tincq\t%%"BF_GEN_REG"\n");
                else
                    fprintf(fp, "\taddq\t$%u, %%"BF_GEN_REG"\n", t.repeat);
            }
            break;

            case CMD_DEC_DP: {
                if (t.repeat == 1)
                    fprintf(fp, "\tdecq\t%%"BF_GEN_REG"\n");
                else
                    fprintf(fp, "\tsubq\t$%u, %%"BF_GEN_REG"\n", t.repeat);
            }
            break;

            case CMD_INC_VAL: {
                if (t.repeat == 1)
                    fprintf(fp, "\tincb\t(%%"BF_GEN_REG")\n");
                else
                    fprintf(fp, "\taddb\t$%u, (%%"BF_GEN_REG")\n", t.repeat);
            }
            break;

            case CMD_DEC_VAL: {
                if (t.repeat == 1)
                    fprintf(fp, "\tdecb\t(%%"BF_GEN_REG")\n");
                else
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
                m_t = &tlp->tokens[t.m_idx];
                fprintf(fp,
                        ".L%u:\n"
                        "\tcmpb\t$0, (%%"BF_GEN_REG")\n"
                        "\tje\t\t.L%u\n",
                        m_t->m_idx,
                        t.m_idx);
            }
            break;

            case CMD_JUMP_B: {
                m_t = &tlp->tokens[t.m_idx];
                fprintf(fp,
                        "\tcmpb\t$0, (%%"BF_GEN_REG")\n"
                        "\tjne\t\t.L%u\n"
                        ".L%u:\n",
                        t.m_idx,
                        m_t->m_idx);
            }
            break;
        } /* end switch(t->op) */
        t = *(++tks);
    } /* end while (t) */

    fprintf(fp,
            "_exit:\n\t"
            "movq\t-8(%%rbp), %%rdi\n\t"
            "callq\tfree\n\t"
            "leave\n\t"
            "movq\t$0, %%rdi\n\t"
            "retq\n\n"
            ".section .note.GNU-stack,\"\",@progbits\n"
            );

    fclose(fp);
    return 0;
}
