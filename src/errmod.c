#include <stdio.h>
#include <string.h>
#include "errmod.h"

static int errcode = TMAN_OK;
static char errmsg[ERRMSGSIZ + 1];

const char *errcodes[__TMAN_STATUS_LAST] = {
    [TMAN_OK] = "OK",

    [TMAN_ECORE] =       "error happened in core",
    [TMAN_ECONF] =       "error to parse config",
    [TMAN_EHOOK] =       "failed to execute hook",
    [TMAN_EPGN] =        "failed to execute plugin command",

    [TMAN_EREQRENV] =    "task environment name required",
    [TMAN_EMISSENV] =    "task environment name does not exist",
    [TMAN_EILLEGENV] =   "illegal environment name",
    [TMAN_ENOCURRENV] =  "no current task environment",
    [TMAN_ENOPREVENV] =  "no previous task environment",
    [TMAN_ENOSUCHENV] =  "no such task environment",
    [TMAN_EENVEXISTS] =  "task environment already exists",

    [TMAN_EREQRID] =     "task ID required",
    [TMAN_EMISSID] =     "task ID does not exist",
    [TMAN_EILLEGID] =    "illegal task ID",
    [TMAN_ENOCURRID] =   "no current task ID",
    [TMAN_ENOPREVID] =   "no previous task ID",
    [TMAN_ENOSUCHID] =   "no such task ID",
    [TMAN_EIDEXISTS] =   "task ID already exists",

    [TMAN_ETASKMKDIR] =  "could not create task directory",
    [TMAN_ETASKRMDIR] =  "could not delete task directory",
    [TMAN_ETASKMKUNIT] = "could not create task unit",
    [TMAN_ETASKRMUNIT] = "could not delete task unit",

    /* Not yet defined error messages.  */
    [TMAN_NODEF_ERR] = "not yet defined error messages",
};

int emod_reset(void)
{
    errcode = TMAN_OK;
    return 0;
}

int emod_set(int err)
{
    errcode = err;
    if (errcode < 0 || errcode >= __TMAN_STATUS_LAST)
        errcode = -1;
    return errcode;
}

const char *emod_strerror(void)
{
    static char errmsg[ERRMSGSIZ + 1];

    if (errcode < 0 || errcode >= __TMAN_STATUS_LAST)
        strncpy(errmsg, "internal unknown error", ERRMSGSIZ);
    return strncpy(errmsg, errcodes[errcode], ERRMSGSIZ);
}
