#ifndef BFI_COMPILER_H
#define BFI_COMPILER_H

/* Compile Brainfuck source code to x64 assembly (AT&T Syntax) */
int bf_compiler_x64gcc(BF_TokenList *tlp, const char *out_path);

#endif /* BFI_COMPILER_H */
