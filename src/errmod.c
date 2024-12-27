#include <stdio.h>
#include <string.h>
#include "errmod.h"

static int errcode = TMAN_OK;

const char *errcodes[__TMAN_STATUS_LAST] = {
    [TMAN_OK] = "OK",

    [TMAN_ECORE] =       "error happened in core",
    [TMAN_ESYSVAR] =     "could not fill system variables",
    [TMAN_EINIT] =       "not inited",
    [TMAN_EMKDIR] =      "could not make directory",
    [TMAN_ECONF] =       "error to parse config",
    [TMAN_EHOOK] =       "failed to execute hook",
    [TMAN_EPGN] =        "failed to execute plugin command",
    [TMAN_EOPENENVDIR]=  "could not open environment directory",
    [TMAN_ESWITCHENV] =  "could not switch to environment",

    [TMAN_EREQRENV] =    "task environment name required",
    [TMAN_EMISSENV] =    "task environment name does not exist",
    [TMAN_EILLEGENV] =   "illegal environment name",
    [TMAN_ENOCURRENV] =  "no current task environment",
    [TMAN_ENOPREVENV] =  "no previous task environment",
    [TMAN_ENOSUCHENV] =  "no such task environment",
    [TMAN_EENVEXISTS] =  "task environment already exists",

    [TMAN_EREQRID] =     "task ID required",
    [TMAN_EMISSID] =     "task ID does not exist",
    [TMAN_EADDID] =      "could not add task to default column",
    [TMAN_EMOVEID] =     "could not move task to column",
    [TMAN_EDELID] =      "could not delete task from column",
    [TMAN_EILLEGID] =    "illegal task ID",
    [TMAN_ENOCURRID] =   "no current task ID",
    [TMAN_ENOPREVID] =   "no previous task ID",
    [TMAN_ENOSUCHID] =   "no such task ID",
    [TMAN_EIDEXISTS] =   "task ID already exists",
    [TMAN_ESWAPIDS] =    "could not swap current and previous task IDs",

    [TMAN_ETASKMKDIR] =  "could not create task directory",
    [TMAN_ETASKRMDIR] =  "could not delete task directory",
    [TMAN_ETASKMKUNIT] = "could not create task unit",
    [TMAN_ETASKRMUNIT] = "could not delete task unit",

    [TMAN_ECOLNEXIST] =  "column does not exist",

    [TMAN_EILLEGUNIT] =  "illegal unit value",

    [TMAN_NOSUCHCMD]  = "no such command",
    [TMAN_NOSUCHPGN]  = "no such plugin",
    [TMAN_NOCMDORPGN]  = "no such command or plugin",

    /* Not yet defined error messages.  */
    [TMAN_NODEF_ERR] = "not yet defined error messages",
};

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
        return strncpy(errmsg, "internal unknown error", ERRMSGSIZ);
    return strncpy(errmsg, errcodes[errcode], ERRMSGSIZ);
}
