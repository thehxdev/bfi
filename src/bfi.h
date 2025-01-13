#ifndef BFI_BFI_H
#define BFI_BFI_H

#include <stddef.h> /* For size_t */
#include "scanner.h"
#include "types.h"


/* a special attribute to call `bf_deinit` on `BF_State`
 * at the end of scope */
#define DEINIT_BF_END __attribute__((cleanup(bf_deinit)))


/* `BF_State` type to store the state of interpreter */
typedef struct __bf_state {
    /* BF commands that read from a source file */
    char *cmds;

    /* count of BF commands */
    size_t cmds_c;

    /* a list of tokens */
    BF_TokenList tl;
} BF_State;


/* initialize BF interpreter */
BF_State bf_init(const char *s_path);


/* deinit BF */
void bf_deinit(BF_State *bfp);


#endif /* BFI_BFI_H */
