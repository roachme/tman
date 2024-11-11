#include <stdarg.h>
#include "common.h"

int elog(char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    fprintf(stderr, "%s: ", PROGRAM);
    vfprintf(stderr, fmt, arg);
    fprintf(stderr, "\n");
    va_end(arg);
    return 1;
}

/*
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
*/

/*
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
*/
