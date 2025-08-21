#include <stdio.h>
#include <limits.h>

#include "dir.h"
#include "osdep.h"

static int _task_dbmkdir(const char *taskdir, tec_arg_t * args)
{
    char path[PATH_MAX + 1];
    const char *fmt = "%s/%s/%s/%s/.tec";
    sprintf(path, fmt, taskdir, args->project, args->board, args->taskid);
    return MKDIR(path);
}

static int _board_dbmkdir(const char *taskdir, tec_arg_t * args)
{
    char path[PATH_MAX + 1];
    sprintf(path, "%s/%s/%s/.tec", taskdir, args->project, args->board);
    return MKDIR(path);
}

static int _project_dbmkdir(const char *taskdir, tec_arg_t * args)
{
    char path[PATH_MAX + 1];
    sprintf(path, "%s/%s/.tec", taskdir, args->project);
    return MKDIR(path);
}

int dir_task_add(const char *taskdir, tec_arg_t * args)
{
    char path[PATH_MAX + 1];
    sprintf(path, "%s/%s/%s/%s", taskdir, args->project, args->board,
            args->taskid);
    return !(MKDIR(path) == 0 && _task_dbmkdir(taskdir, args) == 0);
}

int dir_task_del(const char *taskdir, tec_arg_t * args)
{
    char path[BUFSIZ + 1];
    sprintf(path, "%s/%s/%s/%s", taskdir, args->project, args->board,
            args->taskid);
    return RMDIR(path);
}

int dir_task_move(const char *taskdir, tec_arg_t * src, tec_arg_t * dst)
{
    char new_path[PATH_MAX + 1];
    char old_path[PATH_MAX + 1];
    sprintf(old_path, "%s/%s/%s/%s", taskdir, src->project, src->board,
            src->taskid);
    sprintf(new_path, "%s/%s/%s/%s", taskdir, dst->project, dst->board,
            dst->taskid);
    return rename(old_path, new_path);
}

int dir_board_add(const char *taskdir, tec_arg_t * args)
{
    char path[PATH_MAX + 1];
    sprintf(path, "%s/%s/%s", taskdir, args->project, args->board);
    return !(MKDIR(path) == 0 && _board_dbmkdir(taskdir, args) == 0);
}

int dir_board_del(const char *taskdir, tec_arg_t * args)
{
    char path[PATH_MAX + 1];
    sprintf(path, "%s/%s/%s", taskdir, args->project, args->board);
    return RMDIR(path);
}

int dir_project_add(const char *taskdir, tec_arg_t * args)
{
    char path[PATH_MAX + 1];
    sprintf(path, "%s/%s/", taskdir, args->project);
    return !(MKDIR(path) == 0 && _project_dbmkdir(taskdir, args) == 0);
}

int dir_project_del(const char *taskdir, tec_arg_t * args)
{
    char path[PATH_MAX + 1];
    sprintf(path, "%s/%s", taskdir, args->project);
    return RMDIR(path);
}

int dir_project_rename(const char *taskdir, tec_arg_t * src, tec_arg_t * dst)
{
    char srcpath[PATH_MAX + 1];
    char dstpath[PATH_MAX + 1];

    sprintf(srcpath, "%s/%s", taskdir, src->project);
    sprintf(dstpath, "%s/%s", taskdir, dst->project);
    return MOVE(srcpath, dstpath);
}
