#include <stdio.h>
#include <stdlib.h>
#ifdef NON_STD_CMDS
# include <memory.h>
#endif /* NON_STD_CMDS */

/* BFI */
#include "scanner.h"
#include "interpreter.h"
#include "log.h"


/* Capacity of BrainFuck data array */
#define __BF_ARR_CAP (1<<16)


void *bf_init_data_array(ubyte **arr) {
    *arr = (ubyte*) calloc(__BF_ARR_CAP, sizeof(ubyte));
    if (!*arr) {
        BF_LOG_ERR("%s: Allocating memory for BF data array failed\n", __FUNCTION__);
        return NULL;
    }
    return *arr;
}


/* execute BF commands */
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
