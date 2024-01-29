#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bfi.h"


/* count the commands in a source file */
static size_t __bf_source_file_cmds_count(FILE *fp) {
    char ch;
    size_t count = 0;
    if (!fp)
        return 0;

    while ((ch = fgetc(fp)) != EOF) {
        switch (ch) {
            case '?':
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
static int __bf_read_source(BF_State *bfp, const char *path) {
    char ch;
    size_t i = 0;
    FILE *fp = fopen(path, "rb");
    if (!fp)
        return 1;

    bfp->cmds_c = __bf_source_file_cmds_count(fp);
    bfp->cmds = (char*) calloc(bfp->cmds_c + 1, 1);
    if (!bfp->cmds) {
        BF_LOG_ERR("Allocating memory for BF commands failed.");
        bf_deinit(&bfp);
        exit(1);
    }

    while ((ch = fgetc(fp)) != EOF) {
        switch (ch) {
            case '?':
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


BF_State *bf_init(const char *s_path) {
    BF_State *bfs = (BF_State*) malloc(sizeof(BF_State));
    if (!bfs)
        return NULL;

    bfs->arr = (ubyte*) calloc(__BF_ARR_CAP, sizeof(ubyte));
    if (!bfs->arr) {
        BF_LOG_ERR("Allocating memory for BF data array failed");
        free(bfs);
        exit(1);
    }

    bfs->dptr = 0;
    bfs->cmds_c = 0;
    __bf_read_source(bfs, s_path);
    return bfs;
}


void bf_deinit(BF_State **bfp) {
    BF_State *tmp = *bfp;
    if (tmp) {
        check_then_free(tmp->cmds);
        check_then_free(tmp->arr);
        free(tmp);
    }
}


/* seek to matching closing bracket */
static void seek_to_closing_bracket(BF_State *bfp, int *cptr) {
    int nest = 0;
    while (bfp->cmds[*cptr]) {
        if (bfp->cmds[*cptr] == '[') {
            nest += 1;
        } else if (bfp->cmds[*cptr] == ']' && nest > 0) {
            nest -= 1;
        } else if (bfp->cmds[*cptr] == ']' && nest == 0)
            break;

        *cptr += 1;
    }
}


/* seek to matching opening bracket */
static void seek_to_opening_bracket(BF_State *bfp, int *cptr) {
    int nest = 0;
    while (bfp->cmds[*cptr]) {
        if (bfp->cmds[*cptr] == ']') {
            nest += 1;
        } else if (bfp->cmds[*cptr] == '[' && nest > 0) {
            nest -= 1;
        } else if (bfp->cmds[*cptr] == '[' && nest == 0)
            break;

        *cptr -= 1;
    }
}


int bf_execute(BF_State *bfp) {
    int cptr = 0;
    if (!bfp->cmds)
        return 1;

    while (bfp->cmds[cptr]) {
        switch (bfp->cmds[cptr]) {
            case '?':
                /* non-standard brainf*ck command.
                 * used to clear every thing and reset
                 * data pointer */
                memset(bfp->arr, 0, __BF_ARR_CAP);
                bfp->dptr = 0;
                cptr += 1;
                break;
            case '>':
                bfp->dptr += 1; cptr += 1; break;
            case '<':
                bfp->dptr -= 1; cptr += 1; break;
            case '+':
                bfp->arr[bfp->dptr] += 1; cptr += 1; break;
            case '-':
                bfp->arr[bfp->dptr] -= 1; cptr += 1; break;
            case '.':
                putc(bfp->arr[bfp->dptr], stdout);
                cptr += 1;
                break;
            case ',':
                scanf("%c", &bfp->arr[bfp->dptr]);
                cptr += 1;
                break;
            case '[':
                if (bfp->arr[bfp->dptr] == 0) {
                    cptr += 1;
                    seek_to_closing_bracket(bfp, &cptr);
                    cptr += 1; /* skip ']' */
                } else {
                    cptr += 1;
                }
                break;
            case ']':
                if (bfp->arr[bfp->dptr] != 0) {
                    cptr -= 1;
                    seek_to_opening_bracket(bfp, &cptr);
                    cptr += 1; /* skip '[' */
                } else {
                    cptr += 1;
                }
                break;
        }
    }

    return 0;
}
