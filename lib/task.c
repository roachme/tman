/*  Module to move task from column to column.
    It also supports toggles, i.e. current and previous task IDs.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include "task.h"
#include "col.h"
#include "osdep.h"

static char curr[IDSIZ + 1], prev[IDSIZ + 1];

/* For external use. Used only by getters so no harm to module.  */
static char taskcurr[IDSIZ + 1], taskprev[IDSIZ + 1];

/*
 * Notes:
 * 1. col_del() should delete a file or what? Move it to column blog or done??
*/

static int addcurr(char *prj, char *id)
{
    /* Prevent duplicate toggles.  */
    if (strncmp(id, curr, IDSIZ) == 0) {
        dlog("task: assert: addcurr: tryna add toggle duplicate");
        return 0;
    }

    if (prev[0] != '\0')
        col_set(prj, prev, COLBLOG);

    if (curr[0] != '\0') {
        col_set(prj, curr, COLPREV);
        strncpy(prev, curr, IDSIZ);
    }
    col_set(prj, id, COLCURR);
    strncpy(curr, id, IDSIZ);
    return 0;
}

static int delcurr(char *prj)
{
    col_set(prj, curr, COLBLOG);
    memset(curr, 0, IDSIZ);
    if (prev[0] != '\0') {
        col_set(prj, prev, COLCURR);
        strncpy(curr, prev, IDSIZ);
    }
    return 0;
}

static int delprev(char *prj)
{
    if (curr[0] == '\0') {
        dlog("task: delprev: curr not set");
        return 1;
    }

    col_set(prj, prev, COLBLOG);
    prev[0] = '\0';
    return 0;
}

static int addprev(char *prj, char *id)
{
    col_set(prj, id, COLPREV);
    strncpy(prev, id, IDSIZ);
    return 0;
}

static int swap(char *prj)
{
    char tmp[IDSIZ + 1];

    if (curr[0] == '\0' || prev[0] == '\0') {
        dlog("task: swap: either curr or prev not set");
        return 1;
    }
    strncpy(tmp, prev, IDSIZ);
    return addcurr(prj, tmp);
}

/* Move current task to another column.  */
static int movecurr(char *prj, char *id, char *col)
{
    if (strncmp(col, COLCURR, COLSIZ) == 0)
        return 0; /* do nothing */
    if (strncmp(col, COLPREV, COLSIZ) == 0) {
        /* if previous task ID not set then it's an error, right?  */
        return swap(prj);
    }
    /* note: maybe it's better to use movetask() ?  */
    /* Delete current task ID and move it to another column.  */
    /* FIXME: can't move current task ID to another column.  */
    return delcurr(prj);
}

/* Move previous task to another column.  */
static int moveprev(char *prj, char *id, char *col)
{
    if (strncmp(col, COLPREV, COLSIZ) == 0)
        return 0; /* do nothing */
    if (strncmp(col, COLCURR, COLSIZ) == 0)
        return swap(prj);
    return delprev(prj);
}

/* Move random task to another column. */
static int movetask(char *prj, char *id, char *col)
{
    if (strncmp(col, COLCURR, COLSIZ) == 0) {
        return addcurr(prj, id);
    }
    else if (strncmp(col, COLPREV, COLSIZ) == 0) {
        return addprev(prj, id);
    }
    return col_set(prj, id, col);
}

static int reset()
{
    memset(curr, 0, IDSIZ);
    memset(prev, 0, IDSIZ);
    return 0;
}

static int load(char *prj)
{
    DIR *dir;
    char *col;
    struct dirent *ent;

    if ((dir = opendir(genpath_prj(prj))) == NULL)
        return 1;

    reset();
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        else if ((col = col_get(prj, ent->d_name)) == NULL) {
            fprintf(stderr, "warn: could not get col file: %s:%s\n", prj, ent->d_name);
            continue;
        }
        if (strncmp(col, COLCURR, IDSIZ) == 0)
            strncpy(curr, ent->d_name, IDSIZ);
        else if (strncmp(col, COLPREV, IDSIZ) == 0)
            strncpy(prev, ent->d_name, IDSIZ);
        else if (curr[0] != '\0' && prev[0] != '\0')
            break;
    }
    return closedir(dir);
}

/*
 * Get current task ID in project.
 * @param prj project name
 * @return current task ID
*/
char *task_curr(char *prj)
{
    if (load(prj) || curr[0] == '\0')
        return NULL;
    return strncpy(taskcurr, curr, IDSIZ);
}

/*
 * Get previous task ID in project.
 * @param prj project name
 * @return previous task ID
*/
char *task_prev(char *prj)
{
    if (load(prj) || prev[0] == '\0')
        return NULL;
    return strncpy(taskprev, prev, IDSIZ);
}

/*
 * Check that task ID is valid.
 * @param id task ID
 * @return on success - 1, otherwise 0
*/
int task_chk(char *id)
{
    if (!isalnum(*id++))
        return 0;
    for ( ; *id; ++id)
        if (!(isalnum(*id) || *id == '_' || *id == '-'))
            return 0;
    return isalnum(*--id);
}

/*
 * Check that task ID exists in project.
 * @param prj project name
 * @param id task ID
*/
int task_ext(char *prj, char *id)
{
    return ISDIR(genpath_full(prj, id));
}

/*
 * Add a task to column.
 * @param prj project name
 * @param id task ID
*/
int task_add(char *prj, char *id)
{
    char taskid[IDSIZ + 1];

    strncpy(taskid, id, IDSIZ);
    if (col_set(prj, taskid, COLBLOG)) {
        dlog("task: task_add: could not add new task to default column");
        return 1;
    }
    return 0;
}

/**
 * Delete task from column.
 * @param prj project name
 * @param id task ID
 * @return on success: 0
 * @return on failure: 1
*/
int task_del(char *prj, char *id)
{
    char taskid[IDSIZ + 1];

    if (load(prj))
        return 1;

    strncpy(taskid, id, IDSIZ);
    if (strncmp(taskid, curr, IDSIZ) == 0)
        return delcurr(prj);
    else if (strncmp(taskid, prev, IDSIZ) == 0)
        return delprev(prj);
    return col_del(prj, taskid);
}

int task_iscurr(char *prj, char *id)
{
    char *currid = task_curr(prj);

    if (currid == NULL)
        return FALSE;
    else if (strncmp(currid, id, IDSIZ) == 0)
        return TRUE;
    return FALSE;
}

/*
 * Move task to another column.
 * @param prj project name
 * @param id task ID
 * @param col column name
*/
int task_move(char *prj, char *id, char *col)
{
    char taskid[IDSIZ + 1];

    if (load(prj))
        return 1;

    strncpy(taskid, id, IDSIZ);
    if (strncmp(curr, taskid, IDSIZ) == 0) {
        return movecurr(prj, curr, col);
    }
    else if (strncmp(prev, taskid, IDSIZ) == 0) {
        return moveprev(prj, prev, col);
    }
    return movetask(prj, taskid, col);
}

/**
 * Swap current and previous task IDs in project.
 * @param prj project
*/
int task_swap(char *prj)
{
    return swap(prj);
}
