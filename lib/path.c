#include <stdio.h>
#include <limits.h>
#include <stdarg.h>

#include "path.h"

char *path_generic(char *buf, const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsprintf(buf, fmt, arg);
    va_end(arg);
    return buf;
}

char *path_project_board(const char *taskdir, const tec_arg_t * args)
{
    const char *fmt = "%s/%s/.tec/useboard";
    static char pathname[PATH_MAX + 1];
    return path_generic(pathname, fmt, taskdir, args->project);
}

char *path_project_dir(const char *taskdir, const tec_arg_t * args)
{
    const char *fmt = "%s/%s";
    static char pathname[PATH_MAX + 1];
    return path_generic(pathname, fmt, taskdir, args->project);
}

char *path_board_dir(const char *taskdir, const tec_arg_t * args)
{
    const char *fmt = "%s/%s/%s";
    static char pathname[PATH_MAX + 1];
    return path_generic(pathname, fmt, taskdir, args->project, args->board);
}

char *path_task_dir(const char *taskdir, const tec_arg_t * args)
{
    const char *fmt = "%s/%s/%s/%s";
    static char pathname[PATH_MAX + 1];
    return path_generic(pathname, fmt, taskdir, args->project, args->board,
                        args->taskid);
}

/* Generate path for unit files.  */
char *path_project_unit(const char *taskdir, const tec_arg_t * args)
{
    const char *fmt = "%s/%s/.tec/units";
    static char pathname[PATH_MAX + 1];
    return path_generic(pathname, fmt, taskdir, args->project);
}

char *path_board_unit(const char *taskdir, const tec_arg_t * args)
{
    const char *fmt = "%s/%s/%s/.tec/units";
    static char pathname[PATH_MAX + 1];
    return path_generic(pathname, fmt, taskdir, args->project, args->board);
}

char *path_task_unit(const char *taskdir, const tec_arg_t * args)
{
    const char *fmt = "%s/%s/%s/%s/.tec/units";
    static char pathname[PATH_MAX + 1];
    return path_generic(pathname, fmt, taskdir, args->project, args->board,
                        args->taskid);
}

char *path_project_column(const char *taskdir, const tec_arg_t * args)
{
    const char *fmt = "%s/%s/.tec/column";
    static char pathname[PATH_MAX + 1];
    return path_generic(pathname, fmt, taskdir, args->project);
}

char *path_board_column(const char *taskdir, const tec_arg_t * args)
{
    const char *fmt = "%s/%s/%s/.tec/column";
    static char pathname[PATH_MAX + 1];
    return path_generic(pathname, fmt, taskdir, args->project, args->board);
}

char *path_task_column(const char *taskdir, const tec_arg_t * args)
{
    const char *fmt = "%s/%s/%s/%s/.tec/column";
    static char pathname[PATH_MAX + 1];
    return path_generic(pathname, fmt, taskdir, args->project, args->board,
                        args->taskid);
}
