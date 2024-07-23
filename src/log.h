#ifndef BFI_LOG_H
#define BFI_LOG_H


#ifdef SAFE_BFI
    /* Log error messages */
    #define BF_LOG_ERR(format, ...) \
        (void)fprintf(stderr, "[ERROR] %s: " format, __FUNCTION__, __VA_ARGS__)
#else
    #define BF_LOG_ERR(format, ...)
#endif


#endif /* BFI_LOG_H */
