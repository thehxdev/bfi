#include <stdio.h>

/* BFI */
#include "scanner.h"
#include "interpreter.h"

/* Capacity of BrainFuck data array */
#define BF_ARR_CAP (0x10000)

/* execute BF commands */
int bf_execute(BF_Token *tlp) {
    BF_Token t;
    unsigned int ip = 0, dp = 0;
    unsigned char arr[BF_ARR_CAP] = { 0 };

    while (1) {
        t = tlp[ip];
        switch (t.op) {
            case '>':
                dp += t.data; break;

            case '<':
                dp -= t.data; break;

            case '+':
                arr[dp] += t.data; break;

            case '-':
                arr[dp] -= t.data; break;

            case '[':
                if (arr[dp] == 0)
                    ip = t.data;
                break;

            case ']':
                if (arr[dp] != 0)
                    ip = t.data;
                break;

            case '.':
                while (t.data--)
                    putchar(arr[dp]);
                break;

            case ',':
                while (t.data--)
                    arr[dp] = getchar();
                break;

            case CMD_EXIT:
                return 0;
        }

        ip++;
    }
}
