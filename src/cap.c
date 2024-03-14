/*
 * Cap - Command-line argument parser library in single header file.
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Repository: https://github.com/thehxdev/cap
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Standard Library */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

/* Cap */
#include "cap.h"



/*
 * macros
 */

/* enable debugging.
 * un-comment this line to enable logging */
#define __CAP_DEBUG__

/* Log error messages */
#ifdef __CAP_DEBUG__
# define CAP_LOG_ERR(format, ...) \
     (void)fprintf(stderr, "[ERROR] " format, __VA_ARGS__)

# define CAP_LOG_INF(format, ...) \
     (void)fprintf(stderr, "[INFO] " format, __VA_ARGS__)
#else
# define CAP_LOG_ERR(format, ...)
# define CAP_LOG_INF(format, ...)
#endif

/* Since `free()` ignores NULL pointer, set pointer to NULL
 * after calling `free()` to prevent double free error. */
#define xfree(p) do { free((p)); (p) = NULL; } while (0)


/* general macro to append a new item to a dynamic array */
#define __cap_darr_append(ptr, len, cap, val, val_type)             \
do {                                                                \
    if ((len) % (cap) == 0) {                                       \
        size_t __new_size__ = ((len) + (cap)) * sizeof(val_type);   \
        (ptr) = realloc((ptr), __new_size__);                       \
    }                                                               \
    val_type **__tmp__ = &ptr[len];                                 \
    *__tmp__ = val;                                                 \
    (len) += 1;                                                     \
} while (0)


/* general macro to find an item in a dynamic array */
#define __cap_darr_find(res, tmp, tmp_type, ptr, len, cmp)          \
do {                                                                \
    tmp_type *tmp;                                                  \
    for (size_t i = 0; i < (len); i++) {                            \
        tmp = (ptr)[i];                                             \
        if ((cmp)) {                                                \
            (res) = tmp;                                            \
            break;                                                  \
        }                                                           \
    }                                                               \
} while (0)


/*
 * Type definitions
 */

/* general type to store flag metadata */
typedef struct __Cap_Flag_t {
    /* is flag provided in command-line or not */
    int met;

    /* flag name */
    char *name;

    /* help message */
    char *help;

    /* flag value */
    char *val;
} __Cap_Flag_t;


/* dynamic array to store flags */
typedef struct __Cap_FList_t {
    /* flags array */
    __Cap_Flag_t **flags;
    
    /* capacity */
    size_t cap;

    /* length */
    size_t len;
} __Cap_FList_t;


/* Sub-Command
 * Each sub-command has it's own flag-list */
typedef struct __Cap_Subcmd_t {
    /* sub-command name */
    char *name;

    /* sub-command's raw arguments */
    char **raw_argv;

    /* sub-command's raw args count */
    size_t raw_argc;

    /* flags associated with sub-command */
    __Cap_FList_t *flags;

    /* sub-command help message */
    char *help;
} __Cap_Subcmd_t;


/* dynamic array to store sub-commands */
typedef struct __Cap_SCList_t {
    /* cmds array */
    __Cap_Subcmd_t **cmds;

    /* capacity */
    size_t cap;

    /* length */
    size_t len;
} __Cap_SCList_t;


/* Cap */
struct Cap_t {
    /* name of executable */
    char *prog;

    /* raw command-line arguments */
    char **argv;

    /* length of raw args */
    size_t argc;

    /* provided sub-command from arguments */
    __Cap_Subcmd_t *scmd;

    /* registered sub-commands list */
    __Cap_SCList_t *sub_cmds;

    /* main registered flags (without sub-command) */
    __Cap_FList_t *m_flags;
};




/*
 * Functionalities
 */

/* Since `free()` ignores NULL pointer, set pointer
 * to NULL to prevent double free */
#if 0
static inline void xfree(void *p) {
    free(p);
    p = NULL;
}
#endif


/* Get a character form character array */
/*
static inline char __cap_str_getchar(const char *s,
                                     const size_t slen,
                                     const long idx)
{
    return ((idx >= 0) && (idx < (long)slen)) ? s[idx] : '\0';
}
*/


/* Get a sub-string from a character array */
/*
static char *__cap_str_substr(char *s,
                              const size_t slen,
                              const long start,
                              const long end)
{
    if (s == NULL || start >= (long)slen)
        goto ret;

    s += start;
    if (end != 0 && end <= (long)slen)
        s[end] = '\0';

ret:
    return s;
}
*/


static void __cap_flag_free(__Cap_Flag_t **flag) {
    __Cap_Flag_t *f = *flag;
    if (f) {
        xfree(f->name);
        xfree(f->help);
        /* xfree(f->val); */
        xfree(f);
    }
}


/* free a flag-list */
static void __cap_flist_free(__Cap_FList_t **flp) {
    size_t i;
    __Cap_FList_t *fl = *flp;
    if (fl) {
        for (i = 0; i < fl->len; i++)
            __cap_flag_free(&fl->flags[i]);
        xfree(fl->flags);
        xfree(fl);
    }
}


static void __cap_subcmd_free(__Cap_Subcmd_t **scmd) {
    __Cap_Subcmd_t *sc = *scmd;
    if (sc) {
        __cap_flist_free(&sc->flags);
        xfree(sc->name);
        xfree(sc->help);
        xfree(sc);
    }
}


static void __cap_sclist_free(__Cap_SCList_t **sclist) {
    size_t i;
    __Cap_SCList_t *scl = *sclist;
    if (scl) {
        for (i = 0; i < scl->len; i++)
            __cap_subcmd_free(&scl->cmds[i]);
        xfree(scl->cmds);
        xfree(scl);
    }
}


static __Cap_Subcmd_t *__cap_subcmd_new(const char *name, const char *help) {
    __Cap_Subcmd_t *sc = malloc(sizeof(__Cap_Subcmd_t));
    if (sc == NULL) {
        CAP_LOG_ERR("%s: Failed to allocate memory for new sub-command\n", __FUNCTION__);
        CAP_LOG_ERR("New sub-command name: %s\n", name);
        return NULL;
    }

    sc->name = strdup(name);
    if (sc->name == NULL) {
        __cap_subcmd_free(&sc);
        CAP_LOG_ERR("%s: Failed copy \'name\' parameter to \'sc->name\'\n", __FUNCTION__);
        CAP_LOG_ERR("New sub-command name: %s\n", name);
        return NULL;
    }

    sc->help = strdup(help);
    if (sc->help == NULL) {
        __cap_subcmd_free(&sc);
        CAP_LOG_ERR("%s: Failed copy \'help\' parameter to \'sc->help\'\n", __FUNCTION__);
        CAP_LOG_ERR("New sub-command name: %s\n", name);
        return NULL;
    }

    sc->flags = NULL;
    sc->raw_argc = 0;
    sc->raw_argv = NULL;

    return sc;
}


static __Cap_SCList_t *__cap_sclist_new(const size_t capacity) {
    __Cap_SCList_t *scl = malloc(sizeof(__Cap_SCList_t));
    if (scl == NULL) {
        CAP_LOG_ERR("%s: Failed to allocate memory for new sub-command list\n", __FUNCTION__);
        return NULL;
    }

    scl->len  = 0;
    scl->cap  = capacity;
    scl->cmds = NULL;

    return scl;
}


static __Cap_Subcmd_t *__cap_sclist_find(const __Cap_SCList_t *sclist,
                                         const char *name)
{
    __Cap_Subcmd_t *res = NULL;
    __cap_darr_find(res,
                    tmp,
                    __Cap_Subcmd_t,
                    sclist->cmds,
                    sclist->len,
                    (strcmp(tmp->name, name)) == 0);
    return res;
}


static __Cap_Flag_t *__cap_flag_new(const char *name, const char *help) {
    __Cap_Flag_t *f = malloc(sizeof(__Cap_Flag_t));
    if (f == NULL) {
        CAP_LOG_ERR("%s: Failed to allocate memory for new flag\n", __FUNCTION__);
        CAP_LOG_ERR("New flag name: %s\n", name);
        return NULL;
    }

    f->val  = NULL;
    f->name = strdup(name);
    if (f->name == NULL) {
        CAP_LOG_ERR("%s: Failed to write name of flag to \'f->name\'\n", __FUNCTION__);
        CAP_LOG_ERR("New flag name: %s\n", name);
        __cap_flag_free(&f);
        return NULL;
    }

    f->help = strdup(help);
    if (f->help == NULL) {
        CAP_LOG_ERR("%s: Failed to write flag help message to \'f->help\'\n", __FUNCTION__);
        CAP_LOG_ERR("New flag name: %s\n", name);
        __cap_flag_free(&f);
        return NULL;
    }

    return f;
}


static __Cap_FList_t *__cap_flist_new(const size_t capacity) {
    __Cap_FList_t *fl = malloc(sizeof(__Cap_FList_t));
    if (fl == NULL) {
        CAP_LOG_ERR("%s: Failed to allocate memory for new flag list\n", __FUNCTION__);
        return NULL;
    }

    fl->len  = 0;
    fl->cap  = capacity;
    fl->flags = NULL;

    return fl;
}


static __Cap_Flag_t *__cap_flist_find(const __Cap_FList_t *fl,
                                      const char *name)
{
    __Cap_Flag_t *res = NULL;
    __cap_darr_find(res,
                    tmp,
                    __Cap_Flag_t,
                    fl->flags,
                    fl->len,
                    (strcmp(tmp->name, name)) == 0);
    return res;
}


/* Initialize Cap */
Cap_t *cap_init(int argc, char **argv) {
    Cap_t *cap = malloc(sizeof(Cap_t));
    if (cap == NULL) {
        CAP_LOG_ERR("%s: Failed to initialize cap\n", __FUNCTION__);
        return NULL;
    }

    cap->prog     = argv[0];
    cap->argc     = argc-1;

    if (cap->argc == 0)
        cap->argv = NULL;
    else
        cap->argv = argv+1;

    cap->scmd     = NULL;
    cap->m_flags  = NULL;
    cap->sub_cmds = NULL;

    return cap;
}


void cap_deinit(Cap_t **cap) {
    Cap_t *c = *cap;
    if (c) {
        __cap_sclist_free(&c->sub_cmds);
        __cap_flist_free(&c->m_flags);
        xfree(c);
    }
}


int cap_register_subcmd(Cap_t *cap, const char *name, const char *help) {
    __Cap_SCList_t **scl = &cap->sub_cmds;
    if (*scl == NULL) {
        *scl = __cap_sclist_new(5);
        if (*scl == NULL) {
            CAP_LOG_ERR("%s: New sub-command list is NULL\n", __FUNCTION__);
            return 1;
        }
    }


    __Cap_Subcmd_t *sc = __cap_subcmd_new(name, help);
    if (sc == NULL) {
        CAP_LOG_ERR("%s: new sub-command pointer is NULL\n", __FUNCTION__);
        return 1;
    }

    __cap_darr_append((*scl)->cmds, (*scl)->len, (*scl)->cap, sc, __Cap_Subcmd_t);
    return 0;
}


int cap_register_flag(Cap_t *cap,
                      const char *subcmd,
                      const char *name,
                      const char *help)
{
    __Cap_Subcmd_t *sc;
    __Cap_FList_t **fl;
    __Cap_Flag_t *f;

    if (subcmd == NULL) {
        fl = &cap->m_flags;
    } else {
        sc = __cap_sclist_find(cap->sub_cmds, subcmd);
        if (sc == NULL) {
            CAP_LOG_ERR("%s: sub-command not found in registered sub-commands: %s\n",
                        __FUNCTION__, subcmd);
            CAP_LOG_ERR("New flag\'s name: %s\n", name);
            return 1;
        }
        fl = &sc->flags;
    }

    if (*fl == NULL) {
        *fl = __cap_flist_new(5);
        if (*fl == NULL) {
            CAP_LOG_ERR("%s: Failed to create a new flag-list\n", __FUNCTION__);
            CAP_LOG_ERR("New flag\'s name: %s\n", name);
            return 1;
        }
    }

    f = __cap_flag_new(name, help);
    if (f == NULL) {
        CAP_LOG_ERR("%s: New flag\'s pointer is NULL\n", __FUNCTION__);
        CAP_LOG_ERR("New flag\'s name: %s\n", name);
    }

    __cap_darr_append((*fl)->flags, (*fl)->len, (*fl)->cap, f, __Cap_Flag_t);
    return 0;
}


char *cap_flag_getval(Cap_t *cap, const char *subcmd, const char *name) {
    __Cap_Flag_t *flag;
    __Cap_FList_t **fl;
    __Cap_Subcmd_t *sc;

    if (subcmd == NULL) {
        fl = &cap->m_flags;
    } else {
        sc = __cap_sclist_find(cap->sub_cmds, subcmd);
        if (sc == NULL) {
            CAP_LOG_ERR("%s: sub-command not found in registered sub-commands: %s\n",
                        __FUNCTION__, subcmd);
            return NULL;
        }
        fl = &sc->flags;
    }

    if (*fl == NULL) {
        CAP_LOG_ERR("%s: Trying to get \'%s\' flag value...\n", __FUNCTION__, name);
        CAP_LOG_ERR("%s: flag list is not initialized and it\'s NULL\n", __FUNCTION__);
        return NULL;
    }

    flag = __cap_flist_find(*fl, name);
    if (flag == NULL) {
        CAP_LOG_ERR("%s: Flag not found in registered flags list\n", __FUNCTION__);
        return NULL;
    }

    return flag->val;
}


int cap_flag_provided(Cap_t *cap,
                      const char *subcmd,
                      const char *name)
{
    __Cap_Flag_t *flag;
    __Cap_FList_t **fl;
    __Cap_Subcmd_t *sc;

    if (subcmd == NULL) {
        fl = &cap->m_flags;
    } else {
        sc = __cap_sclist_find(cap->sub_cmds, subcmd);
        if (sc == NULL) {
            CAP_LOG_ERR("%s: sub-command not found in registered sub-commands: %s\n",
                        __FUNCTION__, subcmd);
            return 0;
        }
        fl = &sc->flags;
    }

    flag = __cap_flist_find(*fl, name);
    if (flag == NULL) {
        CAP_LOG_ERR("%s: flag not found in flag-list: %s",
                    __FUNCTION__, name);
        return 0;
    }

    return flag->met;
}


/* remove dashes from beginning of flag name */
static inline char *__cap_flag_ext_name(char *str) {
    while (*str == '-') str++;
    return str;
}


static inline char *__cap_argv_get(char **argv, const int argc, const long idx) {
    return ((idx >= 0) && (idx < argc)) ? argv[idx] : NULL;
}


Cap_RawArgs_t cap_subcmd_rawargs(Cap_t *cap, const char *subcmd) {
    __Cap_Subcmd_t *sc = __cap_sclist_find(cap->sub_cmds, subcmd);

    Cap_RawArgs_t args = {
        .argc = (sc) ? sc->raw_argc : -1,
        .argv = (sc) ? sc->raw_argv : NULL,
    };

    return args;
}


int cap_parse_args(Cap_t *cap) {
    __Cap_Subcmd_t *sc = NULL;
    __Cap_FList_t *flist;
    __Cap_Flag_t *flag;
    int argc = cap->argc;
    char **argv = cap->argv, *subcmd = NULL, *next_arg;

    if (argv == NULL) {
        CAP_LOG_ERR("%s: No arguments provided\n", __FUNCTION__);
        return 1;
    }

    if (**argv != '-') {
        subcmd = *argv;
        argv++;
        argc--;
    }

    if (subcmd) {
        sc = __cap_sclist_find(cap->sub_cmds, subcmd);
        if (sc == NULL) {
            CAP_LOG_ERR("%s: Invalid sub-command: %s\n", __FUNCTION__, subcmd);
        } else {
            cap->scmd = sc;
            sc->raw_argc = argc;
            sc->raw_argv = argv;
        }
    }


    flist = (sc) ? sc->flags : cap->m_flags;
    if (flist == NULL) {
            CAP_LOG_ERR("%s: No flags registered in this flag list\n", __FUNCTION__);
            CAP_LOG_INF("%s: Nothing to parse...\n", __FUNCTION__);
            return 0;
    }

    for (int i = 0; i < argc; i++) {
        char *curr_arg = argv[i];
        if (curr_arg[0] == '-') {
            flag = __cap_flist_find(flist, __cap_flag_ext_name(curr_arg));
            if (flag == NULL) {
                CAP_LOG_ERR("%s: Invalid flag: %s\n", __FUNCTION__, curr_arg);
                continue;
            }
            flag->met = 1;

            next_arg = __cap_argv_get(argv, argc, i+1);
            if (next_arg) {
                if (*next_arg != '-')
                    flag->val = next_arg;
                else
                    flag->val = NULL;
            }
        }
    }

    return 0;
}


int cap_subcmd_provided(Cap_t *cap, const char *subcmd) {
    __Cap_Subcmd_t *sc = cap->scmd;
    return (sc) ? (strcmp(sc->name, subcmd) == 0) : 0;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
