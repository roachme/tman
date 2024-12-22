/*
    Module to move task from column to column.
    It also supports toggles, i.e. current and previous task IDs.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "common.h"
#include "errmod.h"
#include "tman.h"
#include "task.h"
#include "column.h"

static char *base = tmanfs.base;
static char curr[IDSIZ + 1], prev[IDSIZ + 1];

static int reset_toggles()
{
    curr[0] = prev[0] = '\0';
    return 0;
}

static int load_toggles(char *env)
{
    DIR *dir;
    char *id, *col;
    struct dirent *ent;

    if ((dir = opendir(genpath_env(env))) == NULL)
        return emod_set(TMAN_EOPENENVDIR);

    while ((ent = readdir(dir)) != NULL && (id = ent->d_name) != NULL) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        else if ((col = column_get(env, id)) == NULL) {
            fprintf(stderr, "err: could not get task unit: %s\n", id);
            continue;
        }

        if (strncmp(col, MARKCURR, COLSIZ) == 0)
            strncpy(curr, id, IDSIZ);
        else if (strncmp(col, MARKPREV, COLSIZ) == 0)
            strncpy(prev, id, IDSIZ);
    }
    closedir(dir);
    return TRUE;
}

static int save_toggles(char *env)
{
    if (curr[0] != '\0')
        column_add(env, curr, MARKCURR);
    if (prev[0] != '\0')
        column_add(env, prev, MARKPREV);
    return 0;
}

static int _movecurr(char *env, char *col)
{
    if (strncmp(col, MARKCURR, COLSIZ) == 0)
        return 0; // do nothing
    else if (strncmp(col, MARKPREV, COLSIZ) == 0) {
        fprintf(stderr, "swap not allowed this way\n");
        return 1;
    }
    column_add(env, curr, col);
    if (prev[0] != '\0')
        column_add(env, prev, MARKCURR);
    reset_toggles();
    return 0;
}

static int _moveprev(char *env, char *col)
{
    if (strncmp(col, MARKPREV, COLSIZ) == 0)
        return 0; // do nothing
    else if (strncmp(col, MARKCURR, COLSIZ) == 0) {
        // TODO: should be legal.
        fprintf(stderr, "cant move prev to curr\n");
        return 1;
    }

    column_add(env, prev, col);
    reset_toggles();
    return 0;
}

static int _movetask(char *env, char *id, char *col)
{
    if (strncmp(col, MARKPREV, COLSIZ) == 0) {
        fprintf(stderr, "cant move task to prev this way\n");
        return 1;
    }

    if (strncmp(col, MARKCURR, COLSIZ) == 0 && curr[0] != '\0') {
        fprintf(stderr, "_movetask: old curr '%s'\n", curr);
        column_add(env, curr, MARKPREV);
        if (prev[0] != '\0')
            column_add(env, prev, MARKBLOG);
        column_add(env, id, MARKCURR);
    } else {
        fprintf(stderr, "_movetask: rest: new curr '%s'\n", id);
        column_add(env, id, col);
    }
    reset_toggles();
    return 0;
}

static int _delete_curr(char *env)
{
    fprintf(stderr, "_delete_curr: start\n");
    column_add(env, curr, MARKBLOG);
    if (prev[0] != '\0')
        column_add(env, prev, MARKCURR);
    return reset_toggles();
}

static int _delete_prev(char *env)
{
    fprintf(stderr, "_delete_prev: start\n");
    column_add(env, prev, MARKBLOG);
    return reset_toggles();
}

int task_exists(char *env, char *id)
{
    return ISDIR(genpath_full(env, id));
}

int task_add(char *env, char *id, char *col)
{
    if (column_add(env, id, MARKBLOG)) {
        fprintf(stderr, "column_add: error\n");
        return 1;
    }
    return task_move(env, id, col);
}

int task_del(char *env, char *id)
{
    if (load_toggles(env) == FALSE)
        return 1;

    if (strncmp(id, curr, IDSIZ) == 0) {
        fprintf(stderr, "_delete_curr: id: %s\n", id);
        return _delete_curr(env);
    }
    return _delete_prev(env);
}

int task_move(char *env, char *id, char *col)
{
    if (load_toggles(env) == FALSE) {
        fprintf(stderr, "task_move: could not load_toggles\n");
        return 1;
    }

    if (strncmp(id, curr, IDSIZ) == 0) {
        fprintf(stderr, "task_move: _movecurr\n");
        return _movecurr(env, col);
    }
    else if (strncmp(id, prev, IDSIZ) == 0) {
        fprintf(stderr, "task_move: _moveprev\n");
        return _moveprev(env, col);
    }
    else {
        fprintf(stderr, "task_move: _movetask\n");
        return _movetask(env, id, col);
    }
}

char *task_getcurr(char *env)
{
    if (load_toggles(env) == FALSE)
        return NULL;
    return curr[0] != '\0' ? curr : NULL;
}

char *task_getprev(char *env)
{
    if (load_toggles(env) == FALSE)
        return NULL;
    return prev[0] != '\0' ? prev : NULL;
}

int task_swap(char *env)
{
    char tmp[IDSIZ + 1];
    if (load_toggles(env) == FALSE)
        return 1;
    else if (curr[0] == '\0' || prev[0] == '\0')
        return 1;

    // TODO: find a good way to use save_toggles();
    column_add(env, curr, MARKPREV);
    column_add(env, prev, MARKCURR);
    return 0;
}
