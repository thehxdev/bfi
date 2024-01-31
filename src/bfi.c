#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bfi.h"
#include "scanner.h"
#include "xmem.h"


/* count the commands in a source file */
static size_t __bf_source_file_cmds_count(FILE *fp) {
    char ch;
    size_t count = 0;
    if (!fp)
        return 0;

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


/* read a BF source file, remove any unnecessary character
 * and store commands in BF_State */
static int __bf_read_source_file(BF_State *bfp, const char *path) {
    char ch;
    size_t i = 0;
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        BF_LOG_ERR("source file not found.");
        bf_deinit(&bfp);
        exit(1);
    }

    bfp->cmds_c = __bf_source_file_cmds_count(fp);
    bfp->cmds = (char*) calloc(bfp->cmds_c + 1, 1);
    if (!bfp->cmds) {
        BF_LOG_ERR("Allocating memory for BF commands failed.");
        bf_deinit(&bfp);
        exit(1);
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
            case ']': bfp->cmds[i] = ch; i += 1; break;
        }
    }

    fclose(fp);
    return 0;
}


/* check all brackets before execution and if there are
 * non-matching ones, report error and exit */
static void __bf_check_matching_brackets(BF_State *bfp) {
    char c;
    size_t i = 0;
    size_t nest = 0;

    while ((c = bfp->cmds[i])) {
        if (c == '[') {
            nest += 1;
        } else if (c == ']' && nest == 0) {
            BF_LOG_ERR("extra \']\' command found");
            bf_deinit(&bfp);
            exit(1);
        } else if (c == ']' && nest > 0) {
            nest -= 1;
        }

        i += 1;
    }

    /* if all commands are checked and
     * nesting is greater than 0, indicates
     * extra `[` character are used */
    if (nest > 0) {
        BF_LOG_ERR("extra \'[\' commands found");
        bf_deinit(&bfp);
        exit(1);
    }
}


BF_State *bf_init(const char *s_path) {
    BF_State *bfs = (BF_State*) malloc(sizeof(BF_State));
    if (!bfs)
        return NULL;

    bfs->arr = (ubyte*) calloc(__BF_ARR_CAP, sizeof(ubyte));
    if (!bfs->arr) {
        BF_LOG_ERR("Allocating memory for BF data array failed");
        xfree(bfs);
        exit(1);
    }

    bfs->dptr = 0;
    bfs->cmds_c = 0;

    /* read the source file, removed any unnecessary character
     * and store commands to bfs->cmds */
    __bf_read_source_file(bfs, s_path);

    __bf_check_matching_brackets(bfs);

    /* tokenize the commands */
    bfs->tl = __bf_scanner_scan_cmds(bfs->cmds, bfs->cmds_c);
    if (!bfs->tl) {
        BF_LOG_ERR("Scanning BF commands failed");
        bf_deinit(&bfs);
        exit(1);
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


int bf_execute(BF_State *bfp) {
    long cptr = 0, i;
    BF_TokenList *tl = bfp->tl;
    BF_Token *t;

    while (cptr < (long)tl->len) {
        t = __bf_tokenlist_get(tl, cptr);

        switch (t->op) {
#ifdef NON_STD_CMDS
            case '?':
                memset(bfp->arr, 0, __BF_ARR_CAP);
                bfp->dptr = 0;
                cptr += 1;
                break;
#endif /* NON_STD_CMDS */
            case '>':
                bfp->dptr += t->repeat; cptr += 1; break;

            case '<':
                bfp->dptr -= t->repeat; cptr += 1; break;

            case '+':
                bfp->arr[bfp->dptr] += t->repeat; cptr += 1; break;

            case '-':
                bfp->arr[bfp->dptr] -= t->repeat; cptr += 1; break;

            case '.':
                for (i = 0; i < (long)t->repeat; i++)
                    putc(bfp->arr[bfp->dptr], stdout);
                cptr += 1;
                break;

            case ',':
                for (i = 0; i < (long)t->repeat; i++)
                    scanf("%c", &bfp->arr[bfp->dptr]);
                cptr += 1;
                break;

            case '[':
                if (bfp->arr[bfp->dptr] == 0) {
                    cptr = t->m_idx;
                    cptr += 1;
                } else {
                    cptr += 1;
                }
                break;

            case ']':
                if (bfp->arr[bfp->dptr] != 0) {
                    cptr = t->m_idx;
                    cptr += 1;
                } else {
                    cptr += 1;
                }
                break;
        }

        if (bfp->dptr < 0) {
            BF_LOG_ERR("Out of bound access in data array.");
            bf_deinit(&bfp);
            exit(1);
        }
    }

    return 0;
}
