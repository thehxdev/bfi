#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bfi.h"
#include "scanner.h"
#include "xmem.h"

/* general purpose register in x64 asm */
#define BF_GEN_ARR_REG "r14"

/* Count the commands in a source file.
 * To keep things simple, I'm not implemented a dynamic array.
 * Just count commands and allocate a static array to store them. */
static size_t __bf_source_file_cmds_count(FILE *fp) {
    char ch;
    size_t count = 0;
    if (!fp)
        return 0;

    /* Read the input file until the end character
     * by character and Ignore all the characters
     * that are not a valid BrainFuck command. */
    while ((ch = fgetc(fp)) != EOF) {
        switch (ch) {
#ifdef NON_STD_CMDS
            case '?':
#endif /* NON_STD_CMDS */
            case '>':
            case '<':
            case '+':
            case '-':
            case '.':
            case ',':
            case '[':
            case ']': count += 1; break;
        }
    }

    rewind(fp);
    return count;
}


/* read a BF source file and remove any unnecessary character.
 * Then write the commands to `**buff` and length of commands
 * to `*len` */
static int __bf_read_source_file(char **buff, size_t *len, const char *path) {
    char ch;
    size_t i = 0;
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        BF_LOG_ERR("%s: Could not open source file: ", __FUNCTION__);
        perror(NULL);
        return 1;
    }

    *len = __bf_source_file_cmds_count(fp);
    *buff = (char*) calloc((*len) + 1, 1);
    if (!(*buff)) {
        BF_LOG_ERR("%s: Allocating memory for BF commands failed\n", __FUNCTION__);
        return 1;
    }

    while ((ch = fgetc(fp)) != EOF) {
        switch (ch) {
#ifdef NON_STD_CMDS
            case '?':
#endif /* NON_STD_CMDS */
            case '>':
            case '<':
            case '+':
            case '-':
            case '.':
            case ',':
            case '[':
            case ']': (*buff)[i] = ch; i += 1; break;
        }
    }

    fclose(fp);
    return 0;
}


/* check all brackets before execution phase and if there are
 * non-matching ones, report an error and exit */
static int __bf_check_matching_brackets(char *cmds) {
    char c;
    size_t nest = 0;

    while ((c = *cmds)) {
        if (c == '[') {
            nest += 1;
        } else if (c == ']' && nest == 0) {
            BF_LOG_ERR("%s: Extra \']\' command found\n", __FUNCTION__);
            return 1;
        } else if (c == ']' && nest > 0) {
            nest -= 1;
        }
        cmds++;
    }

    /* if all commands are checked and
     * nesting is greater than 0, indicates
     * extra `[` character are used */
    if (nest > 0) {
        BF_LOG_ERR("%s: Extra \'[\' commands found\n", __FUNCTION__);
        return 1;
    }

    return 0;
}


BF_State *bf_init(const char *s_path) {
    int err;
    BF_State *bfs = (BF_State*) malloc(sizeof(BF_State));
    if (!bfs)
        return NULL;

    bfs->arr = (ubyte*) calloc(__BF_ARR_CAP, sizeof(ubyte));
    if (!bfs->arr) {
        BF_LOG_ERR("%s: Allocating memory for BF data array failed\n", __FUNCTION__);
        return NULL;
    }
    bfs->cmds_c = 0;

    /* read the source file, removed any unnecessary character
     * and store commands to bfs->cmds */
    err = __bf_read_source_file(&bfs->cmds, &bfs->cmds_c, s_path);
    if (err)
        return NULL;

    err = __bf_check_matching_brackets(bfs->cmds);
    if (err)
        return NULL;

    /* tokenize the commands */
    bfs->tl = __bf_scan_cmds(bfs->cmds, bfs->cmds_c);
    if (!bfs->tl) {
        BF_LOG_ERR("%s: Scanning BF commands failed\n", __FUNCTION__);
        return NULL;
    }

    /* after scanning commands to tokens, we don't need
     * them anymore */
    xfree(bfs->cmds);
    return bfs;
}


void bf_deinit(BF_State **bfp) {
    BF_State *tmp = *bfp;
    if (tmp) {
        xfree(tmp->cmds);
        xfree(tmp->arr);
        __bf_tokenlist_free(tmp->tl);
        xfree(tmp);
    }
}


int bf_execute(BF_TokenList **tlp, ubyte **darr) {
    long i;
    BF_TokenList *tl = *tlp;
    register ubyte *arr = *darr;
    register BF_Token **tks = tl->tokens, *t = *tks;
    tks[tl->len] = NULL;

    while (t) {
        switch (t->op) {
#ifdef NON_STD_CMDS
            case '?':
                arr = *darr;
                memset(arr, 0, __BF_ARR_CAP);
                break;
#endif /* NON_STD_CMDS */
            case '>':
                arr += t->repeat; break;

            case '<':
                arr -= t->repeat; break;

            case '+':
                *arr += t->repeat; break;

            case '-':
                *arr -= t->repeat; break;

            case '.':
                i = t->repeat;
                while (i--)
                    fputc(*arr, stdout);
                break;

            case ',':
                i = t->repeat;
                while (i--)
                    scanf("%c", arr);
                break;

            case '[':
                if (*arr == 0)
                    tks = &(tl->tokens[t->m_idx]);
                break;

            case ']':
                if (*arr != 0)
                    tks = &(tl->tokens[t->m_idx]);
                break;

#ifdef SAFE_BFI
            default:
                BF_LOG_ERR("%s: Invalid BF Token\n", __FUNCTION__);
                return 1;
#endif /* SAFE_BFI */
        } /* End switch(t->op) */

#ifdef SAFE_BFI
        if (arr < *darr || arr >= (*darr) + __BF_ARR_CAP) {
            BF_LOG_ERR("%s: Out of range access to data array\n", __FUNCTION__);
            return 1;
        }
#endif /* SAFE_BFI */

        t = *(++tks);
    } /* End while(*tsk) */

    return 0;
}


int bf_dump_tokens(BF_TokenList **tlp, const char *out_path) {
    BF_TokenList *tl = *tlp;
    register BF_Token **tks = tl->tokens, *t = *tks;

    FILE *fp = fopen(out_path, "w");
    if (!fp) {
        fprintf(stderr, "[ERROR] Could not open output file: ");
        perror(NULL);
        return 1;
    }

    while (t) {
        switch(t->op) {
#ifdef NON_STD_CMDS
            case '?':
                fprintf(fp, "RESET\n");
                break;
#endif /* NON_STD_CMDS */
            case '>':
                fprintf(fp, "INC_PTR\t\t%zu\n", t->repeat);
                break;

            case '<':
                fprintf(fp, "DEC_PTR\t\t%zu\n", t->repeat);
                break;

            case '+':
                fprintf(fp, "INC_PTR_V\t%zu\n", t->repeat);
                break;

            case '-':
                fprintf(fp, "DEC_PTR_V\t%zu\n", t->repeat);
                break;

            case '.':
                fprintf(fp, "OUT_PTR_V\t%zu\n", t->repeat);
                break;

            case ',':
                fprintf(fp, "GET_PTR_V\t%zu\n", t->repeat);
                break;

            case '[':
                fprintf(fp, "JMP_NOT_Z\t%zu\n", t->m_idx);
                break;

            case ']':
                fprintf(fp, "JMP_IF_Z\t%zu\n", t->m_idx);
                break;

#ifdef SAFE_BFI
            default:
                BF_LOG_ERR("%s: Invalid BF Token\n", __FUNCTION__);
                return 1;
#endif /* SAFE_BFI */
        } /* End switch(t->op) */

        t = *(++tks);
    } /* End while(t) */

    fclose(fp);
    return 0;
}


int bf_compiler_x64nasm(BF_TokenList **tlp, const char *out_path) {
    size_t i;
    BF_TokenList *tl = *tlp;
    register BF_Token **tks = tl->tokens, *t = *tks, *m_t;

    FILE *fp = fopen(out_path, "w");
    if (!fp) {
        fprintf(stderr, "[ERROR] Could not open output file: ");
        perror(NULL);
        return 1;
    }

    fprintf(fp,
            "%%define BF_ARR_LEN 0x10000\n\n"
            "section .text\n"
            "global _start\n\n"
            "extern putchar\n"
            "extern getchar\n"
            "extern calloc\n"
            "extern free\n\n\n"
            "_start:\n\t"
            "push\trbp\n\t"
            "mov\t\trbp, rsp\n\t"
            "sub\t\trsp, 8\n\n\t"
            "mov\t\trdi, BF_ARR_LEN\n\t"
            "mov\t\trsi, 1\n\t"
            "call\tcalloc\n\t"
            "mov\t\t[rbp-8], rax\n\t"
            "cmp\t\trax, 0\n\t"
            "je\t\t_exit\n\t"
            "mov\t\t"BF_GEN_ARR_REG", rax\n\n\t"
            );

    while (t) {
        switch (t->op) {
            case CMD_INC_DP: {
                if (t->repeat == 1)
                    fprintf(fp, "inc\t\t"BF_GEN_ARR_REG"\n\t");
                else
                    fprintf(fp, "add\t\t"BF_GEN_ARR_REG", %zu\n\t", t->repeat);
            }
            break;

            case CMD_DEC_DP: {
                if (t->repeat == 1)
                    fprintf(fp, "dec\t\t"BF_GEN_ARR_REG"\n\t");
                else
                    fprintf(fp, "sub\t\t"BF_GEN_ARR_REG", %zu\n\t", t->repeat);
            }
            break;

            case CMD_INC_VAL: {
                if (t->repeat == 1)
                    fprintf(fp, "inc\t\tbyte["BF_GEN_ARR_REG"]\n\t");
                else
                    fprintf(fp, "add\t\tbyte["BF_GEN_ARR_REG"], %zu\n\t", t->repeat);
            }
            break;

            case CMD_DEC_VAL: {
                if (t->repeat == 1)
                    fprintf(fp, "dec\t\tbyte["BF_GEN_ARR_REG"]\n\t");
                else
                    fprintf(fp, "sub\t\tbyte["BF_GEN_ARR_REG"], %zu\n\t", t->repeat);
            }
            break;

            case CMD_OUTPUT: {
                for (i = 0; i < t->repeat; i++) {
                    fprintf(fp,
                            "mov\t\tdil, byte["BF_GEN_ARR_REG"]\n\t"
                            "call\tputchar\n\t");
                }
            }
            break;

            case CMD_INPUT: {
                for (i = 0; i < t->repeat; i++) {
                    fprintf(fp,
                            "call\tgetchar\n\t"
                            "mov\t\tbyte["BF_GEN_ARR_REG"], al\n\t");
                }
            }
            break;

            case CMD_JUMP_F: {
                m_t = tl->tokens[t->m_idx];
                fprintf(fp,
                        ".L%ld:\n\t"
                        "cmp\t\tbyte["BF_GEN_ARR_REG"], 0\n\t"
                        "je\t\t.L%ld\n\t",
                        m_t->m_idx,
                        t->m_idx);
            }
            break;

            case CMD_JUMP_B: {
                m_t = tl->tokens[t->m_idx];
                fprintf(fp,
                        "cmp\t\tbyte["BF_GEN_ARR_REG"], 0\n\t"
                        "jne\t\t.L%ld\n"
                        ".L%ld:\n\t",
                        t->m_idx,
                        m_t->m_idx);
            }
            break;
        } /* end switch(t->op) */
        t = *(++tks);
    } /* end while (t) */

    fprintf(fp,
            "_exit:\n\t"
            "mov\t\trdi, [rbp-8]\n\t"
            "call\tfree\n\t"
            "leave\n\n\t"
            "mov\t\trax, 60\n\t"
            "xor\t\trdi, rdi\n\t"
            "syscall\n"
            );

    fclose(fp);
    return 0;
}
