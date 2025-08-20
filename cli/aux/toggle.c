#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "../tec.h"

static char prj_curr[PRJSIZ + 1];
//static char prj_prev[PRJSIZ + 1];

static char brd_curr[BRDSIZ + 1];
//static char brd_prev[BRDSIZ + 1];

static char task_curr[IDSIZ + 1];
static char task_prev[IDSIZ + 1];

static char *path_prj_toggle(char *base, const tec_arg_t * args)
{
    const char *fmt = "%s/.tec/toggles";
    static char pathname[PATH_MAX + 1];
    sprintf(pathname, fmt, base);
    //printf("path.c: %s:pathname: '%s'\n", __func__, pathname);
    return pathname;
}

static char *path_brd_toggle(char *base, const tec_arg_t * args)
{
    const char *fmt = "%s/%s/.tec/toggles";
    static char pathname[PATH_MAX + 1];
    sprintf(pathname, fmt, base, args->project);
    //printf("path.c: %s:pathname: '%s'\n", __func__, pathname);
    return pathname;
}

static char *path_task_toggle(char *base, const tec_arg_t * args)
{
    const char *fmt = "%s/%s/%s/.tec/toggles";
    static char pathname[PATH_MAX + 1];
    sprintf(pathname, fmt, base, args->project, args->board);
    //printf("path.c: %s:pathname: '%s'\n", __func__, pathname);
    return pathname;
}

static char *_get_toggle(const char *fname, char *buf, char *togname)
{
    FILE *fp;
    char *toggle;
    tec_unit_t *units;

    toggle = NULL;
    units = NULL;
    if ((fp = fopen(fname, "r")) == NULL)
        return NULL;

    while (fgets(buf, BUFSIZ, fp) != NULL)
        units = tec_unit_parse(units, buf);

    if ((toggle = tec_unit_key(units, togname))) {
        strncpy(buf, toggle, PRJSIZ);
        buf[PRJSIZ] = '\0';
    }

    tec_unit_free(units);
    fclose(fp);
    return buf[0] == '\0' ? NULL : buf;
}

static char *prj_get_curr(char *base, tec_arg_t * args)
{
    return _get_toggle(path_prj_toggle(base, args), prj_curr, "curr");
}

static char *brd_get_curr(char *base, tec_arg_t * args)
{
    return _get_toggle(path_brd_toggle(base, args), brd_curr, "curr");
}

static char *task_get_curr(char *base, tec_arg_t * args)
{
    return _get_toggle(path_task_toggle(base, args), task_curr, "curr");
}

/*
static char *prj_get_prev(char *base, tec_arg_t * args)
{
    return _get_toggle(path_prj_toggle(base, args), prj_prev, "prev");
}

static char *brd_get_prev(char *base, tec_arg_t * args)
{
    return _get_toggle(path_brd_toggle(base, args), brd_prev, "prev");
}
*/

static char *prj_get_prev(char *base, tec_arg_t * args)
{
    return _get_toggle(path_prj_toggle(base, args), task_prev, "prev");
}

static char *task_get_prev(char *base, tec_arg_t * args)
{
    return _get_toggle(path_task_toggle(base, args), task_prev, "prev");
}

/*
static int task_set_curr(char *base, tec_arg_t * args)
{
    return 0;
}

static int task_set_prev(char *base, tec_arg_t * args)
{
    return 0;
}
*/

/* Toggles: project scope: BEGIN.   */
int toggle_project_get_curr(char *base, tec_arg_t * args)
{
    if (!args->project && !(args->project = prj_get_curr(base, args)))
        return 1;
    return 0;
}

int toggle_project_get_prev(char *base, tec_arg_t * args)
{
    if (!args->project && !(args->project = prj_get_prev(base, args))) {
        elog(1, "sserror '%s'", args->project);
        return 1;
    }
    return 0;
}

/* Toggles: project scope: END.   */

int toggle_board_get_curr(char *base, tec_arg_t * args)
{
    if (!args->board && !(args->board = brd_get_curr(base, args)))
        return 1;
    return 0;
}

int toggle_task_get_curr(char *base, tec_arg_t * args)
{
    if (!args->taskid && !(args->taskid = task_get_curr(base, args)))
        return 1;
    return 0;
}

int toggle_task_get_prev(char *base, tec_arg_t * args)
{
    if (!args->taskid && !(args->taskid = task_get_prev(base, args)))
        return 1;
    return 0;
}

int toggle_project_set_curr(char *base, tec_arg_t * args)
{
    char *curr, *prev;
    tec_unit_t *toggles;

    toggles = NULL;
    curr = args->project;
    prev = prj_get_curr(base, args);

    toggles = tec_unit_add(toggles, "curr", curr);
    if (prev)
        toggles = tec_unit_add(toggles, "prev", prev);

    /* Prevent duplicates in toggles.  */
    if (prev && strcmp(curr, prev) == 0) {
        // do nothing
    } else {
        tec_unit_save(path_prj_toggle(base, args), toggles);
    }
    tec_unit_free(toggles);
    return 0;
}

int toggle_board_set_curr(char *base, tec_arg_t * args)
{
    char *curr, *prev;
    tec_unit_t *toggles;

    toggles = NULL;
    curr = args->board;
    prev = brd_get_curr(base, args);

    toggles = tec_unit_add(toggles, "curr", curr);
    if (prev)
        toggles = tec_unit_add(toggles, "prev", prev);

    /* Prevent duplicates in toggles.  */
    if (prev && strcmp(curr, prev) == 0) {
        // do nothing
    } else {
        tec_unit_save(path_brd_toggle(base, args), toggles);
    }
    tec_unit_free(toggles);
    return 0;

}

int toggle_task_set_curr(char *base, tec_arg_t * args)
{
    char *curr, *prev;
    tec_unit_t *toggles;

    toggles = NULL;
    curr = args->taskid;
    prev = task_get_curr(base, args);

    toggles = tec_unit_add(toggles, "curr", curr);
    if (prev)
        toggles = tec_unit_add(toggles, "prev", prev);

    /* Prevent duplicates in toggles.  */
    if (prev && strcmp(curr, prev) == 0) {
        // do nothing
    } else {
        tec_unit_save(path_task_toggle(base, args), toggles);
    }
    tec_unit_free(toggles);
    return 0;
}

int toggle_project_swap(char *base, tec_arg_t * args)
{
    char *curr, *prev;
    tec_unit_t *toggles;

    toggles = NULL;
    curr = prj_get_curr(base, args);
    prev = prj_get_prev(base, args);

    if (curr == NULL || prev == NULL)
        return 1;

    toggles = tec_unit_add(toggles, "curr", prev);
    toggles = tec_unit_add(toggles, "prev", curr);

    return tec_unit_save(path_prj_toggle(base, args), toggles);
}

int toggle_task_swap(char *base, tec_arg_t * args)
{
    char *curr, *prev;
    tec_unit_t *toggles;

    toggles = NULL;
    curr = task_get_curr(base, args);
    prev = task_get_prev(base, args);

    if (curr == NULL || prev == NULL)
        return 1;

    toggles = tec_unit_add(toggles, "curr", prev);
    toggles = tec_unit_add(toggles, "prev", curr);

    return tec_unit_save(path_task_toggle(base, args), toggles);
}
