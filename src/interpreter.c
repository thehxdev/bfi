#include <stdio.h>

/* BFI */
#include "scanner.h"
#include "interpreter.h"
#ifdef SAFE_BFI
    #include "log.h"
#endif // SAFE_BFI


/* Capacity of BrainFuck data array */
#define __BF_ARR_CAP (0x10000)


/* execute BF commands */
int bf_execute(BF_Token *tlp) {
    unsigned int i, ptr = 0;
    ubyte arr[__BF_ARR_CAP] = { 0 };
    BF_Token *tks = tlp, t = *tks;

    while (t.repeat > 0) {
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
                    tks = &(tlp[t.m_idx]);
                break;

            case ']':
                if (arr[ptr] != 0)
                    tks = &(tlp[t.m_idx]);
                break;

#ifdef SAFE_BFI
            default:
                BF_LOG_ERR("Invalid BF Token\n", NULL);
                return 1;
#endif /* SAFE_BFI */
        } /* End switch(t->op) */

#ifdef SAFE_BFI
        if (ptr < 0 || ptr >= __BF_ARR_CAP) {
            BF_LOG_ERR("Out of range access to data array\n", NULL);
            return 1;
        }
#endif /* SAFE_BFI */

        t = *(++tks);
    } /* End while(*tsk) */

    return 0;
}
