#include <stdio.h>
#include <string.h>
#include "errmod.h"

static int errcode = TMAN_OK;
static char errmsg[ERRMSGSIZ + 1];

/* roach: Do NOT change order, cuz it breaks error messages.  */
const struct tman_err_codes errcodes[NERRCODE] = {
    { .err = TMAN_OK, .desc = "OK" },

    { .err = TMAN_ECORE,        .desc = "error happened in core" },
    { .err = TMAN_EHOOK,        .desc = "failed to execute hook" },
    { .err = TMAN_EPGN,         .desc = "failed to execute plugin command" },

    { .err = TMAN_EREQRENV,    .desc = "task environment name required" },
    { .err = TMAN_EMISSENV,    .desc = "task environment name does not exist" },
    { .err = TMAN_EILLEGENV,   .desc = "illegal environment name" },
    { .err = TMAN_ENOCURRENV,  .desc = "no current task environment" },
    { .err = TMAN_ENOPREVENV,  .desc = "no previous task environment" },
    { .err = TMAN_ENOSUCHENV,  .desc = "no such task environment" },
    { .err = TMAN_EENVEXISTS,  .desc = "task environment already exists" },

    { .err = TMAN_EREQRID,     .desc = "task ID required" },
    { .err = TMAN_EMISSID,     .desc = "task ID does not exist" },
    { .err = TMAN_EILLEGID,    .desc = "illegal task ID" },
    { .err = TMAN_ENOCURRID,   .desc = "no current task ID" },
    { .err = TMAN_ENOPREVID,   .desc = "no previous task ID" },
    { .err = TMAN_ENOSUCHID,   .desc = "no such task ID" },
    { .err = TMAN_EIDEXISTS,   .desc = "task ID already exists" },

    { .err = TMAN_ETASKMKDIR,  .desc = "could not create task directory" },
    { .err = TMAN_ETASKRMDIR,  .desc = "could not delete task directory" },
    { .err = TMAN_ETASKMKUNIT, .desc = "could not create task unit" },
    { .err = TMAN_ETASKRMUNIT, .desc = "could not delete task unit" },


    /* Not yet defined error messages.  */
    { .err = TMAN_NODEF_ERR,   .desc = "not yet defined error messages" },
};

int emod_reset(void)
{
    errcode = TMAN_OK;
    return 0;
}

int emod_set(int err)
{
    errcode = err;
    //fprintf(stderr, "emod_set: %d\n", errcode);
    return errcode;
}

char *emod_get(void)
{
    //fprintf(stderr, "emod_get: %d\n", errcode);
    return strncpy(errmsg, errcodes[errcode].desc, ERRMSGSIZ);
}
