#include <string.h>

#include "libtec.h"

static int errcode = LIBTEC_OK;

const char *errcodes[__LIBTEC_STATUS_LAST] = {
    [LIBTEC_OK] = "OK",

    [LIBTEC_SYS_DB] = "database directory not found",
    [LIBTEC_SYS_MALLOC] = "could not malloc memory",

    [LIBTEC_ARG_ILLEG] = "illegal object name",
    [LIBTEC_ARG_NOSUCH] = "no such object",
    [LIBTEC_ARG_EXISTS] = "object already exists",

    [LIBTEC_DIR_DEL] = "could not delete directory",
    [LIBTEC_DIR_MAKE] = "could not create directory",
    [LIBTEC_DIR_MOVE] = "could not rename directory",
    [LIBTEC_DIR_OPEN] = "could not open directory",

    [LIBTEC_UNIT_ADD] = "could not add unit node",
    [LIBTEC_UNIT_DEL] = "could not delete unit node",
    [LIBTEC_UNIT_GET] = "could not get unit values",
    [LIBTEC_UNIT_ILLEG] = "illegal unit value",
    [LIBTEC_UNIT_KEY] = "unit key does not exist",
    [LIBTEC_UNIT_LOAD] = "could not load units",
    [LIBTEC_UNIT_SAVE] = "could not save unit values",
    [LIBTEC_UNIT_SET] = "could not set unit values",

    /* HOTFIX: delete it once error code table is filled.  */
    /* Not yet defined error messages.  */
    [LIBTEC_NODEF_ERR] = "not yet defined error messages",
};

int emod_set(int err)
{
    errcode = err;
    if (errcode < 0 || errcode > __LIBTEC_STATUS_LAST)
        errcode = -1;
    return errcode;
}

char *emod_strerror(int errnum)
{
    static char errmsg[TEC_ERRMSGSIZ + 1];

    if (errnum < 0 || errnum > __LIBTEC_STATUS_LAST)
        return strncpy(errmsg, "internal unknown error", TEC_ERRMSGSIZ);
    return strncpy(errmsg, errcodes[errnum], TEC_ERRMSGSIZ);
}
