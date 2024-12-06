#include "errmod.h"

//enum tman_errno {
//    TMAN_OK = 0,        /* OK, switch to task ID */
//    TMAN_NOPATH = 1,    /* OK, but output should not be treated as path */
//    TMAN_INVOPT = 2,    /* Error, invalid option */
//    TMAN_ECORE  = 3,    /* ?? Error in core execution (under consideration to include) */
//    TMAN_EBIN   = 4,    /* Generic: failed to executed command */
//    TMAN_EHOOK  = 5,    /* Failed to execute hook */
//    TMAN_EPGN   = 6,    /* Failed to execute plugin */
//};

struct errcodes errcodes[NERRCODE] = {
    { .err = 0, .desc = "OK" },
    { .err = 1, .desc = "OK, but no path to output" },
    { .err = 2, .desc = "invalid option" },
};
