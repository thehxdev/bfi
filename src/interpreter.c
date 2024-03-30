#include <stdio.h>
#include <stdlib.h>

/* BFI */
#include "scanner.h"
#include "interpreter.h"
#include "log.h"


/* Capacity of BrainFuck data array */
#define __BF_ARR_CAP (1<<16)


/* execute BF commands */
int bf_execute(BF_TokenList *tlp) {
    long i, ptr = 0;
    ubyte arr[__BF_ARR_CAP] = { 0 };
    register BF_Token *tks = tlp->tokens, t = *tks;

    while (1) {
        if (t.repeat == 0)
            break;

        switch (t.op) {
            case '>':
                ptr += t.repeat; break;

            case '<':
                ptr -= t.repeat; break;

            case '+':
                arr[ptr] += t.repeat; break;

            case '-':
                arr[ptr] -= t.repeat; break;

            case '.':
                i = t.repeat;
                while (i--)
                    putchar(arr[ptr]);
                break;

            case ',':
                i = t.repeat;
                while (i--)
                    scanf("%c", &arr[ptr]);
                break;

            case '[':
                if (arr[ptr] == 0)
                    tks = &(tlp->tokens[t.m_idx]);
                break;

            case ']':
                if (arr[ptr] != 0)
                    tks = &(tlp->tokens[t.m_idx]);
                break;

#ifdef SAFE_BFI
            default:
                BF_LOG_ERR("%s: Invalid BF Token\n", __FUNCTION__);
                return 1;
#endif /* SAFE_BFI */
        } /* End switch(t->op) */

#ifdef SAFE_BFI
        if (ptr < 0 || ptr >= __BF_ARR_CAP) {
            BF_LOG_ERR("%s: Out of range access to data array\n", __FUNCTION__);
            return 1;
        }
#endif /* SAFE_BFI */

        t = *(++tks);
    } /* End while(*tsk) */

    return 0;
}
