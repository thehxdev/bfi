#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bfi.h"
#include "scanner.h"
#include "xmem.h"
#include "log.h"


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
    int err = 0;
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        BF_LOG_ERR("Could not open source file: ", NULL);
        perror(NULL);
        return 1;
    }

    *len = __bf_source_file_cmds_count(fp);
    *buff = (char*) calloc((*len) + 1, 1);
    if (!(*buff)) {
        BF_LOG_ERR("Allocating memory for BF commands failed\n", NULL);
        err = 1;
        goto ret;
    }

    while ((ch = fgetc(fp)) != EOF) {
        switch (ch) {
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

ret:
    fclose(fp);
    return err;
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
            BF_LOG_ERR("Extra \']\' command found\n", NULL);
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
        BF_LOG_ERR("Extra \'[\' commands found\n", NULL);
        return 1;
    }

    return 0;
}


BF_State bf_init(const char *s_path) {
    int err;
    BF_State bfs = {0};

    /* read the source file, removed any unnecessary character
     * and store commands to bfs->cmds */
    err = __bf_read_source_file(&bfs.cmds, &bfs.cmds_c, s_path);
    if (err)
        goto ret;

    err = __bf_check_matching_brackets(bfs.cmds);
    if (err)
        goto ret;

    /* tokenize the commands */
    bfs.tl = __bf_scan_cmds(bfs.cmds, bfs.cmds_c);
    if (!bfs.tl.tokens) {
        BF_LOG_ERR("Scanning BF commands failed\n", NULL);
        goto ret;
    }

    /* after scanning commands to tokens, we don't need
     * them anymore */
    xfree(bfs.cmds);

ret:
    return bfs;
}


void bf_deinit(BF_State *bfp) {
    if (bfp) {
        xfree(bfp->cmds);
        xfree(bfp->tl.tokens);
    }
}
