#include <stdio.h>

/* BFI */
#include "scanner.h"
#include "interpreter.h"
#ifdef SAFE_BFI
    #include "log.h"
#endif // SAFE_BFI

/* Capacity of BrainFuck data array */
#define BF_ARR_CAP (0x10000)

/* execute BF commands */
int bf_execute(BF_Token *tlp) {
    unsigned int i = 0;
    unsigned int ptr = 0;
    ubyte arr[BF_ARR_CAP] = {0};
    BF_Token *tks, t;

    /* init state */
    tks = tlp, t = *tks;

    while (1) {
        switch (t.op) {
            case '>':
                ptr += t.repeat; break;

            case '<':
                ptr -= t.repeat; break;

            case '+':
                arr[ptr] += t.repeat; break;

            case '-':
                arr[ptr] -= t.repeat; break;

            case '[':
                if (arr[ptr] == 0)
                    tks = tlp+t.m_idx;
                break;

            case ']':
                if (arr[ptr] != 0)
                    tks = tlp+t.m_idx;
                break;

            case '.':
                i = t.repeat;
                while (i--)
                    putchar(arr[ptr]);
                break;

            case ',':
                i = t.repeat;
                while (i--)
                    arr[ptr] = getchar();
                break;

            case CMD_EXIT:
                goto exit;

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

exit:
    return 0;
}
