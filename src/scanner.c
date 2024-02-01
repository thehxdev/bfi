#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "scanner.h"
#include "xmem.h"


/* create a new token */
static BF_Token *__bf_token_new(const char cmd) {
    BF_Token *t = malloc(sizeof(BF_Token));
    if (!t)
        return NULL;

    t->op = cmd;
    t->repeat = 1;
    t->m_idx = 0;
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
                xfree(bf_tlp->tokens[i]);
        xfree(bf_tlp->tokens);
        xfree(bf_tlp);
    }
}


/* find the matching opening bracket index in token list.
 *
 * Since C doesn't have multiple return values, I needed to 
 * return the token and it's index. So the functoin returns the token
 * and stores the index to `m_idx` pointer. */
static BF_Token *__bf_find_opening_bracket(const BF_TokenList *bf_tlp, long *m_idx) {
    long nest = 0, i;
    BF_Token *t;
    for (i = (long)bf_tlp->len - 1; i >= 0; i--) {
        t = __bf_tokenlist_get(bf_tlp, i);
        if (t->op == ']')
            nest += 1;
        else if (t->op == '[' && nest > 0)
            nest -= 1;
        else if (t->op == '[' && nest == 0) {
            *m_idx = i;
            return t;
        }
    }
    *m_idx = -1;
    return NULL;
}


static char __bf_cmds_get(const char *bf_cmds, const size_t len, const long idx) {
    return (idx >= 0 && idx < (long)len) ? bf_cmds[idx] : '\0';
}


BF_TokenList *__bf_scan_cmds(const char *bf_cmds, const size_t len) {
    size_t i, j;
    long m_idx;
    char curr_c;
    /* m_t -> matching token. used to handle jumps */
    BF_Token *tmp_t, *m_t;
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
        } else if (curr_c == ']') {
            m_t = __bf_find_opening_bracket(tl, &m_idx);
            m_t->m_idx = tl->len;
            tmp_t->m_idx = m_idx;
        }

        __bf_tokenlist_append(tl, (const BF_Token*)tmp_t);
        xfree(tmp_t);
    }

    return tl;
}


BF_Token *__bf_tokenlist_get(const BF_TokenList *bf_tlp, const long idx) {
    return (idx >= 0 && idx < (long)bf_tlp->len) ? bf_tlp->tokens[idx] : NULL;
}