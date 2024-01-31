#ifndef BFI_BFI_H
#define BFI_BFI_H

#include <stddef.h> /* For size_t */
#include "scanner.h"


#define __BF_ARR_CAP (1<<16)


/* ubyte -> an unsigned 8 bit integer
 * byte -> a signed 8 bit integer */
#if !defined(ubyte) && !defined(byte)
# define byte char
# define ubyte unsigned char
#endif /* byte */

#define DEINIT_BF_END __attribute__((cleanup(bf_deinit)))

#define BF_LOG_ERR(msg) do {            \
    fprintf(stderr, "[ERROR] %s\n", msg); \
} while(0)


typedef struct __bf_state {
    /* data array */
    ubyte *arr;

    /* data pointer location */
    long dptr;

    /* BF commands that read from a source file */
    char *cmds;

    /* count of BF commands */
    size_t cmds_c;

    /* a list of tokens */
    BF_TokenList *tl;
} BF_State;


/* initialize BF interpreter */
BF_State *bf_init(const char *s_path);


/* deinit BF */
void bf_deinit(BF_State **bfp);


/* execute BF commands */
int bf_execute(BF_State *bfp);

#endif /* BFI_BFI_H */
