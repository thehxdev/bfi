#ifndef BFI_INTERPTRETER_H
#define BFI_INTERPTRETER_H

#include "types.h"

/* execute BF commands */
int bf_execute(BF_TokenList **tlp, ubyte **darr);

/* initialize BF data array */
void *bf_init_data_array(ubyte **arr);

#endif /* BFI_INTERPTRETER_H */
