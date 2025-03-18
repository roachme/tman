#include <stdarg.h>
#include <ctype.h>

#include "tman.h"
#include "common.h"

static char *base = tmanfs.base;
static char pathname[PATHSIZ + 1];

char *formpath(char *dst, char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsprintf(dst, fmt, arg);
    va_end(arg);
    return dst;
}

char *genpath_prj(char *prj)
{
    sprintf(pathname, "%s/%s", base, prj);
    return pathname;
}

char *genpath_pgn(char *prj, char *id, char *name, char *cmd)
{
    const char *fmt = "%s/%s/%s -p %s -i %s -b %s %s";
    sprintf(pathname, fmt, tmanfs.pgnins, name, name, prj, id, tmanfs.base, cmd);
    return pathname;
}

char *genpath_full(char *prj, char *id)
{
    sprintf(pathname, "%s/%s/%s", base, prj, id);
    return pathname;
}

char *genpath_col(char *prj, char *id)
{
    sprintf(pathname, "%s/%s/%s/.tman/col", base, prj, id);
    return pathname;
}

char *genpath_unit(char *prj, char *id)
{
    sprintf(pathname, "%s/%s/%s/.tman/unit", base, prj, id);
    return pathname;
}
