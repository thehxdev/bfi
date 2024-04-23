#ifndef BFI_TYPES_H
#define BFI_TYPES_H

/* General Types used by bfi */


/* ubyte -> an unsigned 8 bit integer
 * byte -> a signed 8 bit integer */
#if !defined(ubyte) && !defined(byte)
    #define byte char
    #define ubyte unsigned char
#endif /* byte */


#endif /* BFI_TYPES_H */
