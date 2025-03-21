#include <stdio.h>
#include <string.h>
#include "errmod.h"
#include "common.h"

static int errcode = TMAN_OK;

const char *errcodes[__TMAN_STATUS_LAST] = {
    [TMAN_OK] = "OK",

    [TMAN_ECORE] = "error happened in core",
    [TMAN_ESYSVAR] = "could not fill system variables",
    [TMAN_EINIT] = "not inited",
    [TMAN_EMKDIR] = "could not make directory",
    [TMAN_ECONF] = "error to parse config",
    [TMAN_EHOOK] = "failed to execute hook",
    [TMAN_EPGN] = "failed to execute plugin command",
    [TMAN_EOPENPRJDIR] = "could not open project directory",
    [TMAN_EINIT_PRJMOD] = "could not init module prj",
    [TMAN_SYS_FS] = "could not create directory",

    [TMAN_PRJ_LOAD] = "could not load prject names",
    [TMAN_PRJ_SAVE] = "could not save prject names",
    [TMAN_PRJ_SWAP] = "could not swap prjects",
    [TMAN_PRJ_TOOLONG] = "project name is too long, limit is " xstr(PRJSIZ),
    [TMAN_PRJ_ILLEG] = "illegal prject name",
    [TMAN_PRJ_NOCURR] = "no current prject",
    [TMAN_PRJ_NOPREV] = "no previous prject",
    [TMAN_PRJ_NOSUCH] = "no such prject",
    [TMAN_PRJ_EXISTS] = "prject already exists",
    [TMAN_PRJ_SWITCH] = "could not switch to prject",
    [TMAN_PRJ_DEL_CURR] = "could not unmark current prject",

    [TMAN_ID_ILLEG] = "illegal task ID",
    [TMAN_ID_TOOLONG] = "task ID is too long, limit is " xstr(IDSIZ),
    [TMAN_ID_NOSUCH] = "no such task ID",
    [TMAN_ID_NOCURR] = "no current task ID",
    [TMAN_ID_NOPREV] = "no previous task ID",
    [TMAN_ID_EXISTS] = "task ID already exists",
    [TMAN_ID_SWAP] = "could not swap task IDs",

    [TMAN_COL_ADD] = "could not add task to default column",
    [TMAN_COL_MOVE] = "could not move task to column",
    [TMAN_COL_DEL] = "could not delete task from column",
    [TMAN_COL_EXISTS] = "column does not exist",

    [TMAN_DIR_ID_DEL] = "could not delete task directory",
    [TMAN_DIR_ID_MAKE] = "could not create task directory",
    [TMAN_DIR_ID_OPEN] = "could not open task directory",

    [TMAN_DIR_PRJ_DEL] = "could not delete prj directory",
    [TMAN_DIR_PRJ_MAKE] = "could not create prj directory",
    [TMAN_DIR_PRJ_OPEN] = "could not open prj directory",

    [TMAN_UNIT_SET] = "could not set unit values",
    [TMAN_UNIT_GET] = "could not get unit values",
    [TMAN_UNIT_DEL] = "could not delete task unit",
    [TMAN_UNIT_MAKE] = "could not create task unit",
    [TMAN_UNIT_ILLEG] = "illegal unit value",

    [TMAN_CMD_BIN] = "no such command",
    [TMAN_CMD_PGN] = "no such plugin",
    [TMAN_CMD_GEN] = "no such command or plugin",

    /* HOTFIX: delete it once error code table is filled.  */
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
