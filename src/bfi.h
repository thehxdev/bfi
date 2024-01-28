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

#define check_then_free(p) if((p)) free((p))

#define DEINIT_BF_END __attribute__((cleanup(bf_deinit)))


/* BF Commands:
 * '>' | increment data pointer by one
 * '<' | decrement data pointer by one
 * '+' | increment byte at the data pointer by one
 * '-' | decrement byte at the data pointer by one
 * '.' | output the data at data pointer
 * ',' | accept one byte of input and store it in data pointer
 * '[' | jump to previous command of matching `]`, if the data pointer's data is zero
 * ']' | jump to next command of matching `[`, if the data pointer's data is non-zero
 */


typedef struct __bf_state {
    /* data array */
    ubyte *arr;

    /* data pointer location */
    size_t dptr;

    /* BF commands that came from a source file or command-line arg */
    char *cmds;
} BF_State;


/* initialize BF interpreter */
BF_State *bf_init(const char *s_path);


/* deinit BF */
void bf_deinit(BF_State **bfp);


int bf_execute(BF_State *bfp);

#endif /* BFI_BFI_H */
