#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "cache.h"
#include "../cli.h"

static char prj_curr[PRJSIZ + 1];
static char prj_prev[PRJSIZ + 1];

static char brd_curr[BRDSIZ + 1];
static char brd_prev[BRDSIZ + 1];

static char task_curr[IDSIZ + 1];
static char task_prev[IDSIZ + 1];

static char *path_prj_toggle(char *base, const tman_arg_t * args)
{
    const char *fmt = "%s/.tman/toggles";
    static char pathname[PATH_MAX + 1];
    sprintf(pathname, fmt, base);
    //printf("path.c: %s:pathname: '%s'\n", __func__, pathname);
    return pathname;
}

static char *path_brd_toggle(char *base, const tman_arg_t * args)
{
    const char *fmt = "%s/%s/.tman/toggles";
    static char pathname[PATH_MAX + 1];
    sprintf(pathname, fmt, base, args->prj);
    //printf("path.c: %s:pathname: '%s'\n", __func__, pathname);
    return pathname;
}

static char *path_task_toggle(char *base, const tman_arg_t * args)
{
    const char *fmt = "%s/%s/%s/.tman/toggles";
    static char pathname[PATH_MAX + 1];
    sprintf(pathname, fmt, base, args->prj, args->brd);
    //printf("path.c: %s:pathname: '%s'\n", __func__, pathname);
    return pathname;
}

static tman_unit_t *_get_toggles(const char *fname)
{
    FILE *fp;
    char buf[BUFSIZ + 1];
    tman_unit_t *toggles;

    toggles = NULL;
    if ((fp = fopen(fname, "r")) == NULL)
        return NULL;

    while (fgets(buf, BUFSIZ, fp) != NULL)
        toggles = tman_unit_parse(toggles, buf);

    fclose(fp);
    return toggles;
}

static char *_get_toggle(const char *fname, char *buf, char *tog)
{
    FILE *fp;
    char *curr;
    tman_unit_t *units;

    curr = NULL;
    units = NULL;
    if ((fp = fopen(fname, "r")) == NULL)
        return NULL;

    while (fgets(buf, BUFSIZ, fp) != NULL)
        units = tman_unit_parse(units, buf);

    if ((curr = tman_unit_key(units, tog)) == NULL)
        return NULL;

    strncpy(buf, curr, PRJSIZ);
    buf[PRJSIZ] = '\0';

    tman_unit_free(units);
    fclose(fp);
    return buf;
}

static char *prj_get_curr(char *base, tman_arg_t * args)
{
    return _get_toggle(path_prj_toggle(base, args), prj_curr, "curr");
}

static char *brd_get_curr(char *base, tman_arg_t * args)
{
    return _get_toggle(path_brd_toggle(base, args), brd_curr, "curr");
}

static char *task_get_curr(char *base, tman_arg_t * args)
{
    return _get_toggle(path_task_toggle(base, args), task_curr, "curr");
}

static char *prj_get_prev(char *base, tman_arg_t * args)
{
    return _get_toggle(path_prj_toggle(base, args), prj_prev, "prev");
}

static char *brd_get_prev(char *base, tman_arg_t * args)
{
    return _get_toggle(path_brd_toggle(base, args), brd_prev, "prev");
}

static char *task_get_prev(char *base, tman_arg_t * args)
{
    return _get_toggle(path_task_toggle(base, args), task_prev, "prev");
}

static int task_set_curr(char *base, tman_arg_t * args)
{
    return 0;
}

static int task_set_prev(char *base, tman_arg_t * args)
{
    return 0;
}

int toggle_prj_get_curr(char *base, tman_arg_t * args)
{
    if (!args->prj && !(args->prj = prj_get_curr(base, args)))
        return 1;
    return 0;
}

int toggle_brd_get_curr(char *base, tman_arg_t * args)
{
    if (!args->brd && !(args->brd = brd_get_curr(base, args)))
        return 1;
    return 0;
}

int toggle_task_get_curr(char *base, tman_arg_t * args)
{
    if (!args->task && !(args->task = task_get_curr(base, args)))
        return 1;
    return 0;
}

int toggle_task_get_prev(char *base, tman_arg_t * args)
{
    if (!args->task && !(args->task = task_get_prev(base, args)))
        return 1;
    return 0;
}

int toggle_task_set_curr(char *base, tman_arg_t * args)
{
    char *curr, *prev;
    tman_unit_t *toggles;

    toggles = NULL;
    curr = args->task;
    prev = task_get_curr(base, args);

    toggles = tman_unit_add(toggles, "curr", curr);
    if (prev)
        toggles = tman_unit_add(toggles, "prev", prev);
    return tman_unit_save(path_task_toggle(base, args), toggles);
}

int toggle_task_del_curr(char *base, tman_arg_t * args)
{
    return 1;
}

int toggle_task_del_prev(char *base, tman_arg_t * args)
{
    return 1;
}

int toggle_task_swap(char *base, tman_arg_t * args)
{
    char *curr, *prev;

    curr = task_get_curr(base, args);
    prev = task_get_prev(base, args);

    if (curr == NULL || prev == NULL)
        return 1;

    args->task = prev;
    task_set_curr(base, args);

    args->task = curr;
    task_set_prev(base, args);
    return 0;
}
