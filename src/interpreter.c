#include <stdio.h>

/* BFI */
#include "scanner.h"
#include "interpreter.h"
#ifdef SAFE_BFI
    #include "log.h"
#endif // SAFE_BFI

/* Capacity of BrainFuck data array */
#define __BF_ARR_CAP (0x10000)

static unsigned int i = 0;
static unsigned int ptr = 0;
static ubyte arr[__BF_ARR_CAP] = {0};
static BF_Token *tks_base, *tks, t;

/* execute BF commands */
int bf_execute(BF_Token *tlp) {
    /* init state */
    tks_base = tlp, tks = tlp, t = *tks;

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
                    tks = tlp+t.m_idx;
                break;

            case ']':
                if (arr[ptr] != 0)
                    tks = tlp+t.m_idx;
                break;

            case CMD_EXIT:
                goto exit;

            case CMD_COUNT:
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

exit:
    return 0;
}
