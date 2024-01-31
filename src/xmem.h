#ifndef BF_XMEM_H
#define BF_XMEM_H


/* free a pointer and assign it to NULL. 
 * This will prevent double free since `free`
 * ignores NULL pointers. */
#define xfree(p) { free((p)); (p) = NULL; }


#endif /* BF_XMEM_H */
