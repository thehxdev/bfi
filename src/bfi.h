#ifndef BFI_BFI_H
#define BFI_BFI_H

#include <stddef.h> /* For size_t */
#include "scanner.h"


/* Capacity of BrainFuck data array */
#define __BF_ARR_CAP (1<<16)


/* ubyte -> an unsigned 8 bit integer
 * byte -> a signed 8 bit integer */
#if !defined(ubyte) && !defined(byte)
# define byte char
# define ubyte unsigned char
#endif /* byte */

/* a special attribute to call `bf_deinit` on `BF_State`
 * at the end of scope */
#define DEINIT_BF_END __attribute__((cleanup(bf_deinit)))

/* Log error messages */
#define BF_LOG_ERR(format, ...) fprintf(stderr, "[ERROR] " format, __VA_ARGS__)


/* `BF_State` type to store the state of interpreter */
typedef struct __bf_state {
    /* data array */
    ubyte *arr;

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
int bf_execute(BF_TokenList **tlp, ubyte **darr);


/* write BrainF*ck tokenized commands to a file */
int bf_dump_tokens(BF_TokenList **tlp, const char *out_path);


/* compile BF source code to x86 nasm compatible assembly */
int bf_compiler_x64nasm(BF_TokenList **tlp, const char *out_path);

#endif /* BFI_BFI_H */
