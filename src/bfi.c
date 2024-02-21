#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bfi.h"
#include "scanner.h"
#include "xmem.h"


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
        fprintf(stderr, "Could not open source file: ");
        perror(NULL);
        return 1;
    }

    *len = __bf_source_file_cmds_count(fp);
    *buff = (char*) calloc((*len) + 1, 1);
    if (!(*buff)) {
        BF_LOG_ERR("Allocating memory for BF commands failed.");
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
            BF_LOG_ERR("extra \']\' command found");
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
        BF_LOG_ERR("extra \'[\' commands found");
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
        BF_LOG_ERR("Allocating memory for BF data array failed");
        xfree(bfs);
        exit(1);
    }
    bfs->cmds_c = 0;

    /* read the source file, removed any unnecessary character
     * and store commands to bfs->cmds */
    err = __bf_read_source_file(&bfs->cmds, &bfs->cmds_c, s_path);
    if (err) {
        bf_deinit(&bfs);
        exit(1);
    }

    err = __bf_check_matching_brackets(bfs->cmds);
    if (err) {
        bf_deinit(&bfs);
        exit(1);
    }

    /* tokenize the commands */
    bfs->tl = __bf_scan_cmds(bfs->cmds, bfs->cmds_c);
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


int bf_execute(BF_TokenList **tlp, ubyte **darr) {
    long i;
    ubyte *arr = *darr;
    BF_TokenList *tl = *tlp;

    BF_Token **tks = tl->tokens, *t = *tks;
    tks[tl->len] = NULL;

    while (t) {
        switch (t->op) {
#ifdef NON_STD_CMDS
            case '?':
                arr = *darr;
                memset(arr, 0, __BF_ARR_CAP);
                tks++;
                break;
#endif /* NON_STD_CMDS */
            case '>':
                arr += t->repeat; tks++; break;

            case '<':
                arr -= t->repeat; tks++; break;

            case '+':
                *arr += t->repeat; tks++; break;

            case '-':
                *arr -= t->repeat; tks++; break;

            case '.':
                i = t->repeat;
                while (i--)
                    fputc(*arr, stdout);
                tks++;
                break;

            case ',':
                i = t->repeat;
                while (i--)
                    scanf("%c", arr);
                tks++;
                break;

            case '[':
                if (*arr == 0)
                    tks = &(tl->tokens[t->m_idx]);
                tks++;
                break;

            case ']':
                if (*arr != 0)
                    tks = &(tl->tokens[t->m_idx]);
                tks++;
                break;

#ifdef SAFE_BFI
            default:
                BF_LOG_ERR("Invalid BF Token.");
                exit(1);
#endif /* SAFE_BFI */
        } /* End switch(t->op) */

#ifdef SAFE_BFI
        if (arr < *darr || arr >= (*darr) + __BF_ARR_CAP) {
            BF_LOG_ERR("Out of range access to data array.");
            exit(1);
        }
#endif /* SAFE_BFI */

        t = *tks;
    } /* End while(*tsk) */

    return 0;
}
