#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "scanner.h"
#include "rand.h"


/* create a new token */
static BF_Token *__bf_token_new(const char cmd) {
    BF_Token *t = malloc(sizeof(BF_Token));
    if (!t)
        return NULL;

    t->op = cmd;
    t->repeat = 1;
    t->lable = 0;
    return t;
}


static BF_TokenList *__bf_tokenlist_new(const size_t cap) {
    BF_TokenList *tl = malloc(sizeof(BF_TokenList));
    if (!tl)
        return NULL;

    tl->len = 0;
    tl->cap = cap;
    tl->tokens = NULL;
    return tl;
}


static int __bf_tokenlist_append(BF_TokenList *bf_tlp, const BF_Token *bf_tp) {
    if (!bf_tlp || !bf_tp)
        return 1;

    if (bf_tlp->len % bf_tlp->cap == 0) {
        size_t new_size = (bf_tlp->len + bf_tlp->cap) * sizeof(BF_Token*);
        bf_tlp->tokens = realloc(bf_tlp->tokens, new_size);
        if (!bf_tlp->tokens)
            return 1;
    }

    BF_Token **t = &bf_tlp->tokens[bf_tlp->len];
    *t = malloc(sizeof(BF_Token));
    if (!(*t))
        return 1;

    memmove(*t, bf_tp, sizeof(BF_Token));
    bf_tlp->len += 1;
    return 0;
}


void __bf_tokenlist_free(BF_TokenList *bf_tlp) {
    size_t i;
    if (bf_tlp) {
        if (bf_tlp->tokens)
            for (i = 0; i < bf_tlp->len; i++)
                free(bf_tlp->tokens[i]);
        free(bf_tlp->tokens);
        free(bf_tlp);
    }
}


/* find the matching opening bracket lable in token list */
static long __bf_find_opening_bracket_lable(const BF_TokenList *bf_tlp) {
    size_t nest = 0, i;
    BF_Token *t;
    for (i = bf_tlp->len - 1; i >= 0; i--) {
        t = __bf_tokenlist_get(bf_tlp, i);
        if (t->op == ']')
            nest += 1;
        else if (t->op == '[' && nest > 0)
            nest -= 1;
        else if (t->op == '[' && nest == 0)
            return t->lable;
    }
    return -1;
}


static char __bf_cmds_get(const char *bf_cmds, const size_t len, const long idx) {
    return (idx >= 0 && idx < (long)len) ? bf_cmds[idx] : '\0';
}


BF_TokenList *__bf_scanner_scan_cmds(const char *bf_cmds, const size_t len) {
    size_t i, j;
    char curr_c;
    BF_Token *tmp_t;
    BF_TokenList *tl = __bf_tokenlist_new(25);
    if (!tl)
        return NULL;

    for (i = 0; i < len; i++) {
        curr_c = __bf_cmds_get(bf_cmds, len, i);
        tmp_t = __bf_token_new(curr_c);
        if (!tmp_t) {
            __bf_tokenlist_free(tl);
            return NULL;
        }

        if (curr_c != ']' && curr_c != '[') {
            for (j = i + 1; __bf_cmds_get(bf_cmds, len, j) == curr_c; j++, i++)
                tmp_t->repeat += 1;
        } else if (curr_c == '[') {
            tmp_t->lable = __bf_gen_rand_long(0, (1<<16));
        } else if (curr_c == ']') {
            tmp_t->lable = __bf_find_opening_bracket_lable(tl);
        }

        __bf_tokenlist_append(tl, (const BF_Token*)tmp_t);
        free(tmp_t);
    }

    return tl;
}


BF_Token *__bf_tokenlist_get(const BF_TokenList *bf_tlp, const long idx) {
    return (idx >= 0 && idx < (long)bf_tlp->len) ? bf_tlp->tokens[idx] : NULL;
}
