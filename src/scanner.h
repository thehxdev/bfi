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
enum BF_CMD {
    CMD_EXIT    = 0,
    CMD_INC_DP  = '>',
    CMD_DEC_DP  = '<',
    CMD_INC_VAL = '+',
    CMD_DEC_VAL = '-',
    CMD_OUTPUT  = '.',
    CMD_INPUT   = ',',
    CMD_JUMP_FORWARD  = '[', /* jump forward, if data pointer is zero */
    CMD_JUMP_BACK     = ']', /* jump backward, if data pointer is not zero */
};


typedef struct __bf_token {
    int op, data;
} BF_Token;


/* a dynamic array of tokens */
typedef struct __bf_tokenlist {
    BF_Token *tokens;
    unsigned int len;
    unsigned int cap;
} BF_TokenList;


/* scan BF commands and create a token list */
BF_TokenList *bf_scan_cmds(const char *bf_cmds, const size_t len);


#endif /* BFI_SCANNER_H */
