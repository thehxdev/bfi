#ifndef BFI_SCANNER_H
#define BFI_SCANNER_H

#include <stddef.h> /* For size_t */


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

/* each command is represented by it's own ascii code */
enum __BF_CMD {
    OP_INC_DP  = '>',
    OP_DEC_DP  = '<',
    OP_INC_VAL = '+',
    OP_DEC_VAL = '-',
    OP_OUTPUT  = '.',
    OP_INPUT   = ',',
    OP_JUMP_F  = '[', /* jump forward, if data pointer is zero */
    OP_JUMP_B  = ']', /* jump backward, if data pointer is not zero */
#ifdef NON_STD_CMDS
    OP_CLEAR   = '?',
#endif /* NON_STD_CMDS */
};


typedef struct __bf_token {
    enum __BF_CMD op;

    /* how many times a command repeated in a row? */
    size_t repeat;

    /* to handle '[' and ']' commands,
     * lable the matching ones with same
     * lable to find them easily.
     * a lable is a randomly generated number.
     * for other commands this will be 0 */
    long lable;
} BF_Token;


/* a dynamic array of tokens */
typedef struct __bf_tokenlist {
    BF_Token **tokens;
    size_t len;
    size_t cap;
} BF_TokenList;


/* scan BF commands and create a token list */
BF_TokenList *__bf_scanner_scan_cmds(const char *bf_cmds, const size_t len);


/* free a token list */
void __bf_tokenlist_free(BF_TokenList *bf_tlp);


/* get a token from token list */
BF_Token *__bf_tokenlist_get(const BF_TokenList *bf_tlp, const long idx);

#endif /* BFI_SCANNER_H */
