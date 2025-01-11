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

int elog(int status, const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    fprintf(stderr, "%s: ", PROGRAM);
    vfprintf(stderr, fmt, arg);
    fprintf(stderr, "\n");
    va_end(arg);
    return status;
}

char *genpath_env(char *env)
{
    sprintf(pathname, "%s/%s", base, env);
    return pathname;
}

char *genpath_pgn(char *env, char *id, char *name, char *cmd)
{
    const char *fmt = "%s/%s/%s -e %s -i %s -b %s %s";
    sprintf(pathname, fmt, tmanfs.pgnins, name, name, env, id, tmanfs.base, cmd);
    return pathname;
}

char *genpath_full(char *env, char *id)
{
    sprintf(pathname, "%s/%s/%s", base, env, id);
    return pathname;
}

char *genpath_col(char *env, char *id)
{
    sprintf(pathname, "%s/%s/%s/.tman/col", base, env, id);
    return pathname;
}

char *genpath_unit(char *env, char *id)
{
    sprintf(pathname, "%s/%s/%s/.tman/unit", base, env, id);
    return pathname;
}

int wlog(char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    fprintf(stderr, "%s: ", PROGRAM);
    vfprintf(stderr, fmt, arg);
    fprintf(stderr, "\n");
    va_end(arg);
    return 0;
}

int dlog(char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    fprintf(stdout, "%s: ", PROGRAM);
    vfprintf(stdout, fmt, arg);
    fprintf(stdout, "\n");
    va_end(arg);
    return 0;
}
