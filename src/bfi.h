#ifndef BFI_BFI_H
#define BFI_BFI_H

#include <stddef.h>
#include "scanner.h"

/* `BF_State` type to store the state of interpreter */
typedef struct bf_state {
    /* BF commands that read from a source file */
    char *cmds;

    /* count of BF commands */
    size_t cmds_c;

    /* a list of tokens */
    BF_TokenList *tl;
} BF_State;


/* initialize BF interpreter */
BF_State bf_init(const char *s_path);

#endif /* BFI_BFI_H */
