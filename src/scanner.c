#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "scanner.h"

void bf_tokenlist_append(BF_TokenList *tl, BF_Token *t) {
    if (tl->len == tl->cap) {
        tl->cap <<= 1;
        tl->tokens = realloc(tl->tokens, tl->cap * sizeof(*tl->tokens));
        assert(tl->tokens != NULL);
    }
    memcpy(&tl->tokens[tl->len++], t, sizeof(*t));
}

/* scan the commands and create a token list */
BF_TokenList *bf_scan_cmds(const char *source, size_t n) {
    char ch;
    int i, j, depth;
    BF_Token t;

    BF_TokenList *tl = malloc(sizeof(*tl));
    *tl = (BF_TokenList){
        .len = 0,
        .cap = (1 << 10),
    };
    tl->tokens = malloc(sizeof(*tl->tokens) * tl->cap);
    assert(tl->tokens != NULL);

    for (i = 0; (size_t)i < n; i++) {
        ch = source[i];
        switch (ch) {
        case '>':
        case '<':
        case '+':
        case '-':
        case '.':
        case ',':
        case '[':
        case ']':
            t.op = ch;
            break;

        default:
            continue;
        }

        t.data = 1;
        if (t.op == ']') {
            depth = 0;
            for (j = tl->len - 1; j >= 0; j--) {
                switch (tl->tokens[j].op) {
                case ']':
                    depth++;
                    break;

                case '[':
                    if (depth == 0) {
                        t.data = j;
                        tl->tokens[j].data = tl->len;
                        goto append;
                    }
                    depth--;
                    break;
                }
            }
        } else if (t.op != '[') {
            while (t.op == source[++i] && (size_t)i < n)
                t.data++;
            i--;
        }
    append:
        bf_tokenlist_append(tl, &t);
    }

    t = (BF_Token){ .op = CMD_EXIT, .data = 0 };
    bf_tokenlist_append(tl, &t);

    return tl;
}
