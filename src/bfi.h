#ifndef BFI_BFI_H
#define BFI_BFI_H

#define __BF_ARR_CAP (1<<16)


#ifndef size_t
# define size_t unsigned long
#endif /* size_t */


/* ubyte -> an unsigned 8 bit integer
 * byte -> a signed 8 bit integer */
#if !defined(ubyte) && !defined(byte)
# define byte char
# define ubyte unsigned char
#endif /* byte */

/* removed the `if` statement because `free()` ignores
 * NULL pointer by default! */
#define check_then_free(p) { free((p)); (p) = NULL; }

#define DEINIT_BF_END __attribute__((cleanup(bf_deinit)))

#define BF_LOG_ERR(msg) do {            \
    fprintf(stderr, "[ERROR] %s\n", msg); \
} while(0)


/* BF Commands:
 * '>' | increment data pointer by one
 * '<' | decrement data pointer by one
 * '+' | increment byte at the data pointer by one
 * '-' | decrement byte at the data pointer by one
 * '.' | output the data at data pointer
 * ',' | accept one byte of input and store it in data pointer
 * '[' | jump to next command of matching `]`, if the data pointer's data is zero
 * ']' | jump to next command of matching `[` (goes backward), if the data pointer's data is non-zero
 */


typedef struct __bf_state {
    /* data array */
    ubyte *arr;

    /* data pointer location */
    long dptr;

    /* BF commands that read from a source file */
    char *cmds;

    /* count of BF commands */
    size_t cmds_c;
} BF_State;


/* initialize BF interpreter */
BF_State *bf_init(const char *s_path);


/* deinit BF */
void bf_deinit(BF_State **bfp);


/* execute BF commands */
int bf_execute(BF_State *bfp);

#endif /* BFI_BFI_H */
