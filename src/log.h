#ifndef BFI_LOG_H
#define BFI_LOG_H


/* Log error messages */
#define BF_LOG_ERR(format, ...) (void)fprintf(stderr, "[ERROR] " format, __VA_ARGS__)


#endif /* BFI_LOG_H */
